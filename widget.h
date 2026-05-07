#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QGraphicsView>
#include <QAudioOutput>
#include <QLabel>
#include <QKeyEvent>
#include <QStackedWidget>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class DialogManager;
class GameWidget;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);

private slots:
    void onStartGameClicked();
    void playOpeningCG();
    void onCGFinished();
    void playBackgroundMusic();
    void showGameScene();
    void onDialogAnimationFinished();
    void onDialogCompleted();
    void onPageChanged(int current, int total);
    void onBackgroundChanged(const QString &bgImage);
    void onAudioChanged(const QString &audioFile);
    void playCurrentPageAudio();
    void startMiniGame();
    void onMiniGameLost();
    void onInterrogateClicked();

private:
    void cleanupDialogUI();
    void showInterrogationDialog();
    void playEndingVideo();

    Ui::Widget *ui;
    QStackedWidget *m_stackedWidget;
    QMediaPlayer *m_mediaPlayer;
    QAudioOutput *m_audioOutput;
    QMediaPlayer *m_bgAudioPlayer;
    QAudioOutput *m_bgAudioOutput;
    QMediaPlayer *m_pageAudioPlayer;
    QAudioOutput *m_pageAudioOutput;
    QMediaPlayer *m_interrogationBgPlayer;
    QAudioOutput *m_interrogationBgOutput;
    QGraphicsView *m_graphicsView;
    QLabel *m_dialogLabel;
    DialogManager *m_dialogManager;
    GameWidget *m_gameWidget;
    QString m_pendingAudioFile;
    bool m_gameActive;
    bool m_interrogationMode;
    bool m_endScreenActive;
};
#endif // WIDGET_H
