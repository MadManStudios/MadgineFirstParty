#pragma once

#include "Generic/closure.h"

namespace Engine {
namespace FirstParty {

	struct PlayerInfo;

	struct ServerInfo;

	struct LobbyInfo;

	struct Lobby;

	using MatchmakingCallback = Closure<Serialize::Format(Serialize::SyncManager&)>;
	using SessionStartedCallback = Closure<void(LobbyInfo)>;
}
}
