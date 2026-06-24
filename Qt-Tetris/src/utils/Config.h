#pragma once

#include <QString>
#include <algorithm>

namespace Config
{
    // ================================ 游戏设置 ==================================

    inline const int BASE_SCORE = 100;                                    // 消除一行的得分
    inline const int FALL_SPEED = 3;                                      // 方块下落速度，单位为 BLOCK_WIDTH/s

    // ================================ UI 设置 ==================================

    inline const int BOARD_COLS = 10;                                     // 列数
    inline const int BOARD_ROWS = 20;                                     // 行数
    inline const int BLOCK_WIDTH_ = 50;                                   // 方块的宽度
    inline const int SPAWN_X = 3;                                         // 方块生成时的横坐标
    inline const int SPAWN_Y = -2;                                        // 方块生成时的纵坐标

    // 字体
    inline const QString TITLE_FONT_FAMILY = "Segoe UI";                  // 标题字体
    inline const QString FONT_FAMILY = "Segoe UI";                        // 普通文本字体

    // 方块的颜色
    // 第一套配色方案
    // inline const QString SHAPE_I_COLOR = "#00A896";
    // inline const QString SHAPE_O_COLOR = "#D1B000";
    // inline const QString SHAPE_T_COLOR = "#6A0DAD";
    // inline const QString SHAPE_L_COLOR = "#D35400";
    // inline const QString SHAPE_J_COLOR = "#1F4E96";
    // inline const QString SHAPE_S_COLOR = "#2ECC71";
    // inline const QString SHAPE_Z_COLOR = "#C0392B";
    // 第二套配色方案
    inline const QString SHAPE_I_COLOR = "#5C9EAD";
    inline const QString SHAPE_O_COLOR = "#E9C46A";
    inline const QString SHAPE_T_COLOR = "#A06CD5";
    inline const QString SHAPE_L_COLOR = "#F4A261";
    inline const QString SHAPE_J_COLOR = "#4A7BB0";
    inline const QString SHAPE_S_COLOR = "#87A96B";
    inline const QString SHAPE_Z_COLOR = "#E76F51";

    inline const QString BLOCK_FRAME_COLOR = "#1A1C23";                     // 方块边框颜色
    inline const QString SIDEBAR_TITLE_COLOR = "#AAB2BD";                   // 侧边栏标题的颜色
    inline const QString SIDEBAR_SCORE_COLOR = "#FFFFFF";                   // 侧边栏分数的颜色
    inline const QString SIDEBAR_TIME_COLOR = "#FFFFFF";                    // 侧边栏时间的颜色
    inline const QString PANEL_BACKGROUND_COLOR = "#161B22";                // 暂停/结束面板背景颜色
    inline const QString PANEL_TITLE_COLOR = "#FFFFFF";                     // 暂停/结束面板标题颜色
    inline const QString PANEL_SCORE_COLOR = "#AAB2BD";                     // 暂停/结束面板分数颜色
    inline const QString BOARD_BLOCK_COLOR = "#101418";                     // 游戏背板的方块颜色
    inline const QString BOARD_BLOCK_FRAME_COLOR = "#2A2F36";               // 游戏背板方块边框颜色
    inline const QString FRAME_COLOR = "#E0E0E0";                           // 边界线颜色


    // ================================= 不可修改设置项 =========================================

    inline const int BLOCK_WIDTH = std::clamp(BLOCK_WIDTH_, 30, 50);

    inline const double SIDEBAR_TITLE_FONT_SIZE_RATIO = 0.3;
    inline const double SIDEBAR_SCORE_FONT_SIZE_RATIO = 0.5;
    inline const double SIDEBAR_TIME_FONT_SIZE_RATIO = 0.5;
    inline const double PANEL_TITLE_FONT_SIZE_RATIO = 0.4;
    inline const double PANEL_SCORE_SIZE_RATIO = 0.5;
    inline const double BUTTON_FONT_SIZE_RATIO = 0.4;

    inline const int INTERVAL_ONE = 5;
    inline const int INTERVAL_TWO = 10;
    inline const int INTERVAL_THREE = BLOCK_WIDTH;

    inline const int BLOCK_ROUND_RADIUS = 4;
    inline const int PANEL_ROUND_RADIUS = 8;

    inline const int SIDEBAR_WIDTH_IN_BLOCK = 5; 
    inline const int SIDEBAR_HEIGHT_IN_BLOCK = 12; 

    inline const double PANEL_WIDTH_RATIO = 0.5;
    inline const double PANEL_HEIGHT_RATIO = 0.3;

    inline const int PEN_WIDTH = 1;
    inline const int OVERLAY_OPACITY = 160;                              

    inline const int BOARD_WIDTH = BOARD_COLS * BLOCK_WIDTH + 2 * PEN_WIDTH;
    inline const int BOARD_HEIGHT = BOARD_ROWS * BLOCK_WIDTH + 2 * PEN_WIDTH;

    inline const int SIDEBAR_MIN_WIDTH_IN_BLOCK = 4;
    inline const int SIDEBAR_MAX_WIDTH_IN_BLOCK = 6;
    inline const int SIDEBAR_MIN_HEIGHT_IN_BLOCK = 12;
    inline const int SIDEBAR_MAX_HEIGHT_IN_BLOCK = 20;
    inline const int SIDEBAR_WIDTH = BLOCK_WIDTH * std::clamp(SIDEBAR_WIDTH_IN_BLOCK, SIDEBAR_MIN_WIDTH_IN_BLOCK, SIDEBAR_MAX_WIDTH_IN_BLOCK);
    inline const int SIDEBAR_HEIGHT = BLOCK_WIDTH * std::clamp(SIDEBAR_HEIGHT_IN_BLOCK, SIDEBAR_MIN_HEIGHT_IN_BLOCK, SIDEBAR_MAX_HEIGHT_IN_BLOCK);
    inline const int SIDEBAR_TITLE_HEIGHT = BLOCK_WIDTH;
    inline const int SIDEBAR_SCORE_HEIGHT = BLOCK_WIDTH;
    inline const int SIDEBAR_TIME_HEIGHT = BLOCK_WIDTH;
    inline const int SIDEBAR_TITLE_FONT_SIZE = static_cast<int>(SIDEBAR_TITLE_HEIGHT * SIDEBAR_TITLE_FONT_SIZE_RATIO);
    inline const int SIDEBAR_SCORE_FONT_SIZE = static_cast<int>(SIDEBAR_SCORE_HEIGHT * SIDEBAR_SCORE_FONT_SIZE_RATIO);
    inline const int SIDEBAR_TIME_FONT_SIZE = static_cast<int>(SIDEBAR_TIME_HEIGHT * SIDEBAR_TIME_FONT_SIZE_RATIO);

    inline const int GAME_WIDTH = BOARD_WIDTH + 4 * INTERVAL_ONE + SIDEBAR_WIDTH + INTERVAL_TWO;
    inline const int GAME_HEIGHT = BOARD_HEIGHT + 2 * INTERVAL_ONE;

    inline const int PANEL_MIN_WIDTH = 240;
    inline const int PANEL_MAX_WIDTH = GAME_WIDTH;
    inline const int PANEL_MIN_HEIGHT = 160;
    inline const int PANEL_MAX_HEIGHT = GAME_HEIGHT;
    inline const int PANEL_WIDTH = std::clamp(static_cast<int>(GAME_WIDTH * PANEL_WIDTH_RATIO), PANEL_MIN_WIDTH, PANEL_MAX_WIDTH);
    inline const int PANEL_HEIGHT = std::clamp(static_cast<int>(GAME_HEIGHT * PANEL_HEIGHT_RATIO), PANEL_MIN_HEIGHT, PANEL_MAX_HEIGHT);

}

