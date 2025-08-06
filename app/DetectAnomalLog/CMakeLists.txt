cmake_minimum_required(VERSION 3.16)

project(DetectAnomalLog VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appDetectAnomalLog
    main.cpp
)

qt_add_qml_module(appDetectAnomalLog
    URI DetectAnomalLog
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES PCA.h PCA.cpp
        SOURCES ParseLog.h ParseLog.cpp
        SOURCES LogAnalyzer.h LogAnalyzer.cpp
        SOURCES FileUtils.h FileUtils.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appDetectAnomalLog PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appDetectAnomalLog
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(appDetectAnomalLog PRIVATE /usr/include/eigen3)

target_link_libraries(appDetectAnomalLog
    PRIVATE Qt6::Core
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appDetectAnomalLog
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
