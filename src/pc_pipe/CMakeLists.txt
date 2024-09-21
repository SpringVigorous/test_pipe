find_package(Boost REQUIRED COMPONENTS asio)


file(GLOB sources CONFIGURE_DEPENDS "src/*.cpp")
foreach(source IN LISTS sources)
    get_filename_component(name ${source} NAME_WE)
    add_executable(${name} ${source})
    # 如果需要对特定目标文件设置不同的编译选项
    # target_compile_options(${name} PRIVATE /Wp,return-type)
    target_link_libraries(${name} PRIVATE Boost::asio)

    set_target_properties(${name} PROPERTIES DEBUG_POSTFIX _d)
    target_include_directories(${name} PRIVATE ./include)
    # target_include_directories(${name} PRIVATE /usr/include/python3.12)
    # find_package(pybind11 REQUIRED)
    # target_link_libraries(${name} PRIVATE pybind11)
endforeach()