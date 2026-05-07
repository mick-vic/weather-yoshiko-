#include "ballitem.h"
#include "resourcemanager.h"
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QRandomGenerator>
#include <QFileInfo>
#include <QtMath>

BallItem::BallItem(QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent)
    , m_radius(20)
    , m_baseSpeed(5)
{
    setRect(-m_radius, -m_radius, m_radius * 2, m_radius * 2);
    setBrush(QBrush(QColor(255, 200, 50)));
    setPen(QPen(Qt::black, 2));
    m_velocity = QPointF(0, 0);
}

void BallItem::launch()
{
    qreal angleDeg = QRandomGenerator::global()->bounded(30, 151);
    qreal angleRad = angleDeg * M_PI / 180.0;
    m_velocity = QPointF(m_baseSpeed * cos(angleRad), -m_baseSpeed * sin(angleRad));
}

void BallItem::move()
{
    moveBy(m_velocity.x(), m_velocity.y());
}

void BallItem::setVelocity(qreal vx, qreal vy)
{
    m_velocity = QPointF(vx, vy);
}

void BallItem::bounceHorizontal()
{
    m_velocity.setX(-m_velocity.x());
}

void BallItem::bounceVertical()
{
    m_velocity.setY(-m_velocity.y());
}

void BallItem::setSpeedBoost(qreal multiplier)
{
    m_velocity *= multiplier;
    qreal maxSpeed = 12.0;
    qreal currentSpeed = qSqrt(m_velocity.x() * m_velocity.x() + m_velocity.y() * m_velocity.y());
    if (currentSpeed > maxSpeed) {
        m_velocity *= maxSpeed / currentSpeed;
    }
}

void BallItem::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
    update();
}

void BallItem::setImagePath(const QString &path)
{
    QString fullPath = ResourceManager::getImagePath(path);
    if (QFileInfo(fullPath).exists()) {
        m_pixmap = QPixmap(fullPath);
        update();
    }
}

void BallItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    if (!m_pixmap.isNull()) {
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        painter->drawPixmap(rect().toRect(), m_pixmap);
    } else {
        QGraphicsEllipseItem::paint(painter, option, widget);
    }
}
