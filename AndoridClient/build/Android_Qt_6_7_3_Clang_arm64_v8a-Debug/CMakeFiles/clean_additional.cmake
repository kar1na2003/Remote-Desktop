# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "AndoridClient_autogen"
  "CMakeFiles\\AndoridClient_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\AndoridClient_autogen.dir\\ParseCache.txt"
  )
endif()
