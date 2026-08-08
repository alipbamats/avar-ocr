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
    void initStartOCRButton();
    
    void handleStartOCR();
    void handleImagesListWidgetDoubleClick(QListWidgetItem* item);

    QListWidget* imagesListWidget = NULL;
    QSplitter* windowSplitter = NULL;
    QSplitter* mainSplitter = NULL;
    QScrollArea* ocrArea = NULL;

    QList<OCRImageInfo> OCRImageInfoList;
};

