Project(SteamServices)

add_subdirectory(shared)

if (NOT TARGET Steamworks)
	set(stub STUB)
endif()



MESSAGE(STATUS "--- Steam ---")

if (NOT Steam_AppId)
	MESSAGE(STATUS "Using default AppId. Set 'Steam_AppId' in your toplevel CMakeLists file.")
	set (Steam_AppId 480)
endif ()

MESSAGE(STATUS "Creating steam_appid.txt for Debug build.")
file(WRITE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/steam_appid.txt ${Steam_AppId})

MESSAGE(STATUS "AppId: ${Steam_AppId}")

MESSAGE(STATUS "--- Steam End ---")




set(sources "")
FolderSources(sources "" src)
FolderSources(sources src Madgine)
	FolderSources(sources Madgine firstparty steamservices.cpp steamservices.h steamstreambuf.cpp steamstreambuf.h steamstreamdata.cpp steamstreamdata.h steamsyncmanager.cpp steamsyncmanager.h)

if (TARGET Steamworks)
	set(deps IMPORTED_DEPS Steamworks)
endif()

add_plugin(SteamServices Madgine FirstParty ${sources} PRECOMPILED_HEADER src/Madgine/steamserviceslib.h ${stub} ${deps}) 

target_link_plugins(SteamServices FirstParty)

if (TARGET Steamworks)
	target_link_libraries(SteamServices PUBLIC Steamworks)
endif()

target_compile_definitions(SteamServices PRIVATE STEAM_APP_ID=${Steam_AppId})


if (MADGINE_CONFIGURATION)
	get_target_property(exclude SteamServices EXCLUDE_FROM_ALL)
	if (NOT exclude)
		
		if (NOT Steam_DepotId)
			MESSAGE(WARNING "Steam packaging script needs 'Steam_DepotId' set to work. Skipping Steam packaging!")
		else()
			set_custom_packaging(cmake/cpack/steam.cmake)
		endif()
	endif()
endif()