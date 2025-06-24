#include "../googleplayserviceslib.h"

#include "googleplayservices.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalue/metatable_impl.h"

//#include <steam/steam_api.h>

//#include "steam_sender.h"

#include "Modules/threading/awaitables/awaitabletimepoint.h"

#include "Madgine/root/root.h"

#include "Modules/threading/awaitables/awaitablesender.h"

#include "playservices.h"

#include "Generic/execution/execution.h"

METATABLE_BEGIN_BASE(Engine::FirstParty::GooglePlayServices, Engine::FirstParty::FirstPartyServices)
METATABLE_END(Engine::FirstParty::GooglePlayServices)

UNIQUECOMPONENT(Engine::FirstParty::GooglePlayServices)

namespace Engine {
namespace FirstParty {

    GooglePlayServices::GooglePlayServices(Root::Root &root)
        : FirstPartyServicesImpl<GooglePlayServices>(root)
    {
        PlayServices::setup();        
        mInitialized = true;
    }

    GooglePlayServices::~GooglePlayServices()
    {
    }

    std::string_view GooglePlayServices::key() const
    {
        return "GooglePlayServices";
    }

    std::string GooglePlayServices::currentUserName() const
    {
        throw 0;
    }

    Threading::Task<Leaderboard> GooglePlayServices::getLeaderboardTask(const char *name, Leaderboard::AccessMode accessmode, Leaderboard::ReferenceRank referenceRank, int32_t rangeBegin, int32_t rangeEnd)
    {
        PlayServices::Leaderboards::Scores scores = (co_await PlayServices::Leaderboards::getLeaderboard(name, accessmode, referenceRank, rangeBegin, rangeEnd)).value();
        
        //if (!errorMsg.empty()) {
        //    LOG_ERROR("Error fetching Leaderboard " << name << ": \n "
        //                                     << errorMsg);            
        //}
        Leaderboard transferredScores;

        std::ranges::transform(scores.mScores, std::back_inserter(transferredScores.mEntries), [](const PlayServices::Leaderboards::Scores::Score &score) {
            return Leaderboard::Entry {
                score.mUserId,
                score.mDisplayName,
                score.mRank,
                score.mScore
            };
        });

        co_return transferredScores;        
    }

    Threading::Task<bool> GooglePlayServices::ingestStatTask(const char *name, const char *leaderboardName, int32_t value)
    {
        (co_await PlayServices::Leaderboards::submitScore(leaderboardName, value, name)).value(); 
        //if (result) {
        //    LOG_ERROR("Error updating Stat " << name << " in Leaderboard " << leaderboardName << ": \n "
        //        << *result);
        //    co_return false;
        //} 
        co_return true;
    }

    Threading::Task<bool> GooglePlayServices::unlockAchievementTask(const char *name)
    {
        (co_await PlayServices::Achievements::unlock(name)).value();
        //if (result) {
        //    LOG_ERROR("Error unlocking Achievement: \n" << *result);
        //    co_return false;
        //} 
        co_return true;
    }

    Threading::Task<std::vector<Lobby>> GooglePlayServices::getLobbyListTask()
    {
        throw 0;
    }

    Threading::Task<std::optional<Lobby>> GooglePlayServices::createLobbyTask(size_t maxPlayerCount, MatchmakingCallback cb, SessionStartedCallback sessionCb, std::map<std::string, std::string> properties)
    {
        throw 0;
    }

    Threading::Task<std::optional<Lobby>> GooglePlayServices::joinLobbyTask(uint64_t id, MatchmakingCallback cb, SessionStartedCallback sessionCb)
    {
        throw 0;
    }

    Threading::Task<std::optional<ServerInfo>> GooglePlayServices::startMatchTask()
    {
        throw 0;
    }

    void GooglePlayServices::leaveLobby()
    {
        throw 0;
    }

    void GooglePlayServices::leaveMatch()
    {
        throw 0;
    }

    bool GooglePlayServices::isLobbyOwner() const
    {
        throw 0;
    }

    void GooglePlayServices::setLobbyProperty(std::string_view key, std::string_view value)
    {
        throw 0;
    }

}
}
