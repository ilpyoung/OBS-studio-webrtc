
if(APPLE AND NOT CPACK_GENERATOR)
	set(CPACK_GENERATOR "Bundle")
elseif(WIN32 AND NOT CPACK_GENERATOR)
	set(CPACK_GENERATOR "WIX" "ZIP")
endif()

set(CPACK_PACKAGE_NAME "RemoteFilming-B")
set(CPACK_PACKAGE_VENDOR "remotefilming.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Remote Filming - Live video and audio streaming and recording software")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/UI/data/license/gplv2.txt")

set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

if(NOT DEFINED OBS_VERSION_OVERRIDE)
	if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
		execute_process(COMMAND git describe --always --tags --dirty=-modified
			OUTPUT_VARIABLE OBS_VERSION
			WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
			OUTPUT_STRIP_TRAILING_WHITESPACE)
	else()
		set(OBS_VERSION "${CPACK_PACKAGE_VERSION}")
	endif()
else()
	set(OBS_VERSION "${OBS_VERSION_OVERRIDE}")
endif()

if("${OBS_VERSION}" STREQUAL "")
	message(FATAL_ERROR "Failed to configure OBS_VERSION. Either set OBS_VERSION_OVERRIDE or ensure `git describe` succeeds.")
endif()
MESSAGE(STATUS "OBS_VERSION: ${OBS_VERSION}")

if(INSTALLER_RUN)
	set(CPACK_PACKAGE_EXECUTABLES
		"rfs32" "Remote Filming-B (32bit)"
		"rfs64" "Remote Filming-B (64bit)")
	set(CPACK_CREATE_DESKTOP_LINKS
		"rfs32"
		"rfs64")
else()
	if(WIN32)
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(_output_suffix "64")
		else()
			set(_output_suffix "32")
		endif()
	else()
		set(_output_suffix "")
	endif()

	set(CPACK_PACKAGE_EXECUTABLES "rfs${_output_suffix}" "Remote Filming-B")
	set(CPACK_CREATE_DESKTOP_LINKS "rfs${_output_suffix}")
endif()

set(CPACK_BUNDLE_NAME "RemoteFilming-B")
set(CPACK_BUNDLE_PLIST "${CMAKE_SOURCE_DIR}/cmake/osxbundle/Info.plist")
set(CPACK_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/cmake/osxbundle/obs.icns")
set(CPACK_BUNDLE_STARTUP_COMMAND "${CMAKE_SOURCE_DIR}/cmake/osxbundle/obslaunch.sh")

set(CPACK_WIX_TEMPLATE "${CMAKE_SOURCE_DIR}/cmake/Modules/WIX.template.in")

if(INSTALLER_RUN)
	set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "RemoteFilming-B")
	set(CPACK_WIX_UPGRADE_GUID "9ebf7b9c-a1fd-4017-aee8-7eecdff54f1a")
	set(CPACK_WIX_PRODUCT_GUID "2179a775-abc9-4f4c-8070-582f164a1207")
	set(CPACK_PACKAGE_FILE_NAME "remote-filming-B-${OBS_VERSION}")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
	if(WIN32)
		set(CPACK_PACKAGE_NAME "Remote Filming-B (64bit)")
	endif()
	set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "RemoteFilming-B64")
	set(CPACK_WIX_UPGRADE_GUID "52f6753f-1d82-4397-a539-84d01136f781")
	set(CPACK_WIX_PRODUCT_GUID "3ff3c0d3-e654-4176-9204-e29cc815f3f9")
	set(CPACK_PACKAGE_FILE_NAME "remote-filming-B-x64-${OBS_VERSION}")
else()
	if(WIN32)
		set(CPACK_PACKAGE_NAME "Remote Filming-B (32bit)")
	endif()
	set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "RemoteFilming-B32")
	set(CPACK_WIX_UPGRADE_GUID "f33edcaf-609a-4505-a23f-b19ac9504fa7")
	set(CPACK_WIX_PRODUCT_GUID "b72be7ed-96e4-4d16-87f6-9c6af6c97c16")
	set(CPACK_PACKAGE_FILE_NAME "remote-filming-B-x86-${OBS_VERSION}")
endif()

set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")

if(UNIX_STRUCTURE)
	set(CPACK_SET_DESTDIR TRUE)
endif()

include(CPack)
