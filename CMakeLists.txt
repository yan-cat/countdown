cmake_minimum_required(VERSION 3.25)
project(Countdown VERSION 1.5.0 LANGUAGES CXX)

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
include(GNUInstallDirs)
ecm_add_android_apk(Countdown ANDROID_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android)
set(ANDROID_APK_DIR "${CMAKE_SOURCE_DIR}/android")

# 查找 Qt6 和 Kirigami
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Quick
    QuickControls2
    LinguistTools
    Svg
)
find_package(KF6 REQUIRED COMPONENTS
    Kirigami
    CoreAddons
    Archive
    IconThemes
)
if(ANDROID)
    find_package(QQC2BreezeStyle REQUIRED)
endif()

qt_policy(SET QTP0001 NEW)

# win可执行图标
if(WIN32)
    set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/src/resources/icon/com.countdown.rc")
else()
    set(app_icon_resource_windows "")
endif()

# 添加可执行文件
qt_add_executable(Countdown WIN32
    ${app_icon_resource_windows}
    src/main.cpp
    src/manager.cpp
    src/countdowndata.cpp
    src/datediff.cpp
    src/debug.cpp
    src/updater.cpp
    src/autostart.cpp

    src/include/manager.hpp
    src/include/updater.hpp
    src/include/countdowndata.hpp
    src/include/debug.hpp
    src/include/autostart.hpp

)
# 桌面/非安卓才编译的提醒实现（依赖 D-Bus、桌面弹窗；安卓不编译）
if (NOT ANDROID)
    target_sources(Countdown PRIVATE src/reminder.cpp)
endif()

# 优化大小：针对 Release 或 MinSizeRel 构建启用 LTO
if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result)
    if (result)
        set_property(TARGET Countdown PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

if (ANDROID)
    # 设置安卓文件夹位置
    set_target_properties(Countdown PROPERTIES
        QT_QML_IMPORT_PATH "${CMAKE_INSTALL_PREFIX}/lib/qml;${CMAKE_INSTALL_PREFIX}/qml"
        QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
        QT_DEPLOY_QML_DIR "${CMAKE_INSTALL_PREFIX}"
    )

    # apk版本号
    math(EXPR ANDROID_VERSION_CODE
            "${PROJECT_VERSION_MAJOR} * 10000 + ${PROJECT_VERSION_MINOR} * 100 + ${PROJECT_VERSION_PATCH}")

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml"
        @ONLY
    )
endif()

target_include_directories(Countdown PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/include)
if (LINUX)
    target_compile_options(Countdown PRIVATE -Wall -Wextra -Werror)
endif()

# 链接库
target_link_libraries(Countdown PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Svg
    KF6::CoreAddons
    KF6::Kirigami
    KF6::IconThemes
    KF6::Archive
)
if (LINUX)
    target_link_libraries(Countdown PRIVATE Qt6::DBus)
endif()

if (ANDROID)
    find_package(OpenSSL REQUIRED)
    find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS Kirigami)
    target_link_libraries(Countdown PRIVATE
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
        src/resources/qml/DetailsWindow.qml
        src/resources/qml/LogsWindow.qml

    RESOURCES
        src/resources/icon/com.countdown.svg
        src/resources/icon/Countdown_tray.svg
        com.countdown.desktop

)

if (ANDROID)
    # 关键：修复安卓启动闪退。把 KDE 的 QML 模块复制进 build 目录，
    # 让 androiddeployqt 的 qmlimportscanner（扫描 qml-import-paths=work/build）
    # 能扫到 org.kde.kirigami、org.kde.kirigamiaddons 并把它们打进 APK。
    # 别删
    add_custom_command(TARGET Countdown POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_INSTALL_PREFIX}/lib/qml/org/kde"
            "${CMAKE_BINARY_DIR}/org/kde"
        COMMENT "Copying KDE QML modules into build dir for deployment")

    # 安卓图标
    set(BREEZEICONS_DIR "/home/user/CraftRoot/tmp/breeze-icons")
    file(REAL_PATH "${BREEZEICONS_DIR}" BREEZEICONS_DIR)

    set(_theme_dir "${CMAKE_CURRENT_SOURCE_DIR}/android/assets/qml/org/kde/kirigami/breeze-internal")
    file(REMOVE_RECURSE "${_theme_dir}")
    file(MAKE_DIRECTORY "${_theme_dir}")

    # 需要的图标
    set(_icon_names
        # 左上角菜单
        application-menu
        settings-configure
        update-none
        help-about-symbolic

        # 确定取消
        dialog-ok
        dialog-cancel

        # 新建 编辑 删除
        document-new
        document-edit
        edit-delete

        # 横幅
        dialog-close # 关闭
        dialog-information # 信息
        dialog-positive # 成功
        dialog-warning # 警告
        dialog-error # 失败

        # 其他
        clock # 钟
        arrow-down # 下拉箭头
        process-working-symbolic # 加载转圈圈
    )

    set(_found_count 0)
    foreach(_name ${_icon_names})
        file(GLOB_RECURSE _matches
            "${BREEZEICONS_DIR}/icons/${_name}.svg")
        foreach(_match ${_matches})
            get_filename_component(_match_name "${_match}" NAME)
            get_filename_component(_real "${_match}" REALPATH)
            if(NOT EXISTS "${_real}")
                continue()
            endif()

            file(RELATIVE_PATH _rel_dir "${BREEZEICONS_DIR}/icons" "${_match}")
            get_filename_component(_rel_dir "${_rel_dir}" DIRECTORY)
            set(_dest_dir "${_theme_dir}/icons/${_rel_dir}")
            file(MAKE_DIRECTORY "${_dest_dir}")

            set(_dest_file "${_dest_dir}/${_match_name}")
            if(NOT EXISTS "${_dest_file}")
                file(COPY_FILE "${_real}" "${_dest_file}" ONLY_IF_DIFFERENT)
                math(EXPR _found_count "${_found_count} + 1")
            endif()
        endforeach()
    endforeach()

    message(STATUS "breeze-internal: copied ${_found_count} icon files")

    # 扫描实际存在的目录，生成 index.theme
    set(_dir_list "")
    file(GLOB _cat_dirs "${_theme_dir}/icons/*")
    foreach(_cat ${_cat_dirs})
        if(IS_DIRECTORY "${_cat}")
            get_filename_component(_cat_name "${_cat}" NAME)
            file(GLOB _size_dirs "${_cat}/*")
            foreach(_size_dir ${_size_dirs})
                if(IS_DIRECTORY "${_size_dir}")
                    file(GLOB _svgs "${_size_dir}/*.svg")
                    if(_svgs)
                        get_filename_component(_size_name "${_size_dir}" NAME)
                        list(APPEND _dir_list "icons/${_cat_name}/${_size_name}")
                    endif()
                endif()
            endforeach()
        endif()
    endforeach()

    list(JOIN _dir_list "," _dirs_str)
    set(_theme_content "[Icon Theme]\nName=Breeze\nDirectories=${_dirs_str}\nFollowsColorScheme=true\n")
    foreach(_dir ${_dir_list})
        get_filename_component(_size "${_dir}" NAME)
        if(_size STREQUAL "scalable")
            string(APPEND _theme_content "\n[${_dir}]\nSize=48\nType=Scalable\nMinSize=8\nMaxSize=512\n")
        else()
            string(APPEND _theme_content "\n[${_dir}]\nSize=${_size}\nType=Fixed\n")
        endif()
    endforeach()
    file(WRITE "${_theme_dir}/index.theme" "${_theme_content}")

    list(LENGTH _dir_list _dir_count)
    message(STATUS "breeze-internal theme: ${_dir_count} dirs, ready at ${_theme_dir}")
endif()

# 翻译
qt_add_translations(Countdown
    RESOURCE_PREFIX /i18n
    TS_FILE_BASE countdown
    TS_FILE_DIR translations
    TS_FILES
            translations/countdown_zh_CN.ts
            translations/countdown_en_US.ts
)

if(ANDROID)
    install(TARGETS Countdown
        # 没用的安卓安装  现在有用了😋  哦不不不你又没用了   完美了，就差ui主题了，我们要有安卓版倒数日咯～
        LIBRARY DESTINATION lib/${ANDROID_ABI}
        ARCHIVE DESTINATION lib/${ANDROID_ABI}
    )
else()
    install(TARGETS Countdown
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()
