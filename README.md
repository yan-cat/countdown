# 倒数日
[English](README_en.md) | 中文
## 简介

一个基于 Kirigami / Qt 6 的倒数日桌面应用，用来记录和追踪生日、纪念日、截止日期等重要日子。

界面使用 KDE Frameworks 6 的 Kirigami 组件与 QML 构建，简洁现代，支持 Linux 与 Windows，并预留了 Android 支持。

主要功能：

- 新建、编辑、删除倒计时事项，直观显示距目标日期的剩余天数（或已过天数）
- 支持无重复、月重复、年重复三种倒计时模式
- 到设定日期提醒，可选当天、前一天或自定义天数提醒
- 软件内检查更新（自动检查默认关闭）
- 支持中英文界面（i18n）
- 支持静默（最小化）启动参数与 Debug 日志开关

数据以 JSON 保存于本地，设置项存储在 `~/.config/yancat/Countdown.conf`。

## 画廊

<p align="center">
  <table>
    <tr>
      <td><img src="readme_img/cn/app.png" width="600"></td>
      <td><img src="readme_img/cn/new.png" width="600"></td>
      <td><img src="readme_img/cn/reminder.png" width="600"></td>
    </tr>
  </table>
</p>

## 安装帮助

Release 内提供linux的二进制

通常需要这些包以供运行：

ArchLinux:
```bash
sudo pacman -S qt6-base qt6-declarative kirigami kcoreaddons kiconthemes breeze qqc2-desktop-style qt6-wayland
```

Ubuntu / Debian:
```bash
sudo apt install libqt6core6t64 libqt6qml6 libqt6quick6 \
  libkf6coreaddons6 libkf6iconthemes6 libkf6kirigami6 \
  breeze qml6-module-qtquick-controls qml6-module-qt-labs-platform \
  qml6-module-qtquick-layouts qt6-wayland
```
只支持 **Ubuntu 24.10 (Oracular) 或更高版本**

## 技术细节

项目目录：

```plaintext
countdown
├── android                           # 暂未使用
│   ├── AndroidManifest.xml
│   └── splash.xml
├── CMakeLists.txt
├── Countdown.py                      # KDE Craft 蓝图
├── LICENSE
├── README.md
├── src
│   ├── countdowndata.cpp             # 处理需要显示的数据
│   ├── datediff.cpp                  # 从天计算年月日
│   ├── debug.cpp                     # Debug相关
│   ├── main.cpp                      # 主程序
│   ├── manager.cpp                   # 管理数据
│   ├── reminder.cpp                  # 倒数日提醒
│   └── updater.cpp                   # 检查并更新
│   ├── include                       # 头文件
│   ├── resources
│   │   └── qml
│   │       ├── AboutPageWindow.qml   # 关于页面
│   │       ├── Main.qml              # 主页
│   │       ├── ReminderWindow.qml    # Windows的独立提醒弹窗
│   │       ├── SettingsWindow.qml    # 设置页面
│   │       └── UpdaterWindow.qml     # 更新页面
└── translations                      # I18n
    ├── countdown_en.ts
    ├── countdown_en_US.ts
    └── countdown_zh_CN.ts
```

软件创建的文件（夹）：

```plaintext
~
├── .local
│   └── share
│       └── yancat
│           └── Countdown
│               └── countdowns.json # 倒数日数据
└── .config
    └── yancat
        └── Countdown.conf          # 设置项
```

## 待完成

- [ ] 可选软件背景
- [ ] 安卓支持
- [ ] Webdav云同步事项
- [ ] 桌面磁贴
- [ ] windows深色显示问题修复

## 已完成

- [x] 软件内检查更新和默认关闭的自动检查
- [x] I18n
- [x] Windows 时间到/差几天提醒
- [x] 从老版本数据迁移到新版本
- [x] debug开关
- [x] Linux 时间到/差几天提醒
- [x] 静默启动参数
- [x] 编辑已添加的事项

此程序由 AI 辅助
