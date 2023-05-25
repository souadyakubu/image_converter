/*
 * ImageConverter.h declares a class for
 *  various image conversion operations.
 */

#ifndef IMAGECONVERTER_H_
#define IMAGECONVERTER_H_

#include <omp.h>  // OpenMP functions
#include <tsgl.h> // Canvas, ColorInt, etc.

#include <iostream> // cout, cerr, ...
#include <string>   // string
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h> // sleep()
#endif

using namespace tsgl;
using namespace std;

class ImageConverter
{
public:
    ImageConverter(const std::string &imageFileName, int width, int height);
    ~ImageConverter();
    void run();
    void invertImage();
    void invertImage2();
    void invertImage3();
    void flipVertical();
    void flipHorizontal();

private:
    Canvas myCanvas0;
    int myWidth;
    int myHeight;
    string myImageFileName;

#ifdef _WIN32 // for Windows only; ignored on Linux
    void sleep(unsigned seconds)
    {
        Sleep(seconds * 1000);
    }
#endif
};

#endif /* IMAGECONVERTER_H_ */
