#include "dialogmanager.h"
#include "resourcemanager.h"
#include <QFileInfo>
#include <QUrl>
#include <QFontMetrics>

DialogManager::DialogManager(QLabel *dialogLabel, QObject *parent)
    : QObject(parent)
    , m_dialogLabel(dialogLabel)
    , m_currentPageIndex(0)
    , m_currentCharIndex(0)
    , m_speed(Normal)
    , m_isAnimating(false)
    , m_timer(nullptr)
    , m_typingPlayer(nullptr)
    , m_typingAudioOutput(nullptr)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &DialogManager::displayNextChar);
}

DialogManager::~DialogManager()
{
    stopAnimation();
    clearTypingSound();
}

void DialogManager::addPage(const QString &text, const QString &bgImage, const QString &audioFile)
{
    DialogPage page;
    page.text = text;
    page.backgroundImage = bgImage;
    page.audioFile = audioFile;
    m_pages.append(page);
}

void DialogManager::splitTextIntoPages(const QString &fullText)
{
    if (!m_dialogLabel) return;

    QFontMetrics fm(m_dialogLabel->font());
    int maxWidth = m_dialogLabel->width() - 40;
    int maxHeight = m_dialogLabel->height() - 40;
    int lineSpacing = fm.lineSpacing();
    int maxLines = maxHeight / lineSpacing;

    QString currentLine;
    QStringList currentPageLines;
    int currentLineCount = 0;

    QStringList paragraphs = fullText.split("\n");

    for (const QString &paragraph : paragraphs) {
        QString remaining = paragraph;

        while (!remaining.isEmpty()) {
            QString line = fm.elidedText(remaining, Qt::ElideNone, maxWidth);

            if (line.isEmpty()) {
                break;
            }

            int lineLength = line.length();
            if (lineLength < remaining.length() && !remaining.at(lineLength).isSpace()) {
                int lastSpace = line.lastIndexOf(' ');
                if (lastSpace > 0) {
                    line = line.left(lastSpace);
                    lineLength = lastSpace;
                }
            }

            if (currentLineCount >= maxLines) {
                m_currentPageLines.append(currentPageLines);
                currentPageLines.clear();
                currentLineCount = 0;
            }

            currentPageLines.append(line);
            currentLineCount++;

            if (lineLength < remaining.length()) {
                remaining = remaining.mid(lineLength).trimmed();
            } else {
                remaining.clear();
            }
        }

        if (!paragraph.isEmpty()) {
            if (currentLineCount >= maxLines) {
                m_currentPageLines.append(currentPageLines);
                currentPageLines.clear();
                currentLineCount = 0;
            }
            currentPageLines.append("");
            currentLineCount++;
        }
    }

    if (!currentPageLines.isEmpty()) {
        m_currentPageLines.append(currentPageLines);
    }

    if (m_currentPageLines.isEmpty()) {
        m_currentPageLines.append(QStringList());
    }
}

void DialogManager::startAnimation()
{
    if (m_pages.isEmpty()) return;

    m_isAnimating = true;
    m_currentCharIndex = 0;

    if (m_currentPageIndex < m_pages.size()) {
        const DialogPage &page = m_pages[m_currentPageIndex];
        if (!page.backgroundImage.isEmpty()) {
            emit backgroundChanged(page.backgroundImage);
        }
        if (!page.audioFile.isEmpty()) {
            emit audioChanged(page.audioFile);
        }
        m_currentPageLines.clear();
        splitTextIntoPages(page.text);
        m_timer->start(m_speed);
    }
}

void DialogManager::stopAnimation()
{
    m_isAnimating = false;
    if (m_timer) {
        m_timer->stop();
    }
}

void DialogManager::skipToEnd()
{
    stopAnimation();
    if (m_currentPageIndex < m_pages.size()) {
        QString fullText = m_pages[m_currentPageIndex].text;
        m_dialogLabel->setText(fullText);
    }
}

void DialogManager::displayNextChar()
{
    if (m_currentPageIndex >= m_pages.size()) {
        stopAnimation();
        emit animationFinished();
        return;
    }

    QString currentDisplayText;
    int charCount = 0;

    for (int i = 0; i < m_currentPageLines.size(); i++) {
        if (i > 0) {
            currentDisplayText += "\n";
        }

        const QString &line = m_currentPageLines[i];
        if (charCount + line.length() <= m_currentCharIndex) {
            currentDisplayText += line;
            charCount += line.length() + 1;
        } else {
            int charsToAdd = m_currentCharIndex - charCount;
            if (charsToAdd > 0) {
                currentDisplayText += line.left(charsToAdd);
            }
            break;
        }
    }

    m_dialogLabel->setText(currentDisplayText);

    int totalChars = 0;
    for (const QString &line : m_currentPageLines) {
        totalChars += line.length() + 1;
    }

    if (m_currentCharIndex < totalChars) {
        m_currentCharIndex++;
        playTypingSound();
    } else {
        stopAnimation();
        emit animationFinished();
    }
}

void DialogManager::playTypingSound()
{
    QString soundPath = ResourceManager::getAudioPath("typing.wav");
    if (QFileInfo(soundPath).exists()) {
        if (!m_typingPlayer) {
            m_typingAudioOutput = new QAudioOutput(this);
            m_typingAudioOutput->setVolume(0.3);
            m_typingPlayer = new QMediaPlayer(this);
            m_typingPlayer->setAudioOutput(m_typingAudioOutput);
        }

        m_typingPlayer->setSource(QUrl::fromLocalFile(soundPath));
        m_typingPlayer->play();
    }
}

void DialogManager::clearTypingSound()
{
    if (m_typingPlayer) {
        m_typingPlayer->stop();
        delete m_typingPlayer;
        m_typingPlayer = nullptr;
    }
    if (m_typingAudioOutput) {
        delete m_typingAudioOutput;
        m_typingAudioOutput = nullptr;
    }
}

void DialogManager::handleKeyPress()
{
    if (m_isAnimating) {
        skipToEnd();
        if (m_currentPageIndex == m_pages.size() - 1) {
            emit dialogCompleted();
        }
    } else {
        if (m_currentPageIndex < m_pages.size() - 1) {
            m_currentPageIndex++;
            m_currentCharIndex = 0;
            emit pageChanged(m_currentPageIndex + 1, m_pages.size());
            startAnimation();
        } else {
            emit dialogCompleted();
        }
    }
}

void DialogManager::showCurrentPage()
{
    if (m_currentPageIndex < m_pages.size()) {
        m_dialogLabel->setText(m_pages[m_currentPageIndex].text);
    }
}
