#include "AvarOCR.h"


AvarOCR::AvarOCR(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.setupUi(this);

    this->mainSplitter = new QSplitter(Qt::Vertical);

    QWidget* containerArea = new QWidget(this->mainSplitter);


    // Create a layout for the container
    QHBoxLayout* layout = new QHBoxLayout(containerArea);
    layout->setContentsMargins(2, 2, 2, 2); // Optional spacing
    layout->setSpacing(2);
    layout->setAlignment(Qt::AlignTop);
    QPushButton* runOCRButton = new QPushButton("Run OCR", this);
    runOCRButton->setFixedHeight(35);
    runOCRButton->setFixedWidth(55);
    layout->addWidget(runOCRButton);

    QCheckBox* switchBox = new QCheckBox("Enable Feature");
    switchBox->setStyleSheet(
        "QCheckBox::indicator:checked { image: url(:/res/switchOn); }"
        "QCheckBox::indicator:unchecked { image: url(:/res/switchOff); }"
        "QCheckBox::indicator { width: 40px; height: 20px; }"
    );

    layout->addWidget(switchBox);

 
    containerArea->setLayout(layout);

    this->mainSplitter->addWidget(containerArea);
    
    this->windowSplitter = new QSplitter(Qt::Horizontal);
    this->windowSplitter->setWindowTitle("Split Window Example");
    this->windowSplitter->resize(600, 400);

    this->mainSplitter->addWidget(this->windowSplitter);


    this->initImagesListWidget();
    this->initOcrArea();

    this->initMenuBar();
    this->initStartOCRButton();
    this->setCentralWidget(this->mainSplitter);
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

void AvarOCR::initStartOCRButton()
{
    //QPushButton* button = new QPushButton("Click Me",this);
    //button->setGeometry(50, 50, 100, 30);

    //// Connect the click signal to the handler function
    //QObject::connect(button, &QPushButton::clicked, AvarOCR::handleStartOCR);
}

void AvarOCR::handleStartOCR()
{
    int aa = 0;
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
        "   background-color: #ffffff;"        // цвет выбранного пункта
        "   color: blue;"
        "}"
    );


    this->windowSplitter->addWidget(this->imagesListWidget);

    // 3. Populate the list with images
    QStringList imagePaths = { "C:\\Users\\User\\Desktop\\images\\image1.png", "C:\\Users\\User\\Desktop\\images\\image2.png", "C:\\Users\\User\\Desktop\\images\\image3.png" };

    for (const QString& path : imagePaths) {

        OCRImageInfo ocrImageInfo;
        ocrImageInfo.item = new QListWidgetItem(this->imagesListWidget);
        ocrImageInfo.item->setTextAlignment(Qt::AlignCenter);
        ocrImageInfo.container = new QWidget();
        ocrImageInfo.layout = new QVBoxLayout(ocrImageInfo.container);

        ocrImageInfo.filename = QString(path);

        QLabel* imageLabel = new QLabel("Left Text");

        QPixmap pixmap = QPixmap(path);
        imageLabel->setPixmap(pixmap.scaled(400, 400,Qt::KeepAspectRatio));
        QLabel* filenameLabel = new QLabel(path);

        imageLabel->setAlignment(Qt::AlignCenter);
        filenameLabel->setAlignment(Qt::AlignCenter);

        ocrImageInfo.layout->addWidget(imageLabel);
        ocrImageInfo.layout->addWidget(filenameLabel);
        ocrImageInfo.layout->setContentsMargins(2,2,2,2);
        ocrImageInfo.item->setSizeHint(ocrImageInfo.container->sizeHint());

        ocrImageInfo.container->setLayout(ocrImageInfo.layout);
        this->imagesListWidget->setItemWidget(ocrImageInfo.item, ocrImageInfo.container);

        this->OCRImageInfoList.append(ocrImageInfo);

        //this->OCRImageInfoList.append(ocrImageInfo);

       // QIcon icon(path);
       // QListWidgetItem* item = new QListWidgetItem("sdfdsf");
       // item->setIcon(icon);
       //// item->setText("fsdf");
       // item->setForeground(QBrush(Qt::blue));
       // item->setTextAlignment(Qt::AlignHCenter);
       // this->imagesListWidget->addItem(item);
    }

}

void AvarOCR::handleImagesListWidgetDoubleClick(QListWidgetItem* item)
{
    for (const OCRImageInfo& ocrImageInfo : this->OCRImageInfoList) {
        if (ocrImageInfo.item == item)
        {
            QLabel* imageLabel = new QLabel;
            QPixmap pixmap(ocrImageInfo.filename);
            imageLabel->setPixmap(pixmap);

            imageLabel->resize(pixmap.size());
            this->ocrArea->setWidget(imageLabel);
        }
    }
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


}

AvarOCR::~AvarOCR()
{}

