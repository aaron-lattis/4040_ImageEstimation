

#include "fftanalysis.h"
using namespace img;




void img::load_fft( const ImgProc& input, FFTImgProc& fftoutput )
{
   fftoutput.clear( input.nx(), input.ny(), input.depth() );
   for(int j=0;j<input.ny();j++)
   {
      for(int i=0;i<input.nx();i++)
      {
         std::vector<float> ci;
	 std::vector< std::complex<double> > citilde;
	 input.value(i,j,ci);
	 for(size_t c=0;c<ci.size();c++)
	 {
	    std::complex<double> v(ci[c], 0.0);
	    citilde.push_back(v);
	 }
	 fftoutput.set_value(i,j,citilde);
      }
   }
}


