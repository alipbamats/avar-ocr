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
#include <windows.h>
#include <QWindow>
#include <filesystem>
#include <thread> 
#include <filesystem>
#include "switch.h"
#include <fstream>
#include <chrono>
#include <cstdlib>

struct OCRImageInfo
{
    QString filename;
    QString textFilename;
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

    Ui::AvarOCRClass ui;

    void OpenFileDialog();
    void initImagesListWidget();
    void initOcrArea();
    void initMenuBar();
    void initOperationsPannel();
    void initWorkFolder();
    
    void handleStartOCR();
    void handleViewFormatSwitch(bool checked);
    void handleImagesListWidgetDoubleClick(QListWidgetItem* item);

    void viewProcess(std::wstring processName, QList<QString> args, QString filename, DWORD  dwCreationFlags, const wchar_t* envBlock);
    void initNotepad();

    //void viewAsText(QScrollArea * scrollArea, QString text);
    //void viewAsImage(QScrollArea * scrollArea, QString imagePath);
    bool killProcess(DWORD pid);
    void ocrThread();
  

    QListWidget* imagesListWidget = NULL;
    QSplitter* windowSplitter = NULL;
    QSplitter* mainSplitter = NULL;
    QScrollArea* ocrArea = NULL;
    QLabel* switchLabel = NULL;
    SwitchButton* viewFormatSwitch = NULL;
    QLabel* infoLabel = NULL;
    OCRImageInfo* curentOCRImageInfo = NULL;

    DWORD lastPID = NULL;

    QList<OCRImageInfo> OCRImageInfoList;
    const std::wstring execNotepadPath = L".\\Notepad++\\notepad++.exe";
    const std::wstring execPaintPath = L"C:\\Windows\\system32\\mspaint.exe";
    const std::wstring workFolder = L".\\~tmp";
};

