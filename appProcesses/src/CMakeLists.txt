cmake_minimum_required(VERSION 3.19)
project(appProcesses LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Charts)

qt_standard_project_setup()

qt_add_executable(appProcesses
    WIN32 MACOSX_BUNDLE
    main.cpp
    MainWindow.cpp MainWindow.h MainWindow.ui
    process/ProcessInformationHandler.cpp process/ProcessInformationHandler.h
    process/ProcessInfo.h
    process/Logger.h process/Logger.cpp
    process/ProcessModel.h process/ProcessModel.cpp
)

target_link_libraries(appProcesses
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Charts
        psapi
)

include(GNUInstallDirs)

install(TARGETS appProcesses
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET appProcesses
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
