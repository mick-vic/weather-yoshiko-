# 天气良子 / Weather Yoshiko

一个基于 Qt 6 的视觉小说 + 打砖块小游戏项目。

---

## 项目简介

大胃袋良子最爱吃板面，但是板面总是被范小勤偷吃。良子必须用他强大的胃袋击碎小勤的防御，揭开板面被盗的真相——然而真相远比他想象的要残酷……

本作包含：
- **Galgame 式对话系统**：逐字显示文本，支持翻页、背景切换、音频播放
- **开场 CG 动画**：v1.mp4 全屏播放
- **打砖块小游戏**：操控丹尼尔的 V 形手臂反弹球体，击碎砖块，打败范小勤
- **多结局对话**：通关后展开剧情对话，揭示世界观真相
- **结尾 CG**：v2.mp4 + end.jpg 定格画面

---

## 截图

> TODO：添加游戏截图

---

## 技术栈

| 类别 | 技术 |
|------|------|
| 语言 | C++17 |
| 框架 | Qt 6.10.2 |
| 构建 | CMake |
| 图形 | QGraphicsView / QGraphicsScene |
| 音频 | QMediaPlayer + QAudioOutput |
| 视频 | QGraphicsVideoItem |

---

## 项目结构

```
tianqiliangzi/
├── main.cpp              # 入口
├── widget.h/cpp/ui       # 主界面（页面切换、事件处理）
├── resourcemanager.h/cpp # 资源路径管理
├── dialogmanager.h/cpp   # Galgame 对话框系统（逐字动画）
├── gamewidget.h/cpp      # 打砖块游戏控件
├── gamescene.h/cpp       # 游戏场景（碰撞检测、游戏逻辑）
├── ballitem.h/cpp        # 球体
├── brickitem.h/cpp       # 砖块
├── danielplayer.h/cpp    # 丹尼尔（玩家角色，V形手臂）
├── fanxiaoqin.h/cpp      # 范小勤（敌方方块）
├── res.qrc               # Qt 资源文件
├── resource.rc           # Windows 图标资源
├── app.ico               # 程序图标
├── CMakeLists.txt        # CMake 构建配置
└── assets/               # 资源文件夹
    ├── images/           # 图片（背景、贴图）
    ├── audio/            # 音频（BGM、音效）
    └── videos/           # 视频（CG 动画）
```

---

## 构建方法

### 环境要求

- Qt 6.5+（推荐 6.10）
- CMake 3.16+
- 支持 C++17 的编译器（MSVC / MinGW / Clang）
- Windows / macOS / Linux

### 编译步骤

```bash
# 1. 配置
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64

# 2. 编译
cmake --build build

# 3. 运行（资源文件会自动复制到构建目录）
./build/tianqiliangzi
```

### Windows 打包发布

```bash
# Release 编译
cmake -B build-release -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/path/to/Qt
cmake --build build-release

# 使用 windeployqt 收集依赖
windeployqt build-release/tianqiliangzi.exe --qmldir /path/to/Qt/qml
```

---

## 操作方法

| 操作 | 按键 |
|------|------|
| 推进对话 / 跳过 CG | 鼠标左键 / 回车 |
| 打砖块 - 胳膊角度 | 鼠标移动 |
| 打砖块 - 发射球 | 鼠标左键 |
| 打砖块 - 通关（调试） | 回车键 |

---

## 许可证

本项目仅用于学习和娱乐目的。

---

## 致谢

- [Qt](https://www.qt.io/) - 跨平台 GUI 框架
- 所有角色和剧情均为原创
