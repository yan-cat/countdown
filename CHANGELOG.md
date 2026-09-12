# 更新日志 (Changelog)

本文件格式参考 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.1.0/)，版本号遵循 [语义化版本](https://semver.org/lang/zh-CN/)。

## [Unreleased](https://github.com/yan-cat/countdown/compare/v1.2.3...main)
# 更新日志 (Changelog)

### 新增 (Added) 
- 软件logo与desktop文件
- 运行在beta版时的提示

### 变更 (Changed) 
- 对现有功能的改动描述

### 弃用 (Deprecated) 
- 即将移除的功能预告（给用户缓冲期）

### 移除 (Removed) 
- 已移除的功能

### 修复 (Fixed) 
- 不重启软件的情况下到第二天不会正确提醒日程

### 安全 (Security) 
- 修复的安全漏洞

## [1.2.3](https://github.com/yan-cat/countdown/compare/v1.2.2...v1.2.3) - 2026-09-11

### 变更 (Changed)
- 有新版本时自动打开更新窗口变更为有新版本时发送通知提醒

### 修复 (Fixed)
- 检查更新功能失效

## [1.2.2](https://github.com/yan-cat/countdown/compare/v1.2.1...v1.2.2) - 2026-09-11

### 新增 (Added)
- 点击卡片查看倒数日详情
- 删除倒数日时的确认删除弹窗
- 可开关的临近倒数日红框标记（可自定义天数）
- 下载增加加速源（可在设置中开关）
- 详情页可显示调试信息（调试选项内开关）
- 卡片自动多列排布，窗口越宽每行显示越多

### 变更 (Changed)
- **更改设置为驼峰命名**（升级后部分设置会恢复默认值，需重新设置一次）
- 移动"到时间时提醒"的显示到详情
- 优化代码结构与可读性
- 优化卡片动画（悬停缩放 + 点击脉冲）

### 修复 (Fixed)
- 某些地方的翻译丢失
- manager过早销毁导致关闭软件qml报错
- 修复语言文件生成和删除本不该出现的en翻译文件（不是en_US）

## [1.2.1](https://github.com/yan-cat/countdown/compare/v1.2.0...v1.2.1) - 2026-09-03

### 新增 (Added)
- 软件内的查看新版本日志、检查并下载新版本，支持 Windows 与 Linux
- 默认关闭的自动检查更新并提醒
- I18n本地化，目前支持简体中文（zh_CN）和英语（en_US）

### 变更 (Changed)
- 移动关于页面到独立窗口

### 修复 (Fixed)
- 关闭Debug时debug日志开关未隐藏

## [1.2.0](https://github.com/yan-cat/countdown/compare/v1.1.2...v1.2.0) - 2026-08-30

### 新增 (Added)
- 倒数日可启用通知了，支持 Windows 弹窗通知和 Linux 原生桌面通知
- 可开关 Debug 日志了
- 新增倒数日编辑功能，可修改已创建的倒数日
- 自动迁移旧版倒数日数据，升级后数据无缝衔接

### 修复 (Fixed)
- 修复“年月日”显示模式下的日期计算错误（该模式为估算值，精确计算请使用“显示天”模式）
- 修复 Windows 安装包未生成桌面快捷方式的问题

<!-- 1.2.0 之前的版本未采用本规范，历史记录见 GitHub Releases -->
