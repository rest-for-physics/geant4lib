if (${REST_DECAY0} MATCHES "ON")
    add_definitions(-DUSE_Decay0)
    list( APPEND rest_features "Decay0" )

    # TODO Issue #6 at rest-framework
    #----------------------------------------------------------------------------
    # Find package Decay0 and ROOT
    find_package(BxDecay0 1.0 CONFIG COMPONENTS manager REQUIRED)
    #
    # find_package was not working properly for me in 1.0.9. So I was using 
    # bxdecay0-config. But in case Decay0 flag is enabled and we do not
    # have a bxdecay0 installation we might run into problems
    #

    #execute_process(
    # COMMAND bxdecay0-config --version
    # OUTPUT_VARIABLE BxDecay0_VERSION
    # OUTPUT_STRIP_TRAILING_WHITESPACE)
    #
	#execute_process(
    #COMMAND bxdecay0-config --incdir 
    # OUTPUT_VARIABLE BxDecay0_INCLUDE_DIRS
    #   OUTPUT_STRIP_TRAILING_WHITESPACE)
  #
	#execute_process(
    #COMMAND bxdecay0-config --libdir
    # OUTPUT_VARIABLE BxDecay0_LIBRARY_DIRS
    # OUTPUT_STRIP_TRAILING_WHITESPACE)

    message (STATUS "Found BxDecay0. Version ='${BxDecay0_VERSION}'")
    message (STATUS "Found BxDecay0. Include path ='${BxDecay0_INCLUDE_DIRS}'")
    message (STATUS "Found BxDecay0. Library path ='${BxDecay0_LIBRARY_DIRS}'")

    target_link_libraries( RestGeant4 -lBxDecay0 ) 
    include_directories(${BxDecay0_INCLUDE_DIRS})
    set(feature_added "Decay0")
    set(feature_added ${feature_added} PARENT_SCOPE)
else ()
    set(REST_DECAY0 OFF)
    set(excludes ${excludes} TRestGeant4ParticleCollectionDecay0 )
endif (${REST_DECAY0} MATCHES "ON")

if (NOT ${REST_EVE} MATCHES "ON")
    set(excludes ${excludes} TRestGeant4EventViewer )
endif ()

COMPILELIB("")

file(GLOB_RECURSE MAC "${CMAKE_CURRENT_SOURCE_DIR}/macros/*")
INSTALL(FILES ${MAC} DESTINATION ./macros/geant4)