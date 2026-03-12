#pragma once

#include "Meta/serialize/syncmanager.h"

#include <steam/steam_api.h>

#include "Generic/execution/signal.h"

namespace Engine {
namespace FirstParty {

    struct SteamSyncManager : Serialize::SyncManager {
        using Serialize::SyncManager::SyncManager;

        void setup();

        void listen(size_t count, Serialize::Format format);

        Execution::Future<Serialize::SyncManagerResult> connect(CSteamID target, Serialize::Format format, TimeOut timeout = {});        

        void disconnect();

        Execution::SignalStub<void> &playersConnected();

        Serialize::ParticipantId resolvePlayerId(CSteamID id);

    protected:
        void removeSlaveStream(Serialize::SyncManagerResult reason = Serialize::SyncManagerResult::UNKNOWN_ERROR) override;
        std::map<Serialize::ParticipantId, Serialize::FormattedMessageStream>::iterator removeMasterStream(std::map<Serialize::ParticipantId, Serialize::FormattedMessageStream>::iterator it, Serialize::SyncManagerResult reason = Serialize::SyncManagerResult::UNKNOWN_ERROR) override;

    private:
        Serialize::Format mFormat = nullptr;

        HSteamListenSocket mListenSocket = k_HSteamListenSocket_Invalid;

        size_t mRemainingPlayersToConnect;
        Execution::Signal<void> mPlayersConnected;

        STEAM_CALLBACK(SteamSyncManager, onConnectionUpdate, SteamNetConnectionStatusChangedCallback_t);

        std::map<CSteamID, Serialize::ParticipantId> mStoredPlayers;
    };

}
}