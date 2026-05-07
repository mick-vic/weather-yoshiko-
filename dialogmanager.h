#ifndef DIALOGUEMANAGER_H
#define DIALOGUEMANAGER_H

#include <QObject>
#include <QLabel>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVector>
#include <QString>

struct DialogPage {
    QString text;
    QString backgroundImage;
    QString audioFile;
};

class DialogManager : public QObject
{
    Q_OBJECT

public:
    enum Speed {
        Slow = 100,
        Normal = 50,
        Fast = 25,
        VeryFast = 10
    };

    explicit DialogManager(QLabel *dialogLabel, QObject *parent = nullptr);
    ~DialogManager();

    void addPage(const QString &text, const QString &bgImage = "", const QString &audioFile = "");
    void clearPages() { m_pages.clear(); m_currentPageIndex = 0; }
    void setSpeed(Speed speed) { m_speed = speed; }
    void setSpeed(int ms) { m_speed = ms; }
    void skipToEnd();
    bool isAnimating() const { return m_isAnimating; }
    int currentPage() const { return m_currentPageIndex; }
    int totalPages() const { return m_pages.size(); }
    void clearTypingSound();

signals:
    void animationFinished();
    void dialogCompleted();
    void pageChanged(int currentPage, int totalPages);
    void backgroundChanged(const QString &bgImage);
    void audioChanged(const QString &audioFile);

public slots:
    void startAnimation();
    void stopAnimation();
    void handleKeyPress();

private slots:
    void displayNextChar();
    void playTypingSound();

private:
    void splitTextIntoPages(const QString &fullText);
    void showCurrentPage();

    QLabel *m_dialogLabel;
    QVector<DialogPage> m_pages;
    QVector<QString> m_currentPageLines;
    int m_currentPageIndex;
    int m_currentCharIndex;
    int m_speed;
    bool m_isAnimating;
    QTimer *m_timer;

    QMediaPlayer *m_typingPlayer;
    QAudioOutput *m_typingAudioOutput;
};

#endif // DIALOGUEMANAGER_H
