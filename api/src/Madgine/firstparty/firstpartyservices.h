#pragma once

#include "Madgine/root/rootcomponentcollector.h"

#include "Madgine/root/rootcomponentbase.h"

#include "Modules/threading/taskfuture.h"

#include "Generic/closure.h"

#include "Generic/execution/value.h"

namespace Engine {
namespace FirstParty {

    struct Leaderboard {

        enum ReferenceRank {
            USER,
            FIRST
        };

        enum AccessMode {
            FRIENDS,
            GLOBAL
        };

        struct Entry {
            uintptr_t mUserId;
            std::string mDisplayName;
            size_t mRank;
            int32_t mScore;
        };

        std::vector<Entry> mEntries;
        size_t mTotalEntries;
    };

    struct Lobby {
        uint64_t mId;
        std::map<std::string, std::string> mProperties;
    };

    struct PlayerInfo {
        std::string mName;

        bool operator==(const PlayerInfo& other) const = default;
    };

    struct LobbyInfo {
        bool mIsOwner;
        std::vector<PlayerInfo> mPlayers;
        std::map<std::string, std::string> mProperties;

        bool operator==(const LobbyInfo& other) const = default;
    };

    struct ServerInfo {
        LobbyInfo mSession;
        std::vector<Serialize::ParticipantId> mIds;
    };


    struct MADGINE_FIRST_PARTY_EXPORT FirstPartyServices : Root::VirtualRootComponentBase<FirstPartyServices> {

        FirstPartyServices(Root::Root &root);

        static bool isConnected();

        ///////// IDENTITY

        virtual std::string currentUserName() const = 0;

        ///////// LEADERBOARD

        Threading::TaskFuture<Leaderboard> getLeaderboard(const char *name, Leaderboard::AccessMode accessmode = Leaderboard::GLOBAL, Leaderboard::ReferenceRank referenceRank = Leaderboard::FIRST, int32_t rangeBegin = 0, int32_t rangeEnd = 99);
        virtual Threading::Task<Leaderboard> getLeaderboardTask(const char *name, Leaderboard::AccessMode accessmode = Leaderboard::GLOBAL, Leaderboard::ReferenceRank referenceRank = Leaderboard::FIRST, int32_t rangeBegin = 0, int32_t rangeEnd = 99) = 0;

        ///////// STATS

        Threading::TaskFuture<bool> ingestStat(const char *name, const char *leaderboardName, int32_t value);
        virtual Threading::Task<bool> ingestStatTask(const char *name, const char *leaderboardName, int32_t value) = 0;

        ///////// ACHIEVEMENTS

        Threading::TaskFuture<bool> unlockAchievement(const char *name);
        virtual Threading::Task<bool> unlockAchievementTask(const char *name) = 0;

        ///////// MATCHMAKING

        using MatchmakingCallback = Closure<Serialize::Format(Serialize::SyncManager &)>;
        using SessionStartedCallback = Closure<void(LobbyInfo)>;

        Threading::TaskFuture<std::vector<Lobby>> getLobbyList(std::map<std::string, std::string> filters = {});
        virtual Threading::Task<std::vector<Lobby>> getLobbyListTask(std::map<std::string, std::string> filters = {}) = 0;

        Threading::TaskFuture<std::optional<LobbyInfo>> createLobby(size_t maxPlayerCount, MatchmakingCallback cb, SessionStartedCallback sessionCb = {}, std::map<std::string, std::string> properties = {});
        virtual Threading::Task<std::optional<LobbyInfo>> createLobbyTask(size_t maxPlayerCount, std::map<std::string, std::string> properties = {}) = 0;

        Threading::TaskFuture<std::optional<LobbyInfo>> joinLobby(uint64_t id, MatchmakingCallback cb, SessionStartedCallback sessionCb);
        virtual Threading::Task<std::optional<LobbyInfo>> joinLobbyTask(uint64_t id) = 0;

        Threading::TaskFuture<std::optional<ServerInfo>> startMatch();
        virtual Threading::Task<std::optional<ServerInfo>> startMatchTask() = 0;

		Threading::TaskFuture<std::variant<std::monostate, ServerInfo, LobbyInfo>> startMatchmaking(size_t playerCount, MatchmakingCallback matchmakingCb, SessionStartedCallback sessionStartedCb, std::map<std::string, std::string> properties);
		Threading::Task<std::variant<std::monostate, ServerInfo, LobbyInfo>> startMatchmakingTask(size_t playerCount, std::map<std::string, std::string> properties);
        void cancelMatchmaking();

        virtual void setLobbyProperty(std::string_view key, std::string_view value) = 0;

        Execution::ValueStub<std::optional<LobbyInfo>> &lobbyInfo();
        Execution::Value<std::optional<LobbyInfo>> mLobbyInfo;

        virtual void leaveLobby() = 0;
        virtual void leaveMatch() = 0;



        MatchmakingCallback mCurrentMatchmakingCallback;
        SessionStartedCallback mSessionStartedCallback;


        bool mInitialized = false;
    };

    template <typename T>
    using FirstPartyServicesImpl = VirtualScope<T, Root::RootComponentVirtualImpl<T, FirstPartyServices>>;

}
}