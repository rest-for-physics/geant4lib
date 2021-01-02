set(contents)

if (NOT DEFINED RESTLIB_DETECTOR OR NOT ${RESTLIB_DETECTOR} MATCHES "ON")
	set(excludes TRestGeant4ToHitsProcess)
else()
	target_link_libraries( RestDetector)
endif (NOT DEFINED RESTLIB_DETECTOR OR NOT ${RESTLIB_DETECTOR} MATCHES "ON")

COMPILEDIR(RestGeant4)

