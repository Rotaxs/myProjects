# Qt-Tetris

## 技术栈

- C++
- Qt
- cmake

## 构建方式

使用下面的命令克隆仓库

```bash
git clone git@github.com:Rotaxs/Qt-Tetris.git
```

**通过 Qt Creator 构建** 

打开 Qt Creator，按下快捷键 `Ctrl + O` 弹出打开文件的面板，选择仓库中的 `CMakeLists.txt` 文件，等待项目初始化

选择 `Release`，然后点击构建按钮即可

这时可以在项目的 `build` 目录下找到相应的 `exe` 文件

![Qt Creator 构建](./assets/qt_creator_build.png)

## 项目简介

本项目基于 Qt 开发，实现了一个简单的俄罗斯方块游戏

项目并非一比一复刻，例如

- 没有实现原版的等级机制
- 没有实现方块下落速度随着游戏进行不断加快

![项目演示](./assets/gameDemo01.png)

## 项目特色

- 实现简易存档功能
- UI 层和逻辑层解耦，层次清晰
- 所有控件均使用 `QPainter` 绘制

## 游戏玩法

```mermaid
flowchart TD
    A["程序启动"] --> B["MainWindow 创建 GameView 和 GameManager"]
    B --> C{"是否存在可读存档?"}
    
    C -- "是" --> D["loadGame 读取存档"]
    D --> E{"读档成功?"}
    E -- "是" --> F["进入 RUNNING 状态"]
    E -- "否" --> G["start 开始新游戏"]
    C -- "否" --> G["start 开始新游戏"]

    G --> H["生成首个 currentGroup 和 nextGroup"]
    H --> I["启动 QTimer"]
    F --> I

    I --> J["游戏循环 tick()"]
    J --> K{"当前方块能否向下移动?"}
    
    K -- "是" --> L["moveDown"]
    L --> I
    K -- "否" --> M["nextRound"]

    M --> N["lock 锁定当前方块到棋盘"]
    N --> O["clearFullRows 消行"]
    O --> P["addScore 更新分数"]
    P --> Q["spawnNextGroup 生成新方块"]
    Q --> R{"新方块是否越界或冲突?"}
    
    R -- "是" --> S["OVER 状态"]
    S --> T["清空存档 clearSaveGame"]
    T --> U["显示 GAME OVER 面板"]
    R -- "否" --> I

    U --> V{"用户选择"}
    V -- "RETRY" --> G
    V -- "EXIT" --> W["退出程序"]

    J --> X{"用户输入"}
    X -- "左移/右移" --> Y["moveLeft / moveRight"]
    X -- "软降" --> Z["moveDown"]
    X -- "旋转" --> AA["rotate"]
    X -- "硬降" --> AB["hardDrop"]
    X -- "暂停" --> AC["pause"]
    X -- "其他" --> I

    AC --> AD["PAUSED 状态"]
    AD --> AE["显示暂停面板"]
    AE --> AF{"用户选择"}
    AF -- "BACK" --> F
    AF -- "SAVE" --> AG["saveGame 保存进度"]
    AF -- "EXIT" --> W
```



**按键控制**

- `上/W`：方块旋转
- `下/S`：方块向下移动一格
- `左/A`：方块向左移动一格
- `右/D`：方块向右移动一格
- `SPACE`：方块直接落到底部
- `ESC/P`：暂停

**暂停界面**

暂停界面有三个按钮，分别是 `BACK`（返回游戏），`SAVE`（游戏存档），`EXIT`（退出游戏）

![暂停界面](./assets/gameDemo02.png)

**游戏存档**

游戏的存档路径在 `C:\Users\username\AppData\Local\Tetris`

当游戏启动时，会首先读取存档，如果存档为空，则开始新的游戏；否则会读取存档中未结束的游戏继续游戏

如果游戏结束，会自动清空存档

**游戏结束**

游戏结束时会弹出游戏结束的面板，这里会有两个按钮 `RETRY`（重新开始），`EXIT`（退出游戏）

![游戏结束](./assets/gameDemo03.png)
