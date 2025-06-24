#pragma once

#include "Madgine/firstparty/firstpartyservices.h"

#include "Modules/threading/taskqueue.h"

//#include <steam/steam_api.h>

namespace Engine {
namespace FirstParty {

    struct GooglePlayServices : FirstPartyServicesImpl<GooglePlayServices> {

        GooglePlayServices(Root::Root &root);
        ~GooglePlayServices();

        virtual std::string_view key() const override;

        ///////// IDENTITY

        std::string currentUserName() const override;

        ////////// LEADERBOARD

        virtual Threading::Task<Leaderboard> getLeaderboardTask(const char *name, Leaderboard::AccessMode accessmode, Leaderboard::ReferenceRank referenceRank, int32_t rangeBegin, int32_t rangeEnd) override;

        /////////// STATS        

        virtual Threading::Task<bool> ingestStatTask(const char *name, const char *leaderboardName, int32_t value) override;
        
        /////////// ACHIEVEMENTS

        virtual Threading::Task<bool> unlockAchievementTask(const char *name) override;

        /////////// MATCHMAKING

        Threading::Task<std::vector<Lobby>> getLobbyListTask() override;
        Threading::Task<std::optional<Lobby>> createLobbyTask(size_t maxPlayerCount, MatchmakingCallback cb, SessionStartedCallback sessionCb, std::map<std::string, std::string> properties) override;
        Threading::Task<std::optional<Lobby>> joinLobbyTask(uint64_t id, MatchmakingCallback cb, SessionStartedCallback sessionCb) override;
        Threading::Task<std::optional<ServerInfo>> startMatchTask() override;
        void leaveLobby() override;
        void leaveMatch() override;
        bool isLobbyOwner() const override;

        void setLobbyProperty(std::string_view key, std::string_view value) override;

        MatchmakingCallback mCurrentMatchmakingCallback;
        SessionStartedCallback mSessionStartedCallback;


        //////////////////        
    };

}
}