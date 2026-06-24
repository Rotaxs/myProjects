#include "mainwindow.h"

#include <QGuiApplication>
#include <QMessageBox>
#include <QScreen>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 中央部件
    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // 初始化 gameView 和 gameManager
    m_gameView = new GameView(centralWidget);
    m_gameManager = new GameManager();

    m_gameView->setBoard(&m_gameManager->board());
    m_gameView->setCurrentGroup(&m_gameManager->currentGroup());
    m_gameView->setNextGroup(&m_gameManager->nextGroup());
    m_gameView->setScore(&m_gameManager->score());
    m_gameView->setOverlayMode(GameView::OverlayMode::NONE);
    
    layout->addWidget(m_gameView);
    
    // UI 层与逻辑层的通信
    connect(m_gameManager, &GameManager::stateChange, m_gameView, [this]() {
        m_gameView->setElapsedMs(m_gameManager->elapsedMs());
        // 将 GameState 映射为 OverlayMode
        GameView::OverlayMode mode = GameView::OverlayMode::NONE;
        switch (m_gameManager->state())
        {
        case GameManager::GameState::PAUSED:
            mode = GameView::OverlayMode::PAUSED;
            break;
        case GameManager::GameState::OVER:
            mode = GameView::OverlayMode::GAME_OVER;
            break;
        default: break;
        }
        m_gameView->setOverlayMode(mode);
        m_gameView->update();
    });
    connect(m_gameView, &GameView::moveLeftRequested, m_gameManager, [this]() {
        m_gameManager->moveLeft();
    });
    connect(m_gameView, &GameView::moveRightRequested, m_gameManager, [this]() {
        m_gameManager->moveRight(); 
    });
    connect(m_gameView, &GameView::softDropRequested, m_gameManager, [this]() {
        m_gameManager->moveDown();
    });
    connect(m_gameView, &GameView::rotateRequested, m_gameManager, [this]() {
        m_gameManager->rotate();
    });
    connect(m_gameView, &GameView::hardDropRequested, m_gameManager, [this]() {
        m_gameManager->hardDrop();
    });
    connect(m_gameView, &GameView::pauseRequested, m_gameManager, [this]() {
        m_gameManager->pause();
    });
    connect(m_gameView, &GameView::retryRequested, m_gameManager, [this]() {
        m_gameManager->restart();
        m_gameView->setFocus();
    });
    connect(m_gameView, &GameView::backRequested, m_gameManager, [this]() {
        m_gameManager->back();
        m_gameView->setFocus();
    });
    connect(m_gameView, &GameView::exitRequested, this, [this]() {
        close();
    });
    connect(m_gameView, &GameView::saveRequested, this, [this]() {
        const bool saved = m_gameManager->saveGame();
        if (saved)
            QMessageBox::information(this, "Save Game", "Save successful.");
        else
            QMessageBox::warning(this, "Save Game", "Save failed.");
        m_gameView->setFocus();
    });

    if (!m_gameManager->loadGame())
        m_gameManager->start();

    // 设置屏幕大小和屏幕位置
    resize(Config::GAME_WIDTH, Config::GAME_HEIGHT);
    // 获取当前所在屏幕
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        // 获取屏幕可用区域
        QRect screenGeometry = screen->availableGeometry();
        // 计算居中坐标
        int x = (screenGeometry.width() - this->width()) / 2 + screenGeometry.x();
        int y = (screenGeometry.height() - this->height()) / 2 + screenGeometry.y();
        this->move(x, y);
    }
}

MainWindow::~MainWindow()
{
}
