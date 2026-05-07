#include "gamewidget.h"
#include "gamescene.h"
#include "resourcemanager.h"
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QPainter>
#include <QTimer>
#include <QPixmap>
#include <QFileInfo>
#include <QIcon>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
    , m_scene(nullptr)
    , m_view(nullptr)
    , m_gameLoop(nullptr)
    , m_gameState(State_Idle)
    , m_overlay(nullptr)
    , m_statusLabel(nullptr)
    , m_actionBtn(nullptr)
{
    m_view = new QGraphicsView(this);
    m_scene = new GameScene(this);
    m_view->setScene(m_scene);
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setRenderHint(QPainter::Antialiasing);

    QString bgPath = ResourceManager::getImagePath("lv.png");
    if (QFileInfo(bgPath).exists()) {
        QPixmap bgPixmap(bgPath);
        m_view->setBackgroundBrush(QBrush(bgPixmap.scaled(1280, 720,
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    } else {
        m_view->setBackgroundBrush(QBrush(QColor(30, 30, 60)));
    }

    m_gameLoop = new QTimer(this);
    connect(m_gameLoop, &QTimer::timeout, m_scene, &GameScene::updateGame);

    connect(m_scene, &GameScene::ballFell, this, [this]() {
        setGameState(State_Lose);
        showGameOverOverlay("", "", QColor(200, 60, 60));
        emit gameLost();
    });

    connect(m_scene, &GameScene::targetHit, this, [this]() {
        setGameState(State_Lose);
        showGameOverOverlay("肥禽敢偷吃板面", "看我良子用胃袋夹碎他的脑袋！！！", QColor(255, 30, 20));
        emit gameLost();

        m_actionBtn->setIcon(QIcon());
        m_actionBtn->setIconSize(QSize(0, 0));
        m_actionBtn->setText("拷问肥勤");
        m_actionBtn->setStyleSheet(
            "QPushButton {"
            "background: transparent;"
            "border: 2px solid rgba(255, 255, 255, 0.5);"
            "color: white;"
            "font-size: 22px;"
            "font-family: 'SimHei';"
            "border-radius: 8px;"
            "}"
            "QPushButton:hover {"
            "background: rgba(255, 255, 255, 0.2);"
            "}"
        );
        disconnect(m_actionBtn, &QPushButton::clicked, nullptr, nullptr);
        connect(m_actionBtn, &QPushButton::clicked, this, [this]() {
            m_overlay->hide();
            emit interrogateClicked();
        });

        QMediaPlayer *snd = new QMediaPlayer(this);
        QAudioOutput *out = new QAudioOutput(snd);
        snd->setAudioOutput(out);
        snd->setSource(QUrl::fromLocalFile(ResourceManager::getAudioPath("wk.m4a")));
        connect(snd, &QMediaPlayer::playbackStateChanged, snd,
                [snd, out](QMediaPlayer::PlaybackState state) {
            if (state == QMediaPlayer::StoppedState) {
                snd->deleteLater();
                out->deleteLater();
            }
        });
        snd->play();
    });

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_view);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    m_overlay = new QWidget(this);
    m_overlay->setFixedSize(1280, 720);
    m_overlay->setStyleSheet("background: rgba(0, 0, 0, 0.7);");
    m_overlay->hide();

    QVBoxLayout *overlayLayout = new QVBoxLayout(m_overlay);
    overlayLayout->setAlignment(Qt::AlignCenter);

    m_statusLabel = new QLabel(m_overlay);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet(
        "QLabel { background: transparent; color: white; font-size: 20px; font-family: 'SimHei'; }"
    );
    overlayLayout->addWidget(m_statusLabel);

    QString iconPath = ResourceManager::getImagePath("fan2.jpg");
    QPixmap btnPixmap;
    bool hasIcon = QFileInfo(iconPath).exists();
    if (hasIcon) {
        btnPixmap = QPixmap(iconPath);
    }

    m_actionBtn = new QPushButton(m_overlay);
    m_actionBtn->setFixedSize(240, 100);
    m_actionBtn->setCursor(Qt::PointingHandCursor);
    if (hasIcon) {
        m_actionBtn->setIcon(QIcon(btnPixmap.scaled(240, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
        m_actionBtn->setIconSize(QSize(240, 100));
        m_actionBtn->setStyleSheet(
            "QPushButton {"
            "background: transparent;"
            "border: none;"
            "}"
            "QPushButton:hover {"
            "background: rgba(255, 255, 255, 0.2);"
            "border-radius: 8px;"
            "}"
        );
    } else {
        m_actionBtn->setStyleSheet(
            "QPushButton {"
            "background: rgba(255, 255, 255, 0.15);"
            "border: 2px solid rgba(255, 255, 255, 0.5);"
            "border-radius: 40px;"
            "color: white;"
            "font-size: 16px;"
            "}"
            "QPushButton:hover {"
            "background: rgba(255, 255, 255, 0.35);"
            "border-color: white;"
            "}"
        );
        m_actionBtn->setText("↻");
    }
    overlayLayout->addWidget(m_actionBtn, 0, Qt::AlignCenter);

    connect(m_actionBtn, &QPushButton::clicked, this, [this]() {
        if (m_gameState == State_Lose) {
            m_overlay->hide();
            resetGame();
            startGame();
            emit gameRestart();
        }
    });

    setFocusPolicy(Qt::StrongFocus);
}

GameWidget::~GameWidget()
{
    if (m_gameLoop->isActive()) {
        m_gameLoop->stop();
    }
}

void GameWidget::startGame()
{
    m_scene->initializeGame();
    setGameState(State_Playing);
    m_gameLoop->start(16);
    setFocus();
}

void GameWidget::resetGame()
{
    if (m_gameLoop->isActive()) {
        m_gameLoop->stop();
    }
    m_scene->resetGame();
    setGameState(State_Idle);
}

void GameWidget::setGameState(GameState state)
{
    m_gameState = state;
}

void GameWidget::showGameOverOverlay(const QString &title, const QString &subtitle,
                                      const QColor &color)
{
    m_statusLabel->setText(title + "\n\n" + subtitle);
    m_statusLabel->setStyleSheet(
        QString("QLabel { background: transparent; color: %1; font-size: 20px; font-family: 'SimHei'; }")
            .arg(color.name())
    );
    m_overlay->show();
}

void GameWidget::keyPressEvent(QKeyEvent *event)
{
    if (m_gameState == State_Playing) {
        if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left) {
            m_scene->setMoveLeft(true);
        } else if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right) {
            m_scene->setMoveRight(true);
        } else if (event->key() == Qt::Key_Space) {
            m_scene->launchBall();
        } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            setGameState(State_Lose);
            showGameOverOverlay("肥勤居然敢偷吃板面", "看我良子用胃袋夹碎他的脑袋", QColor(255, 30, 20));
            emit gameLost();

            m_actionBtn->setIcon(QIcon());
            m_actionBtn->setIconSize(QSize(0, 0));
            m_actionBtn->setText("拷问肥勤");
            m_actionBtn->setStyleSheet(
                "QPushButton {"
                "background: transparent;"
                "border: 2px solid rgba(255, 255, 255, 0.5);"
                "color: white;"
                "font-size: 22px;"
                "font-family: 'SimHei';"
                "border-radius: 8px;"
                "}"
                "QPushButton:hover {"
                "background: rgba(255, 255, 255, 0.2);"
                "}"
            );
            disconnect(m_actionBtn, &QPushButton::clicked, nullptr, nullptr);
            connect(m_actionBtn, &QPushButton::clicked, this, [this]() {
                m_overlay->hide();
                emit interrogateClicked();
            });

            QMediaPlayer *snd = new QMediaPlayer(this);
            QAudioOutput *out = new QAudioOutput(snd);
            snd->setAudioOutput(out);
            snd->setSource(QUrl::fromLocalFile(ResourceManager::getAudioPath("wk.m4a")));
            connect(snd, &QMediaPlayer::playbackStateChanged, snd,
                    [snd, out](QMediaPlayer::PlaybackState state) {
                if (state == QMediaPlayer::StoppedState) {
                    snd->deleteLater();
                    out->deleteLater();
                }
            });
            snd->play();
        }
    }
    QWidget::keyPressEvent(event);
}

void GameWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (m_gameState == State_Playing) {
        if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left) {
            m_scene->setMoveLeft(false);
        } else if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right) {
            m_scene->setMoveRight(false);
        }
    }
    QWidget::keyReleaseEvent(event);
}
