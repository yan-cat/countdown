cmake_minimum_required(VERSION 3.16)
project(Countdown VERSION 1.2.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(BINARY_ICONS_RESOURCE ON)

# 关键：定义 KF6 版本
set(KF6_MIN_VERSION 6.12.0)

set(QML_IMPORT_PATH "/usr/lib/qt6/qml;/usr/lib/qml" CACHE STRING "QML Import Paths")

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

# Android APK 打包配置（基于 ECM / androiddeployqt）
include(ECMAddAndroidApk)
ecm_add_android_apk(Countdown ANDROID_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android)
set(ANDROID_APK_DIR "${CMAKE_SOURCE_DIR}/android")

# 查找 Qt6 和 Kirigami
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Quick
    QuickControls2
    LinguistTools
)
if (NOT ANDROID)
    list(APPEND QT_REQUIRED_COMPONENTS DBus)
endif()

find_package(KF6 REQUIRED COMPONENTS
    Kirigami
    CoreAddons
    Archive
    IconThemes
)

qt_policy(SET QTP0001 NEW)

# 添加可执行文件
qt6_add_executable(Countdown
    src/main.cpp
    src/manager.cpp
    src/countdowndata.cpp
    src/datediff.cpp
    src/debug.cpp
    src/updater.cpp

    src/include/manager.h
    src/include/updater.h
    src/include/countdowndata.h

)
# 桌面/非安卓才编译的提醒实现（依赖 D-Bus、桌面弹窗；安卓不编译）
if (NOT ANDROID)
    target_sources(Countdown PRIVATE src/reminder.cpp)
endif()

# Qt 部署用的 QML 搜索路径 target 属性；在 craft 的 ecm_add_android_apk 流程里并不生效，
# 真正让 KDE QML 模块进包的是下方 POST_BUILD 复制那步。保留无害，可留可删。
if (ANDROID)
    set_target_properties(Countdown PROPERTIES
        QT_QML_IMPORT_PATH "${CMAKE_INSTALL_PREFIX}/lib/qml;${CMAKE_INSTALL_PREFIX}/qml"
    )
endif()

target_include_directories(Countdown PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/include)

# 链接库
target_link_libraries(Countdown PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    KF6::CoreAddons
    KF6::Kirigami
    KF6::IconThemes
    KF6::Archive
)
if (NOT ANDROID)
    target_link_libraries(Countdown PRIVATE Qt6::DBus)
endif()

# Android 专属：安卓下需显式链接 Qt6::Svg、OpenSSL::SSL、KF6::Kirigami
# （KDE 官方要求，否则可能缺符号/启动异常）
if (ANDROID)
    find_package(Qt6 COMPONENTS Svg REQUIRED)
    find_package(OpenSSL REQUIRED)
    find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS Kirigami)
    target_link_libraries(Countdown PRIVATE
        Qt6::Svg
        OpenSSL::SSL
        KF6::Kirigami
    )
endif()

target_compile_definitions(Countdown PRIVATE
    APP_VERSION="${PROJECT_VERSION}" # 版本号丢宏
)

# qml资源
qt_add_qml_module(Countdown
    URI com.countdown
    VERSION 1.0
    QML_FILES
        src/resources/qml/Main.qml
        src/resources/qml/AboutPageWindow.qml
        src/resources/qml/SettingsWindow.qml
        src/resources/qml/ReminderWindow.qml
        src/resources/qml/UpdaterWindow.qml
)

# 关键：修复安卓启动闪退。把 KDE 的 QML 模块复制进 build 目录，
# 让 androiddeployqt 的 qmlimportscanner（扫描 qml-import-paths=work/build）
# 能扫到 org.kde.kirigami、org.kde.kirigamiaddons 并把它们打进 APK。
if (ANDROID)
    add_custom_command(TARGET Countdown POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_INSTALL_PREFIX}/lib/qml/org/kde"
            "${CMAKE_BINARY_DIR}/org/kde"
        COMMENT "Copying KDE QML modules into build dir for deployment")
endif()
# Android 专属：打包应用用到的 Breeze 图标（安卓没有系统图标主题，必须自带）
if (ANDROID)
    kirigami_package_breeze_icons(ICONS
        settings-configure
        update-none
        help-about
        document-new
        document-edit
        edit-delete
        download
    )
endif()

# 翻译
qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES zh_CN en_US)# 支持的语言

qt_add_translations(Countdown
    RESOURCE_PREFIX /i18n
    TS_FILE_BASE countdown
    TS_FILE_DIR translations
)

# win的不知道什么编译参数
if (WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Release" OR
        CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" OR
        CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        set_target_properties(Countdown PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()
endif()

install(TARGETS Countdown
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}

    # 没用的安卓安装  现在有用了😋
    LIBRARY DESTINATION lib/${ANDROID_ABI}
    ARCHIVE DESTINATION lib/${ANDROID_ABI}
)
