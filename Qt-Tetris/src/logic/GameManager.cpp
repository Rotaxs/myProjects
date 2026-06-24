#include "logic/GameManager.h"
#include "utils/SaveGameUtils.h"

#include <QTimer>
#include <QRandomGenerator>
#include <QDebug>

#include <algorithm>

GameManager::GameManager(QObject *parent) :
    QObject(parent),
    m_score(0), m_state(GameState::READY), m_elapsedMs(0),
    m_fallGap(1000 / Config::FALL_SPEED),
    m_board(Config::BOARD_ROWS,
            Config::BOARD_COLS,
            Config::BLOCK_WIDTH,
            Config::BOARD_BLOCK_COLOR,
            Config::BOARD_BLOCK_FRAME_COLOR)
{
    connect(&m_timer, &QTimer::timeout, this, &GameManager::tick);
    spawnFirstGroup();
}

GameManager::~GameManager() { }

/**
 * @brief 获取游戏进行的时间
 * @return 游戏进行的时间
 */
const int GameManager::elapsedMs() const
{
    if (m_state == GameState::RUNNING)
        return m_elapsedMs + m_elapsedTimer.elapsed();
    return m_elapsedMs;
}

/**
 * @brief 清空棋盘
 */
void GameManager::clearBoard()
{
    for (int row = 0; row < m_board.getRows(); row++)
    {
        for (int col = 0; col < m_board.getCols(); col++)
        {
            resetCell(row, col);
        }
    }
}

/**
 * @brief 保存游戏，首先判断能否存盘
 * @return 如果保存成功，范围 true，否则返回 false
 */
bool GameManager::saveGame() const
{
    if (m_state != GameState::RUNNING && m_state != GameState::PAUSED)
        return false;

    // 必要的状态信息
    SaveGameUtils::SavedGameData data;
    data.version = 1;
    data.rows = m_board.getRows();
    data.cols = m_board.getCols();
    data.score = m_score;
    data.elapsedMs = elapsedMs();
    data.fallGap = m_fallGap;
    data.stateName = (m_state == GameState::PAUSED ? "PAUSED" : "RUNNING");
    data.currentGroup = m_currentGroup;
    data.nextGroup = m_nextGroup;
    // 已经锁定的方块
    for (int row = 0; row < m_board.getRows(); row++)
    {
        for (int col = 0; col < m_board.getCols(); col++)
        {
            const Block &cell = m_board[row][col];
            if (!cell.isOccupied()) continue;

            data.boardCells.append({
                row,
                col,
                cell.getColor(),
                cell.getFrameColor()
            });
        }
    }

    const QString path = SaveGameUtils::defaultSaveFilePath();
    if (!SaveGameUtils::writeSaveFile(path, data))
    {
        qDebug() << "GameManager::saveGame: 写入存档失败" << path;
        return false;
    }

    qDebug() << "GameManager::saveGame: 存档成功" << path;
    return true;
}

/**
 * @brief 读取存档，并判断能否读取
 * @return 如果读取失败，返回 false，否则返回 true
 */
bool GameManager::loadGame()
{
    const QString path = SaveGameUtils::defaultSaveFilePath();
    SaveGameUtils::SavedGameData data;
    if (!SaveGameUtils::readSaveFile(path, data))
        return false;

    if (data.version != 1)
    {
        qDebug() << "GameManager::loadGame: 存档版本不匹配";
        return false;
    }
    if (data.rows != m_board.getRows() ||
        data.cols != m_board.getCols())
    {
        qDebug() << "GameManager::loadGame: 存档棋盘尺寸与当前配置不匹配";
        return false;
    }

    m_timer.stop();
    clearBoard();
    // 读取锁定的方块
    for (const SaveGameUtils::SavedCell &cell : data.boardCells)
    {
        if (cell.row < 0 || cell.row >= m_board.getRows() ||
            cell.col < 0 || cell.col >= m_board.getCols())
            return false;

        m_board[cell.row][cell.col]
            .setPosX(cell.col)
            .setPosY(cell.row)
            .setOccupied(true)
            .setColor(cell.color)
            .setFrameColor(cell.frameColor);
    }
    
    m_currentGroup = data.currentGroup;
    m_nextGroup = data.nextGroup;
    m_score = data.score;
    m_elapsedMs = data.elapsedMs;
    m_fallGap = data.fallGap;
    // 进入游戏时直接继续，不停在 PAUSED 面板。
    m_state = GameState::RUNNING;
    m_elapsedTimer.start();
    m_timer.start(m_fallGap);
    emit stateChange();

    qDebug() << "GameManager::loadGame: 读档成功" << path;
    return true;
}

/**
 * @brief 清空保存的存档
 */
void GameManager::clearSaveGame() const
{
    const QString path = SaveGameUtils::defaultSaveFilePath();
    if (!SaveGameUtils::clearSaveFile(path))
    {
        qDebug() << "GameManager::clearSaveGame: 无法清空存档文件" << path;
        return;
    }
    qDebug() << "GameManager::clearSaveGame: 已清空存档" << path;
}

/**
 * @brief 在游戏开始前的初始化，便于 spawnNextGroup 中 currentGroup 直接继承 nextGroup
 * @param spawnX, spawnY 方块生成的坐标
 */
void GameManager::spawnFirstGroup(int spawnX, int spawnY)
{
    if (m_state != GameState::READY) return;
    int shape_id = QRandomGenerator::global()->bounded(7);
    BlockGroup::Shape nextShape = static_cast<BlockGroup::Shape>(shape_id);
    m_nextGroup = BlockGroup(spawnX, spawnY, nextShape);
    
    qDebug() << "GameManager::spawnFirstGroup: 生成第一个 BlockGroup，形状为" 
             << BlockGroup::getShapeName(m_nextGroup.getShape());
}

/**
 * @brief 生成下一个 BlockGroup
 * @param spawnX, spawnY 方块生成的坐标
 */
void GameManager::spawnNextGroup(int spawnX, int spawnY)
{
    // 生成当前方块
    m_currentGroup = m_nextGroup;
    // 生成下一个方块
    int shape_id = QRandomGenerator::global()->bounded(7);
    BlockGroup::Shape neShape = static_cast<BlockGroup::Shape>(shape_id);
    m_nextGroup = BlockGroup(spawnX, spawnY, neShape);

    qDebug() << "GameManager::spawnNextGroup: 成功生成一个 BlockGroup，形状为" 
             << BlockGroup::getShapeName(m_currentGroup.getShape());
    qDebug() << "GameManager::spawnNextGroup: 下一个 BlockGroup 的形状为" 
             << BlockGroup::getShapeName(m_nextGroup.getShape());
}

/**
 * @brief 判断 currentGroup 能否朝 (dx, dy) 移动
 * @param dx, dy 方向向量
 */
bool GameManager::canMove(const int &dx, const int &dy)
{
    for (int i = 0; i < 4; i++)
    {
        const Block &cell = m_currentGroup[i];
        int row = cell.getY() + dy;
        int col = cell.getX() + dx;
        // if (row < 0) continue;
        if (!isPosValid(row, col)) 
            return false;
    }
    return true;
}

/**
 * @brief 使 currentGroup 朝 (dx, dy) 的方向移动
 * @param dx, dy 方向向量
 */
void GameManager::moveBy(const int &dx, const int &dy)
{
    if (m_state != GameState::RUNNING) return;
    if (!canMove(dx, dy))
    {
        qDebug() << "GameManager::moveBy: 方块越界或无法移动";
        return;
    }
    m_currentGroup.addPosBy(dx, dy);
    emit stateChange();
}

/**
 * @brief 使 currentGroup 向左移动
 */
void GameManager::moveLeft()
{ 
    if (m_state != GameState::RUNNING) return;
    moveBy(-1, 0); 
    qDebug() << "GameManager::moveLeft: 方块向左移动";
}

/**
 * @brief 使 currentGroup 向右移动
 */
void GameManager::moveRight()
{ 
    if (m_state != GameState::RUNNING) return;
    moveBy(1, 0); 
    qDebug() << "GameManager::moveRight: 方块向右移动";
}

/**
 * @brief 使 currentGroup 向下移动
 */
void GameManager::moveDown()
{ 
    if (m_state != GameState::RUNNING) return;
    moveBy(0, 1); 
    qDebug() << "GameManager::moveLeft: 方块向下移动";
}

/**
 * @brief 使 currentGroup 向上移动
 */
void GameManager::moveUp()
{
    if (m_state != GameState::RUNNING) return;
    moveBy(0, -1);
    qDebug() << "GameManager::moveUp: 方块向上移动";
}

/**
 * @brief 判断坐标是否在棋盘内
 * @param row, col 要判断的坐标
 * @return 如果坐标合法，返回 true，否则返回 false
 */
bool GameManager::isPosValid(const int &row, const int &col)
{
    if (col < 0 || col >= m_board.getCols())
        return false;
    if (row >= m_board.getRows())
        return false;
    if (row < 0)
        return true;

    return !m_board[row][col].isOccupied();
}

/**
 * @brief 判断方块偏移 (offsetX, offsetY) 后能否旋转
 * @param offsetX, offsetY 偏移坐标
 * @return 如果能旋转，返回 true，否则返回 false
 */
bool GameManager::canRotate(const int &offsetX, const int &offsetY)
{
    int curRotateState = m_currentGroup.getRotateState();
    int shape_id = static_cast<int>(m_currentGroup.getShape());
    int nxtRotateState = (curRotateState + 1) % 4;
    for (int i = 0; i < 4; i++)
    {
        const Block &state = ROTATION_SHAPE[shape_id][nxtRotateState][i];
        int row = state.getY() + m_currentGroup.getPosY() + offsetY;
        int col = state.getX() + m_currentGroup.getPosX() + offsetX;
        if (!isPosValid(row, col))
            return false;
    }
    return true;
}

/**
 * @brief 使 currentGroup 旋转，如果在边界处旋转会越界，则先尝试偏移后再旋转，如果还是无法旋转，就不旋转
 */
void GameManager::rotate()
{
    if (m_state != GameState::RUNNING) return;
    if (canRotate(0, 0))
    {
    }
    else if (canRotate(1, 0))
    {
        m_currentGroup.addPosBy(1, 0);
    }
    else if (canRotate(-1, 0))
    {
        m_currentGroup.addPosBy(-1, 0);
    }
    else if (canRotate(0, -1))
    {
        m_currentGroup.addPosBy(0, -1);
    }
    else if (m_currentGroup.getShape() == BlockGroup::Shape::SHAPE_I && canRotate(2, 0))
    {
        m_currentGroup.addPosBy(2, 0);
    }
    else
    {
        qDebug() << "GameManager::rotate: 无法旋转";
        return;
    }
    m_currentGroup.toNextRotationState();
    qDebug() << "GameManager::rotate: 方块旋转";
    emit stateChange();
}

/**
 * @brief 方块硬降：方块直接到达底部合法位置
 */
void GameManager::hardDrop()
{
    if (m_state != GameState::RUNNING) return;
    while (canMove(0, 1))
    {
        m_currentGroup.addPosBy(0, 1);
    }

    qDebug() << "GameManager::hardDrop: 方块移动到底部";
    nextRound();
}

/**
 * @brief 判断第 row 行是否满了
 * @param row 要判断的行
 * @param 如果满了，返回 true，否则返回 false
 */
bool GameManager::isFullRow(const int &row) const
{
    for (int col = 0; col < m_board.getCols(); col++)
    {
        if (!m_board[row][col].isOccupied())
            return false;
    }
    return true;
}

/**
 * @brief 重置棋盘上的 cell
 * @param row, col 要重置的 cell 的坐标
 */
void GameManager::resetCell(const int &row, const int &col)
{
    m_board[row][col].setPosX(col).setPosY(row).setOccupied(false)
                     .setColor(Config::BOARD_BLOCK_COLOR).setFrameColor(Config::BOARD_BLOCK_FRAME_COLOR);
}

/**
 * @brief 复制第 srcRow 行到第 dstRow 行
 * @param dstRow 目标行
 * @param srcRow 要复制的行
 */
void GameManager::copyRow(const int &dstRow, const int &srcRow)
{
    for (int col = 0; col < m_board.getCols(); col++)
    {
        m_board[dstRow][col] = m_board[srcRow][col];
        m_board[dstRow][col].setPosX(col).setPosY(dstRow);
    }
}

/**
 * @brief 清空满的行
 * @return 放回成功消除的行数
 */
int GameManager::clearFullRows()
{
    int rows = 0;
    for (int row = m_board.getRows() - 1; row >= 0; row--)
    {
        if (!isFullRow(row)) continue;
        rows++;
        for (int r = row; r > 0; r--)
            copyRow(r, r - 1);
        for (int col = 0; col < m_board.getCols(); col++)
            resetCell(0, col);
        // 处理连续消行的情况
        // 当上一行和当前行都满时，清除当前行，上一行满的就会复制下来
        // 使用 row++ 使得下一层判断时还是再当前行，避免漏清除的情况
        row++;
    }
    return rows;
}

/**
 * @brief 处理每个 tick 要处理的事件，比如方块下落和检测消行等
 */
void GameManager::tick()
{
    if (m_state != GameState::RUNNING) return;
    if (!canMove(0, 1))
    {
        qDebug() << "GameManager::tick: 方块无法继续下落";
        nextRound();
        return;
    }
    moveDown();
}

/**
 * @brief 当前方块触底后，需要生成新的方块，进入下一轮
 */
void GameManager::nextRound()
{
    // 判断锁定时新的方块是否越界
    if (isLockOut())
    {
        m_timer.stop();
        m_state = GameState::OVER;
        // 游戏结束时清空存档
        clearSaveGame();
        emit stateChange();
        return;
    }
    // 方块无法移动时将其锁定再 board 中
    lock();
    // 根据消除的行数积分
    int lines = clearFullRows();
    if (lines)
        addScore(lines);
    spawnNextGroup();
    
    if (isOver()) 
    {
        m_timer.stop();
        m_state = GameState::OVER;
        clearSaveGame();
        emit stateChange();
        return;
    }

    emit stateChange();
}

/**
 * @brief 更新分数
 * @param lines 消除的行数
 */
void GameManager::addScore(const int &lines)
{
    int baseScore = Config::BASE_SCORE;
    int gotScore = 0;
    switch (lines)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4: gotScore = lines * baseScore; break;
    default: gotScore += 4 * baseScore; break;
    }
    m_score += gotScore;
    qDebug().noquote() << QString("GameManager::addScore: 消除了 %1 行，获得了 %2 分").arg(lines).arg(gotScore);
}

/**
 * @brief 当方块无法移动时，锁定当前方块，将其写入到 board 中
 */
void GameManager::lock()
{
    for (int i = 0; i < 4; i++)
    {
        const Block &cell = m_currentGroup[i];
        int row = cell.getY();
        int col = cell.getX();
        if (row < 0) continue;
        m_board[row][col] = cell;
    }
    qDebug() << "GameManager::block: 方块已锁定";
}

/**
 * @brief 判断锁定时方块是否超出顶部
 * @return 如果越界，返回 true，否则返回 false
 */
bool GameManager::isLockOut() const
{
    for (int i = 0; i < 4; i++)
    {
        if (m_currentGroup[i].getY() < 0)
        {
            qDebug() << "GameManager::isLockOut: 方块锁定时超出顶部，游戏结束";
            return true;
        }
    }
    return false;
}

/**
 * @brief 检查游戏是否结束，即判断是否右方块越界
 * @return 如果有方块，返回 true，否则返回 false
 */
bool GameManager::isOver()
{
    for (int i = 0; i < 4; i++)
    {
        const Block &cell = m_currentGroup[i];
        int row = cell.getY();
        int col = cell.getX();
        if (!isPosValid(row, col))
        {
            qDebug() << "GameManger::isOver: 游戏结束";
            return true;
        }
    }
    return false;
}

/**
 * @brief 游戏暂停
 */
void GameManager::pause()
{
    if (m_state != GameState::RUNNING) return;
    m_elapsedMs += m_elapsedTimer.elapsed();
    m_state = GameState::PAUSED;
    m_timer.stop();
    emit stateChange();
}

/**
 * @brief 游戏重新开始
 */
void GameManager::restart()
{
    m_timer.stop();
    clearBoard();
    m_score = 0;
    m_elapsedMs = 0;
    m_state = GameState::READY;
    spawnFirstGroup();
    start();
}

/**
 * @brief 从暂停界面返回到游戏
 */
void GameManager::back()
{
    if (m_state != GameState::PAUSED) return;
    m_state = GameState::RUNNING;
    m_elapsedTimer.start();
    m_timer.start(m_fallGap);
    emit stateChange();
}

/**
 * @brief 开始游戏
 */
void GameManager::start()
{
    if (m_state != GameState::READY) return;
    m_state = GameState::RUNNING;
    m_elapsedMs = 0;
    m_elapsedTimer.start();
    spawnNextGroup();
    m_timer.start(m_fallGap);
    emit stateChange();
}
