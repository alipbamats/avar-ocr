#include "AvarOCR.h"


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

    this->infoLabel = new QLabel("Start", this);
    this->infoLabel->setFixedHeight(25);
    this->mainSplitter->addWidget(this->infoLabel);
    this->setCentralWidget(this->mainSplitter);
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
    this->ocrArea = new QScrollArea();
    this->windowSplitter->addWidget(this->ocrArea);
}

void AvarOCR::handleViewFormatSwitch(bool checked)
{
    if (checked)
    {
        this->switchLabel->setText("Text");
        if(this->curentOCRImageInfo!=NULL)
            this->viewAsText(this->ocrArea, this->curentOCRImageInfo->ocrText);
    }
    else
    {
        this->switchLabel->setText("Image");
        if (this->curentOCRImageInfo != NULL)
            this->viewAsImage(this->ocrArea, this->curentOCRImageInfo->filename);
    }
}

void AvarOCR::handleStartOCR()
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
         // 5. Print the recognized text
         std::cout << "OCR Result:\n" << outText << std::endl;

         // 6. Free memory and clean up pointers
         api->End();
         delete api;
         delete[] outText;
         pixDestroy(&image);
      
    }
 
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
                this->viewAsText(this->ocrArea, ocrImageInfo.ocrText);
            }
            else
            {
                this->viewAsImage(this->ocrArea, ocrImageInfo.filename);
            }

            QString infoText = QString("Show: \"%1\"").arg(ocrImageInfo.filename);
            this->infoLabel->setText(infoText);
        }
    }
}

void AvarOCR::viewAsText(QScrollArea * scrollArea, QString text)
{

    QSize visibleSize = scrollArea->viewport()->size();
    QTextEdit* textEditor = new QTextEdit();
    textEditor->resize(visibleSize);
    textEditor->setText(text);
    scrollArea->setWidget(textEditor);
}

void AvarOCR::viewAsImage(QScrollArea * scrollArea, QString imagePath)
{
    QLabel* imageLabel = new QLabel;
    QPixmap pixmap(imagePath);
    imageLabel->setPixmap(pixmap);

    imageLabel->resize(pixmap.size());
    scrollArea->setWidget(imageLabel);
}

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

        ocrImageInfo.filename = QString(path);

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

AvarOCR::~AvarOCR()
{}

