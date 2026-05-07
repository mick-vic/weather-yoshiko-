#include "brickitem.h"
#include "resourcemanager.h"
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QFileInfo>

BrickItem::BrickItem(int row, int col, BrickType type, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
    , m_row(row)
    , m_col(col)
    , m_type(type)
    , m_hp(1)
    , m_isDestroyed(false)
{
    setBrush(QBrush(QColor(100, 150, 255)));
    setPen(QPen(Qt::black, 2));
}

void BrickItem::hit()
{
    m_hp--;
    if (m_hp <= 0) {
        m_isDestroyed = true;
        hide();
    } else {
        QBrush b = brush();
        setBrush(QBrush(b.color().darker(150)));
    }
}

void BrickItem::setColor(const QColor &color)
{
    setBrush(QBrush(color));
}

void BrickItem::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
    update();
}

void BrickItem::setImagePath(const QString &path)
{
    QString fullPath = ResourceManager::getImagePath(path);
    if (QFileInfo(fullPath).exists()) {
        m_pixmap = QPixmap(fullPath);
        update();
    }
}

void BrickItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                      QWidget *widget)
{
    if (!m_pixmap.isNull()) {
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        painter->drawPixmap(rect().toRect(), m_pixmap);
    } else {
        QGraphicsRectItem::paint(painter, option, widget);
    }
}
