cmake_minimum_required(VERSION 3.25)

project(MControlCenter VERSION 0.5.2 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS
    Widgets
    LinguistTools
    DBus
)

qt_standard_project_setup()

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.ui
    src/mainwindow.cpp src/mainwindow.h
    src/operate.cpp src/operate.h
    src/powermonitor.cpp src/powermonitor.h
    src/gamemonitor.cpp src/gamemonitor.h
    src/helper.cpp src/helper.h
    src/msi-ec_helper.cpp src/msi-ec_helper.h
    src/settings.cpp src/settings.h
    src/resources.qrc
)

# src/resources.qrc embeds the .qm files next to the .ts sources, so generate
# them in-tree at build time. qt_add_translations() would put them in the build
# directory and leave the qrc references unresolved.
file(GLOB TS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/i18n/*.ts")
set(QM_FILES "")
foreach(ts_file IN LISTS TS_FILES)
    string(REPLACE ".ts" ".qm" qm_file "${ts_file}")
    list(APPEND QM_FILES "${qm_file}")
    add_custom_command(
        OUTPUT "${qm_file}"
        COMMAND Qt6::lrelease "${ts_file}" -qm "${qm_file}"
        DEPENDS "${ts_file}"
        VERBATIM
    )
endforeach()
add_custom_target(translations ALL DEPENDS ${QM_FILES})
set_source_files_properties(src/resources.qrc PROPERTIES OBJECT_DEPENDS "${QM_FILES}")

qt_add_executable(mcontrolcenter
    ${PROJECT_SOURCES}
)

add_subdirectory(src/helper ${CMAKE_BINARY_DIR}/helper)

target_link_libraries(mcontrolcenter PRIVATE
    Qt6::Widgets
    Qt6::DBus
)

target_compile_definitions(mcontrolcenter PRIVATE
    MControlCenter_VERSION="${PROJECT_VERSION}")
