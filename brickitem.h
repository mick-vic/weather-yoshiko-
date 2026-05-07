#ifndef BRICKITEM_H
#define BRICKITEM_H

#include <QGraphicsRectItem>
#include <QColor>
#include <QPixmap>

class BrickItem : public QGraphicsRectItem
{
public:
    enum BrickType {
        Type_Normal,
        Type_Hard,
        Type_Gold
    };

    explicit BrickItem(int row, int col, BrickType type = Type_Normal, QGraphicsItem *parent = nullptr);

    void hit();
    bool isDestroyed() const { return m_isDestroyed; }
    int getRow() const { return m_row; }
    int getCol() const { return m_col; }
    BrickType getType() const { return m_type; }

    void setColor(const QColor &color);
    void setPixmap(const QPixmap &pixmap);
    void setImagePath(const QString &path);
    const QPixmap &getPixmap() const { return m_pixmap; }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

private:
    int m_row;
    int m_col;
    BrickType m_type;
    int m_hp;
    bool m_isDestroyed;
    QPixmap m_pixmap;
};

#endif // BRICKITEM_H
