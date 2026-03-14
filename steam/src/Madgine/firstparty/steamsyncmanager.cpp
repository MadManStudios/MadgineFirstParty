#include "../steamserviceslib.h"
#include "steamsyncmanager.h"

#include "steamsyncmanager.h"

#include <steam/steam_api.h>

#include "steamstreambuf.h"

#include "steamstreamdata.h"

#include "Meta/serialize/streams/syncstreamdata.h"

namespace Engine {
namespace FirstParty {

    void SteamSyncManager::setup()
    {
        SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg, [](ESteamNetworkingSocketsDebugOutputType nType, const char *pszMsg) {
            Log::MessageType type;
            switch (nType) {
            case k_ESteamNetworkingSocketsDebugOutputType_Bug:
                type = Log::MessageType::FATAL_TYPE;
                break;
            case k_ESteamNetworkingSocketsDebugOutputType_Error:
                type = Log::MessageType::ERROR_TYPE;
                break;
            case k_ESteamNetworkingSocketsDebugOutputType_Important:
            case k_ESteamNetworkingSocketsDebugOutputType_Warning:
                type = Log::MessageType::WARNING_TYPE;
                break;
            case k_ESteamNetworkingSocketsDebugOutputType_Msg:
                type = Log::MessageType::INFO_TYPE;
                break;
            default:
                throw 0;
            }
            Engine::Log::LogDummy out { type };
            out << pszMsg;
        });

        SteamNetworkingUtils()->InitRelayNetworkAccess();
    }

    void SteamSyncManager::listen(size_t count, Serialize::Format format)
    {
        mFormat = format;

        mRemainingPlayersToConnect = count;

        mListenSocket = SteamNetworkingSockets()->CreateListenSocketP2P(0, 0, nullptr);
    }

    Execution::Future<Serialize::SyncManagerResult> SteamSyncManager::connect(CSteamID target, Serialize::Format format, TimeOut timeout)
    {
        SteamNetworkingIdentity identity;
        identity.SetSteamID(target);
        HSteamNetConnection con = SteamNetworkingSockets()->ConnectP2P(identity, 0, 0, nullptr);

        return setSlaveStream(format, std::make_unique<SteamStreambuf>(con), timeout, std::make_unique<SteamStreamData>(*this, createStreamId(), target));
    }

    void SteamSyncManager::disconnect()
    {
        if (mListenSocket != k_HSteamListenSocket_Invalid) {
            SteamNetworkingSockets()->CloseListenSocket(mListenSocket);
            mListenSocket = k_HSteamListenSocket_Invalid;
            mStoredPlayers.clear();
        }
        removeAllStreams();
        clearTopLevelItems();
    }

    Execution::SignalStub<void> &SteamSyncManager::playersConnected()
    {
        return mPlayersConnected;
    }

    Serialize::ParticipantId SteamSyncManager::resolvePlayerId(CSteamID id)
    {
        if (id == SteamUser()->GetSteamID())
            return Serialize::sLocalMasterParticipantId;

        for (const Serialize::FormattedMessageStream& out : getMasterMessageTargets()) {
            if (static_cast<SteamStreamData*>(out.data())->user() == id) {
                return out.id();
            }
        }
         
        throw 0;
    }

    void SteamSyncManager::removeSlaveStream(Serialize::SyncManagerResult reason)
    {
        SyncManager::removeSlaveStream(reason);
    }

    void SteamSyncManager::removeMasterStream(Serialize::ParticipantId id, Serialize::SyncManagerResult reason)
    {
        SteamStreamData *data = static_cast<SteamStreamData*>(getMasterStream(id).data());

        mStoredPlayers[data->user()] = data->id();

        return SyncManager::removeMasterStream(id, reason);
    }

    void SteamSyncManager::onConnectionUpdate(SteamNetConnectionStatusChangedCallback_t *con)
    {
        if (con->m_info.m_hListenSocket != k_HSteamListenSocket_Invalid) {
            switch (con->m_info.m_eState) {
            case k_ESteamNetworkingConnectionState_Connecting: {
                EResult result = SteamNetworkingSockets()->AcceptConnection(con->m_hConn);
                assert(result == k_EResultOK);

                CSteamID user = con->m_info.m_identityRemote.GetSteamID();

                Serialize::ParticipantId id;
                auto it = mStoredPlayers.find(user);
                if (it != mStoredPlayers.end())
                    id = it->second;
                else
                    id = createStreamId();

                [[maybe_unused]] bool success = SteamNetworkingSockets()->SetConnectionUserData(con->m_hConn, id);
                assert(success);

                std::unique_ptr<SteamStreamData> data = std::make_unique<SteamStreamData>(*this, id, user);

                Serialize::SyncManagerResult result2 = addMasterStream(mFormat, std::make_unique<SteamStreambuf>(con->m_hConn), std::move(data));
                assert(result2 == Serialize::SyncManagerResult::SUCCESS);
                break;
            }
            case k_ESteamNetworkingConnectionState_Connected:
                if (--mRemainingPlayersToConnect == 0)
                    mPlayersConnected.emit();
                break;
            case k_ESteamNetworkingConnectionState_ClosedByPeer:
            case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
                removeMasterStream(SteamNetworkingSockets()->GetConnectionUserData(con->m_hConn), Serialize::SyncManagerResult::TIMEOUT);
                break;
            }
        }
        else {
            switch (con->m_info.m_eState) {
            case k_ESteamNetworkingConnectionState_Connecting: 
                break;
            case k_ESteamNetworkingConnectionState_Connected:
                break;
            case k_ESteamNetworkingConnectionState_ClosedByPeer:
            case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
                assert(SteamNetworkingSockets()->GetConnectionUserData(con->m_hConn) == -1); //Not a Master Stream
                removeSlaveStream(Serialize::SyncManagerResult::TIMEOUT);
                break;
            }
        }
    }

}
}