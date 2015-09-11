## Main cmake configuration file

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
## Set the base name of the output for this project
SET(OUTPUT_NAME sol)
PROJECT(${OUTPUT_NAME})

SET(MAJOR_VERSION 2)
SET(MINOR_VERSION 0)
SET(PATCH_VERSION 0)

# CPack configuration
SET(CPACK_PACKAGE_VENDOR "BigBalloon Productions Inc.")
SET(CPACK_PACKAGE_CONTACT "cbrucks@gmail.com")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Chris brucks") #required
SET(CPACK_DEBIAN_PACKAGE_NAME "${OUTPUT_NAME}")
SET(CPACK_PACKAGE_DESCRIPTION "Shutdown On LAN (SOL)")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Provides Shutdown on LAN (SOL) capability utilizing the magic packet format used for Wake On LAN (WOL).")
SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
SET(CPACK_SET_DESTDIR "on")
if(WIN32)

elseif(UNIX)
    ## Build a deb install package 
    SET(CPACK_GENERATOR "DEB")
    
    # set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr" CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
    set(CPACK_INSTALL_PREFIX "/usr" CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
    
    ## Include the Debian configuration scripts
    SET(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/debian/postrm;")
    
    ## Include the initsystem script to start the daemon automatically with certain run levels
    INSTALL(FILES "debian/${OUTPUT_NAME}" DESTINATION "/etc/init.d/" COMPONENT config)
    
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
    SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
endif()
INCLUDE(CPack)

## Add Source files
FILE(GLOB SRC *.c *.h)

## Add the executable build to the project
add_executable(${OUTPUT_NAME} ${SRC}) 

install(TARGETS ${OUTPUT_NAME} DESTINATION bin 
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)