/*
 * main.cpp is a driver for an ImageConverter program
 *  using OpenMP to parallelize the image processing
 *  and TSGL to view the processing in near real time.
 */

#include "ImageConverter.h"

int main()
{
    omp_set_num_threads(1);
    ImageConverter iConverter("./pics/beads.jpg", 800, 800);
    iConverter.run();
}