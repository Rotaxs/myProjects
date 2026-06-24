cmake_minimum_required(VERSION 3.16)
project(TetrisGame VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 自动处理机制（必须开启）
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 寻找 Qt 库
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

set(TETRIS_SOURCES
    src/main.cpp
    src/utils/Config.h

    # UI 页面
    src/ui/MainWindow.h         src/ui/MainWindow.cpp 
    src/ui/GameView.h           src/ui/GameView.cpp
    # 方块类
    src/logic/Block.h           src/logic/Block.cpp 
    src/logic/BlockGroup.h      src/logic/BlockGroup.cpp
    src/logic/Board.h           src/logic/Board.cpp
    # 游戏逻辑
    src/logic/GameManager.h     src/logic/GameManager.cpp
    # 文件处理
    src/utils/SaveGameUtils.h   src/utils/SaveGameUtils.cpp
)

# 声明所有的源文件
add_executable(Tetris WIN32 ${TETRIS_SOURCES})

# 将 src 作为根目录
target_include_directories(Tetris PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 链接 Qt 库
target_link_libraries(Tetris PRIVATE Qt6::Widgets)
