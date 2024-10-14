#include "drawgraphics.h"
#include <QPainter>
#include <QPointF>
#include <QPainterPath>
#include <QPalette>
#include <iostream>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

DrawGraphics::DrawGraphics(QWidget *parent)
    : QWidget(parent), isDrawing(false)
{
    setMinimumSize(700, 450);  // Set the size of the widget
}

DrawGraphics::~DrawGraphics() {
    for (GraphicItem* item : graphics) {
        delete item;
    }
    graphics.clear(); // Clear the vector
}


QVector<int> DrawGraphics::getSelectedParameters(){
    int cc = -1, tt = -1, ss = -1, sh = -1;
    if (isEditing && editIndex!=-1){
        QColor c = graphics[editIndex]->getColor();
        int t = graphics[editIndex]->getThickness();
        Qt::PenStyle s = graphics[editIndex]->getStyle();
        GraphicItem* item = graphics[editIndex];

        if(c == Qt::black){cc = 0;}
        else if(c == Qt::red){cc = 1;}
        else if(c == Qt::blue){cc = 2;}

        tt = t;

        if(s == Qt::SolidLine){ss = 0;}
        else if(s == Qt::DashLine){ss = 1;}
        else if(s == Qt::DotLine){ss = 2;}

        if (dynamic_cast<LineItem*>(item)) {
            sh = 0;
        } else if (dynamic_cast<RectangleItem*>(item)) {
            sh = 1;
        } else if (dynamic_cast<EllipseItem*>(item)) {
            sh = 2;
        }
    }
    QVector<int> parameters;
    parameters.append(cc);
    parameters.append(tt);
    parameters.append(ss);
    parameters.append(sh);
    return parameters;
}


void DrawGraphics::mousePressEvent(QMouseEvent *event)
{
    if (isLoading){this->setLoad(0);}
    if (event->button() == Qt::LeftButton) {
        if (isEditing){
            update();
            QPointF p = event->position();
            editIndex = -1;
            for (int i = graphics.size()-1; i >= 0; i--) {
                if (graphics[i]->isInside(&p)){
                    editIndex = i;
                    graphics[i]->setSelected(true);
                    // 把按钮们set成currentGraphic的值
                    QVector<int> parameters = getSelectedParameters();
                    emit selectedGraphicChanged(parameters[0], parameters[1], parameters[2], parameters[3]);
                    break;
                }else{
                    graphics[i]->setSelected(false);
                }
            }
            if (editIndex!=-1){
                for (int i = 0; i < editIndex; i++) {
                    graphics[i]->setSelected(false);
                }

            }
            if (editIndex!=-1){ // something is selected
                if (isMoving){
                    // 记录开始位置
                    moveStartPoint = p;
                }
                else if (isResizing){
                    // 记录开始位置
                    resizeStartPoint = p;
                }
            }
        }
        else {
            QPointF p = event->position();
            switch(currentShape){
            case 0:
                currentGraphic = new LineItem(QLineF(p,p));
                break;
            case 1:
                currentGraphic = new RectangleItem(QRectF(p,p));
                break;
            case 2:
                currentGraphic = new EllipseItem(QRectF(p,p));
                break;
            }

            currentGraphic->setColor(currentColor);
            currentGraphic->setThickness(currentThickness);
            currentGraphic->setStyle(currentStyle);
            isDrawing = true;  // Start drawing
        }
    }
}

void DrawGraphics::mouseMoveEvent(QMouseEvent *event)
{
    QPointF p = event->position();  // Update the current mouse position
    if (isEditing && editIndex!=-1){
        if (isMoving){
            graphics[editIndex]->move(&moveStartPoint,&p);
            moveStartPoint = p; // !!!
            update();
        }
        else if (isResizing && editIndex!=-1){
            if(graphics[editIndex]->resize(&resizeStartPoint,&p)){
                resizeStartPoint = p; // !!!
                update();
            }
        }
    }
    else{
        if (isDrawing) {
            currentGraphic->setEnd(&p);
            update();
        }
    }
}

void DrawGraphics::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (isDrawing){
            graphics.append(currentGraphic);
            isDrawing = false;
        }
    }
}

void DrawGraphics::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event); // to draw the widget
    QPainter painter(this); // local variable, so that it will be destroyed after execution of this function
    // painter.fillRect(this->rect(), Qt::white);
    QPen pen;

    for (int i = 0; i < graphics.size(); ++i) {
        pen.setColor(graphics[i]->getColor());
        pen.setWidth(graphics[i]->getThickness());
        pen.setStyle(graphics[i]->getStyle());
        painter.setPen(pen);
        graphics[i]->draw(&painter);
        if (graphics[i]->getSelected()){
            pen.setColor(Qt::lightGray);
            pen.setWidth(1);
            pen.setStyle(Qt::DashDotLine);
            painter.setPen(pen);
            graphics[i]->showSelected(&painter);
        }
    }
    if (currentGraphic && !isLoading) {
        pen.setColor(currentGraphic->getColor());

        pen.setWidth(currentGraphic->getThickness());

        pen.setStyle(currentGraphic->getStyle());

        painter.setPen(pen); //here's line 199

        currentGraphic->draw(&painter);

    }
}

void DrawGraphics::setColor(int c)
{
    switch(c) {
    case 0:
        currentColor = Qt::black;
        break;
    case 1:
        currentColor = Qt::red;
        break;
    case 2:
        currentColor = Qt::blue;
        break;
    }
}

void DrawGraphics::updateColor(int t){
    if (isEditing && isChangingParameters && editIndex!=-1){
        graphics[editIndex]->setColor(currentColor);
        update();  // Redraw the widget to reflect the changes
    }
}

void DrawGraphics::setThickness(int t)
{
    currentThickness = t;
}

void DrawGraphics::updateThickness(int t){
    if (isEditing && isChangingParameters && editIndex!=-1){
        graphics[editIndex]->setThickness(currentThickness);
        update();  // Redraw the widget to reflect the changes
    }
}

void DrawGraphics::setStyle(int s)
{
    switch(s) {
    case 0:
        currentStyle = Qt::SolidLine;
        break;
    case 1:
        currentStyle = Qt::DashLine;
        break;
    case 2:
        currentStyle = Qt::DotLine;
        break;
    }
}

void DrawGraphics::updateStyle(int t){
    if (isEditing && isChangingParameters && editIndex!=-1){
        graphics[editIndex]->setStyle(currentStyle);
        update();  // Redraw the widget to reflect the changes
    }
}

void DrawGraphics::setShape(int s)
{
    currentShape = s;
}

void DrawGraphics::setEdit(bool e)
{
    isEditing = e;
    if (!isEditing){
        for (int i = 0; i < graphics.size(); ++i) {
            graphics[i]->setSelected(false);
        }
        update();
    }
}

void DrawGraphics::setEditMode(int m)
{
    switch(m){
    case(0):
        isMoving = true;
        isResizing = false;
        isChangingParameters = false;
        break;
    case(1):
        isMoving = false;
        isResizing = true;
        isChangingParameters = false;
        break;
    case(2):
        isMoving = false;
        isResizing = false;
        isChangingParameters = true;
    }
}

QJsonArray DrawGraphics::toJsonArray(){
    QJsonArray JsonArray;
    for (int i = 0; i < graphics.size(); ++i){
        QJsonObject jsonItem;
        GraphicItem *item = graphics[i];
        jsonItem["shape"] = item->getShape();   // Assume `getShape` returns a string/int representing the shape type
        jsonItem["color"] = item->getColor().name();  // Assuming getColor returns a QColor object
        jsonItem["thickness"] = item->getThickness();
        jsonItem["style"] = item->getStyle();  // Assuming getStyle returns a line style
        jsonItem["p1x"] = item->getP1X();  // Position data
        jsonItem["p1y"] = item->getP1Y();
        jsonItem["p2x"] = item->getP2X();
        jsonItem["p2y"] = item->getP2Y();
        JsonArray.append(jsonItem);
    }
    return JsonArray;
};


void DrawGraphics::drawJsonArray(QJsonArray jsonArray){
    for (GraphicItem* item : graphics) {
        delete item;  // Free memory for each item
    }
    graphics.clear();  // Clear the container

    update();
    for (const QJsonValue& value : jsonArray) {
        QJsonObject jsonItem = value.toObject();
        int shape = jsonItem["shape"].toInt();
        GraphicItem* item;
        switch (shape) {
        case 0:
            item = new LineItem(QLine());
            break;
        case 1:
            item = new RectangleItem(QRectF());
            break;
        case 2:
            item = new EllipseItem(QRectF());
            break;
        }
        item->setColor(QColor(jsonItem["color"].toString()));
        item->setThickness(jsonItem["thickness"].toInt());
        int style = jsonItem["style"].toInt();
        switch (style) {
        case 0:
            currentStyle = Qt::SolidLine;
            break;
        case 1:
            currentStyle = Qt::DashLine;
            break;
        case 2:
            currentStyle = Qt::DotLine;
            break;
        }
        QPointF* p;
        p->setX(jsonItem["p1x"].toDouble());
        p->setY(jsonItem["p1y"].toDouble());
        item->setStart(p);
        p->setX(jsonItem["p2x"].toDouble());
        p->setY(jsonItem["p2y"].toDouble());
        item->setEnd(p);
        this->graphics.append(item);
    }
    update();
}


void DrawGraphics::setLoad(int l){
    isLoading = l;
}
