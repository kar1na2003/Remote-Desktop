# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\ClientDesktop_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ClientDesktop_autogen.dir\\ParseCache.txt"
  "ClientDesktop_autogen"
  )
endif()
