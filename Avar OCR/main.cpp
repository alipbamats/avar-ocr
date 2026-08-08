#include "AvarOCR.h"
#include <QtWidgets/QApplication>

#include <tesseract/baseapi.h>

#include <iostream>
#include <leptonica/allheaders.h>

int main(int argc, char *argv[])
{
    //// 1. Initialize the Tesseract API instance
    //tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    //api->Init("data", "eng");
    //// 2. Initialize OCR engine with English ("eng") and LSTM engine mode
    //// Note: Change the path to where your "tessdata" folder is stored.
    //if (api->Init("C:\\Program Files\\Tesseract-OCR\\tessdata", "eng", tesseract::OEM_LSTM_ONLY)) {
    //    std::cerr << "Could not initialize tesseract." << std::endl;
    //    return 1;
    //}

    //// 3. Open the image file using Leptonica library
    //Pix* image = pixRead("C:\\Users\\User\\Desktop\\image.png");
    //if (!image) {
    //    std::cerr << "Could not open input image." << std::endl;
    //    api->End();
    //    return 1;
    //}

    //// 4. Set the image into the engine and run OCR
    //api->SetImage(image);
    //char* outText = api->GetUTF8Text();

    //// 5. Print the recognized text
    //std::cout << "OCR Result:\n" << outText << std::endl;

    //// 6. Free memory and clean up pointers
    //api->End();
    //delete api;
    //delete[] outText;
    //pixDestroy(&image);

    QApplication app(argc, argv);
    AvarOCR window;
    window.show();
    return app.exec();
}
