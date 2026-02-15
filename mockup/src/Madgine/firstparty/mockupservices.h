#pragma once

#include "Madgine/firstparty/firstpartyservices.h"

#include "Modules/threading/taskqueue.h"

#include "Madgine/serialize/network/networkmanager.h"

#include "Meta/serialize/container/noparent.h"

#include "mockupstate.h"

namespace Engine {
namespace FirstParty {

    struct MockupServices;

    struct MockupClientState : MockupState {
        MockupClientState(MockupServices &services);

        void update();
        Threading::Task<bool> connect();
        void disconnect();

        void updateLobbyInfoImpl(std::optional<LobbyInfo> info) override;
        void sendServerAddressImpl(SocketAddress address, LobbyInfo session) override;

        MockupServices &mServices;

        bool mIsOwner;
    };

    struct MockupServices : FirstPartyServicesImpl<MockupServices> {

        MockupServices(Root::Root &root);
        ~MockupServices();

        std::string_view key() const override;

        ////////// IDENTITY

        std::string currentUserName() const override;

        ////////// LEADERBOARD

        Threading::Task<Leaderboard> getLeaderboardTask(const char *name, Leaderboard::AccessMode accessmode, Leaderboard::ReferenceRank referenceRank, int32_t rangeBegin, int32_t rangeEnd) override;

        /////////// STATS        

        Threading::Task<bool> ingestStatTask(const char *name, const char *leaderboardName, int32_t value) override;

        /////////// ACHIEVEMENTS

        Threading::Task<bool> unlockAchievementTask(const char *name) override;

        /////////// MATCHMAKING

        Threading::Task<std::vector<Lobby>> getLobbyListTask(std::map<std::string, std::string> filters) override;
        Threading::Task<std::optional<LobbyInfo>> createLobbyTask(size_t maxPlayerCount, std::map<std::string, std::string> properties = {}) override;
        Threading::Task<std::optional<LobbyInfo>> joinLobbyTask(uint64_t id) override;
        Threading::Task<std::optional<ServerInfo>> startMatchTask() override;
        void leaveLobby() override;
        void leaveMatch() override;

        void setLobbyProperty(std::string_view key, std::string_view value) override;
        void updateLobbyInfo(std::optional<LobbyInfo> info);
        void connectToServer(SocketAddress address, LobbyInfo session);

        //////////////////

        Serialize::NoParent<MockupClientState> mState;
        Network::NetworkManager mSyncManager;
    };

}
}