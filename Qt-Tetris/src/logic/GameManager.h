#pragma once

#include "utils/Config.h"
#include "logic/Block.h"
#include "logic/Board.h"
#include "logic/BlockGroup.h"

#include <QTimer>
#include <QElapsedTimer>

class GameManager : public QObject
{
    Q_OBJECT
public:
    enum class GameState { READY, RUNNING, PAUSED, OVER };
private:
    Board m_board;
    BlockGroup m_currentGroup, m_nextGroup;
    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    int m_elapsedMs;
    int m_score;
    int m_fallGap;
    GameState m_state;
public:
    GameManager(QObject *parent = nullptr);
    ~GameManager();
    const Board& board() const { return m_board; }
    const BlockGroup& currentGroup() const { return m_currentGroup; }
    const BlockGroup& nextGroup() const { return m_nextGroup; }
    const int& score() const { return m_score; }
    const int elapsedMs() const;
    const GameState& state() const { return m_state; }
    void spawnNextGroup(int spawnX = Config::SPAWN_X, int spawnY = Config::SPAWN_Y);
    void spawnFirstGroup(int spawnX = Config::SPAWN_X, int spawnY = Config::SPAWN_Y);
    void start();
    void restart();
    void pause();
    void tick();
    bool saveGame() const;
    bool loadGame();
    void clearSaveGame() const;
    bool canMove(const int &dx = 0, const int &dy = 0);
    void moveBy(const int &dx = 0, const int &dy = 0);
    void moveLeft();
    void moveRight();
    void moveDown();
    void moveUp();
    bool canRotate(const int &offsetX = 0, const int &offsetY = 0);
    void rotate();
    void hardDrop();
    void lock();
    bool isOver();
    bool isLockOut() const;
    int clearFullRows();
    bool isFullRow(const int &row) const;
    void resetCell(const int &row, const int &col);
    void copyRow(const int &dstRow, const int &srcRow);
    bool isPosValid(const int &row = 0, const int &col = 0);
    void addScore(const int &lines);
    void nextRound();
    void back();
private:
    void clearBoard();
signals:
    void stateChange();
};
