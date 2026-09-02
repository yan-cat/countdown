# 倒数日
## 简介

一个简单的倒数日应用

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

- [ ] 软件内检查更新和默认关闭的自动检查
- [ ] 可选软件背景
- [ ] 安卓支持
- [ ] Webdav云同步事项
- [ ] 桌面磁贴
- [ ] windows深色显示问题修复
- [x] I18n
- [x] Windows 时间到/差几天提醒
- [x] 从老版本数据迁移到新版本
- [x] debug开关
- [x] Linux 时间到/差几天提醒
- [x] 静默启动参数
- [x] 编辑已添加的事项

此程序由 AI 辅助
