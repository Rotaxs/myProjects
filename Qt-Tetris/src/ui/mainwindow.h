#pragma once

#include "ui/GameView.h"
#include "logic/Board.h"
#include "logic/GameManager.h"

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    GameView *m_gameView;
    GameManager *m_gameManager;
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
