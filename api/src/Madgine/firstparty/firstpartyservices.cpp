#include "../firstpartylib.h"

#include "firstpartyservices.h"

#include "Madgine/root/root.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Modules/threading/awaitables/awaitablesender.h"

METATABLE_BEGIN(Engine::FirstParty::FirstPartyServices)
READONLY_PROPERTY(Initialized, mInitialized)
METATABLE_END(Engine::FirstParty::FirstPartyServices)

VIRTUALUNIQUECOMPONENTBASE(Engine::FirstParty::FirstPartyServices)

namespace Engine {
	namespace FirstParty {

		FirstPartyServices::FirstPartyServices(Root::Root& root)
			: VirtualRootComponentBase(root)
		{
		}

		bool FirstPartyServices::isConnected()
		{
			return is_instantiated() && getSingleton().mInitialized;
		}

		Threading::TaskFuture<Leaderboard> FirstPartyServices::getLeaderboard(const char* name, Leaderboard::AccessMode accessmode, Leaderboard::ReferenceRank referenceRank, int32_t rangeBegin, int32_t rangeEnd)
		{
			return mRoot.taskQueue()->queueTask(getLeaderboardTask(name, accessmode, referenceRank, rangeBegin, rangeEnd));
		}

		Threading::TaskFuture<bool> FirstPartyServices::ingestStat(const char* name, const char* leaderboardName, int32_t value)
		{
			return mRoot.taskQueue()->queueTask(ingestStatTask(name, leaderboardName, value));
		}

		Threading::TaskFuture<bool> FirstPartyServices::unlockAchievement(const char* name)
		{
			return mRoot.taskQueue()->queueTask(unlockAchievementTask(name));
		}

		Threading::TaskFuture<std::vector<Lobby>> FirstPartyServices::getLobbyList(std::map<std::string, std::string> filters)
		{
			return mRoot.taskQueue()->queueTask(getLobbyListTask(std::move(filters)));
		}

		Threading::TaskFuture<std::optional<LobbyInfo>> FirstPartyServices::createLobby(size_t maxPlayerCount, MatchmakingCallback cb, SessionStartedCallback sessionCb, std::map<std::string, std::string> properties)
		{
			mCurrentMatchmakingCallback = std::move(cb);
			mSessionStartedCallback = std::move(sessionCb);

			return mRoot.taskQueue()->queueTask(createLobbyTask(maxPlayerCount, std::move(properties)));
		}

		Threading::TaskFuture<std::optional<LobbyInfo>> FirstPartyServices::joinLobby(uint64_t id, MatchmakingCallback cb, SessionStartedCallback sessionCb)
		{
			mCurrentMatchmakingCallback = std::move(cb);
			mSessionStartedCallback = std::move(sessionCb);

			return mRoot.taskQueue()->queueTask(joinLobbyTask(id));
		}

		Threading::TaskFuture<std::optional<ServerInfo>> FirstPartyServices::startMatch()
		{
			return mRoot.taskQueue()->queueTask(startMatchTask());
		}

		Threading::TaskFuture<std::variant<std::monostate, ServerInfo, LobbyInfo>> FirstPartyServices::startMatchmaking(size_t playerCount, MatchmakingCallback matchmakingCb, SessionStartedCallback sessionStartedCb, std::map<std::string, std::string> properties)
		{
			mCurrentMatchmakingCallback = std::move(matchmakingCb);
			mSessionStartedCallback = std::move(sessionStartedCb);

			return mRoot.taskQueue()->queueTask(startMatchmakingTask(playerCount, std::move(properties)));
		}

		Threading::Task<std::variant<std::monostate, ServerInfo, LobbyInfo>> FirstPartyServices::startMatchmakingTask(size_t playerCount, std::map<std::string, std::string> properties)
		{
			properties["_matchmaking"] = "true";

			bool found = false;
			LobbyInfo lobby;

			std::vector<Lobby> lobbies = co_await getLobbyListTask(properties);
			for (Lobby& candidate : lobbies) {
				std::optional<LobbyInfo> info = co_await joinLobbyTask(candidate.mId);
				if (info) {
					lobby = std::move(*info);
					found = true;
					break;
				}
			}

			if (!found) {
				std::optional<LobbyInfo> info = co_await createLobbyTask(playerCount, properties);
				if (!info)
					co_return{};
				lobby = std::move(*info);
			}

			while (lobby.mPlayers.size() < playerCount) {
				std::optional<LobbyInfo> info = (co_await mLobbyInfo.sender(lobby)).value();
				if (!info)
					co_return {};
				lobby = std::move(*info);
			}

			if (lobby.mIsOwner) {
				std::optional<ServerInfo> server = co_await startMatchTask();
				if (server)
					co_return std::move(*server);
				else
					co_return{};
			}
			else {
				co_return std::move(lobby);
			}
		}

		void FirstPartyServices::cancelMatchmaking()
		{
			leaveLobby();
		}

		Execution::ValueStub<std::optional<LobbyInfo>>& FirstPartyServices::lobbyInfo()
		{
			return mLobbyInfo;
		}

	}
}
