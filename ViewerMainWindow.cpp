#include <iostream>
#include <stdio.h>
#include <QtGui>
#include <QGLFormat>
#include "GLDisplay.h"
#include "ViewerMainWindow.h"

ViewerMainWindow::ViewerMainWindow()
{
#if 0
    QGLFormat fmt;
    fmt.setProfile(QGLFormat::CoreProfile);
    fmt.setVersion(3,2);
    QGLFormat::setDefaultFormat(fmt);
#endif

    QWidget *central = new QWidget;
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    central->setLayout(mainLayout);

    mGLDisplay = new GLDisplay();
    mGLDisplay->setMinimumSize(QSize(875, 500));
    mainLayout->addWidget(mGLDisplay, 1);

    QVBoxLayout *controlsLayout = new QVBoxLayout();
    mainLayout->addLayout(controlsLayout);

    const int maxGroupWidth = 300;
    QGroupBox *lightPosGroupBox = new QGroupBox("Light Position");
    lightPosGroupBox->setMaximumWidth(maxGroupWidth);
    QGroupBox *viewGroupBox = new QGroupBox("Perspective Controls");
    viewGroupBox->setMaximumWidth(maxGroupWidth);
    QGroupBox *cameraGroupBox = new QGroupBox("Camera Controls");
    cameraGroupBox->setMaximumWidth(maxGroupWidth);
    QGroupBox *rotateGroupBox = new QGroupBox("Rotate Shape");
    rotateGroupBox->setMaximumWidth(maxGroupWidth);
    QGroupBox *lightGroupBox = new QGroupBox("Lighting Options");
    lightGroupBox->setMaximumWidth(maxGroupWidth);
    QGroupBox *colorGroupBox = new QGroupBox("Color Options");
    colorGroupBox->setMaximumWidth(maxGroupWidth);
    QGroupBox *fileGroupBox = new QGroupBox("File");
    fileGroupBox->setMaximumWidth(maxGroupWidth);
    QGroupBox *shadingGroupBox = new QGroupBox("Shading Options");
    shadingGroupBox->setMaximumWidth(maxGroupWidth);

    QHBoxLayout *refreshLayout = new QHBoxLayout();

    controlsLayout->addWidget(lightPosGroupBox);
    controlsLayout->addWidget(viewGroupBox);
    controlsLayout->addWidget(cameraGroupBox);
    controlsLayout->addWidget(rotateGroupBox);
    controlsLayout->addWidget(lightGroupBox);
    controlsLayout->addWidget(colorGroupBox);
    controlsLayout->addWidget(fileGroupBox);
    controlsLayout->addWidget(shadingGroupBox);
    controlsLayout->addLayout(refreshLayout);
    controlsLayout->addStretch();

    //Translation controls
    QHBoxLayout *lightPosGroupBoxLayout = new QHBoxLayout;
    lightPosGroupBox->setLayout(lightPosGroupBoxLayout);

    QPushButton *light = new QPushButton("Move");

    QLabel *lightXLabel = new QLabel(" X");
    lightPosGroupBoxLayout->addWidget(lightXLabel);
    mXLightLineEdit = new QLineEdit;
    mXLightLineEdit->setText("0.0");
    mXLightLineEdit->setFixedWidth(lineWidth);
    lightPosGroupBoxLayout->addWidget(mXLightLineEdit);

    QLabel *lightYLabel = new QLabel(" Y");
    lightPosGroupBoxLayout->addWidget(lightYLabel);
    mYLightLineEdit = new QLineEdit;
    mYLightLineEdit->setText("0.0");
    mYLightLineEdit->setFixedWidth(lineWidth);
    lightPosGroupBoxLayout->addWidget(mYLightLineEdit);

    QLabel *lightZLabel = new QLabel(" Z");
    lightPosGroupBoxLayout->addWidget(lightZLabel);
    mZLightLineEdit = new QLineEdit;
    mZLightLineEdit->setText("2.0");
    mZLightLineEdit->setFixedWidth(lineWidth);
    lightPosGroupBoxLayout->addWidget(mZLightLineEdit);

    lightPosGroupBoxLayout->addWidget(light);
    QObject::connect(light, SIGNAL(clicked()), this, SLOT(lightPosChanged()));
    lightPosGroupBoxLayout->addStretch();

    //Perspective controls
    QHBoxLayout *viewGroupBoxLayout = new QHBoxLayout;
    viewGroupBox->setLayout(viewGroupBoxLayout);

    QPushButton *view = new QPushButton("Change\nPerspective");

    QLabel *fovyLabel = new QLabel(" Fovy");
    viewGroupBoxLayout->addWidget(fovyLabel);
    mFOV = new QDial();
    mFOV->setMaximumWidth(dialWidth);
    mFOV->setRange(1,fovMax-1);
    mFOV->setNotchTarget(fovMax/20);
    mFOV->setNotchesVisible(1);
    mFOV->setValue(45);
    mFOV->setSingleStep(fovMax/72);
    mFOV->setPageStep(fovMax/8);
    viewGroupBoxLayout->addWidget(mFOV);
    viewGroupBoxLayout->addWidget(mFOV);
    QObject::connect(mFOV, SIGNAL(valueChanged(int)), this, SLOT(viewChanged()));

    QHBoxLayout *nBox = new QHBoxLayout;
    QLabel *nearLabel = new QLabel("Near");
    nBox->addStretch();
    nBox->addWidget(nearLabel);
    mNear = new QLineEdit;
    mNear->setText("0.5");
    mNear->setFixedWidth(lineWidth);
    nBox->addWidget(mNear);

    QHBoxLayout *fBox = new QHBoxLayout;
    QLabel *farLabel = new QLabel(" Far");
    fBox->addStretch();
    fBox->addWidget(farLabel);
    mFar = new QLineEdit;
    mFar->setText("5.0");
    mFar->setFixedWidth(lineWidth);
    fBox->addWidget(mFar);

    QVBoxLayout *N_FBox = new QVBoxLayout;
    N_FBox->addLayout(nBox);
    N_FBox->addLayout(fBox);
    viewGroupBoxLayout->addLayout(N_FBox);

    viewGroupBoxLayout->addWidget(view);
    QObject::connect(view, SIGNAL(clicked()), this, SLOT(viewChanged()));

    viewGroupBoxLayout->addStretch();

    //Camera Controls
    QHBoxLayout *cameraGroupBoxLayout = new QHBoxLayout;
    cameraGroupBox->setLayout(cameraGroupBoxLayout);

    QLabel *rLabel = new QLabel("  r");
    cameraGroupBoxLayout->addWidget(rLabel);
    mR = new QLineEdit;
    mR->setText("3.0");
    mR->setFixedWidth(lineWidth);
    cameraGroupBoxLayout->addWidget(mR);

    QLabel *cameraLabels[2];
    cameraLabels[0] = new QLabel(QString("  ") + QChar(0x98, 0x03));
    cameraLabels[1] = new QLabel(QString("  ") + QChar(0xC6, 0x03));

    for(int i=0; i<2; i++){
        cameraDials[i] = new QDial();
        cameraDials[i]->setMaximumWidth(dialWidth);
        cameraDials[i]->setRange(0,cameraMax-1);
        cameraDials[i]->setNotchTarget(cameraMax/35);
        cameraDials[i]->setNotchesVisible(1);
        cameraDials[i]->setWrapping(1);
        cameraDials[i]->setValue(0);
        cameraDials[i]->setSingleStep(cameraMax/72);
        cameraDials[i]->setPageStep(cameraMax/8);
        cameraGroupBoxLayout->addWidget(cameraLabels[i]);
        cameraGroupBoxLayout->addWidget(cameraDials[i]);
        QObject::connect(cameraDials[i], SIGNAL(valueChanged(int)), this, SLOT(cameraChanged()));
    }//for
    cameraGroupBoxLayout->addStretch();

    //Rotation Controls
    QHBoxLayout *rotateGroupBoxLayout = new QHBoxLayout;
    rotateGroupBox->setLayout(rotateGroupBoxLayout);

    QLabel *axesLabels[3];
    axesLabels[0] = new QLabel(" X");
    axesLabels[1] = new QLabel(" Y");
    axesLabels[2] = new QLabel(" Z");

    for(int i=0; i<3; i++){
        axesDials[i] = new QDial();
        axesDials[i]->setMaximumWidth(dialWidth);
        axesDials[i]->setRange(0,axesMax-1);
        axesDials[i]->setNotchTarget(axesMax/35);
        axesDials[i]->setNotchesVisible(1);
        axesDials[i]->setWrapping(1);
        axesDials[i]->setValue(0);
        axesDials[i]->setSingleStep(axesMax/72);
        axesDials[i]->setPageStep(axesMax/8);
        rotateGroupBoxLayout->addWidget(axesLabels[i]);
        rotateGroupBoxLayout->addWidget(axesDials[i]);
        QObject::connect(axesDials[i], SIGNAL(valueChanged(int)), this, SLOT(rotateChanged()));
    }//for
    rotateGroupBoxLayout->addStretch();

    //Light controls
    QHBoxLayout *lightGroupBoxLayout = new QHBoxLayout;
    lightGroupBox->setLayout(lightGroupBoxLayout);

    QLabel *lightLabels[2];
    lightLabels[0] = new QLabel(" Light\nIntensity");
    lightLabels[1] = new QLabel(" Shine\nCoefficient");

    for(int i=0; i<2; i++){
        lightDials[i] = new QDial();
        lightDials[i]->setMaximumWidth(dialWidth);
        lightDials[i]->setRange(0,lightNotch-1);
        lightDials[i]->setNotchTarget(lightNotch/10);
        lightDials[i]->setNotchesVisible(1);
        if(i==0)
            lightDials[i]->setValue(0.2*lightNotch);
        else if(i==1)
            lightDials[i]->setValue(0.5*lightNotch);
        lightDials[i]->setSingleStep(lightNotch/100);
        lightDials[i]->setPageStep(lightNotch/10);
        lightGroupBoxLayout->addWidget(lightLabels[i]);
        lightGroupBoxLayout->addWidget(lightDials[i]);
        QObject::connect(lightDials[i], SIGNAL(valueChanged(int)), this, SLOT(lightChanged()));
    }//for
    lightGroupBoxLayout->addStretch();

    //Color Controls
    QHBoxLayout *colorGroupBoxLayout = new QHBoxLayout;
    colorGroupBox->setLayout(colorGroupBoxLayout);

    QLabel *colorLabels[3];
    colorLabels[0] = new QLabel(" Red");
    colorLabels[1] = new QLabel(" Green");
    colorLabels[2] = new QLabel(" Blue");

    for(int i=0; i<3; i++){
        colorDials[i] = new QDial();
        colorDials[i]->setMaximumWidth(dialWidth);
        colorDials[i]->setRange(0,colorMax-1);
        colorDials[i]->setNotchTarget(colorMax/10);
        colorDials[i]->setNotchesVisible(1);
        colorDials[i]->setValue(colorMax/2);
        colorDials[i]->setSingleStep(colorMax/100);
        colorDials[i]->setPageStep(colorMax/10);
        colorGroupBoxLayout->addWidget(colorLabels[i]);
        colorGroupBoxLayout->addWidget(colorDials[i]);
        QObject::connect(colorDials[i], SIGNAL(valueChanged(int)), this, SLOT(colorChanged()));
    }//for
    colorGroupBoxLayout->addStretch();

    //file controls
    QHBoxLayout *fileGroupBoxLayout = new QHBoxLayout;
    fileGroupBox->setLayout(fileGroupBoxLayout);

    QPushButton *load = new QPushButton("Load File");

    QLabel *fileLabel = new QLabel(" File Name");
    fileGroupBoxLayout->addWidget(fileLabel);
    file = new QLineEdit;
    file->setText("cube.obj");
    fileGroupBoxLayout->addWidget(file);

    fileGroupBoxLayout->addWidget(load);
    QObject::connect(load, SIGNAL(clicked()), this, SLOT(fileChanged()));
    fileGroupBoxLayout->addStretch();

    //shading options
    QHBoxLayout *shadingGroupBoxLayout = new QHBoxLayout;
    shadingGroupBox->setLayout(shadingGroupBoxLayout);

    shadingOptions = new QComboBox();
    shadingGroupBoxLayout->addWidget(shadingOptions);
    smooth = QString("Smooth Shading");
    phong = QString("Phong Shading");
    shadingOptions->addItem(smooth);
    shadingOptions->addItem(phong);
    QObject::connect(shadingOptions, SIGNAL(currentIndexChanged(QString)),
                     this, SLOT(shadingChanged(QString)));

    //refresh button
    refreshLayout->addStretch();
    QPushButton *refresh = new QPushButton("Refresh");
    refreshLayout->addWidget(refresh);
    refreshLayout->addStretch();

    QObject::connect(refresh, SIGNAL(clicked()), this, SLOT(refresh()));

    QObject::connect(mXLightLineEdit, SIGNAL(returnPressed()), this, SLOT(refresh()));
    QObject::connect(mYLightLineEdit, SIGNAL(returnPressed()), this, SLOT(refresh()));
    QObject::connect(mZLightLineEdit, SIGNAL(returnPressed()), this, SLOT(refresh()));

    QObject::connect(mNear, SIGNAL(returnPressed()), this, SLOT(refresh()));
    QObject::connect(mFar, SIGNAL(returnPressed()), this, SLOT(refresh()));

    QObject::connect(mR, SIGNAL(returnPressed()), this, SLOT(refresh()));

    QObject::connect(file, SIGNAL(returnPressed()), this, SLOT(refresh()));

}//ViewerMainWindow Constructor

void ViewerMainWindow::lightPosChanged()
{
    mGLDisplay->setLightPos(mXLightLineEdit->text().toDouble(),
                             mYLightLineEdit->text().toDouble(),
                             mZLightLineEdit->text().toDouble());
}//lightChanged

void ViewerMainWindow::viewChanged(){
    mGLDisplay->setView(mNear->text().toDouble(),
                        mFar->text().toDouble(),
                        mFOV->value());
}//viewChanged

void ViewerMainWindow::cameraChanged(){
    mGLDisplay->setCamera(
        mR->text().toDouble(),
        (double)cameraDials[0]->value(),
        (double)cameraDials[1]->value());
}//cameraChanged

void ViewerMainWindow::rotateChanged(){
    mGLDisplay->setRotate(
        (double)axesDials[0]->value(),
        (double)axesDials[1]->value(),
        (double)axesDials[2]->value());
}//rotateChanged

void ViewerMainWindow::lightChanged(){
    mGLDisplay->setLight(
        intensityMax*lightDials[0]->value()/lightNotch,
        shineMax*lightDials[1]->value()/lightNotch);
}//lightChanged

void ViewerMainWindow::colorChanged(){
    mGLDisplay->setColor(
        (double)colorDials[0]->value()/colorMax,
        (double)colorDials[1]->value()/colorMax,
        (double)colorDials[2]->value()/colorMax);
}//colorChanged

void ViewerMainWindow::fileChanged(){
    mGLDisplay->setFile(file->text());
}//loadFile

void ViewerMainWindow::shadingChanged(QString curText){
    if(curText.compare(smooth) == 0)
        mGLDisplay->setShading(GLDisplay::smoothOptionNumber);
    else if(curText.compare(phong) == 0)
        mGLDisplay->setShading(GLDisplay::phongOptionNumber);
}//shadingChanged

void ViewerMainWindow::refresh(){
    lightPosChanged();
    viewChanged();
    cameraChanged();
    rotateChanged();
    lightChanged();
    colorChanged();
    fileChanged();
}//refresh
