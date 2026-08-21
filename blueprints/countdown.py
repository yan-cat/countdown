# blueprints/countdown.py

from Blueprint import Blueprint
from CraftStandard import CraftStandard
from CraftOS.osutils import OsUtils

class Countdown(Blueprint, CraftStandard):
    def __init__(self):
        Blueprint.__init__(self)
        CraftStandard.__init__(self)

        # 1. 设置源代码来源（在 GitHub Actions 中，源码已检出到 C:\path\to\your\repo）
        #    我们可以直接指向本地路径，或者指向 Git 仓库。
        #    本地路径示例（使用绝对路径）：
        self.source = "src/your-project"  # 相对路径相对于 Craft 的源码目录，但更推荐用绝对路径
        #    或者使用 Git 仓库（如果在 CI 中不想依赖本地路径）：
        #    self.source = "git://github.com/your-username/countdown.git"
        #    self.branch = "main"

        # 2. 声明依赖 (Craft 会自动构建它们)
        self.depends = [
            "kf6/kcoreaddons",    # KF6 CoreAddons
            "kf6/kirigami",       # KF6 Kirigami
            "qt6/qtdeclarative",  # Qt6 Quick + Qml
        ]

        # 3. 设置构建类型
        self.buildType = "Release"

        # 4. 设置 CMake 参数（可选）
        #    Craft 会自动添加 -DCMAKE_INSTALL_PREFIX 等，我们只需指定额外参数
        self.configureOptions = [
            "-DCMAKE_PREFIX_PATH=C:/CraftRoot/install",
            # 如果你的项目需要特定版本，可以在这里加
        ]

    def build(self):
        """执行构建流程"""
        # 调用 CraftStandard 的 build 方法，它会处理 cmake --build . --config Release
        return CraftStandard.build(self)
