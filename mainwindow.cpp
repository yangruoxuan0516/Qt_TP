#include <QMessageBox>
// #include <iostream>
#include <QBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include "mainwindow.h"
// #include "ui_mainwindow.h"
#include "drawgraphics.h"
#include <QMenuBar>
#include <QToolBar>
// #include "graphicitem.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QFileDialog>
#include <QTextEdit>
#include <QPainterPath>
#include <iostream>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    // , ui(new Ui::MainWindow)
{
    // ui->setupUi(this);
    QWidget * mainWidget = new QWidget(this);
    QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

    this->dg = new DrawGraphics(this);


    QHBoxLayout *buttonLayout = new QHBoxLayout();

// --- Code to create buttons for color selection ---
    QVBoxLayout *colorLayout = new QVBoxLayout();
    // Create radio buttons
    QRadioButton *blackButton = new QRadioButton("Black", this);
    QRadioButton *redButton = new QRadioButton("Red", this);
    QRadioButton *blueButton = new QRadioButton("Blue", this);
    blackButton->setChecked(true);
    // Create a QButtonGroup and add the buttons
    // QButtonGroup *colorGroup = new QButtonGroup(this);
    colorGroup = new QButtonGroup(this);
    colorGroup->addButton(blackButton, 0);  // Assign an ID to each button
    colorGroup->addButton(redButton, 1);
    colorGroup->addButton(blueButton, 2);
    // Add buttons to layout
    colorLayout->addWidget(blackButton);
    colorLayout->addWidget(redButton);
    colorLayout->addWidget(blueButton);

    buttonLayout->addLayout(colorLayout);
    connect(colorGroup, QOverload<int>::of(&QButtonGroup::idClicked), this->dg, &DrawGraphics::setColor);
    connect(colorGroup, QOverload<int>::of(&QButtonGroup::idClicked), this->dg, &DrawGraphics::updateColor);

// --- Code to create buttons for thickness selection ---
    QVBoxLayout *thicknessLayout = new QVBoxLayout();
    // Create radio buttons for thickness
    QRadioButton *thinButton = new QRadioButton("Thin", this);
    QRadioButton *mediumButton = new QRadioButton("Medium", this);
    QRadioButton *thickButton = new QRadioButton("Thick", this);
    thinButton->setChecked(true);
    // Create a QButtonGroup and add the buttons
    // QButtonGroup *thicknessGroup = new QButtonGroup(this);
    thicknessGroup = new QButtonGroup(this);
    thicknessGroup->addButton(thinButton, 1);    // ID 0 for thin
    thicknessGroup->addButton(mediumButton, 2);  // ID 1 for medium
    thicknessGroup->addButton(thickButton, 3);   // ID 2 for thick
    // Add buttons to layout
    thicknessLayout->addWidget(thinButton);
    thicknessLayout->addWidget(mediumButton);
    thicknessLayout->addWidget(thickButton);

    // Add thickness layout to the main layout
    buttonLayout->addStretch();
    buttonLayout->addLayout(thicknessLayout);

    // Connect thickness buttons to change thickness
    connect(thicknessGroup, QOverload<int>::of(&QButtonGroup::idClicked), this->dg, &DrawGraphics::setThickness);
    // Connect thicknessGroup signal to update slot in DrawGraphics
    connect(thicknessGroup, QOverload<int>::of(&QButtonGroup::idClicked), this->dg, &DrawGraphics::updateThickness);


// --- Code to create buttons for style selection ---
    // Add layout for the line style buttons
    QVBoxLayout *styleLayout = new QVBoxLayout();

    // Create radio buttons for line styles
    QRadioButton *solidButton = new QRadioButton("Solid Line", this);
    QRadioButton *dashButton = new QRadioButton("Dash Line", this);
    QRadioButton *dotButton = new QRadioButton("Dot Line", this);
    solidButton->setChecked(true);

    // Create a QButtonGroup and add the buttons
    // QButtonGroup *styleGroup = new QButtonGroup(this);
    styleGroup = new QButtonGroup(this);
    styleGroup->addButton(solidButton, 0);  // ID 0 for solid line
    styleGroup->addButton(dashButton, 1);   // ID 1 for dash line
    styleGroup->addButton(dotButton, 2);    // ID 2 for dot line

    // Add buttons to the layout
    styleLayout->addWidget(solidButton);
    styleLayout->addWidget(dashButton);
    styleLayout->addWidget(dotButton);

    // Add the layout to the main layout
    buttonLayout->addStretch();
    buttonLayout->addLayout(styleLayout);

    // Connect the button group to the setStyle(int) function in DrawGraphics
    connect(styleGroup, QOverload<int>::of(&QButtonGroup::idClicked), this->dg, &DrawGraphics::setStyle);
    connect(styleGroup, QOverload<int>::of(&QButtonGroup::idClicked), this->dg, &DrawGraphics::updateStyle);

// --- Code to create buttons for shape selection ---
    // Add layout for the shape selection buttons
    // QVBoxLayout *shapeLayout = new QVBoxLayout();
    // Create radio buttons for shape selection
    QRadioButton *lineButton = new QRadioButton("Line", this);
    QRadioButton *rectButton = new QRadioButton("Rectangle", this);
    QRadioButton *ellipseButton = new QRadioButton("Ellipse", this);
    lineButton->setChecked(true);
    // Create a QButtonGroup and add the buttons
    // QButtonGroup *shapeGroup = new QButtonGroup(this);
    shapeGroup = new QButtonGroup(this);
    shapeGroup->addButton(lineButton, 0);   // ID 0 for line
    shapeGroup->addButton(rectButton, 1);   // ID 1 for rectangle
    shapeGroup->addButton(ellipseButton, 2); // ID 2 for ellipse
    // Add buttons to the layout
    // shapeLayout->addWidget(lineButton);
    // shapeLayout->addWidget(rectButton);
    // shapeLayout->addWidget(ellipseButton);
    // Add the layout to the main layout
    // buttonLayout->addStretch();
    // buttonLayout->addLayout(shapeLayout);
    // Connect the button group to the setShape(int) function in DrawGraphics
    connect(shapeGroup, QOverload<int>::of(&QButtonGroup::idClicked), this->dg, &DrawGraphics::setShape);


// --- Code to create buttons for edit ---
    // Create a button for Edit Mode
    QPushButton *editModeButton = new QPushButton("Draw Mode - click to switch to Edit Mode", this);
    editModeButton->setCheckable(true);  // Make the button toggleable
    editModeButton->setDefault(false);

    // Create radio buttons for editing actions (Move and Resize)
    QRadioButton *moveButton = new QRadioButton("Move", this);
    QRadioButton *resizeButton = new QRadioButton("Resize", this);
    QRadioButton *changeParameterButton = new QRadioButton("Change Parameters", this);

    // Initially hide the radio buttons since Edit Mode is not active
    moveButton->setVisible(false);
    resizeButton->setVisible(false);
    changeParameterButton->setVisible(false);

    // Add the Edit Mode button and the radio buttons to the layout
    QWidget *editWidget = new QWidget(this);
    editWidget->setFixedHeight(110);
    QVBoxLayout *editLayout = new QVBoxLayout(editWidget);
    editLayout->addWidget(editModeButton);
    editLayout->addStretch();
    editLayout->addWidget(moveButton);
    editLayout->addWidget(resizeButton);
    editLayout->addWidget(changeParameterButton);

    editLayout->addStretch();
    editLayout->addWidget(lineButton);
    editLayout->addWidget(rectButton);
    editLayout->addWidget(ellipseButton);


    QButtonGroup *editGroup = new QButtonGroup(this);
    editGroup->addButton(moveButton, 0);   // ID 0 for line
    editGroup->addButton(resizeButton, 1);   // ID 1 for rectangle
    editGroup->addButton(changeParameterButton,2);

    // Connect the button group to the setShape(int) function in DrawGraphics
    connect(editGroup, QOverload<int>::of(&QButtonGroup::idClicked), this->dg, &DrawGraphics::setEditMode);

// set parameters when it's edit mode && something is selected && we want to change it's parameters
    connect(this->dg, &DrawGraphics::selectedGraphicChanged, this, &MainWindow::setParametersFromSelection);

// update graphic parameters, not just update "currentxxx" but call "setxxx" on graphic[editIndex], if in edit mode && something is selected && isChangingParameters


    connect(editModeButton, &QPushButton::toggled, this, [=](bool checked) {
        if (checked) {
            // Switch to Edit Mode
            editModeButton->setText("Edit Mode - click to switch to Draw Mode");

            // Show edit action buttons
            moveButton->setVisible(true);
            resizeButton->setVisible(true);
            changeParameterButton->setVisible(true);

            // Hide shape selection buttons
            lineButton->setVisible(false);
            rectButton->setVisible(false);
            ellipseButton->setVisible(false);
        } else {
            // Switch to Draw Mode
            editModeButton->setText("Draw Mode - click to switch to Edit Mode");

            // Hide edit action buttons
            moveButton->setVisible(false);
            resizeButton->setVisible(false);
            changeParameterButton->setVisible(false);

            // Show shape selection buttons
            lineButton->setVisible(true);
            rectButton->setVisible(true);
            ellipseButton->setVisible(true);
        }
    });
    connect(editModeButton, &QPushButton::toggled, this->dg, &DrawGraphics::setEdit);




    mainLayout->addWidget(editWidget);
    mainLayout->addWidget(this->dg);
    mainLayout->addSpacing(50);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addSpacing(20);


    // Connect menu actions to save and open functions
    // Create a menu bar
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // Create "File" menu
    QMenu *fileMenu = menuBar->addMenu(tr("&File"));

    // Create Save action
    QAction *saveAction = new QAction(QIcon(":/images/mySAVE.png"), tr("&Save"), this);
    saveAction->setShortcut(QKeySequence::Save);  // Optional: Set shortcut Ctrl+S
    fileMenu->addAction(saveAction);

    // Create Open action
    QAction *openAction = new QAction(QIcon(":/images/myOPEN.png"), tr("&Open"), this);
    openAction->setShortcut(QKeySequence::Open);  // Optional: Set shortcut Ctrl+O
    fileMenu->addAction(openAction);

    // Create a toolbar and add the actions
    QToolBar *toolbar = addToolBar(tr("File"));
    toolbar->addAction(saveAction);
    toolbar->addAction(openAction);


    connect(saveAction, &QAction::triggered, this, &MainWindow::savePainting);
    connect(openAction, &QAction::triggered, this, &MainWindow::loadPainting);


    this->setCentralWidget(mainWidget);
}


void MainWindow::setParametersFromSelection(int color, int thickness, int style, int shape)
{
    // 设置颜色按钮
    QAbstractButton* button = colorGroup->button(color);
    if (button) {
        button->click();  // 模拟点击，触发 signal 和对应的槽函数
    }

    // 设置厚度按钮
    button = thicknessGroup->button(thickness);
    if (button) {
        button->click();  // 模拟点击
    }  // 设置样式按钮
    button = styleGroup->button(style);
    if (button) {
        button->click();  // 模拟点击
    }
    button = shapeGroup->button(shape);
    if (button) {
        button->click();  // 模拟点击
    }
}


MainWindow::~MainWindow()
{
    // delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Exit Confirmation"),
                                  tr("Are you sure you want to exit?"),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        event->accept();
    } else {
        event->ignore();
    }
}




void MainWindow::savePainting() {
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save File"),
                                                    "/Users/ruox/Documents/DoubleDegree/cours_2/IGR201/TP2/test_",
                                                    tr("JSON Files (*.json)"));

    if (!filename.isEmpty()) {
        QFile file(filename);

        if (file.open(QIODevice::WriteOnly)) {
            QJsonArray jsonArray = this->dg->toJsonArray();
            QJsonDocument jsonDoc(jsonArray);
            file.write(jsonDoc.toJson());  // Write the JSON to the file
            file.close();
        }
    }
}

// Slot for opening a painting
void MainWindow::loadPainting() {
    this->dg->setLoad(1);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Painting"), "/Users/ruox/Documents/DoubleDegree/cours_2/IGR201/TP2", tr("JSON Files (*.json)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray fileData = file.readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
            QJsonArray jsonArray = jsonDoc.array();
            this->dg->drawJsonArray(jsonArray);
            file.close();
        }
    }
}
