
#ifndef FFTANALYSIS_H
#define FFTANALYSIS_H

#include <string>
#include "fftimgproc.h"
#include "imgproc.h"

namespace img
{

void load_fft( const ImgProc& input, FFTImgProc& fftoutput );

}
#endif
