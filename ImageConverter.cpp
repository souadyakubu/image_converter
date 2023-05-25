/*
 * ImageConverter.cpp declares a class for
 *  various image conversion operations.
 */

#include "ImageConverter.h"

#include <unistd.h> // sleep()

#include <cmath>

//-----------------  Method definitions ------------------------------

/* explicit constructor
 *
 * @param imageFileName, a string.
 * @param width, an int.
 * @param height, an int.
 *
 * Precondition: imageFileName contains the name of a valid image file
 *               && width > 0 && height > 0
 * Postcondition: myImageFileName == imageFileName
 *                && myWidth == width && myHeight = height
 *                && a width x height Canvas containing the image
 *                    from imageFileName has been displayed on screen
 *                && a blank width x height Canvas as also been displayed.
 */
ImageConverter::ImageConverter(const std::string &imageFileName, int width,
                               int height)
    : myCanvas0(0, 0, width, height, imageFileName),
      myWidth(width),
      myHeight(height),
      myImageFileName(imageFileName)
{
    myCanvas0.start();
    myCanvas0.getBackground()->drawImage(0, 0, 0, imageFileName, width, height,
                                         0, 0, 0);
}

/* destructor
 *
 * Precondition: this ImageConverter object is going out of scope.
 * Postcondition: a "normal termnation" message has been displayed.
 */
ImageConverter::~ImageConverter()
{
    cout << "\nImageConverter terminated normally." << endl;
}

/* method to run image-conversion methods
 * PostCondition: the converted version of the image from myCanvas0
 *                 has been displayed in a new Canvas
 *               && myCanvas0 has been closed.
 */
void ImageConverter::run()
{
    // invertImage();
    // invertImage2();
    // invertImage3();
    // flipVertical();
    flipHorizontal();
    myCanvas0.wait();
}

/* invertImage() inverts the image from myImageFileName.
 *
 * Precondition: myCanvas0 contains a TSGL Image that is to be inverted
 *            && myWidth == the number of columns in which to display the image
 *            && myHeight == the number of rows in which to display the image.
 *
 * Postcondition: a myWidth x myHeight Canvas has been displayed
 *                 containing the inverse of the image from myCanvas0
 *                 (which contains the image from myImageFileName).
 */

void ImageConverter::invertImage()
{
    // record starting time
    double startTime = omp_get_wtime();

    Canvas canvas1(myWidth + 50, 0, myWidth, myHeight,
                   myImageFileName + " Inverted");
    canvas1.start();
    Background *background0 = myCanvas0.getBackground();
    Background *background1 = canvas1.getBackground();

    const int WIN_WIDTH = myCanvas0.getWindowWidth(),
              WIN_HEIGHT = myCanvas0.getWindowHeight(),
              START_X = -WIN_WIDTH / 2, // left column
        START_Y = -WIN_HEIGHT / 2,      // bottom row
        STOP_X = +WIN_WIDTH / 2,        // right column + 1
        STOP_Y = +WIN_HEIGHT / 2;       // top row + 1

// loop through the image rows
#pragma omp parallel for
    for (int y = START_Y; y < STOP_Y; ++y)
    { // bottom to top
        // loop through the image columns
        for (int x = START_X; x < STOP_X; ++x)
        { // left to right
            // read the pixel at canvas0[row][col]
            ColorInt pixelColor = background0->getPixel(x, y);
            // compute its inverse
            int invertedR = 255 - pixelColor.R;
            int invertedG = 255 - pixelColor.G;
            int invertedB = 255 - pixelColor.B;
            ColorInt invertedColor(invertedR, invertedG, invertedB);
            // draw the inverse at the same spot in canvas1
            background1->drawPixel(x, y, invertedColor);
        } // inner for
        // slow the processing to simulate a very large image
        canvas1.sleep();
    } // outer for

    // compute and display the time difference
    double totalTime = omp_get_wtime() - startTime;
    cout << "\n\nImage inversion took " << totalTime << " seconds.\n"
         << endl;

    // save converted image
    // canvas1.takeScreenShot();

    canvas1.wait();
} // invertImage

void ImageConverter::invertImage2()
{
    // record starting time
    double startTime = omp_get_wtime();

    Canvas canvas2(myWidth + 50, 0, myWidth, myHeight,
                   myImageFileName + " Inverted, Chunk-Size 1");
    canvas2.start();
    Background *background0 = myCanvas0.getBackground();
    Background *background2 = canvas2.getBackground();

    const int WIN_WIDTH = myCanvas0.getWindowWidth(),
              WIN_HEIGHT = myCanvas0.getWindowHeight(),
              START_X = -WIN_WIDTH / 2, START_Y = -WIN_HEIGHT / 2,
              STOP_X = +WIN_WIDTH / 2, STOP_Y = +WIN_HEIGHT / 2;

// launch additional threads
#pragma omp parallel
    {
        // how many workers do we have?
        unsigned numThreads = omp_get_num_threads();
        // which one am I?
        unsigned id = omp_get_thread_num();

        // loop through the image rows
        for (int y = START_Y + id; y < STOP_Y; y += numThreads)
        {
            // loop through the image columns
            for (int x = START_X; x < STOP_X; ++x)
            {
                // read the pixel at canvas0[row][col]
                ColorInt pixelColor = background0->getPixel(x, y);
                // compute its inverse
                int invertedR = 255 - pixelColor.R;
                int invertedG = 255 - pixelColor.G;
                int invertedB = 255 - pixelColor.B;
                ColorInt invertedColor(invertedR, invertedG, invertedB);
                // draw the inverse at the same spot in canvas2
                background2->drawPixel(x, y, invertedColor);
            } // inner for
            // slow the processing to simulate a very large image
            canvas2.sleep();
        } // outer for
    }     // #pragma omp parallel

    // compute and display the time difference
    double totalTime = omp_get_wtime() - startTime;
    cout << "\n\nImage inversion took " << totalTime << " seconds.\n"
         << endl;

    canvas2.wait();
} // invertImage2

void ImageConverter::invertImage3()
{
    // record starting time
    double startTime = omp_get_wtime();

    Canvas canvas3(myWidth + 50, 0, myWidth, myHeight,
                   myImageFileName + " Inverted, Equal-Sized Chunks ");
    canvas3.start();
    Background *background0 = myCanvas0.getBackground();
    Background *background3 = canvas3.getBackground();

    const int WIN_WIDTH = myCanvas0.getWindowWidth(),
              WIN_HEIGHT = myCanvas0.getWindowHeight(),
              START_X = -WIN_WIDTH / 2, START_Y = -WIN_HEIGHT / 2,
              STOP_X = +WIN_WIDTH / 2, STOP_Y = +WIN_HEIGHT / 2;

// launch additional threads
#pragma omp parallel
    {
        // how many workers do we have?
        unsigned numThreads = omp_get_num_threads();
        // which one am I?
        unsigned id = omp_get_thread_num();

        // compute this thread's chunk size (iterations % numThreads may != 0)
        //  giving any 'leftover' iterations to the final thread
        double iterations = STOP_Y - START_Y;
        unsigned chunkSize = (unsigned)ceil(iterations / numThreads);

        // compute this thread's starting and stopping index values
        int start = id * chunkSize + START_Y;
        int stop = 0;
        if (id < numThreads - 1)
        {
            stop = start + chunkSize;
        }
        else
        {
            stop = STOP_Y;
        }

        // loop through image rows in equal-sized chunks
        for (int y = start; y < stop; ++y)
        {
            // loop through the image columns
            for (int x = START_X; x < STOP_X; ++x)
            {
                // read the pixel at canvas0[row][col]
                ColorInt pixelColor = background0->getPixel(x, y);
                // compute its inverse
                int invertedR = 255 - pixelColor.R;
                int invertedG = 255 - pixelColor.G;
                int invertedB = 255 - pixelColor.B;
                ColorInt invertedColor(invertedR, invertedG, invertedB);
                // draw the inverse at the same spot in canvas3
                background3->drawPixel(x, y, invertedColor);
            } // inner for
            // slow the processing to simulate a very large image
            canvas3.sleep();
        } // outer for
    }     // #pragma omp parallel

    // compute and display the time difference
    double totalTime = omp_get_wtime() - startTime;
    cout << "\n\nImage inversion took " << totalTime << " seconds.\n"
         << endl;

    canvas3.wait();
} // invertImage3

void ImageConverter::flipVertical()
{
    // record starting time
    double startTime = omp_get_wtime();

    Canvas canvas1(myWidth + 50, 0, myWidth, myHeight,
                   myImageFileName + " Flipped Vertically");
    canvas1.start();
    Background *background0 = myCanvas0.getBackground();
    Background *background1 = canvas1.getBackground();

    const int WIN_WIDTH = myCanvas0.getWindowWidth(),
              WIN_HEIGHT = myCanvas0.getWindowHeight(),
              START_X = -WIN_WIDTH / 2, // left column
        START_Y = -WIN_HEIGHT / 2,      // bottom row
        STOP_X = +WIN_WIDTH / 2,        // right column + 1
        STOP_Y = +WIN_HEIGHT / 2;       // top row + 1

    // loop through the image rows
#pragma omp parallel for
    for (int y = START_Y; y < STOP_Y; ++y)
    {
        // loop through the image columns
        for (int x = START_X; x < STOP_X; ++x)
        {
            // read the pixel at canvas0[row][col]
            ColorInt pixelColor = background0->getPixel(x, y);
            // draw the pixel at the same spot in canvas1, but vertically flipped
            background1->drawPixel(x, STOP_Y - 1 - (y - START_Y), pixelColor);
        } // inner for
        // slow the processing to simulate a very large image
        canvas1.sleep();
    } // outer for

    // compute and display the time difference
    double totalTime = omp_get_wtime() - startTime;
    cout << "\n\nVertical image flipping took " << totalTime << " seconds.\n"
         << endl;
    canvas1.takeScreenShot();
    canvas1.wait();
} // flipVertical

void ImageConverter::flipHorizontal()
{
    // record starting time
    double startTime = omp_get_wtime();

    Canvas canvas1(0, myHeight + 50, myWidth, myHeight,
                   myImageFileName + " Flipped Horizontally");
    canvas1.start();
    Background *background0 = myCanvas0.getBackground();
    Background *background1 = canvas1.getBackground();

    const int WIN_WIDTH = myCanvas0.getWindowWidth(),
              WIN_HEIGHT = myCanvas0.getWindowHeight(),
              START_X = -WIN_WIDTH / 2, // left column
        START_Y = -WIN_HEIGHT / 2,      // bottom row
        STOP_X = +WIN_WIDTH / 2,        // right column + 1
        STOP_Y = +WIN_HEIGHT / 2;       // top row + 1

    // loop through the image rows
#pragma omp parallel for
    for (int y = START_Y; y < STOP_Y; ++y)
    {
        // loop through the image columns
        for (int x = START_X; x < STOP_X; ++x)
        {
            // read the pixel at canvas0[row][col]
            ColorInt pixelColor = background0->getPixel(x, y);
            // draw the pixel at the same spot in canvas1, but horizontally flipped
            background1->drawPixel(STOP_X - 1 - (x - START_X), y, pixelColor);
        } // inner for
        // slow the processing to simulate a very large image
        canvas1.sleep();
    } // outer for

    // compute and display the time difference
    double totalTime = omp_get_wtime() - startTime;
    cout << "\n\nHorizontal image flipping took " << totalTime << " seconds.\n"
         << endl;
    canvas1.takeScreenShot();
    canvas1.wait();
}
