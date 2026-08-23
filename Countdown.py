import info
from Package.CMakePackageBase import *
from CraftCore import CraftCore


class subinfo(info.infoclass):
    def setTargets(self):
        self.displayName = "Countdown"
        self.description = "A countdown application built with Kirigami"
        self.webpage = "https://github.com/yan-cat/countdown"

        self.svnTargets["main"] = "[git]https://github.com/yan-cat/countdown.git|main"
        self.defaultTarget = "main"

    def setDependencies(self):
        # 构建依赖
        self.buildDependencies["kde/frameworks/extra-cmake-modules"] = None

        # 运行时依赖
        self.runtimeDependencies["libs/qt6/qtbase"] = None
        self.runtimeDependencies["libs/qt6/qtdeclarative"] = None
        self.runtimeDependencies["kde/frameworks/tier1/kcoreaddons"] = None
        self.runtimeDependencies["kde/frameworks/tier1/kirigami"] = None
        self.runtimeDependencies["kde/unreleased/kirigami-addons"] = None
        self.runtimeDependencies["kde/frameworks/tier1/breeze-icons"] = None
        self.runtimeDependencies["kde/frameworks/tier3/kiconthemes"] = None


class Package(CMakePackageBase):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
