#include "danielplayer.h"
#include "resourcemanager.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QFileInfo>
#include <QtMath>

class ArmRectItem : public QGraphicsRectItem
{
public:
    ArmRectItem(const QRectF &rect, const QPixmap &pixmap, QGraphicsItem *parent = nullptr)
        : QGraphicsRectItem(rect, parent), m_pixmap(pixmap)
    {
        setPen(Qt::NoPen);
        setBrush(Qt::NoBrush);
    }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override
    {
        if (!m_pixmap.isNull()) {
            painter->setRenderHint(QPainter::SmoothPixmapTransform);
            painter->drawPixmap(rect().toRect(), m_pixmap);
        }
    }

private:
    QPixmap m_pixmap;
};

DanielPlayer::DanielPlayer(QGraphicsItem *parent)
    : QGraphicsItemGroup(parent)
    , m_head(nullptr)
    , m_headPixmap(nullptr)
    , m_x(640)
    , m_y(620)
    , m_armsClamped(false)
{
    createGraphics();
}

void DanielPlayer::createGraphics()
{
    m_head = new QGraphicsRectItem(-50, -88, 100, 100, this);
    m_head->setBrush(QBrush(QColor(100, 150, 100)));
    m_head->setPen(QPen(Qt::black, 2));

    m_headPixmap = nullptr;

    updateArms();
    setHeadImagePath("dans.png");
}

void DanielPlayer::updateArms()
{
    for (auto *bat : m_bats) {
        delete bat;
    }
    m_bats.clear();

    qreal angleDefault = 0.0;
    qreal angleClamp = 40.0;
    qreal length = 120;
    qreal halfWidth = 12;
    qreal shoulderOffset = 30;

    qreal leftAngle = m_armsClamped ? angleClamp : angleDefault;
    qreal rightAngle = m_armsClamped ? -angleClamp : angleDefault;

    QPixmap leftPm;
    if (!m_leftArmPm.isNull()) {
        leftPm = m_leftArmPm.scaled(length, halfWidth * 2,
                                     Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    ArmRectItem *leftBat = new ArmRectItem(
        QRectF(-length, -halfWidth, length, halfWidth * 2), leftPm, this);
    leftBat->setPos(-shoulderOffset, 0);
    leftBat->setTransformOriginPoint(0, 0);
    leftBat->setRotation(leftAngle);
    if (m_leftArmPm.isNull()) {
        leftBat->setPen(QPen(Qt::black, 2));
        leftBat->setBrush(QBrush(QColor(139, 69, 19)));
    }
    m_bats.append(leftBat);

    QPixmap rightPm;
    if (!m_rightArmPm.isNull()) {
        rightPm = m_rightArmPm.scaled(length, halfWidth * 2,
                                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    ArmRectItem *rightBat = new ArmRectItem(
        QRectF(0, -halfWidth, length, halfWidth * 2), rightPm, this);
    rightBat->setPos(shoulderOffset, 0);
    rightBat->setTransformOriginPoint(0, 0);
    rightBat->setRotation(rightAngle);
    if (m_rightArmPm.isNull()) {
        rightBat->setPen(QPen(Qt::black, 2));
        rightBat->setBrush(QBrush(QColor(139, 69, 19)));
    }
    m_bats.append(rightBat);
}

void DanielPlayer::moveLeft(qreal speed)
{
    if (m_x > 50) {
        m_x -= speed;
        setPos(m_x, m_y);
    }
}

void DanielPlayer::moveRight(qreal speed)
{
    if (m_x < 1230) {
        m_x += speed;
        setPos(m_x, m_y);
    }
}

void DanielPlayer::clampArms()
{
    if (!m_armsClamped) {
        m_armsClamped = true;
        updateArms();
    }
}

void DanielPlayer::unclampArms()
{
    if (m_armsClamped) {
        m_armsClamped = false;
        updateArms();
    }
}

qreal DanielPlayer::getSpeedMultiplier() const
{
    return m_armsClamped ? 1.8 : 1.0;
}

void DanielPlayer::setHeadPixmap(const QPixmap &pixmap)
{
    m_headPm = pixmap;
    if (m_headPixmap) {
        delete m_headPixmap;
        m_headPixmap = nullptr;
    }
    if (!pixmap.isNull()) {
        m_headPixmap = new QGraphicsPixmapItem(this);
        m_headPixmap->setPixmap(pixmap.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        m_headPixmap->setOffset(-50, -88);
        m_head->setBrush(Qt::transparent);
        m_head->setPen(Qt::NoPen);
    }
}

void DanielPlayer::setHeadImagePath(const QString &path)
{
    QString fullPath = ResourceManager::getImagePath(path);
    if (QFileInfo(fullPath).exists()) {
        setHeadPixmap(QPixmap(fullPath));
    }
}

void DanielPlayer::setLeftArmPixmap(const QPixmap &pixmap)
{
    m_leftArmPm = pixmap;
    updateArms();
}

void DanielPlayer::setRightArmPixmap(const QPixmap &pixmap)
{
    m_rightArmPm = pixmap;
    updateArms();
}

void DanielPlayer::setArmImagePath(const QString &leftPath, const QString &rightPath)
{
    QString leftFull = ResourceManager::getImagePath(leftPath);
    if (QFileInfo(leftFull).exists()) {
        m_leftArmPm = QPixmap(leftFull);
    }
    QString rightFull = ResourceManager::getImagePath(rightPath);
    if (QFileInfo(rightFull).exists()) {
        m_rightArmPm = QPixmap(rightFull);
    }
    updateArms();
}
