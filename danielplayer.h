#ifndef DANIELPLAYER_H
#define DANIELPLAYER_H

#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>

class DanielPlayer : public QGraphicsItemGroup
{
public:
    explicit DanielPlayer(QGraphicsItem *parent = nullptr);

    void moveLeft(qreal speed);
    void moveRight(qreal speed);
    void clampArms();
    void unclampArms();
    bool isArmsClamped() const { return m_armsClamped; }
    qreal getSpeedMultiplier() const;

    void setHeadPixmap(const QPixmap &pixmap);
    void setHeadImagePath(const QString &path);
    void setLeftArmPixmap(const QPixmap &pixmap);
    void setRightArmPixmap(const QPixmap &pixmap);
    void setArmImagePath(const QString &leftPath, const QString &rightPath);

    QList<QGraphicsRectItem*> getBatItems() const { return m_bats; }
    qreal getX() const { return m_x; }
    qreal getY() const { return m_y; }

private:
    void createGraphics();
    void updateArms();

    QGraphicsRectItem *m_head;
    QGraphicsPixmapItem *m_headPixmap;
    QList<QGraphicsRectItem*> m_bats;

    qreal m_x;
    qreal m_y;
    bool m_armsClamped;

    QPixmap m_headPm;
    QPixmap m_leftArmPm;
    QPixmap m_rightArmPm;
};

#endif // DANIELPLAYER_H
