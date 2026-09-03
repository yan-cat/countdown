cmake_minimum_required(VERSION 3.16)
project(Countdown VERSION 1.2.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(BINARY_ICONS_RESOURCE ON)

# ===== 关键：定义 KF6 版本 =====
set(KF6_MIN_VERSION 6.12.0)
# ===============================
set(QML_IMPORT_PATH "/usr/lib/qt6/qml;/usr/lib/qml" CACHE STRING "QML Import Paths")

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

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
    IconThemes
    Archive
)

qt_policy(SET QTP0001 NEW)

# 添加可执行文件
qt6_add_executable(Countdown
    src/main.cpp
    src/manager.cpp
    src/countdowndata.cpp
    src/datediff.cpp
    src/reminder.cpp
    src/debug.cpp
    src/updater.cpp

    src/include/manager.h
    src/include/updater.h
    src/include/countdowndata.h

)

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

target_compile_definitions(Countdown PRIVATE
    APP_VERSION="${PROJECT_VERSION}" # 版本号丢宏
)


# qml资源
qt_add_qml_module(Countdown
    URI com.countdown
    VERSION 1.0
    NO_IMPORT_SCAN
    QML_FILES
        src/resources/qml/Main.qml
        src/resources/qml/AboutPageWindow.qml
        src/resources/qml/SettingsWindow.qml
        src/resources/qml/ReminderWindow.qml
        src/resources/qml/UpdaterWindow.qml
)
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

    # 没用的安卓安装
    LIBRARY DESTINATION lib/${ANDROID_ABI}
    ARCHIVE DESTINATION lib/${ANDROID_ABI}
)
