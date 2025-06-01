include("D:/Source/My/ProcessInfoView-/appProcesses/build/.qt/QtDeploySupport-RelWithDebInfo.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/appProcesses-plugins-RelWithDebInfo.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE D:/Source/My/ProcessInfoView-/appProcesses/build/RelWithDebInfo/appProcesses.exe
    GENERATE_QT_CONF
)
