Project(GooglePlayServices)

add_subdirectory(shared)

if (NOT TARGET PlayServices)
	set(stub STUB)
endif()



MESSAGE(STATUS "--- Google Play ---")

if (NOT EXISTS ${MADGINE_CONFIGURATION}/res/values/games-ids.xml)
	MESSAGE(STATUS "No games-ids.xml provided. Plugin disabled. Add res/values/games-ids.xml to your MADGINE_CONFIGURATION location.")
	set(stub STUB)
endif ()

#MESSAGE(STATUS "Creating steam_appid.txt for Debug build.")
#file(WRITE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/steam_appid.txt ${Steam_AppId})

#MESSAGE(STATUS "AppId: ${Steam_AppId}")

MESSAGE(STATUS "--- Google Play End ---")

set(sources "")
FolderSources(sources "" src)
FolderSources(sources src Madgine)
	FolderSources(sources Madgine firstparty googleplayservices.cpp googleplayservices.h)

add_plugin(GooglePlayServices Madgine FirstParty ${sources} PRECOMPILED_HEADER src/Madgine/googleplayserviceslib.h ${stub}
	UNIQUE_COMPONENTS
		VIRTUAL Engine::FirstParty::FirstPartyServices Engine::Root::RootComponent Engine::FirstParty::GooglePlayServices)

target_link_plugins(GooglePlayServices FirstParty)

if (TARGET PlayServices)
	target_link_libraries(GooglePlayServices PRIVATE PlayServices)
endif()


