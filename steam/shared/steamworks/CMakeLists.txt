Project(Steamworks)

set(Steamworks_Dir $ENV{STEAMWORKS_SDK} CACHE FILEPATH "")

if (Steamworks_Dir)

	if (NOT EXISTS ${Steamworks_Dir})
		MESSAGE(WARNING "Provided Steamworks SDK directory does not exist: ${Steamworks_Dir}")
		return()
	endif()


	add_library(Steamworks SHARED IMPORTED GLOBAL)

	set_target_properties(Steamworks PROPERTIES
		IMPORTED_LOCATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/steam_api64.dll
		IMPORTED_IMPLIB ${Steamworks_Dir}/redistributable_bin/win64/steam_api64.lib)

	target_include_directories(Steamworks INTERFACE ${Steamworks_Dir}/public)


	add_custom_target(SteamworksCopy ${CMAKE_COMMAND} -E copy_if_different
			${Steamworks_Dir}/redistributable_bin/win64/steam_api64.dll
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

	add_dependencies(Steamworks SteamworksCopy)

else()
	MESSAGE(STATUS "No Steamworks SDK directory provided. Set 'Steamworks_Dir' in your toplevel CMakeLists.txt file or the environment variable 'STEAMWORKS_SDK' to enable Steamworks support.")

endif()