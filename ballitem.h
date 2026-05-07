#ifndef BALLITEM_H
#define BALLITEM_H

#include <QGraphicsEllipseItem>
#include <QPointF>
#include <QPixmap>

class BallItem : public QGraphicsEllipseItem
{
public:
    explicit BallItem(QGraphicsItem *parent = nullptr);

    void launch();
    void move();
    void setVelocity(qreal vx, qreal vy);
    void bounceHorizontal();
    void bounceVertical();
    void setSpeedBoost(qreal multiplier);

    void setPixmap(const QPixmap &pixmap);
    void setImagePath(const QString &path);
    const QPixmap &getPixmap() const { return m_pixmap; }

    qreal getRadius() const { return m_radius; }
    QPointF getVelocity() const { return m_velocity; }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

private:
    qreal m_radius;
    qreal m_baseSpeed;
    QPointF m_velocity;
    QPixmap m_pixmap;
};

#endif // BALLITEM_H
