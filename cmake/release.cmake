INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_GENERATOR TGZ)

set(CPACK_PACKAGE_NAME "iroha")
set(CPACK_PACKAGE_VENDOR "Soramitsu LLC")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Iroha - simple decentralized blockchain")

SET(CPACK_PACKAGE_VENDOR              "Soramitsu LLC")
SET(CPACK_RESOURCE_FILE_LICENSE       "${CMAKE_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_CONTACT             "Bogdan Vaneev <bogdan@soramitsu.co.jp>")

# change build type
SET(CMAKE_BUILD_TYPE Release)

if(NOT IROHA_VERSION)
  message(WARNING  "IROHA_VERSION is not specified, using commit hash as version")
  get_git_revision(GIT_SHA1)
  remove_line_terminators(${GIT_SHA1} GIT_SHA1)
  set(IROHA_VERSION "0x${GIT_SHA1}")
endif()

SET(CPACK_PACKAGE_VERSION ${IROHA_VERSION})
message(STATUS "[IROHA_VERSION] '${IROHA_VERSION}'")


SET(CPACK_STRIP_FILES TRUE)

set(CPACK_COMPONENTS_ALL binaries libraries)

if(WIN32)
  # cmake is running on windows
elseif (APPLE)
  # cmake is running on mac os
  include(cmake/release/osx-bundle.cmake)
elseif(UNIX)
  # cmake is running on unix

  if(NOT NO_DEB)
    include(cmake/release/linux-deb.cmake)
  endif()

  if(NOT NO_RPM)
    include(cmake/release/linux-rpm.cmake)
  endif()

endif()

INCLUDE(CPack)
