#ifndef DRAWGRAPHICS_H
#define DRAWGRAPHICS_H

#include <QWidget>
#include <QMouseEvent>
#include <QVector>
#include <QColor>
#include "graphicitem.h"


class DrawGraphics: public QWidget
{
    Q_OBJECT
public:
    DrawGraphics(QWidget *parent = nullptr);
    ~DrawGraphics();

    void setColor(int c);
    void setThickness(int t);
    void setStyle(int s);
    void setShape(int s);
    void setEdit(bool e);
    void setEditMode(int m);
    QVector<int> getSelectedParameters();

    void updateColor(int c);
    void updateThickness(int t);
    void updateStyle(int s);

    QJsonArray toJsonArray();
    void drawJsonArray(QJsonArray jsonArray);

    void setLoad(int l);

protected:
    void paintEvent(QPaintEvent *event) override;   // To repaint the widget
    void mousePressEvent(QMouseEvent *event) override;  // Handle mouse press
    void mouseMoveEvent(QMouseEvent *event) override;   // Handle mouse movement
    void mouseReleaseEvent(QMouseEvent *event) override; // Handle mouse release

private:
    bool isDrawing;
    bool isEditing;
    // bool isSelected;
    bool isMoving;
    bool isResizing;
    bool isChangingParameters;
    int editIndex; // -1表示没有选中

    bool isLoading;

    QColor currentColor = Qt::black;
    int currentThickness = 1;
    Qt::PenStyle currentStyle = Qt::SolidLine;
    int currentShape = 0;
    GraphicItem* currentGraphic = new LineItem(QLineF(QPointF(0, 0), QPointF(0, 0)));
    QVector<GraphicItem*> graphics;

    QPointF moveStartPoint;
    QPointF resizeStartPoint;


signals:
    void selectedGraphicChanged(int color, int thickness, int style, int shape);
};
#endif // DRAWGRAPHICS_H
