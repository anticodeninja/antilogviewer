list(APPEND REQUIRED_LIBS_QUALIFIED Qt5::Test)
list(APPEND REQUIRED_LIBS Test)
find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)

file(GLOB SOURCE_FILES
        "*.cpp"
        "../src/*.cpp"
        "../src/chain_elements/*.cpp"
        "../src/dialogs/*.cpp")
list(FILTER SOURCE_FILES EXCLUDE REGEX ../src/main.cpp)

add_executable(${PROJECT_NAME}_tests ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME}_tests PRIVATE . ../src)
target_link_libraries(${PROJECT_NAME}_tests ${REQUIRED_LIBS_QUALIFIED})
copy_qt_dependencies(${PROJECT_NAME}_tests)