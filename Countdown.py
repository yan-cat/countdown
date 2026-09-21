import info
from Package.CMakePackageBase import *
from CraftCore import CraftCore
import os
import re

class subinfo(info.infoclass):
    def setTargets(self):
        self.displayName = "Countdown"
        self.description = "A countdown application built with Kirigami"
        self.webpage = "https://github.com/yan-cat/countdown"

        self.svnTargets["main"] = "[git]https://github.com/yan-cat/countdown.git|main"
        self.svnTargets["dev"] = "[git]https://github.com/yan-cat/countdown.git|dev"
        self.defaultTarget = "main"

        self.buildType = "MinSizeRel"

    def setDependencies(self):
        # 构建依赖
        self.buildDependencies["kde/frameworks/extra-cmake-modules"] = None
        self.buildDependencies["libs/qt6/qttools"] = None

        # 运行时依赖
        self.runtimeDependencies["libs/qt6/qtbase"] = None
        self.runtimeDependencies["libs/qt6/qtdeclarative"] = None
        self.runtimeDependencies["kde/frameworks/tier1/kcoreaddons"] = None
        self.runtimeDependencies["kde/frameworks/tier1/kirigami"] = None
        self.runtimeDependencies["kde/unreleased/kirigami-addons"] = None
        self.runtimeDependencies["kde/frameworks/tier1/breeze-icons"] = None
        self.runtimeDependencies["kde/frameworks/tier3/kiconthemes"] = None
        self.runtimeDependencies["kde/plasma/breeze"] = None
        self.runtimeDependencies["kde/frameworks/tier3/qqc2-desktop-style"] = None


class Package(CMakePackageBase):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

    def createPackage(self):
        self.scriptname = os.path.join(self.sourceDir(), "Countdown.nsi") # 使用自定义的 NSIS 脚本

        # 版本号
        cmake_file = os.path.join(self.sourceDir(), "CMakeLists.txt")
        version = "0.0.0"
        with open(cmake_file, encoding="utf-8") as f:
            content = f.read()
        # 匹配 project(Countdown VERSION 1.2.4.0 ...) 或 project(Countdown VERSION 1.2.4)
        m = re.search(r"project\s*\(\s*\S+\s+VERSION\s+([0-9.]+)", content)
        if m:
            version = m.group(1)
            # 补齐 4 段，NSIS 通常需要 x.x.x.x 格式
            parts = version.split(".")
            while len(parts) < 4:
                parts.append("0")
            version = ".".join(parts[:4])

        self.defines["appVersion"] = version
        self.defines["icon"] = os.path.join(self.sourceDir(), "src", "resources", "icon", "com.countdown.ico")
        self.defines["unicon"] = os.path.join(self.sourceDir(), "src", "resources", "icon", "com.countdown.uninstall.ico")
        self.defines["company"] = "yancat"

        self.defines["shortcuts"] = [
            {
                "name": "Countdown",
                "target": "bin/Countdown.exe",
                "description": self.subinfo.description,
            }
        ]
        self.blacklist_file.append(os.path.join(self.sourceDir(), "exclude_list.txt"))
        self.defines["strip"] = True
        self.subinfo.options.package.disableStriping=True
        return super().createPackage()
