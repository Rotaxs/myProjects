#include "ui/GameView.h"
#include "utils/Config.h"
#include "logic/Block.h"

#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>

GameView::GameView(QWidget *parent) :
        m_board(nullptr), 
        m_currentGroup(nullptr), m_nextGroup(nullptr),
        m_elapsedMs(0), 
        m_score(nullptr), m_overlayMode(OverlayMode::NONE),
        m_hoveredAction(OverlayAction::NONE)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setFocus();
    int margin = 2; // 预留空间
    this->setMinimumSize(Config::GAME_WIDTH + margin, Config::GAME_HEIGHT + margin);
    this->resize(Config::GAME_WIDTH + margin, Config::GAME_HEIGHT + margin);
}

GameView::~GameView() { }

void GameView::setBoard(const Board *board)
{
    m_board = board;
    update();
}

void GameView::setCurrentGroup(const BlockGroup *group)
{
    m_currentGroup = group;
    update();
}

void GameView::setNextGroup(const BlockGroup *group)
{
    m_nextGroup = group;
    update();
}

void GameView::setScore(const int *score)
{
    m_score = score;
    update();
}

void GameView::setElapsedMs(int elapsedMs)
{
    m_elapsedMs = elapsedMs;
    update();
}

void GameView::setOverlayMode(OverlayMode mode)
{
    m_overlayMode = mode;
    update();
}

/**
 * @brief 绘制游戏面板和侧边栏的边框
 * @param painter 画笔
 * @param borderX, borderY 游戏边框的左上角的坐标
 */
void GameView::drawBorder(QPainter &painter, const int &borderX, const int &borderY) const
{
    painter.setPen(QPen(QColor(Config::FRAME_COLOR), Config::PEN_WIDTH));
    painter.drawRect(borderX, borderY, Config::BOARD_WIDTH + 2 * Config::INTERVAL_ONE, Config::BOARD_HEIGHT + 2 * Config::INTERVAL_ONE);
    int sidebarX = borderX + Config::BOARD_WIDTH + 2 * Config::INTERVAL_ONE + Config::INTERVAL_TWO;
    int sidebarY = borderY;
    painter.drawRect(sidebarX, sidebarY, Config::SIDEBAR_WIDTH, Config::SIDEBAR_HEIGHT);
}

/**
 * @brief 绘制游戏面板和已经锁定的方块
 * @param painter 画笔
 * @param boardX, boradY 游戏面板左上角的坐标
 * @param board 游戏面板矩阵
 */
void GameView::drawBoard(QPainter &painter, int boardX, int boardY, const Board &board) const
{
    if (!m_board)
    {
        qFatal() << "GameView::drawBoard: Board 还未被设置，程序终止";
        return;
    }
    // 先画底板
    for (int row = 0; row < board.getRows(); row++)
    {
        for (int col = 0; col < board.getCols(); col++)
        {
            const Block &cell = board[row][col];
            if (cell.isOccupied()) continue;
            int blockWidth = cell.getWidth();
            painter.setBrush(cell.getColor());
            painter.setPen(cell.getFrameColor());
            painter.drawRect(
                boardX + cell.getX() * blockWidth,
                boardY + cell.getY() * blockWidth,
                blockWidth, blockWidth
            );
        }
    }
    // 再画已经占用的格子
    int roundRadius = Config::BLOCK_ROUND_RADIUS;
    for (int row = 0; row < board.getRows(); row++)
    {
        for (int col = 0; col < board.getCols(); col++)
        {
            const Block &cell = board[row][col];
            if (!cell.isOccupied()) continue;
            int blockWidth = cell.getWidth();
            painter.setBrush(cell.getColor());
            painter.setPen(QPen(cell.getFrameColor()));
            painter.drawRoundedRect(
                boardX + cell.getX() * blockWidth,
                boardY + cell.getY() * blockWidth,
                blockWidth, blockWidth,
                roundRadius, roundRadius, 
                Qt::AbsoluteSize
            );
        }
    }
}

/**
 * @brief 绘制当前活跃的方块
 * @param painter 画笔
 * @param posX, posY 方块组的坐标
 */
void GameView::drawNextGroupPreview(QPainter &painter, const int &posX, const int &posY) const
{
    if (!m_nextGroup) return;
    const BlockGroup &group = *m_nextGroup;

    const int titleHeight = Config::SIDEBAR_TITLE_HEIGHT;
    const int contentWidth = Config::SIDEBAR_WIDTH - 2 * Config::INTERVAL_ONE;

    QRect titleRect(posX, posY, contentWidth, titleHeight);

    painter.setPen(QColor(Config::SIDEBAR_TITLE_COLOR));
    painter.setFont(QFont(Config::TITLE_FONT_FAMILY, Config::SIDEBAR_TITLE_FONT_SIZE, QFont::Bold));
    painter.drawText(titleRect, Qt::AlignHCenter | Qt::AlignVCenter, "NEXT");

    // 归一化处理，同时处理中心坐标
    int minX = group[0].getX();
    int maxX = group[0].getX();
    int minY = group[0].getY();
    int maxY = group[0].getY();
    for (int i = 0; i < 4; i++)
    {
        minX = std::min(minX, group[i].getX());
        maxX = std::max(maxX, group[i].getX());
        minY = std::min(minY, group[i].getY());
        maxY = std::max(maxY, group[i].getY());
    }

    const int blockWidth = group[0].getWidth();
    const int maxWidth = (maxX - minX + 1) * blockWidth;
    const int maxHeight = (maxY - minY + 1) * blockWidth;
    const int startX = posX + (Config::SIDEBAR_WIDTH - maxWidth) / 2;
    const int startY = posY + blockWidth + titleHeight;
    const int roundRadius= Config::BLOCK_ROUND_RADIUS;

    for (int i = 0; i < 4; i++)
    {
        const Block &cell = (*m_nextGroup)[i];
        int localX = cell.getX() - minX;
        int localY = cell.getY() - minY;
        painter.setBrush(cell.getColor());
        painter.setPen((QPen(cell.getFrameColor())));
        painter.drawRoundedRect(
            startX + localX * blockWidth,
            startY + localY * blockWidth,
            blockWidth, blockWidth,
            roundRadius, roundRadius,
            Qt::AbsoluteSize
        );
    }
}

/**
 * @brief 绘制当前活跃的方块
 * @param painter 画笔
 * @param posX, posY 方块组的坐标
 */
void GameView::drawCurrentGroup(QPainter &painter, const int &posX, const int &posY) const
{
    if (!m_currentGroup) return;
    int roundRadius = Config::BLOCK_ROUND_RADIUS;
    for (int i = 0; i < 4; i++)
    {
        const Block &cell = (*m_currentGroup)[i];
        if (cell.getY() < 0) continue;
        const int blockWidth = cell.getWidth();
        painter.setBrush(cell.getColor());
        painter.setPen((QPen(cell.getFrameColor())));
        painter.drawRoundedRect(
            posX + cell.getX() * blockWidth,
            posY + cell.getY() * blockWidth,
            blockWidth, blockWidth,
            roundRadius, roundRadius,
            Qt::AbsoluteSize
        );
    }
}

/**
 * @brief 绘制分数
 * @param painter 画笔
 * @param posX, posY 分数的坐标
 */
void GameView::drawScore(QPainter &painter, const int &posX, const int &posY) const
{
    if (!m_score) return;
    const int titleHeight = Config::SIDEBAR_TITLE_HEIGHT;
    const int valueHeight = Config::SIDEBAR_SCORE_HEIGHT;
    const int contentWidth = Config::SIDEBAR_WIDTH - 2 * Config::INTERVAL_ONE;

    QRect titleRect(posX, posY, contentWidth, titleHeight);
    QRect valueRect(posX, posY + titleHeight, contentWidth, valueHeight);
    // 绘制分数标题 SCORE
    painter.setPen(QColor(Config::SIDEBAR_TITLE_COLOR));
    painter.setFont(QFont(Config::TITLE_FONT_FAMILY, Config::SIDEBAR_TITLE_FONT_SIZE, QFont::Bold));
    painter.drawText(titleRect, Qt::AlignHCenter | Qt::AlignVCenter, "SCORE");
    // 绘制实际分数
    painter.setPen(QColor(Config::SIDEBAR_SCORE_COLOR));
    painter.setFont(QFont(Config::FONT_FAMILY, Config::SIDEBAR_SCORE_FONT_SIZE, QFont::DemiBold));
    painter.drawText(valueRect, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(*m_score));
}

/**
 * @brief 绘制时间
 * @param painter 画笔
 * @param posX, posY 时间的坐标
 */
void GameView::drawTime(QPainter &painter, const int &posX, const int &posY) const
{
    const int titleHeight = Config::SIDEBAR_TITLE_HEIGHT;
    const int valueHeight = Config::SIDEBAR_TIME_HEIGHT;
    const int contentWidth = Config::SIDEBAR_WIDTH - 2 * Config::INTERVAL_ONE;

    QRect titleRect(posX, posY, contentWidth, titleHeight);
    QRect valueRect(posX, posY + titleHeight, contentWidth, valueHeight);

    painter.setPen(QColor(Config::SIDEBAR_TITLE_COLOR));
    painter.setFont(QFont(Config::TITLE_FONT_FAMILY, Config::SIDEBAR_TITLE_FONT_SIZE, QFont::Bold));
    painter.drawText(titleRect, Qt::AlignHCenter | Qt::AlignVCenter, "TIME");

    int totalSeconds = m_elapsedMs / 1000;
    int hours = totalSeconds / 3600;
    int minutes = totalSeconds % 3600 / 60;
    int seconds = totalSeconds % 60;
    QString timeText = QString("%1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));

    painter.setPen(QColor(Config::SIDEBAR_TIME_COLOR));
    painter.setFont(QFont(Config::FONT_FAMILY, Config::SIDEBAR_TIME_FONT_SIZE, QFont::DemiBold));
    painter.drawText(valueRect, Qt::AlignHCenter | Qt::AlignVCenter, timeText);
}

/**
 * @brief 绘制侧边栏
 * @param painter 画笔
 * @param sidebarX, sidebarY 侧边栏的坐标
 */
void GameView::drawSideBar(QPainter &painter, const int &sidebarX, const int &sidebarY) const
{
    int timeX = sidebarX;
    int timeY = sidebarY + Config::SIDEBAR_TITLE_HEIGHT + Config::SIDEBAR_SCORE_HEIGHT + Config::INTERVAL_THREE;
    int groupX = sidebarX;
    int groupY = timeY + Config::SIDEBAR_TITLE_HEIGHT + Config::SIDEBAR_TIME_HEIGHT + Config::INTERVAL_THREE;
    
    drawScore(painter, sidebarX, sidebarY);
    drawTime(painter, timeX, timeY);
    drawNextGroupPreview(painter, groupX, groupY);
}

/**
 * @brief 获取面板的 QRect，显示在蒙版之上，用于绘制暂停，结束，开始等界面
 * @return 面板的 QRect
 */
QRect GameView::overlayPanelRect() const
{
    const int borderX = (this->width() - Config::GAME_WIDTH) / 2;
    const int borderY = (this->height() - Config::GAME_HEIGHT) / 2;
    QRect gameRect(borderX, borderY, Config::GAME_WIDTH, Config::GAME_HEIGHT);
    const int panelWidth = Config::PANEL_WIDTH;
    const int panelHeight = Config::PANEL_HEIGHT;
    return QRect(
        gameRect.center().x() - panelWidth / 2,
        gameRect.center().y() - panelHeight / 2,
        panelWidth, panelHeight
    );
}

/**
 * @brief 获取鼠标光标悬停的按钮的操作
 * @param pos 鼠标光标的位置 
 * @return 鼠标光标悬停的按钮所要进行的操作
 */
OverlayAction GameView::hoveredOverlayAction(const QPoint &pos) const
{
    const QVector<OverlayButton> buttons = overlayButtons(overlayPanelRect());
    for (const OverlayButton &button : buttons)
    {
        if (button.rect.contains(pos))
            return button.action;
    }
    return OverlayAction::NONE;
}

/**
 * @brief 更新鼠标光标状态，如果鼠标不在按钮上，将鼠标形状设置为 ArrowCursor，并将 m_hoveredAction 设置为 NONE，否则将鼠标形状设置为 PointingHandCursor，且更新 m_hoveredAction 为相应按钮的值
 * @param pos 鼠标光标的坐标
 */
void GameView::updateOverlayHover(const QPoint &pos)
{
    const OverlayAction nextAction = hoveredOverlayAction(pos);
    if (nextAction == m_hoveredAction) return;
    m_hoveredAction = nextAction;
    setCursor(m_hoveredAction == OverlayAction::NONE ? Qt::ArrowCursor : Qt::PointingHandCursor);
    update();
}

/**
 * @brief 绘制蒙版和面板
 * @param painter 画笔
 * @param callback 回调函数，决定面板的类型（可选的有 pauseCallback，overCallback）
 */
void GameView::drawOverlay(QPainter &painter) const
{
    if (!m_score)
    {
        qFatal() << "GameView::overCallbck: m_score 为空指针";
        return;
    }
    const int borderX = (this->width() - Config::GAME_WIDTH) / 2;
    const int borderY = (this->height() - Config::GAME_HEIGHT) / 2;
    // 蒙版
    int margin = 2;
    QRect gameRect(borderX, borderY, Config::GAME_WIDTH, Config::GAME_HEIGHT + margin);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, Config::OVERLAY_OPACITY));
    painter.drawRect(gameRect);
    // 绘制面板
    QRect panelRect = overlayPanelRect();
    const int roundRadius = Config::PANEL_ROUND_RADIUS;
    painter.setBrush(QColor(Config::PANEL_BACKGROUND_COLOR));
    painter.setPen(QPen(QColor(Config::FRAME_COLOR), 1));
    painter.drawRoundedRect(panelRect, roundRadius, roundRadius);

    int fontSize = 0;
    const int interval = Config::INTERVAL_ONE;
    const int panelWidth = panelRect.width() - 2 * interval;
    const int panelHeight = panelRect.height() - 2 * interval;
    const int panelX = panelRect.x() + interval;
    const int panelY = panelRect.y() + interval;
    const int titleHeight = static_cast<int>(panelHeight * 0.4);
    const int scoreHeight = static_cast<int>(panelHeight * 0.3);
    
    const QRect titleRect(panelX, panelY, panelWidth, titleHeight);
    const QRect scoreRect(panelX, panelY + titleHeight, panelWidth, scoreHeight);

    // 绘制 标题
    fontSize = titleHeight * 0.3;
    painter.setPen(Config::PANEL_TITLE_COLOR);
    painter.setFont(QFont(Config::TITLE_FONT_FAMILY, fontSize, QFont::Bold));
    if (m_overlayMode == OverlayMode::PAUSED)
        painter.drawText(titleRect, Qt::AlignHCenter | Qt::AlignVCenter, "PAUSE");
    else if (m_overlayMode == OverlayMode::GAME_OVER)
        painter.drawText(titleRect, Qt::AlignHCenter | Qt::AlignVCenter, "GAME OVER");
    // 绘制 SCORE 和分数
    fontSize = scoreHeight * 0.2;
    margin = scoreRect.width() * 0.2;
    const int width = (scoreRect.width() - 2 * margin) / 2;
    const QRect scoreNameRect(scoreRect.x() + margin, scoreRect.y(), width, scoreRect.height());
    const QRect scoreValueRect(scoreRect.x() + margin + width, scoreRect.y(), width, scoreRect.height());
    painter.setPen(QColor(Config::PANEL_SCORE_COLOR));
    painter.setFont(QFont(Config::FONT_FAMILY, fontSize, QFont::Bold));
    painter.drawText(scoreNameRect, Qt::AlignHCenter | Qt::AlignVCenter, "SCORE");
    painter.drawText(scoreValueRect, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(*m_score));

    drawOverlayButtons(painter, overlayButtons(panelRect));
    
}

/**
 * @brief 按照当前的状态获取按钮
 * @param panelRect 面板的 QRect
 * @return 如果 state 是 PAUSEED，那么返回 BACK, SAVE, EXIT 三个按钮，如果是 OVER，则返回 RETRY, EXIT 两个按钮
 */
QVector<OverlayButton> GameView::overlayButtons(const QRect &panelRect) const
{
    QVector<OverlayButton> buttons;
    if (m_overlayMode == OverlayMode::GAME_OVER)
    {
        buttons.append({ OverlayAction::RETRY, "RETRY", QRect() });
        buttons.append({ OverlayAction::EXIT, "EXIT", QRect() });
    }
    else if (m_overlayMode == OverlayMode::PAUSED)
    {
        buttons.append({ OverlayAction::BACK, "BACK", QRect() });
        buttons.append({ OverlayAction::SAVE, "SAVE", QRect() });
        buttons.append({ OverlayAction::EXIT, "EXIT", QRect() });
    }
    if (buttons.isEmpty()) return buttons;

    const int count = buttons.size();
    const int gap = static_cast<int>(panelRect.width() * 0.05);
    const int buttonWidth = static_cast<int>(panelRect.width() * (count == 2 ? 0.28 : 0.22));
    const int buttonHeight = static_cast<int>(panelRect.height() * 0.18);
    const int totalWidth = count * buttonWidth + (count - 1) * gap;
    const int startX = panelRect.center().x() - totalWidth / 2;
    const int buttonY = panelRect.bottom() - buttonHeight - static_cast<int>(panelRect.height() * 0.10);

    for (int i = 0; i < count; i++)
    {
        buttons[i].rect = QRect(
            startX + i * (buttonWidth + gap),
            buttonY,
            buttonWidth, buttonHeight
        );
    }
    return buttons;
}

/**
 * @brief 绘制按钮
 * @param painter 画笔
 * @param buttons 要绘制的按钮列表
 */
void GameView::drawOverlayButtons(QPainter &painter, const QVector<OverlayButton> &buttons) const
{
    const int count = buttons.size();
    for (int i = 0; i < count; i++)
    {
        const QRect &buttonRect = buttons[i].rect;
        const int fontSize = Config::BUTTON_FONT_SIZE_RATIO * buttonRect.height();
        const bool hovered = (m_hoveredAction == buttons[i].action);
        painter.setPen(Qt::NoPen);
        painter.setBrush(hovered ? QColor(255, 255, 255, 40) : QColor(255, 255, 255, 14));
        painter.drawRoundedRect(buttonRect, Config::PANEL_ROUND_RADIUS, Config::PANEL_ROUND_RADIUS);
        painter.setPen(buttons[i].textColor);
        painter.setFont(QFont(buttons[i].fontFamily, hovered ? fontSize + 2 : fontSize, QFont::DemiBold));
        painter.drawText(buttonRect, Qt::AlignHCenter | Qt::AlignVCenter, buttons[i].text);
    }
}

/**
 * @brief 判断是否是暂停或结束状态，以判断是否能切换到蒙版
 * @return true 表示在暂停或结束状态，否则不在暂停或结束状态
 */
bool GameView::canOverlay() const
{
    return m_overlayMode != OverlayMode::NONE;
}

void GameView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    // 开启抗锯齿，使得圆角和线条更加平滑
    painter.setRenderHint(QPainter::Antialiasing);
    const Board &board = *m_board;
    int borderX = (this->width() - Config::GAME_WIDTH) / 2;
    int borderY = (this->height() - Config::GAME_HEIGHT) / 2; 
    int boardX = borderX + Config::INTERVAL_ONE;
    int boardY = borderY + Config::INTERVAL_ONE;
    int sidebarX = borderX
             + Config::BOARD_WIDTH
             + 2 * Config::INTERVAL_ONE
             + Config::INTERVAL_TWO
             + Config::INTERVAL_ONE
             + Config::PEN_WIDTH * 5;
    int sidebarY = boardY;
    drawBorder(painter, borderX, borderY);
    drawBoard(painter, boardX, boardY, board);
    drawCurrentGroup(painter, boardX, boardY);
    drawSideBar(painter, sidebarX, sidebarY);
    if (canOverlay())
    {
        drawOverlay(painter);
    }
}

void GameView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
    case Qt::Key_A: emit moveLeftRequested(); break;

    case Qt::Key_Right:
    case Qt::Key_D: emit moveRightRequested(); break;

    case Qt::Key_Down:
    case Qt::Key_S: emit softDropRequested(); break;

    case Qt::Key_Up:
    case Qt::Key_W: emit rotateRequested(); break;
    
    case Qt::Key_Space: emit hardDropRequested(); break;

    case Qt::Key_P:
    case Qt::Key_Escape: emit pauseRequested(); break;

    default: QWidget::keyPressEvent(event); return;
    }
    event->accept();
}

void GameView::mouseMoveEvent(QMouseEvent *event)
{
    updateOverlayHover(event->pos());
    QWidget::mouseMoveEvent(event);
}

void GameView::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if (m_hoveredAction != OverlayAction::NONE)
    {
        m_hoveredAction = OverlayAction::NONE;
        setCursor(Qt::ArrowCursor);
        update();
    }
    QWidget::leaveEvent(event);
}

void GameView::mousePressEvent(QMouseEvent *event)
{
    if (canOverlay())
    {
        const OverlayAction action = hoveredOverlayAction(event->pos());
        switch (action)
        {
        case OverlayAction::RETRY: emit retryRequested(); break;
        case OverlayAction::BACK: emit backRequested(); break;
        case OverlayAction::SAVE: emit saveRequested(); break;
        case OverlayAction::EXIT: emit exitRequested(); break;
        default: break;
        }
    }
    QWidget::mousePressEvent(event);
}
