#include "fanxiaoqin.h"
#include "resourcemanager.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QFileInfo>

FanXiaoqin::FanXiaoqin(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    qreal size = 80;
    setRect(-size / 2, -size / 2, size, size);
    setBrush(QBrush(QColor(200, 100, 100)));
    setPen(QPen(Qt::black, 2));
}

void FanXiaoqin::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
    update();
}

void FanXiaoqin::setImagePath(const QString &path)
{
    QString fullPath = ResourceManager::getImagePath(path);
    if (QFileInfo(fullPath).exists()) {
        m_pixmap = QPixmap(fullPath);
        update();
    }
}

QRectF FanXiaoqin::getBoundingRect() const
{
    return sceneBoundingRect();
}

bool FanXiaoqin::checkCollision(const QPointF &ballPos, qreal ballRadius)
{
    QRectF targetRect = sceneBoundingRect();
    QRectF ballRect(ballPos.x() - ballRadius, ballPos.y() - ballRadius,
                    ballRadius * 2, ballRadius * 2);
    return targetRect.intersects(ballRect);
}

void FanXiaoqin::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                        QWidget *widget)
{
    if (!m_pixmap.isNull()) {
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        painter->drawPixmap(rect().toRect(), m_pixmap);
    } else {
        QGraphicsRectItem::paint(painter, option, widget);
    }
}
