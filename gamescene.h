#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include "danielplayer.h"
#include "ballitem.h"
#include "brickitem.h"
#include "fanxiaoqin.h"

class GameScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GameScene(QObject *parent = nullptr);
    ~GameScene();

    void initializeGame();
    void resetGame();
    void updateGame();
    void setMoveLeft(bool move);
    void setMoveRight(bool move);
    void launchBall();
    void setArmsClamped(bool clamped);
    int remainingBricks() const;

    DanielPlayer *daniel() const { return m_daniel; }
    BallItem *ball() const { return m_ball; }
    FanXiaoqin *target() const { return m_target; }
    const QList<BrickItem*> &bricks() const { return m_bricks; }

signals:
    void ballFell();
    void targetHit();
    void bricksCleared();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void applyBatBounce(QGraphicsRectItem *bat);
    void applyBrickBounce(BrickItem *brick);
    bool allBricksDestroyed() const;

    DanielPlayer *m_daniel;
    BallItem *m_ball;
    FanXiaoqin *m_target;
    QList<BrickItem*> m_bricks;
    bool m_moveLeft;
    bool m_moveRight;
    bool m_ballLaunched;
    bool m_targetHit;
};

#endif // GAMESCENE_H
