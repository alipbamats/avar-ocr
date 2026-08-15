#include "AvarOCR.h"


// Structure to pass data into the enumeration callback
struct HandleData {
    DWORD processId;
    HWND windowHandle;
};

// Callback function to evaluate each top-level window
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    HandleData& data = *reinterpret_cast<HandleData*>(lParam);
    DWORD processId = 0;

    // Get the process ID of the iterated window
    GetWindowThreadProcessId(hwnd, &processId);

    // Ensure it belongs to our target process and is the main top-level window
    if (data.processId == processId && GetWindow(hwnd, GW_OWNER) == NULL && IsWindowVisible(hwnd)) {
        data.windowHandle = hwnd;
        return FALSE; // Stop enumerating, we found it!
    }
    return TRUE; // Continue scanning
}

HWND GetWindowHandleByPID(DWORD processId) {
    HandleData data;
    data.processId = processId;
    data.windowHandle = NULL;
    while (true)
    {
        EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&data));
        if (data.windowHandle) break;
    }

    return data.windowHandle;
}


AvarOCR::AvarOCR(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.setupUi(this);

    this->mainSplitter = new QSplitter(Qt::Vertical);

    initOperationsPannel();
    
    this->windowSplitter = new QSplitter(Qt::Horizontal);
    this->windowSplitter->setWindowTitle("Split Window Example");
    this->windowSplitter->resize(600, 400);

    this->mainSplitter->addWidget(this->windowSplitter);

    this->initImagesListWidget();
    this->initOcrArea();
    this->initMenuBar();

    this->initWorkFolder();
    this->initNotepad();

    this->infoLabel = new QLabel("Start", this);
    this->infoLabel->setFixedHeight(25);
    this->mainSplitter->addWidget(this->infoLabel);
    this->setCentralWidget(this->mainSplitter);
}


void AvarOCR::initNotepad()
{
    const char* username = std::getenv("USERNAME");
    std::filesystem::path notapadConfigFolder = L"C:\\Users\\"/std::filesystem::path(username)/L"AppData\\Roaming\\Notepad++\\plugins\\Config";
    std::filesystem::path commonHunspellFolder = L"C:\\Users\\" / std::filesystem::path(username) / L"AppData\\Roaming\\Notepad++\\plugins\\Config\\Hunspell";
    std::filesystem::path localHunspellDir = ".\\Notepad++\\Hunspell";
    if (!std::filesystem::is_directory(notapadConfigFolder))
    {
        STARTUPINFO si;
       
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;

        ZeroMemory(&pi, sizeof(pi));
        if (!CreateProcess(NULL, (LPWSTR)this->execNotepadPath.c_str(), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi)) {
            std::cerr << "CreateProcess failed. Error: " << GetLastError() << std::endl;
            return;
        }
        uint64_t start_time = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
        while (true)
        {
            uint64_t current_time = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count();
           // if ((current_time - start_time) > 4) break;
           
            if (std::filesystem::is_directory(notapadConfigFolder)) break;
        }
        this->killProcess(pi.dwProcessId);
    }
    if (!std::filesystem::is_directory(commonHunspellFolder))
    {
        std::filesystem::create_directory(commonHunspellFolder);
        const auto copyOptions = std::filesystem::copy_options::recursive
            | std::filesystem::copy_options::skip_existing;

        std::filesystem::copy(localHunspellDir, commonHunspellFolder, copyOptions);
    }
}

void AvarOCR::initOperationsPannel()
{
    QWidget* containerArea = new QWidget(this->mainSplitter);
    containerArea->setFixedHeight(40);

    // Create a layout for the container
    QHBoxLayout* layout = new QHBoxLayout(containerArea);
    layout->setContentsMargins(5, 5, 0, 0); // Optional spacing
    //layout->setSpacing(100);
    layout->setAlignment(Qt::AlignLeft);
    QPushButton* runOCRButton = new QPushButton("Run OCR", this);
    //runOCRButton->setFixedHeight(35);
    //runOCRButton->setFixedWidth(55);
    connect(runOCRButton, &QPushButton::clicked, this, &AvarOCR::handleStartOCR);
    layout->addWidget(runOCRButton);

    this->viewFormatSwitch = new SwitchButton(this);
    this->viewFormatSwitch->setFixedHeight(25);
    this->viewFormatSwitch->setFixedWidth(50);
    this->viewFormatSwitch->setChecked(false);

    connect(viewFormatSwitch, &QPushButton::toggled, this, &AvarOCR::handleViewFormatSwitch);

    layout->addWidget(viewFormatSwitch);

    switchLabel = new QLabel("Image", this);
    layout->addWidget(switchLabel);

    containerArea->setLayout(layout);

    this->mainSplitter->addWidget(containerArea);

}

void AvarOCR::initWorkFolder()
{
    if (std::filesystem::is_directory(this->workFolder))
    {
       std::filesystem::remove_all(this->workFolder);
    }
    std::filesystem::create_directory(this->workFolder);
}

void AvarOCR::initMenuBar()
{
    QMenuBar* menuBar = new QMenuBar();
    QMenu* menu = new QMenu("Menu");

    QAction* openAction = menu->addAction("Open");

    QObject::connect(openAction, &QAction::triggered, this, &AvarOCR::OpenFileDialog);

    //menu->addAction("Select files", this, SLOT(this->OpenFileDialog()));
    //menu->addAction("Exit", this, SLOT(close()));
    menuBar->addMenu(menu);
    menuBar->show();

    this->setMenuWidget(menuBar);
}

void AvarOCR::initOcrArea()
{
    this->windowSplitter->addWidget(new QWidget());


    //STARTUPINFO si;
    //PROCESS_INFORMATION pi;
    //ZeroMemory(&si, sizeof(si));
    //si.cb = sizeof(si);
    //ZeroMemory(&pi, sizeof(pi));
    //QList<QString> processCommandArgsList;

    //processCommandArgsList.append(QString(this->execNotepadPath));
    //processCommandArgsList.append(QString(L"-multiInst"));
    //QString processCommand = processCommandArgsList.join(L" ");
    //std::wstring wstr_command = reinterpret_cast<const wchar_t*>(processCommand.utf16());
    //// Create the child process
    //if (!CreateProcess(NULL, (LPWSTR)wstr_command.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
    //    std::cerr << "CreateProcess failed. Error: " << GetLastError() << std::endl;
    //    return;
    //}
    //this->handleNotepad = pi.hProcess;
    //this->pidNotepad = pi.dwProcessId;
    //HWND hWnd = GetWindowHandleByPID(pi.dwProcessId);
    //QWindow* foreignWindow = QWindow::fromWinId(WId(hWnd));

    //// 5. Create the QWidget wrapper
    //QWidget* containerWidget = QWidget::createWindowContainer(foreignWindow, this);
    //QWidget* removedWidget = this->windowSplitter->replaceWidget(1, containerWidget);
    //delete removedWidget;
}

void AvarOCR::handleViewFormatSwitch(bool checked)
{
    if (checked)
    {
        this->switchLabel->setText("Text");
        if (this->curentOCRImageInfo != NULL)
        {
            QList<QString> args;
            args.append(QString(L"-multiInst"));
            const wchar_t* envBlock = L"MY_VAR=HelloWorld\0"
                L"OTHER_VAR=123\0";
            this->viewProcess(this->execNotepadPath, args, this->curentOCRImageInfo->textFilename, CREATE_UNICODE_ENVIRONMENT, envBlock);
        }
    }
    else
    {
        this->switchLabel->setText("Image");
        if (this->curentOCRImageInfo != NULL)
        {
            QList<QString> args;
            this->viewProcess(this->execPaintPath, args, this->curentOCRImageInfo->filename,NULL,NULL);
        }
    }
}
void AvarOCR::ocrThread()
{
    for (OCRImageInfo& ocrImageInfo : this->OCRImageInfoList) {

        // 1. Initialize the Tesseract API instance
        tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
        api->Init("data", "avar");
        // 2. Initialize OCR engine with English ("eng") and LSTM engine mode
        // Note: Change the path to where your "tessdata" folder is stored.
        if (api->Init(".\\tessdata", "avar", tesseract::OEM_LSTM_ONLY)) {
            std::cerr << "Could not initialize tesseract." << std::endl;
            continue;
        }
        QByteArray filenameArray = ocrImageInfo.filename.toUtf8();
        QString infoText = QString("OCR: \"%1\"").arg(ocrImageInfo.filename);
        this->infoLabel->setText(infoText);
        const char* c_str = filenameArray.constData();
        // 3. Open the image file using Leptonica library
        Pix* image = pixRead(c_str);
        if (!image) {
            std::cerr << "Could not open input image." << std::endl;
            api->End();
            continue;
        }

        // 4. Set the image into the engine and run OCR
        api->SetImage(image);
        char* outText = api->GetUTF8Text();
        ocrImageInfo.ocrText = QString(outText);
        std::string std_filename = ocrImageInfo.filename.toUtf8().constData();
        std::filesystem::path fsysImagePath(std_filename.c_str());
        //fsysImagePath.filename
        std::filesystem::path fsysTmpTextPath = this->workFolder / fsysImagePath.stem();
        fsysTmpTextPath += ".txt";
        std::ofstream outFile(fsysTmpTextPath);
        outFile << ocrImageInfo.ocrText.toUtf8().constData();
        outFile.close();
        ocrImageInfo.textFilename = QString(fsysTmpTextPath.c_str());
        // 5. Print the recognized text
        std::cout << "OCR Result:\n" << outText << std::endl;

        // 6. Free memory and clean up pointers
        api->End();
        delete api;
        delete[] outText;
        pixDestroy(&image);

    }
}
void AvarOCR::handleStartOCR()
{

    std::thread threadStartOCR([this]() {
        ocrThread();
        });
    threadStartOCR.detach();


    //for (OCRImageInfo& ocrImageInfo : this->OCRImageInfoList) {

    //     // 1. Initialize the Tesseract API instance
    //     tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    //     api->Init("data", "avar");
    //     // 2. Initialize OCR engine with English ("eng") and LSTM engine mode
    //     // Note: Change the path to where your "tessdata" folder is stored.
    //     if (api->Init(".\\tessdata", "avar", tesseract::OEM_LSTM_ONLY)) {
    //         std::cerr << "Could not initialize tesseract." << std::endl;
    //         continue;
    //     }
    //     QByteArray filenameArray = ocrImageInfo.filename.toUtf8();
    //     QString infoText = QString("OCR: \"%1\"").arg(ocrImageInfo.filename);
    //     this->infoLabel->setText(infoText);
    //     const char* c_str = filenameArray.constData();
    //     // 3. Open the image file using Leptonica library
    //     Pix* image = pixRead(c_str);
    //     if (!image) {
    //         std::cerr << "Could not open input image." << std::endl;
    //         api->End();
    //         continue;
    //     }

    //     // 4. Set the image into the engine and run OCR
    //     api->SetImage(image);
    //     char* outText = api->GetUTF8Text();
    //     ocrImageInfo.ocrText = QString(outText);
    //     // 5. Print the recognized text
    //     std::cout << "OCR Result:\n" << outText << std::endl;

    //     // 6. Free memory and clean up pointers
    //     api->End();
    //     delete api;
    //     delete[] outText;
    //     pixDestroy(&image);
    //  
    //}
 
}
void AvarOCR::initImagesListWidget()
{
    // 1. Initialize your list widget
    this->imagesListWidget = new QListWidget();

    // 2. Configure for an image-grid or visual list layout
    this->imagesListWidget->setViewMode(QListWidget::ListMode);
    this->imagesListWidget->setIconSize(QSize(500,500)); // Set thumb size
    this->imagesListWidget->setResizeMode(QListWidget::Adjust);
    this->imagesListWidget->setMovement(QListView::Static); // Prevent dragging if undesired
    this->imagesListWidget->setSpacing(10);
    this->imagesListWidget->setFixedWidth(500);
    connect(this->imagesListWidget, &QListWidget::itemDoubleClicked,
        this, &AvarOCR::handleImagesListWidgetDoubleClick);

    this->imagesListWidget->setStyleSheet(
        "QListWidget::item {"
        "   border-bottom: 6px solid #e0e0e0;" // линия под каждым элементом
        "   padding: 16px;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #аfffff;"        // цвет выбранного пункта
        "   color: blue;"
        "}"
    );


    this->windowSplitter->addWidget(this->imagesListWidget);

    //// 3. Populate the list with images
    //QStringList imagePaths = { "C:\\Users\\User\\Desktop\\images\\image1.png", "C:\\Users\\User\\Desktop\\images\\image2.png", "C:\\Users\\User\\Desktop\\images\\image3.png" };

    //for (const QString& path : imagePaths) {

    //    OCRImageInfo ocrImageInfo;
    //    ocrImageInfo.item = new QListWidgetItem(this->imagesListWidget);
    //    ocrImageInfo.item->setBackground(Qt::green); // или QColor("#00ff00")
    //    ocrImageInfo.item->setTextAlignment(Qt::AlignCenter);
    //    ocrImageInfo.container = new QWidget();
    //    ocrImageInfo.layout = new QVBoxLayout(ocrImageInfo.container);

    //    ocrImageInfo.filename = QString(path);
    //    ocrImageInfo.textFilename = QString(".\\Notepad++\\1.txt");

    //    QLabel* imageLabel = new QLabel("Left Text");

    //    QPixmap pixmap = QPixmap(path);
    //    imageLabel->setPixmap(pixmap.scaled(400, 400,Qt::KeepAspectRatio));
    //    QLabel* filenameLabel = new QLabel(path);

    //    imageLabel->setAlignment(Qt::AlignCenter);
    //    filenameLabel->setAlignment(Qt::AlignCenter);

    //    ocrImageInfo.layout->addWidget(imageLabel);
    //    ocrImageInfo.layout->addWidget(filenameLabel);
    //    ocrImageInfo.layout->setContentsMargins(2,2,2,2);
    //    ocrImageInfo.item->setSizeHint(ocrImageInfo.container->sizeHint());

    //    ocrImageInfo.container->setLayout(ocrImageInfo.layout);
    //    this->imagesListWidget->setItemWidget(ocrImageInfo.item, ocrImageInfo.container);

    //    this->OCRImageInfoList.append(ocrImageInfo);

    //}

}

void AvarOCR::handleImagesListWidgetDoubleClick(QListWidgetItem* item)
{
    for (OCRImageInfo& ocrImageInfo : this->OCRImageInfoList) {
        if (ocrImageInfo.item == item)
        {
            this->curentOCRImageInfo = &ocrImageInfo;
            if (this->viewFormatSwitch->isChecked())
            {
                QList<QString> args;
                args.append(QString(L"-multiInst"));
                const wchar_t* envBlock = L"MY_VAR=HelloWorld\0"
                    L"OTHER_VAR=123\0";

                this->viewProcess(this->execNotepadPath, args, ocrImageInfo.textFilename, CREATE_UNICODE_ENVIRONMENT, envBlock);
            }
            else
            {
                QList<QString> args;
                this->viewProcess(this->execPaintPath, args, ocrImageInfo.filename,NULL,NULL);
                //this->viewAsImage(this->ocrArea, ocrImageInfo.filename);
            }

            QString infoText = QString("Show: \"%1\"").arg(ocrImageInfo.filename);
            this->infoLabel->setText(infoText);
        }
    }
}
void AvarOCR::viewProcess(std::wstring processName, QList<QString> args, QString filename, DWORD  dwCreationFlags, const wchar_t* envBlock)
{
    this->killProcess(this->lastPID);
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    QList<QString> processCommandArgsList;

    processCommandArgsList.append(QString(processName));
    
    for (const auto& arg : args) {
        processCommandArgsList.append(arg);
    }
    processCommandArgsList.append(filename);
    QString processCommand = processCommandArgsList.join(L" ");
    std::wstring wstr_command = reinterpret_cast<const wchar_t*>(processCommand.utf16());
    // Create the child process
    if (!CreateProcess(NULL, (LPWSTR)wstr_command.c_str(), NULL, NULL, FALSE, dwCreationFlags, (LPVOID)envBlock, NULL, &si, &pi)) {
        std::cerr << "CreateProcess failed. Error: " << GetLastError() << std::endl;
        return;
    }
    this->lastPID = pi.dwProcessId;
    HWND hWnd = GetWindowHandleByPID(pi.dwProcessId);
    QWindow* foreignWindow = QWindow::fromWinId(WId(hWnd));

    // 5. Create the QWidget wrapper
    QWidget* containerWidget = QWidget::createWindowContainer(foreignWindow, this);
    QWidget* removedWidget = this->windowSplitter->replaceWidget(1, containerWidget);
    delete removedWidget;

}
//void AvarOCR::viewAsText(QScrollArea * scrollArea, QString text)
//{
//    STARTUPINFO si;
//    PROCESS_INFORMATION pi;
//    ZeroMemory(&si, sizeof(si));
//    si.cb = sizeof(si);
//    ZeroMemory(&pi, sizeof(pi));
//    QList<QString> processCommandArgsList;
//
//    processCommandArgsList.append(QString(this->execNotepadPath));
//    processCommandArgsList.append(QString(L"-multiInst"));
//    processCommandArgsList.append(text);
//    QString processCommand = processCommandArgsList.join(L" ");
//    std::wstring wstr_command = reinterpret_cast<const wchar_t*>(processCommand.utf16());
//    // Create the child process
//    if (!CreateProcess(NULL, (LPWSTR)wstr_command.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
//        std::cerr << "CreateProcess failed. Error: " << GetLastError() << std::endl;
//        return;
//    }
//    this->lastPID = pi.dwProcessId;
//    HWND hWnd = GetWindowHandleByPID(pi.dwProcessId);
//    QWindow* foreignWindow = QWindow::fromWinId(WId(hWnd));
//
//    // 5. Create the QWidget wrapper
//    QWidget* containerWidget = QWidget::createWindowContainer(foreignWindow, this);
//    QWidget* removedWidget = this->windowSplitter->replaceWidget(1, containerWidget);
//    delete removedWidget;
//}
//
//void AvarOCR::viewAsImage(QScrollArea * scrollArea, QString imagePath)
//{
//    QLabel* imageLabel = new QLabel;
//    QPixmap pixmap(imagePath);
//    imageLabel->setPixmap(pixmap);
//
//    imageLabel->resize(pixmap.size());
//    QWidget* removedWidget = this->windowSplitter->replaceWidget(1, imageLabel);
//    delete removedWidget;
//}

void AvarOCR::OpenFileDialog()
{
    //QString fileName = QFileDialog::getOpenFileName(
    //    this,
    //    tr("Open Document"),                 // Dialog title
    //    "C:/",                               // Default starting directory
    //    tr("Text Files (*.txt);;All Files (*)") // File filters
    //);

    QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        tr("Select One or More Images"),
        "/home",
        tr("Images (*.png *.xpm *.jpg);;Text files (*.txt);;All Files (*)")
    );


    // 3. Populate the list with images
   // QStringList imagePaths = { "C:\\Users\\User\\Desktop\\images\\image1.png", "C:\\Users\\User\\Desktop\\images\\image2.png", "C:\\Users\\User\\Desktop\\images\\image3.png" };

    for (const QString& path : fileNames) {

        OCRImageInfo ocrImageInfo;
        ocrImageInfo.item = new QListWidgetItem(this->imagesListWidget);
        ocrImageInfo.item->setBackground(Qt::green); // или QColor("#00ff00")
        ocrImageInfo.item->setTextAlignment(Qt::AlignCenter);
        ocrImageInfo.container = new QWidget();
        ocrImageInfo.layout = new QVBoxLayout(ocrImageInfo.container);

        ocrImageInfo.filename = QString(path).replace("/","\\");

        QLabel* imageLabel = new QLabel("Left Text");

        QPixmap pixmap = QPixmap(path);
        imageLabel->setPixmap(pixmap.scaled(400, 400, Qt::KeepAspectRatio));
        QLabel* filenameLabel = new QLabel(path);

        imageLabel->setAlignment(Qt::AlignCenter);
        filenameLabel->setAlignment(Qt::AlignCenter);

        ocrImageInfo.layout->addWidget(imageLabel);
        ocrImageInfo.layout->addWidget(filenameLabel);
        ocrImageInfo.layout->setContentsMargins(2, 2, 2, 2);
        ocrImageInfo.item->setSizeHint(ocrImageInfo.container->sizeHint());

        ocrImageInfo.container->setLayout(ocrImageInfo.layout);
        this->imagesListWidget->setItemWidget(ocrImageInfo.item, ocrImageInfo.container);

        this->OCRImageInfoList.append(ocrImageInfo);

    }

}

bool AvarOCR::killProcess(DWORD pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL) {
       
        std::cerr << "Failed to open process. Error: " << GetLastError() << std::endl;
        return false;
    }
    BOOL result = TerminateProcess(hProcess, 1);
    CloseHandle(hProcess);
    return result != 0;
}

AvarOCR::~AvarOCR()
{
    this->killProcess(this->lastPID);
}

