

#include <complex>
#include <cmath>
#include "LinearWaveEstimate.h"
#include "imgproc.h"
#include "fftanalysis.h"

#include <iostream>



#include <OpenImageIO/imageio.h>
OIIO_NAMESPACE_USING

using namespace img;


LinearWaveEstimate::LinearWaveEstimate(const ImgProc& init, const double dispersionFactor) : alpha(dispersionFactor), frameCount(0)
{
   //Use the passed in ImgProc init to set A and B to the correct sizes
   A.clear(init.nx(), init.ny(), init.depth());
   B.clear(init.nx(), init.ny(), init.depth());
}


double LinearWaveEstimate::dispersion(double kx, double ky) const 
{
   //Calculate kt using the formula provided and return it as freq
   double kmag = std::sqrt(kx * kx + ky * ky);
   double freq = alpha * std::sqrt(kmag);
   return freq;
}


void LinearWaveEstimate::ingest(const ImgProc& I)
{
   FFTImgProc Itilde;
   img::load_fft(I, Itilde); //Itilde is the fft version of I 
   Itilde.fft_forward();

   for (int j = 0; j < Itilde.ny(); j++) //Loop over all fourier space grid points 
   {
//#pragma omp parallel for
      for (int i = 0; i < Itilde.nx(); i++)
      {
         std::vector< std::complex<double> > itilde; 
         std::vector< std::complex<double> > a;
         std::vector< std::complex<double> > b;

         Itilde.value(i, j, itilde); //Put the current multichannel value of Itilde at (i,j) into itilde 
         //Put the current multichannel values of A and B at (i,j) into a and b respectively
         A.value(i, j, a);
         B.value(i, j, b); 

         std::vector< std::complex<double> > aupdate = a;
         std::vector< std::complex<double> > bupdate = b;

         //Compute i*kt*n
         std::complex<double> phase(0.0, frameCount * dispersion(Itilde.kx(i), Itilde.ky(j)));
         //Compute e^(i*kt*n) and store it in phase
         phase = std::exp(phase);

         double oneOverN = 1.0/(frameCount + 1);

         //Apply the LWE algorithm to each channel
         for( size_t c = 0; c < itilde.size(); c++) 
         {
            aupdate[c] += (itilde[c]/phase - b[c]/(phase*phase) - a[c]) * oneOverN;
            bupdate[c] += (itilde[c]*phase - a[c]*(phase*phase) - b[c]) * oneOverN;
         }
         //Set the values of A and B at (i,j) to the updated multichannel values
         A.set_value(i, j, aupdate);
         B.set_value(i, j, bupdate);
      }
   }
   frameCount ++;
}


void LinearWaveEstimate::value(int i, int j, int n, std::vector< std::complex<double> >& amplitude) const
{
   //Compute i*kt*n
   std::complex<double> phase(0.0, n * dispersion(A.kx(i), A.ky(j)));
   //Compute e^(i*kt*n) and store it in phase
   phase = std::exp(phase);

   std::vector< std::complex<double> > a;
   std::vector< std::complex<double> > b;

   //Put the current multichannel values of A and B at (i,j) into a and b respectively
   A.value(i, j, a);
   B.value(i, j, b);

   amplitude.resize(a.size());

   //Calculate the amplitude at each channel
   for (size_t c = 0; c < a.size(); c++)
   {
      amplitude[c] = a[c] * phase + b[c]/phase;
   }
}


void img::extract_image(const LinearWaveEstimate& l, int frame, ImgProc& img)
{
   img.clear(l.getA().nx(), l.getA().ny(), l.getA().depth());

   FFTImgProc fftimg;
   fftimg.clear(img.nx(), img.ny(), img.depth());
   
   //Completely fills the FFT image fftimg with amplitudes calculated by the value() function 
   for (int j = 0; j < img.ny(); j++)
   {
      for (int i = 0; i < img.nx(); i++)
      {
         std::vector< std::complex<double> > v;
         l.value(i, j, frame, v);
         fftimg.set_value(i, j, v);
      }
   }

   fftimg.fft_backward();

   for (int j = 0; j < img.ny(); j++)
   {
      for (int i = 0; i < img.nx(); i++)
      {
         std::vector< std::complex<double> > v;
         fftimg.value(i, j, v); //Put the current multichannel value of fftimg at (i,j) into v

         std::vector<float> iv(v.size());

         
         for(size_t c = 0; c < v.size(); c++)
         {
            iv[c] = v[c].real(); //Take the real part from v at each channel and store it in iv
         }
         img.set_value(i, j, iv); //Set the value of img at (i,j) to the multichannel value now stored in iv
      }
   }
}



