#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include "gamescene.h"

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    enum GameState {
        State_Idle,
        State_Playing,
        State_Lose
    };

    explicit GameWidget(QWidget *parent = nullptr);
    ~GameWidget();

    void startGame();
    void resetGame();
    GameState getGameState() const { return m_gameState; }

signals:
    void gameLost();
    void gameRestart();
    void interrogateClicked();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void setGameState(GameState state);
    void showGameOverOverlay(const QString &title, const QString &subtitle,
                              const QColor &color);

    GameScene *m_scene;
    QGraphicsView *m_view;
    QTimer *m_gameLoop;
    GameState m_gameState;
    QWidget *m_overlay;
    QLabel *m_statusLabel;
    QPushButton *m_actionBtn;
};

#endif // GAMEWIDGET_H
