

#ifndef LINEARWAVEESTIMATE_H
#define LINEARWAVEESTIMATE_H


#include "fftanalysis.h"
#include "imgproc.h"
#include <string> 

namespace img
{

class LinearWaveEstimate 
{
  public:

    LinearWaveEstimate(const ImgProc& init, const double dispersionFactor);
    ~LinearWaveEstimate(){}

    //! Compute the amplitudes A and B at each wave vector, for each channel 
    void ingest(const ImgProc& I);

    //! Retrieve A and B
    const FFTImgProc& getA() const {return A;}
    const FFTImgProc& getB() const {return B;}

    //! produces fourier space amplitude given coordinates in fourier space and the frame number
    void value (int i, int j, int n, std::vector< std::complex<double> >& amplitude) const;

  protected:

    //! Amplitudes A and B
    FFTImgProc A;
    FFTImgProc B;

  private:

    double alpha;
    int frameCount;

    //! Calculates kt 
    double dispersion(double kx, double ky) const;

};

//! Produces an ImgProc image estimate at a certain frame based on the values of A and B
void extract_image(const LinearWaveEstimate& l, int frame, ImgProc& img);

}

#endif
