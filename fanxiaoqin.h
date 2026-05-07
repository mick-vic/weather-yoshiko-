#ifndef FANXIAOQIN_H
#define FANXIAOQIN_H

#include <QGraphicsRectItem>
#include <QPixmap>

class FanXiaoqin : public QGraphicsRectItem
{
public:
    explicit FanXiaoqin(QGraphicsItem *parent = nullptr);

    void setPixmap(const QPixmap &pixmap);
    void setImagePath(const QString &path);
    QRectF getBoundingRect() const;
    bool checkCollision(const QPointF &ballPos, qreal ballRadius);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

private:
    QPixmap m_pixmap;
};

#endif // FANXIAOQIN_H
