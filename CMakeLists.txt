set(contents)

set (excludes)

if (NOT DEFINED REST_DECAY0 OR NOT ${REST_DECAY0} MATCHES "ON")
    set(excludes ${excludes} TRestGeant4ParticleCollectionDecay0 )
elseif()
    target_link_libraries( RestGeant4 -lBxDecay0 ) 
    include_directories(${BxDecay0_INCLUDE_DIRS})
endif (NOT DEFINED REST_DECAY0 OR NOT ${REST_DECAY0} MATCHES "ON")

COMPILEDIR(RestGeant4)
