#include "widget.h"
#include "./ui_widget.h"
#include "resourcemanager.h"
#include "dialogmanager.h"
#include "gamewidget.h"
#include <QUrl>
#include <QFileInfo>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsVideoItem>
#include <QPixmap>
#include <QPainter>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QLabel>
#include <QKeyEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QPushButton>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , m_stackedWidget(nullptr)
    , m_mediaPlayer(nullptr)
    , m_audioOutput(nullptr)
    , m_bgAudioPlayer(nullptr)
    , m_bgAudioOutput(nullptr)
    , m_pageAudioPlayer(nullptr)
    , m_pageAudioOutput(nullptr)
    , m_interrogationBgPlayer(nullptr)
    , m_interrogationBgOutput(nullptr)
    , m_graphicsView(nullptr)
    , m_dialogLabel(nullptr)
    , m_dialogManager(nullptr)
    , m_gameWidget(nullptr)
    , m_gameActive(false)
    , m_interrogationMode(false)
    , m_endScreenActive(false)
{
    ui->setupUi(this);
    setFixedSize(1280, 720);

    m_stackedWidget = new QStackedWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_stackedWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    QWidget *startPage = new QWidget(this);
    startPage->setFixedSize(1280, 720);

    QGraphicsScene *scene = new QGraphicsScene(startPage);
    scene->setSceneRect(0, 0, 1280, 720);

    QGraphicsView *graphicsView = new QGraphicsView(scene, startPage);
    graphicsView->setGeometry(0, 0, 1280, 720);
    graphicsView->setFrameShape(QFrame::NoFrame);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setBackgroundBrush(Qt::NoBrush);

    QString bgImagePath = ResourceManager::getImagePath("Gemini_Generated_Image_wqj06awqj06awqj0.png");
    QPixmap bgPixmap(bgImagePath);
    if (!bgPixmap.isNull()) {
        scene->addPixmap(bgPixmap.scaled(1280, 720, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }

    QString btnImagePath = ResourceManager::getImagePath("begin.png");
    ui->startBtn->setParent(startPage);
    ui->startBtn->setFixedSize(200, 60);
    ui->startBtn->setText("开始游戏");
    ui->startBtn->setStyleSheet(
        QString("QPushButton {"
            "border-image: url(%1) 0 0 0 0 stretch stretch;"
            "color: white;"
            "font-size: 24px;"
            "font-weight: bold;"
            "border: none;"
        "}")
        .arg(btnImagePath)
    );
    ui->startBtn->move(50, 620);

    m_stackedWidget->addWidget(startPage);

    m_gameWidget = new GameWidget(this);
    m_gameWidget->setFixedSize(1280, 720);
    m_stackedWidget->addWidget(m_gameWidget);

    m_stackedWidget->setCurrentWidget(startPage);

    playBackgroundMusic();

    connect(ui->startBtn, &QPushButton::clicked, this, &Widget::onStartGameClicked);
    connect(m_gameWidget, &GameWidget::gameLost, this, &Widget::onMiniGameLost);
    connect(m_gameWidget, &GameWidget::interrogateClicked, this, &Widget::onInterrogateClicked);
}

void Widget::playBackgroundMusic()
{
    QString bgMusicPath = ResourceManager::getAudioPath("background.mp3");
    if (QFileInfo(bgMusicPath).exists()) {
        m_bgAudioOutput = new QAudioOutput(this);
        m_bgAudioPlayer = new QMediaPlayer(this);
        m_bgAudioPlayer->setAudioOutput(m_bgAudioOutput);
        m_bgAudioPlayer->setLoops(QMediaPlayer::Infinite);
        m_bgAudioPlayer->setSource(QUrl::fromLocalFile(bgMusicPath));
        m_bgAudioPlayer->play();
    }
}

void Widget::onStartGameClicked()
{
    playOpeningCG();
}

void Widget::playOpeningCG()
{
    if (m_bgAudioPlayer) {
        m_bgAudioPlayer->stop();
    }

    if (m_graphicsView) {
        delete m_graphicsView;
        m_graphicsView = nullptr;
    }

    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 1280, 720);

    m_graphicsView = new QGraphicsView(scene, this);
    m_graphicsView->setGeometry(0, 0, 1280, 720);
    m_graphicsView->setFrameShape(QFrame::NoFrame);
    m_graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_graphicsView->show();

    m_audioOutput = new QAudioOutput(this);
    m_audioOutput->setVolume(0.8);

    m_mediaPlayer = new QMediaPlayer(this);
    QGraphicsVideoItem *videoItem = new QGraphicsVideoItem();
    videoItem->setSize(QSizeF(1280, 720));
    scene->addItem(videoItem);

    m_mediaPlayer->setVideoOutput(videoItem);
    m_mediaPlayer->setAudioOutput(m_audioOutput);
    m_mediaPlayer->setLoops(1);

    QString videoPath = ResourceManager::getVideoPath("v1.mp4");
    if (QFileInfo(videoPath).exists()) {
        m_mediaPlayer->setSource(QUrl::fromLocalFile(videoPath));

        connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state) {
            if (state == QMediaPlayer::StoppedState) {
                onCGFinished();
            }
        });

        m_mediaPlayer->play();
    } else {
        onCGFinished();
    }

    this->setFocus();
}

void Widget::onCGFinished()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
        delete m_mediaPlayer;
        m_mediaPlayer = nullptr;
    }
    if (m_audioOutput) {
        delete m_audioOutput;
        m_audioOutput = nullptr;
    }

    showGameScene();
}

void Widget::showGameScene()
{
    if (m_graphicsView) {
        delete m_graphicsView;
        m_graphicsView = nullptr;
    }

    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 1280, 720);

    m_graphicsView = new QGraphicsView(scene, this);
    m_graphicsView->setGeometry(0, 0, 1280, 720);
    m_graphicsView->setFrameShape(QFrame::NoFrame);
    m_graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_graphicsView->show();

    QString imagePath = ResourceManager::getImagePath("p1.png");
    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        scene->addPixmap(pixmap.scaled(1280, 720, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }

    if (!m_dialogLabel) {
        m_dialogLabel = new QLabel(this);
    }
    m_dialogLabel->setGeometry(40, 520, 1200, 200);
    m_dialogLabel->setStyleSheet(
        "QLabel {"
        "background: rgba(255, 192, 203, 0.85);"
        "color: white;"
        "font-size: 24px;"
        "font-family: \"SimHei\";"
        "font-weight: bold;"
        "padding: 40px;"
        "border-radius: 20px;"
        "border: 4px solid rgba(255, 105, 180, 0.6);"
        "text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.8);"
        "}"
    );
    m_dialogLabel->setWordWrap(true);
    m_dialogLabel->show();
    m_dialogLabel->raise();

    if (!m_dialogManager) {
        m_dialogManager = new DialogManager(m_dialogLabel, this);
        connect(m_dialogManager, &DialogManager::animationFinished, this, &Widget::onDialogAnimationFinished);
        connect(m_dialogManager, &DialogManager::dialogCompleted, this, &Widget::onDialogCompleted);
        connect(m_dialogManager, &DialogManager::pageChanged, this, &Widget::onPageChanged);
        connect(m_dialogManager, &DialogManager::backgroundChanged, this, &Widget::onBackgroundChanged);
        connect(m_dialogManager, &DialogManager::audioChanged, this, &Widget::onAudioChanged);
    }

    m_dialogManager->clearPages();
    m_dialogManager->addPage("雨已经下了不知多久了，良子正在吃着板面......", "p1.png", "a1.m4a");
    m_dialogManager->addPage("良子：\n\n不好，大胃袋兜不住了，得去释放一下。", "p2.png", "a2.m4a");
    m_dialogManager->addPage("鬼鬼祟祟......", "p5.jpg", "");
    m_dialogManager->addPage("良子：\n\n欸，舒服......\n\n不对，谁偷吃了我的板面！！！。", "p3.png", "a3.m4a");
    m_dialogManager->addPage("小勤：\n\n阿里巴巴~~~", "p4.jpg", "albb.m4a");
    m_dialogManager->addPage("小勤：\n\n不好，被发现了，快跑。", "p6.jpg", "wk.m4a");
    m_dialogManager->addPage("肥禽貌似躲在了这块墙后面，你要知道他最擅长开挖掘机", "mm.png", "");
    m_dialogManager->addPage("良子：\n\n还好丹尼尔也在这里", "dan1.png", "xb.mp3");
    m_dialogManager->addPage("丹尼尔：\n\n人猿泰山会帮你找回板面", "dan2.png", "danni.m4a");

    m_dialogManager->setSpeed(DialogManager::Normal);
    m_dialogManager->startAnimation();
}

void Widget::onDialogAnimationFinished()
{
}

void Widget::onDialogCompleted()
{
    if (m_interrogationMode) {
        m_interrogationMode = false;
        playEndingVideo();
    } else {
        startMiniGame();
    }
}

void Widget::cleanupDialogUI()
{
    if (m_dialogManager) {
        m_dialogManager->stopAnimation();
        m_dialogManager->clearTypingSound();
    }

    if (m_pageAudioPlayer) {
        m_pageAudioPlayer->stop();
        delete m_pageAudioPlayer;
        m_pageAudioPlayer = nullptr;
    }
    if (m_pageAudioOutput) {
        delete m_pageAudioOutput;
        m_pageAudioOutput = nullptr;
    }

    if (m_dialogLabel) {
        m_dialogLabel->hide();
    }

    if (m_graphicsView) {
        delete m_graphicsView;
        m_graphicsView = nullptr;
    }
}

void Widget::startMiniGame()
{
    cleanupDialogUI();

    m_gameActive = true;
    m_stackedWidget->setCurrentWidget(m_gameWidget);

    QTimer::singleShot(100, this, [this]() {
        m_gameWidget->startGame();
        m_gameWidget->setFocus(Qt::OtherFocusReason);
    });
}

void Widget::onMiniGameLost()
{
    m_gameActive = false;
}

void Widget::onInterrogateClicked()
{
    m_gameActive = false;
    m_stackedWidget->setCurrentIndex(0);
    showInterrogationDialog();
}

void Widget::showInterrogationDialog()
{
    cleanupDialogUI();

    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 1280, 720);

    m_graphicsView = new QGraphicsView(scene, this);
    m_graphicsView->setGeometry(0, 0, 1280, 720);
    m_graphicsView->setFrameShape(QFrame::NoFrame);
    m_graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_graphicsView->show();

    QString imagePath = ResourceManager::getImagePath("q1.jpg");
    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        scene->addPixmap(pixmap.scaled(1280, 720, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }

    if (!m_dialogLabel) {
        m_dialogLabel = new QLabel(this);
    }
    m_dialogLabel->setGeometry(40, 520, 1200, 200);
    m_dialogLabel->setStyleSheet(
        "QLabel {"
        "background: rgba(255, 192, 203, 0.85);"
        "color: white;"
        "font-size: 24px;"
        "font-family: \"SimHei\";"
        "font-weight: bold;"
        "padding: 40px;"
        "border-radius: 20px;"
        "border: 4px solid rgba(255, 105, 180, 0.6);"
        "text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.8);"
        "}"
    );
    m_dialogLabel->setWordWrap(true);
    m_dialogLabel->show();
    m_dialogLabel->raise();

    if (!m_dialogManager) {
        m_dialogManager = new DialogManager(m_dialogLabel, this);
        connect(m_dialogManager, &DialogManager::animationFinished, this, &Widget::onDialogAnimationFinished);
        connect(m_dialogManager, &DialogManager::dialogCompleted, this, &Widget::onDialogCompleted);
        connect(m_dialogManager, &DialogManager::pageChanged, this, &Widget::onPageChanged);
        connect(m_dialogManager, &DialogManager::backgroundChanged, this, &Widget::onBackgroundChanged);
        connect(m_dialogManager, &DialogManager::audioChanged, this, &Widget::onAudioChanged);
    }

    m_interrogationMode = true;

    QString ttPath = ResourceManager::getAudioPath("tt.mp3");
    if (QFileInfo(ttPath).exists()) {
        if (m_interrogationBgPlayer) {
            m_interrogationBgPlayer->stop();
            delete m_interrogationBgPlayer;
        }
        if (m_interrogationBgOutput) {
            delete m_interrogationBgOutput;
        }
        m_interrogationBgOutput = new QAudioOutput(this);
        m_interrogationBgPlayer = new QMediaPlayer(this);
        m_interrogationBgPlayer->setAudioOutput(m_interrogationBgOutput);
        m_interrogationBgPlayer->setLoops(QMediaPlayer::Infinite);
        m_interrogationBgPlayer->setSource(QUrl::fromLocalFile(ttPath));
        m_interrogationBgPlayer->play();
    }

    m_dialogManager->clearPages();
    m_dialogManager->addPage("小勤：\n\n你想知道我偷吃板面的真相吗？", "q1.jpg", "");
    m_dialogManager->addPage("小勤：\n\n因为良子你......", "q1.jpg", "");
    m_dialogManager->addPage("小勤：\n\n根本就不是什么大胃袋良子......", "q1.jpg", "");
    m_dialogManager->addPage("小勤：\n\n而是天气良子，只要你还未停止吃板面，雨就不会停止！", "q1.jpg", "");
    m_dialogManager->addPage("良子：\n\n是这样吗？", "x1.jpg", "");
    m_dialogManager->addPage("良子：\n\n需要放弃我最爱的板面吗？", "x3.jpg", "");
    m_dialogManager->addPage("良子：\n\n但是，比起晴空，", "x2.jpg", "");
    m_dialogManager->addPage("良子：\n\n我更需要板面！", "x4.jpg", "");
    m_dialogManager->setSpeed(DialogManager::Normal);
    m_dialogManager->startAnimation();
}

void Widget::onPageChanged(int current, int total)
{
}

void Widget::playEndingVideo()
{
    if (m_interrogationBgPlayer) {
        m_interrogationBgPlayer->stop();
        delete m_interrogationBgPlayer;
        m_interrogationBgPlayer = nullptr;
    }
    if (m_interrogationBgOutput) {
        delete m_interrogationBgOutput;
        m_interrogationBgOutput = nullptr;
    }

    if (m_dialogManager) {
        m_dialogManager->stopAnimation();
        m_dialogManager->clearTypingSound();
    }

    if (m_pageAudioPlayer) {
        m_pageAudioPlayer->stop();
        delete m_pageAudioPlayer;
        m_pageAudioPlayer = nullptr;
    }
    if (m_pageAudioOutput) {
        delete m_pageAudioOutput;
        m_pageAudioOutput = nullptr;
    }

    if (m_dialogLabel) {
        m_dialogLabel->hide();
    }

    if (m_graphicsView) {
        delete m_graphicsView;
        m_graphicsView = nullptr;
    }

    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 1280, 720);

    m_graphicsView = new QGraphicsView(scene, this);
    m_graphicsView->setGeometry(0, 0, 1280, 720);
    m_graphicsView->setFrameShape(QFrame::NoFrame);
    m_graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_graphicsView->show();

    m_audioOutput = new QAudioOutput(this);
    m_audioOutput->setVolume(0.8);

    m_mediaPlayer = new QMediaPlayer(this);
    QGraphicsVideoItem *videoItem = new QGraphicsVideoItem();
    videoItem->setSize(QSizeF(1280, 720));
    scene->addItem(videoItem);

    m_mediaPlayer->setVideoOutput(videoItem);
    m_mediaPlayer->setAudioOutput(m_audioOutput);
    m_mediaPlayer->setLoops(1);

    QString videoPath = ResourceManager::getVideoPath("v2.mp4");
    if (QFileInfo(videoPath).exists()) {
        m_mediaPlayer->setSource(QUrl::fromLocalFile(videoPath));

        connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state) {
            if (state == QMediaPlayer::StoppedState) {
                if (m_mediaPlayer) {
                    m_mediaPlayer->stop();
                    delete m_mediaPlayer;
                    m_mediaPlayer = nullptr;
                }
                if (m_audioOutput) {
                    delete m_audioOutput;
                    m_audioOutput = nullptr;
                }

                QGraphicsScene *scene = m_graphicsView->scene();
                if (scene) {
                    scene->clear();
                    QString endPath = ResourceManager::getImagePath("end.jpg");
                    QPixmap endPixmap(endPath);
                    if (!endPixmap.isNull()) {
                        scene->addPixmap(endPixmap.scaled(1280, 720, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
                    }

                    QGraphicsTextItem *endText = scene->addText("即使太阳不再出现，也要将板面一直吃下去");
                    endText->setDefaultTextColor(QColor(255, 105, 180));
                    QFont endFont("YouYuan", 26);
                    endFont.setBold(true);
                    endFont.setStyleStrategy(QFont::PreferAntialias);
                    endText->setFont(endFont);
                    endText->setPos(30, 20);
                }

                QAudioOutput *endOut = new QAudioOutput(this);
                QMediaPlayer *endPlayer = new QMediaPlayer(this);
                endPlayer->setAudioOutput(endOut);
                endPlayer->setSource(QUrl::fromLocalFile(ResourceManager::getAudioPath("end.m4a")));
                endPlayer->play();

                m_endScreenActive = true;
            }
        });

        m_mediaPlayer->play();
    } else {
        cleanupDialogUI();
        m_stackedWidget->setCurrentIndex(0);
    }

    this->setFocus();
}

void Widget::onBackgroundChanged(const QString &bgImage)
{
    if (m_graphicsView && !bgImage.isEmpty()) {
        QString imagePath = ResourceManager::getImagePath(bgImage);
        QPixmap pixmap(imagePath);
        if (!pixmap.isNull()) {
            QGraphicsScene *scene = m_graphicsView->scene();
            scene->clear();
            scene->addPixmap(pixmap.scaled(1280, 720, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }
    }
}

void Widget::onAudioChanged(const QString &audioFile)
{
    m_pendingAudioFile = audioFile;
    QTimer::singleShot(50, this, &Widget::playCurrentPageAudio);
}

void Widget::playCurrentPageAudio()
{
    if (m_pendingAudioFile.isEmpty()) {
        return;
    }

    QString audioPath = ResourceManager::getAudioPath(m_pendingAudioFile);

    if (!QFileInfo(audioPath).exists()) {
        return;
    }

    if (m_pageAudioPlayer) {
        m_pageAudioPlayer->stop();
        delete m_pageAudioPlayer;
        m_pageAudioPlayer = nullptr;
    }
    if (m_pageAudioOutput) {
        delete m_pageAudioOutput;
        m_pageAudioOutput = nullptr;
    }

    m_pageAudioOutput = new QAudioOutput(this);
    m_pageAudioOutput->setVolume(1.0);

    m_pageAudioPlayer = new QMediaPlayer(this);
    m_pageAudioPlayer->setAudioOutput(m_pageAudioOutput);

    QUrl audioUrl = QUrl::fromLocalFile(audioPath);
    m_pageAudioPlayer->setSource(audioUrl);

    QTimer::singleShot(150, [this]() {
        if (m_pageAudioPlayer) {
            m_pageAudioPlayer->play();
        }
    });
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if (m_endScreenActive) {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            m_endScreenActive = false;
            cleanupDialogUI();
            m_stackedWidget->setCurrentIndex(0);
        }
        return;
    }

    if (m_gameActive) {
        QWidget::keyPressEvent(event);
        return;
    }

    if (m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            m_mediaPlayer->stop();
        }
    } else if (m_dialogManager) {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space) {
            m_dialogManager->handleKeyPress();
        }
    }
    QWidget::keyPressEvent(event);
}

void Widget::mousePressEvent(QMouseEvent *)
{
    if (m_gameActive) {
        return;
    }

    if (!m_mediaPlayer || m_mediaPlayer->playbackState() != QMediaPlayer::PlayingState) {
        if (m_dialogManager) {
            m_dialogManager->handleKeyPress();
        }
    }
}

Widget::~Widget()
{
    if (m_bgAudioPlayer) {
        m_bgAudioPlayer->stop();
        delete m_bgAudioPlayer;
    }
    if (m_bgAudioOutput) {
        delete m_bgAudioOutput;
    }
    if (m_pageAudioPlayer) {
        m_pageAudioPlayer->stop();
        delete m_pageAudioPlayer;
    }
    if (m_pageAudioOutput) {
        delete m_pageAudioOutput;
    }
    if (m_interrogationBgPlayer) {
        m_interrogationBgPlayer->stop();
        delete m_interrogationBgPlayer;
    }
    if (m_interrogationBgOutput) {
        delete m_interrogationBgOutput;
    }
    delete m_dialogManager;
    delete m_dialogLabel;
    delete ui;
}
