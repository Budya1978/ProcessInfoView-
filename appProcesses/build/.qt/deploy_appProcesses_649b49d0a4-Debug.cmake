include("D:/Source/My/ProcessInfoView-/appProcesses/build/.qt/QtDeploySupport-Debug.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/appProcesses-plugins-Debug.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE D:/Source/My/ProcessInfoView-/appProcesses/build/Debug/appProcesses.exe
    GENERATE_QT_CONF
)
