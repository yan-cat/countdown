# 倒数日
## 简介

一个简单的倒数日应用

---
## 技术细节

项目目录：

```plaintext
countdown
├── android
│   └── AndroidManifest.xml        暂未使用
├── CMakeLists.txt
├── LICENSE
├── README.md
└── src
    ├── countdowndata.cpp          处理需要显示的数据
    ├── countdowndata.h
    ├── datediff.cpp               从天计算年月日
    ├── datediff.h
    ├── main.cpp                   主程序
    ├── manager.cpp                管理数据
    ├── manager.h
    └── resources
        └── qml
            ├── AboutPage.qml      关于页面
            ├── Main.qml主页面
            └── SettingsWindow.qml 设置页面
```

软件创建的文件（夹）：

```plaintext
~
├── .local
│   └── share
│       └── yancat
│           └── Countdown
│               └── countdowns.json 倒数日数据
└── .config
    └── yancat
        └── Countdown.conf          设置项
```

ds辅助编写
