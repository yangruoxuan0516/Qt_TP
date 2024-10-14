// graphicitem.h
#ifndef GRAPHICITEM_H
#define GRAPHICITEM_H

#include <QLineF>
#include <QRectF>
#include <QPainter>
// #include <memory>
#include <iostream>

class GraphicItem {
public:
    virtual ~GraphicItem(){}// = default;
    virtual void setStart(QPointF* p) = 0;
    virtual void setEnd(QPointF* p) = 0;
    virtual qreal getP1X() = 0;
    virtual qreal getP1Y() = 0;
    virtual qreal getP2X() = 0;
    virtual qreal getP2Y() = 0;
    virtual void draw(QPainter* painter) const = 0;

    void setColor(QColor color){currentColor = color;};
    void setThickness(int thickness){currentThickness = thickness;};
    void setStyle(Qt::PenStyle style){currentStyle = style;};
    QColor getColor(){return currentColor;}
    int getThickness(){return currentThickness;}
    Qt::PenStyle getStyle(){return currentStyle;}
    virtual int getShape(){return -1;};

    // edit -> select
    virtual bool isInside(QPointF* p) const = 0;
    void setSelected(bool selected){isSelected = selected;};
    bool getSelected(){return isSelected;}
    virtual void showSelected(QPainter* painter) const = 0;

    qreal threshold = 10;

    // edit -> move
    virtual void move(QPointF* start, QPointF* end) = 0;
    // edit -> resize
    virtual bool resize(QPointF* start, QPointF* end) = 0; // if resize, return true, so that perform "resizeStartPoint = p; update();" in drawgraphic.cpp"

private:
    QColor currentColor = Qt::black;
    int currentThickness = 1;
    Qt::PenStyle currentStyle = Qt::SolidLine;
    bool isSelected = false;
};

class LineItem : public GraphicItem {
public:
    LineItem(const QLineF& line) : line(line) {}
    int getShape()override{return 0;}
    void setStart(QPointF* p)override{
        line.setP1(*p);
    }
    void setEnd(QPointF* p)override{
        line.setP2(*p);
    }
    qreal getP1X()override{
        return line.p1().x();
    }
    qreal getP1Y()override{
        return line.p1().y();
    }
    qreal getP2X()override{
        return line.p2().x();
    }
    qreal getP2Y()override{
        return line.p2().y();
    }
    void draw(QPainter* painter) const override {
        painter->drawLine(line);
    }
    bool isInside(QPointF* p) const override {                                                                              // to correct here: "intersect" with the border but not contain
        // // Get the two points that define the line
        // QPointF p1 = line.p1();
        // QPointF p2 = line.p2();
        // // If the line is degenerate (i.e., both endpoints are the same), return the distance to one endpoint
        // if (p1 == p2) {
        //     return QLineF(p1, *p).length();
        // }
        // // Calculate the perpendicular distance from the point to the line segment
        // qreal numerator = qAbs((p2.y() - p1.y()) * p->x() - (p2.x() - p1.x()) * p->y() + p2.x() * p1.y() - p2.y() * p1.x());
        // qreal denominator = QLineF(p1, p2).length();
        // qreal distance = numerator / denominator;
        // painter->drawRect(QRectF(line.p1(),line.p2()));
        // QLineF line(p1, p2);
        // return distance <= threshold;
        QPointF p1 = line.p1();
        QPointF p2 = line.p2();

        // Calculate the vector perpendicular to the line
        QLineF perpendicularLine = line.normalVector();
        perpendicularLine.setLength(threshold);  // Half of the width, extend in both directions

        // Get the perpendicular vector to offset the points symmetrically
        QPointF offset1 = perpendicularLine.p2() - perpendicularLine.p1();

        // Create the four points of the rectangle
        // Offset in both positive and negative directions to wrap the rectangle around the line
        QPointF rectP1 = p1 + offset1;  // Offset point for p1 (one side)
        QPointF rectP2 = p1 - offset1;  // Offset point for p1 (other side)
        QPointF rectP3 = p2 - offset1;  // Offset point for p2 (other side)
        QPointF rectP4 = p2 + offset1;  // Offset point for p2 (one side)

        // Create a polygon (rectangle) from the four points
        QPolygonF rectangle;
        rectangle << rectP1 << rectP2 << rectP3 << rectP4;
        return rectangle.containsPoint(*p,Qt::OddEvenFill);
    }
    void showSelected(QPainter* painter) const override {
        // painter->drawRect(QRectF(line.p1(),line.p2()));
        // QLineF line(p1, p2);
        QPointF p1 = line.p1();
        QPointF p2 = line.p2();

        // Calculate the vector perpendicular to the line
        QLineF perpendicularLine = line.normalVector();
        perpendicularLine.setLength(threshold);  // Half of the width, extend in both directions

        // Get the perpendicular vector to offset the points symmetrically
        QPointF offset1 = perpendicularLine.p2() - perpendicularLine.p1();

        // Create the four points of the rectangle
        // Offset in both positive and negative directions to wrap the rectangle around the line
        QPointF rectP1 = p1 + offset1;  // Offset point for p1 (one side)
        QPointF rectP2 = p1 - offset1;  // Offset point for p1 (other side)
        QPointF rectP3 = p2 - offset1;  // Offset point for p2 (other side)
        QPointF rectP4 = p2 + offset1;  // Offset point for p2 (one side)

        // Create a polygon (rectangle) from the four points
        QPolygonF rectangle;
        rectangle << rectP1 << rectP2 << rectP3 << rectP4;
        painter->drawPolygon(rectangle);
    }
    void move(QPointF* start, QPointF* end) override {
        qreal deltaX = end->x() - start->x();
        qreal deltaY = end->y() - start->y();
        line.translate(deltaX, deltaY);
    }
    bool resize(QPointF* start, QPointF* end) override {
        QLineF l1(line.p1(),*start);
        QLineF l2(line.p2(),*start);
        if (l1.length()<threshold){
            qreal deltaX = end->x() - start->x();
            qreal deltaY = end->y() - start->y();
            QPointF p1 = line.p1();
            line.setP1(QPointF(p1.x()+deltaX,p1.y()+deltaY));
            return true;
        }
        else if (l2.length()<threshold){
            qreal deltaX = end->x() - start->x();
            qreal deltaY = end->y() - start->y();
            QPointF p2 = line.p2();
            line.setP2(QPointF(p2.x()+deltaX,p2.y()+deltaY));
            return true;
        }
        return false;
    }
private:
    QLineF line;
};

class RectangleItem : public GraphicItem {
public:
    RectangleItem(const QRectF& rect) : rect(rect) {}
    int getShape()override{return 1;}
    void setStart(QPointF* p) override{
        rect.setX(p->x());
        rect.setY(p->y());
    }
    void setEnd(QPointF* p) override{
        rect.setWidth(p->x()-rect.left());
        rect.setHeight(p->y()-rect.top());
    }
    qreal getP1X()override{
        return rect.topLeft().x();
    }
    qreal getP1Y()override{
        return rect.topLeft().y();
    }
    qreal getP2X()override{
        return rect.topLeft().x()+rect.width();
    }
    qreal getP2Y()override{
        return rect.topLeft().y()+rect.height();
    }
    void draw(QPainter* painter) const override {
        painter->drawRect(rect);
    }
    bool isInside(QPointF* p) const override{
        QRectF normalisedRect = QRectF(qMin(rect.left(),rect.right()),qMin(rect.top(),rect.bottom()),qAbs(rect.width()),qAbs(rect.height()));
        QRectF enlargedRect = normalisedRect.adjusted(-threshold, -threshold, threshold, threshold);
        QRectF shrinkedRect = normalisedRect.adjusted(threshold, threshold, -threshold, -threshold);
        if (rect.width() < 2*threshold || rect.height() < 2*threshold){
            return enlargedRect.contains(*p);
        }else{
            return enlargedRect.contains(*p) && !shrinkedRect.contains(*p);
        }
    }
    void showSelected(QPainter* painter) const override {
        QRectF normalisedRect = QRectF(qMin(rect.left(),rect.right()),qMin(rect.top(),rect.bottom()),qAbs(rect.width()),qAbs(rect.height()));
        QRectF enlargedRect = normalisedRect.adjusted(-threshold, -threshold, threshold, threshold);
        painter->drawRect(enlargedRect);
    }
    void move(QPointF* start, QPointF* end) override {
        qreal deltaX = end->x() - start->x();
        qreal deltaY = end->y() - start->y();
        rect.translate(deltaX, deltaY);  // Use the QRectF's translate method
    }
    bool resize(QPointF* start, QPointF* end) override {
        QRectF oldRect = rect; // Store the original rectangle for comparison

        // Threshold checking for corners
        QPointF topLeft = rect.topLeft();
        QPointF topRight = rect.topRight();
        QPointF bottomLeft = rect.bottomLeft();
        QPointF bottomRight = rect.bottomRight();

        // Check if the drag start point is near one of the corners
        if (QLineF(topLeft, *start).length() < threshold) {
            // Resize by moving the top-left corner
            rect.setTopLeft(*end);
            return true;
        }
        else if (QLineF(topRight, *start).length() < threshold) {
            // Resize by moving the top-right corner
            rect.setTopRight(*end);
            return true;
        }
        else if (QLineF(bottomLeft, *start).length() < threshold) {
            // Resize by moving the bottom-left corner
            rect.setBottomLeft(*end);
            return true;
        }
        else if (QLineF(bottomRight, *start).length() < threshold) {
            // Resize by moving the bottom-right corner
            rect.setBottomRight(*end);
            return true;
        }
        // Threshold checking for edges
        // qreal midTop = (rect.left() + rect.right()) / 2;
        // qreal midLeft = (rect.top() + rect.bottom()) / 2;

        // Resize horizontally (left or right edge)
        if (qAbs(start->x() - rect.left()) < threshold) {
            rect.setLeft(end->x());
            return true;
        }
        else if (qAbs(start->x() - rect.right()) < threshold) {
            rect.setRight(end->x());
            return true;
        }

        // Resize vertically (top or bottom edge)
        if (qAbs(start->y() - rect.top()) < threshold) {
            rect.setTop(end->y());
            return true;
        }
        else if (qAbs(start->y() - rect.bottom()) < threshold) {
            rect.setBottom(end->y());
            return true;
        }

        return false; // No resizing occurred
    }

private:
    QRectF rect;
};

class EllipseItem : public GraphicItem {
public:
    EllipseItem(const QRectF& ellipse) : ellipse(ellipse) {}
    int getShape()override{return 2;}
    void setStart(QPointF* p) override{
        ellipse.setX(p->x());
        ellipse.setY(p->y());
    }
    void setEnd(QPointF* p) override{
        ellipse.setWidth(p->x()-ellipse.left());
        ellipse.setHeight(p->y()-ellipse.top());
    }
    qreal getP1X()override{
        return ellipse.topLeft().x();
    }
    qreal getP1Y()override{
        return ellipse.topLeft().y();
    }
    qreal getP2X()override{
        return ellipse.topLeft().x()+ellipse.width();
    }
    qreal getP2Y()override{
        return ellipse.topLeft().y()+ellipse.height();
    }
    void draw(QPainter* painter) const override {
        painter->drawEllipse(ellipse);
    }
    bool isInside(QPointF* p) const override{
        // Center of the ellipse
        qreal cx = ellipse.center().x();
        qreal cy = ellipse.center().y();
        // Radii of the ellipse
        qreal rx = ellipse.width() / 2.0;
        qreal ry = ellipse.height() / 2.0;
        // Check if the point satisfies the ellipse equation
        qreal x = p->x();
        qreal y = p->y();
        qreal tmp = (std::pow(x - cx, 2) / std::pow(rx, 2) + std::pow(y - cy, 2) / std::pow(ry, 2));
        bool condition1 ;
        if (ellipse.width() < 2*threshold || ellipse.height() < 2*threshold){
            condition1 = tmp <= 1.1;
        }else{
            condition1 = tmp <= 1.1 && tmp >= 0.9;
        }


        QRectF normalisedEllipse = QRectF(qMin(ellipse.left(),ellipse.right()),qMin(ellipse.top(),ellipse.bottom()),qAbs(ellipse.width()),qAbs(ellipse.height()));
        QPointF topLeft = normalisedEllipse.topLeft();
        QPointF topRight = normalisedEllipse.topRight();
        QPointF bottomLeft = normalisedEllipse.bottomLeft();
        QPointF bottomRight = normalisedEllipse.bottomRight();

        QPolygonF t1 = CornerTriangle(topLeft, threshold, 1,1);
        QPolygonF t2 = CornerTriangle(topRight, threshold, -1,1);
        QPolygonF t3 = CornerTriangle(bottomLeft, threshold, 1,-1);
        QPolygonF t4 = CornerTriangle(bottomRight, threshold, -1,-1);

        // bool condition2 = t1.containsPoint(p) || t2.containsPoint(*p) || t3.contains(*p) || t4.contains(*p);
        bool condition2 = t1.containsPoint(*p, Qt::OddEvenFill) ||
                          t2.containsPoint(*p, Qt::OddEvenFill) ||
                          t3.containsPoint(*p, Qt::OddEvenFill) ||
                          t4.containsPoint(*p, Qt::OddEvenFill);
        return condition1 || condition2;
    }
    QPolygonF CornerTriangle(QPointF corner, qreal size, int i, int j) const {
        QPolygonF triangle;
        size = size*0.5;
        triangle << QPointF(corner.x() - i*size, corner.y() - j*size)
                 << QPointF(corner.x() + i*size*3, corner.y() - j*size)
                 << QPointF(corner.x() - i*size, corner.y() + j*size*3);
        return triangle;
    }
    void showSelected(QPainter* painter) const override {
        QRectF normalisedEllipse = QRectF(qMin(ellipse.left(),ellipse.right()),qMin(ellipse.top(),ellipse.bottom()),qAbs(ellipse.width()),qAbs(ellipse.height()));
        QRectF enlargedEllipse = normalisedEllipse.adjusted(-threshold, -threshold, threshold, threshold);
        painter->drawEllipse(enlargedEllipse);

        QPointF topLeft = normalisedEllipse.topLeft();
        QPointF topRight = normalisedEllipse.topRight();
        QPointF bottomLeft = normalisedEllipse.bottomLeft();
        QPointF bottomRight = normalisedEllipse.bottomRight();

        // Draw a triangle at each corner
        drawCornerTriangle(painter, topLeft, threshold, 1,1);
        drawCornerTriangle(painter, topRight, threshold, -1,1);
        drawCornerTriangle(painter, bottomLeft, threshold, 1,-1);
        drawCornerTriangle(painter, bottomRight, threshold, -1,-1);
    }
    void drawCornerTriangle(QPainter* painter, QPointF corner, qreal size, int i, int j) const {
        QPolygonF triangle;
        triangle << corner
                 << QPointF(corner.x() + i*size, corner.y())
                 << QPointF(corner.x(), corner.y() + j*size);

        painter->drawPolygon(triangle);
    }

    void move(QPointF* start, QPointF* end) override {
        qreal deltaX = end->x() - start->x();
        qreal deltaY = end->y() - start->y();
        ellipse.translate(deltaX, deltaY);  // Use the QRectF's translate method
    }
    bool resize(QPointF* start, QPointF* end) override {
        QRectF oldEllipse = ellipse; // Store the original bounding box of the ellipse

        // Threshold checking for corners of the bounding rectangle
        QPointF topLeft = ellipse.topLeft();
        QPointF topRight = ellipse.topRight();
        QPointF bottomLeft = ellipse.bottomLeft();
        QPointF bottomRight = ellipse.bottomRight();

        // Check if the drag start point is near one of the corners
        if (QLineF(topLeft, *start).length() < threshold) {
            // Resize by moving the top-left corner
            ellipse.setTopLeft(*end);
            return true;
        }
        else if (QLineF(topRight, *start).length() < threshold) {
            // Resize by moving the top-right corner
            ellipse.setTopRight(*end);
            return true;
        }
        else if (QLineF(bottomLeft, *start).length() < threshold) {
            // Resize by moving the bottom-left corner
            ellipse.setBottomLeft(*end);
            return true;
        }
        else if (QLineF(bottomRight, *start).length() < threshold) {
            // Resize by moving the bottom-right corner
            ellipse.setBottomRight(*end);
            return true;
        }

        // Threshold checking for edges (horizontal and vertical resizing)
        if (qAbs(start->x() - ellipse.left()) < threshold) {
            // Resize by moving the left edge
            ellipse.setLeft(end->x());
            return true;
        }
        else if (qAbs(start->x() - ellipse.right()) < threshold) {
            // Resize by moving the right edge
            ellipse.setRight(end->x());
            return true;
        }
        else if (qAbs(start->y() - ellipse.top()) < threshold) {
            // Resize by moving the top edge
            ellipse.setTop(end->y());
            return true;
        }
        else if (qAbs(start->y() - ellipse.bottom()) < threshold) {
            // Resize by moving the bottom edge
            ellipse.setBottom(end->y());
            return true;
        }
        return false; // No resizing occurred
    }

private:
    QRectF ellipse;
};


#endif // GRAPHICITEM_H
