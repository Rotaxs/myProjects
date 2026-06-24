#pragma once

#include "logic/Block.h"
#include "logic/BlockGroup.h"
#include "utils/Config.h"
#include "logic/Board.h"

#include <QEvent>
#include <QMouseEvent>
#include <QWidget>
#include <QVector>

enum class OverlayAction { NONE, RETRY, EXIT, SAVE, BACK };

struct OverlayButton
{
    OverlayAction action;
    QString text;
    QRect rect;
    QColor textColor = QColor("#FFFFFF");
    QString fontFamily = Config::FONT_FAMILY;
};

class GameView : public QWidget
{
    Q_OBJECT
public:
    enum class OverlayMode { NONE, PAUSED, GAME_OVER };
private: 
    const Board *m_board;
    const BlockGroup *m_currentGroup, *m_nextGroup;
    const int *m_score;
    int m_elapsedMs;
    OverlayMode m_overlayMode;
    OverlayAction m_hoveredAction;
public:
    GameView(QWidget *parent = nullptr);
    ~GameView();
    void setBoard(const Board *board);
    void setCurrentGroup(const BlockGroup *group);
    void setNextGroup(const BlockGroup *group);
    void setScore(const int *score);
    void setElapsedMs(int elapsedMs);
    void setOverlayMode(OverlayMode mode);
private:
    void drawBorder(QPainter &painter, const int &borderX, const int &borderY) const;
    void drawBoard(QPainter &painter, int boardX, int boardY, const Board &board) const;
    void drawCurrentGroup(QPainter &painter, const int &posX, const int &posY) const;
    void drawSideBar(QPainter &painter, const int &sideBarX, const int &sideBarY) const;
    void drawNextGroupPreview(QPainter &painter, const int &posX, const int &posY) const;
    void drawScore(QPainter &painter, const int &posX, const int &posY) const;
    void drawTime(QPainter &painter, const int &posX, const int &posY) const;
    void drawOverlay(QPainter &painter) const;
    void drawOverlayButtons(QPainter &painter, const QVector<OverlayButton> &buttons) const;
    QRect overlayPanelRect() const;
    OverlayAction hoveredOverlayAction(const QPoint &pos) const;
    QVector<OverlayButton> overlayButtons(const QRect &panelRect) const;
    void updateOverlayHover(const QPoint &pos);
    bool canOverlay() const;
protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
signals:
    void moveLeftRequested();
    void moveRightRequested();
    void softDropRequested();
    void hardDropRequested();
    void rotateRequested();
    void pauseRequested();
    void retryRequested();
    void exitRequested();
    void saveRequested();
    void backRequested();
};
