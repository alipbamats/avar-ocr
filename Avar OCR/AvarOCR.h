#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AvarOCR.h"
#include <QListWidget>
#include <QSplitter>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QTextEdit>
#include <QFileDialog>
#include <QIcon>
#include <QStringList>
#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QCheckBox>

#include <iostream>
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>

#include "switch.h"

struct OCRImageInfo
{
    QString filename;
    QString ocrText;
    QWidget* container = NULL;
    QListWidgetItem* item = NULL;
    QVBoxLayout* layout = NULL;
};

class AvarOCR : public QMainWindow
{
    Q_OBJECT

public:
    AvarOCR(QWidget *parent = nullptr);
    ~AvarOCR();

private:
    Ui::AvarOCRClass ui;

    void OpenFileDialog();
    void initImagesListWidget();
    void initOcrArea();
    void initMenuBar();
    void initOperationsPannel();
    
    void handleStartOCR();
    void handleViewFormatSwitch(bool checked);
    void handleImagesListWidgetDoubleClick(QListWidgetItem* item);

    void viewAsText(QScrollArea * scrollArea, QString text);
    void viewAsImage(QScrollArea * scrollArea, QString imagePath);

    QListWidget* imagesListWidget = NULL;
    QSplitter* windowSplitter = NULL;
    QSplitter* mainSplitter = NULL;
    QScrollArea* ocrArea = NULL;
    QLabel* switchLabel = NULL;
    SwitchButton* viewFormatSwitch = NULL;
    QLabel* infoLabel = NULL;
    OCRImageInfo* curentOCRImageInfo = NULL;

    QList<OCRImageInfo> OCRImageInfoList;
};

