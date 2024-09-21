

set(PROCESS_TOOLS process_tools)

FILE(GLOB_RECURSE process_tools_hds include/${CURDLL}/*.*)
FILE(GLOB_RECURSE process_tools_srcs src/*.*)

set(process_tools_all ${process_tools_hds} ${process_tools_srcs})


find_package(Boost REQUIRED COMPONENTS filesystem process locale)

add_library(${PROCESS_TOOLS}  SHARED ${process_tools_all})
target_compile_definitions(${PROCESS_TOOLS} PRIVATE __PPROCESS_TOOLS__)
set_target_properties(${PROCESS_TOOLS} PROPERTIES DEBUG_POSTFIX _d)


target_link_libraries(${PROCESS_TOOLS} PRIVATE Boost::process  Boost::filesystem Boost::locale)
target_include_directories(${PROCESS_TOOLS} PUBLIC include )




