#include "gamescene.h"
#include <QGraphicsSceneMouseEvent>
#include <QtMath>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include "resourcemanager.h"

GameScene::GameScene(QObject *parent)
    : QGraphicsScene(parent)
    , m_daniel(nullptr)
    , m_ball(nullptr)
    , m_target(nullptr)
    , m_moveLeft(false)
    , m_moveRight(false)
    , m_ballLaunched(false)
    , m_targetHit(false)
{
    setSceneRect(0, 0, 1280, 720);
}

GameScene::~GameScene()
{
}

void GameScene::initializeGame()
{
    clear();
    m_bricks.clear();
    m_ballLaunched = false;
    m_targetHit = false;
    m_moveLeft = false;
    m_moveRight = false;

    m_daniel = new DanielPlayer();
    m_daniel->setPos(640, 640);
    m_daniel->setArmImagePath("s2.png", "s1.png");
    addItem(m_daniel);

    m_ball = new BallItem();
    m_ball->setPos(640, 600);
    m_ball->setImagePath("dou1.jpg");
    addItem(m_ball);

    m_target = new FanXiaoqin();
    m_target->setPos(640, 207);
    m_target->setImagePath("fan1.jpg");
    addItem(m_target);

    int brickSize = 50;
    int gap = 4;
    int cols = 9;
    int rows = 7;
    int totalW = cols * (brickSize + gap) - gap;
    int startX = (1280 - totalW) / 2;
    int startY = 20;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            bool isEdgeRow = (row <= 1 || row >= rows - 2);
            bool isEdgeCol = (col <= 1 || col >= cols - 2);

            if (isEdgeRow || isEdgeCol) {
                BrickItem *brick = new BrickItem(row, col, BrickItem::Type_Normal);
                brick->setPos(startX + col * (brickSize + gap),
                              startY + row * (brickSize + gap));
                brick->setRect(0, 0, brickSize, brickSize);
                brick->setImagePath("sao.png");
                addItem(brick);
                m_bricks.append(brick);
            }
        }
    }
}

void GameScene::resetGame()
{
    initializeGame();
}

void GameScene::updateGame()
{
    qreal speed = 8.0;

    if (m_moveLeft) {
        m_daniel->moveLeft(speed);
    }
    if (m_moveRight) {
        m_daniel->moveRight(speed);
    }

    if (m_ballLaunched) {
        m_ball->move();

        qreal ballX = m_ball->x();
        qreal ballY = m_ball->y();
        qreal ballRadius = m_ball->getRadius();

        if (ballX - ballRadius <= 0) {
            m_ball->setPos(ballRadius, ballY);
            m_ball->bounceHorizontal();
        } else if (ballX + ballRadius >= 1280) {
            m_ball->setPos(1280 - ballRadius, ballY);
            m_ball->bounceHorizontal();
        }

        if (ballY - ballRadius <= 0) {
            m_ball->setPos(ballX, ballRadius);
            m_ball->bounceVertical();
        }

        if (ballY - ballRadius >= 720) {
            emit ballFell();
            return;
        }

        QList<QGraphicsRectItem*> bats = m_daniel->getBatItems();
        for (QGraphicsRectItem *bat : bats) {
            QRectF batRect = bat->sceneBoundingRect();
            if (batRect.intersects(QRectF(ballX - ballRadius, ballY - ballRadius,
                                          ballRadius * 2, ballRadius * 2))) {
                applyBatBounce(bat);
                break;
            }
        }

        for (BrickItem *brick : m_bricks) {
            if (brick->isDestroyed()) continue;
            QRectF brickRect = brick->sceneBoundingRect();
            if (brickRect.intersects(QRectF(ballX - ballRadius, ballY - ballRadius,
                                            ballRadius * 2, ballRadius * 2))) {
                applyBrickBounce(brick);
                brick->hit();
                break;
            }
        }

        if (!m_targetHit) {
            if (m_target->checkCollision(QPointF(ballX, ballY), ballRadius)) {
                m_targetHit = true;
                emit targetHit();
            }
        }
    } else {
        m_ball->setPos(m_daniel->getX(), m_daniel->getY() - 40);
    }
}

void GameScene::applyBatBounce(QGraphicsRectItem *bat)
{
    qreal ballX = m_ball->x();
    qreal batCenterX = bat->sceneBoundingRect().center().x();
    qreal batWidth = bat->sceneBoundingRect().width();

    qreal offsetFromCenter = (ballX - batCenterX) / (batWidth / 2.0);

    qreal angle = offsetFromCenter * 60.0;
    qreal angleRad = angle * M_PI / 180.0;

    qreal speed = qSqrt(m_ball->getVelocity().x() * m_ball->getVelocity().x()
                        + m_ball->getVelocity().y() * m_ball->getVelocity().y());

    qreal newVx = speed * qSin(angleRad);
    qreal newVy = -speed * qCos(angleRad);
    m_ball->setVelocity(newVx, newVy);

    qreal speedMult = m_daniel->getSpeedMultiplier();
    if (speedMult > 1.0) {
        m_ball->setSpeedBoost(speedMult);
    }
}

void GameScene::applyBrickBounce(BrickItem *brick)
{
    qreal ballX = m_ball->x();
    qreal ballY = m_ball->y();
    qreal ballRadius = m_ball->getRadius();
    QRectF brickRect = brick->sceneBoundingRect();

    qreal overlapLeft = (ballX + ballRadius) - brickRect.left();
    qreal overlapRight = brickRect.right() - (ballX - ballRadius);
    qreal overlapTop = (ballY + ballRadius) - brickRect.top();
    qreal overlapBottom = brickRect.bottom() - (ballY - ballRadius);

    qreal minOverlap = qMin(qMin(overlapLeft, overlapRight), qMin(overlapTop, overlapBottom));

    if (minOverlap == overlapLeft || minOverlap == overlapRight) {
        m_ball->bounceHorizontal();
    } else {
        m_ball->bounceVertical();
    }
}

bool GameScene::allBricksDestroyed() const
{
    for (const BrickItem *brick : m_bricks) {
        if (!brick->isDestroyed()) {
            return false;
        }
    }
    return true;
}

int GameScene::remainingBricks() const
{
    int count = 0;
    for (const BrickItem *brick : m_bricks) {
        if (!brick->isDestroyed()) count++;
    }
    return count;
}

void GameScene::setMoveLeft(bool move)
{
    m_moveLeft = move;
}

void GameScene::setMoveRight(bool move)
{
    m_moveRight = move;
}

void GameScene::launchBall()
{
    if (!m_ballLaunched) {
        m_ballLaunched = true;
        m_ball->launch();
    }
}

void GameScene::setArmsClamped(bool clamped)
{
    if (clamped) {
        m_daniel->clampArms();
    } else {
        m_daniel->unclampArms();
    }
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        setArmsClamped(true);

        QMediaPlayer *sndPlayer = new QMediaPlayer(this);
        QAudioOutput *sndOutput = new QAudioOutput(sndPlayer);
        sndPlayer->setAudioOutput(sndOutput);
        sndPlayer->setSource(QUrl::fromLocalFile(ResourceManager::getAudioPath("danni.m4a")));
        connect(sndPlayer, &QMediaPlayer::playbackStateChanged, sndPlayer,
                [sndPlayer, sndOutput](QMediaPlayer::PlaybackState state) {
            if (state == QMediaPlayer::StoppedState) {
                sndPlayer->deleteLater();
                sndOutput->deleteLater();
            }
        });
        sndPlayer->play();
    }
}

void GameScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton) {
        setArmsClamped(false);
    }
}
