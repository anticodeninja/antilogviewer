file(GLOB SOURCE_FILES
        "*.cpp"
        "chain_elements/*.cpp"
        "dialogs/*.cpp")

qt_add_resources(RESOURCE_FILES resources.qrc)
if (WIN32)
    list(APPEND RESOURCE_FILES resources.rc)
endif()

add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES} ${RESOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE .)
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})
copy_qt_dependencies(${PROJECT_NAME})