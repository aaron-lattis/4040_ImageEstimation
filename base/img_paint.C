//------------------------------------------------
//
//  img_paint
//
//
//-------------------------------------------------




#include <cmath>
#include <omp.h>
#include "imgproc.h"
#include "CmdLineFind.h"
#include <vector>

#include "LinearWaveEstimate.h"


#include <GL/gl.h>   // OpenGL itself.
#include <GL/glu.h>  // GLU support library.
#include <GL/glut.h> // GLUT support library.


#include <iostream>
#include <stack>


using namespace std;
using namespace img;

ImgProc image;

void setNbCores( int nb )
{
   omp_set_num_threads( nb );
}

void cbMotion( int x, int y )
{
}

void cbMouse( int button, int state, int x, int y )
{
}

void cbDisplay( void )
{
   glClear(GL_COLOR_BUFFER_BIT );
   glDrawPixels( image.nx(), image.ny(), GL_RGB, GL_FLOAT, image.raw() );
   glutSwapBuffers();
}

void cbIdle()
{
   glutPostRedisplay();	
}

void cbOnKeyboard( unsigned char key, int x, int y )
{
   switch (key) 
   {
      case 'f':
	 image.flip();
	 cout << "Flip\n";
	 break; 

      case 'o':
	 image.createImageFile();
	 cout << "File created\n";
	 break;    
   }

}

void PrintUsage()
{
   cout << "img_paint keyboard choices\n";
   cout << "f         flip the image\n";
   cout << "o         create new EXR image file called 'newFile.exr'\n";
}


int main(int argc, char** argv)
{
   setNbCores(8);

   cout << "\n\nLinear wave estimation is being applied.\n";
   cout << "An estimation of frame 99 will be shown when the process is complete!\n\n";

   image.load(argv[1]);
   
   LinearWaveEstimate estimate = LinearWaveEstimate(image, 3.33);

   for (int i = 1; i < argc; i ++)
   {
      if (i > 1)
      {
         image.load(argv[i]);
      }

      estimate.ingest(image); //pass each provided image through the ingest code
   }

   int frameNumber = 99; //I have determined frame 99 is the frame where the message is most clear

   PrintUsage();

   extract_image(estimate, frameNumber, image);
   
   // GLUT routines
   glutInit(&argc, argv);

   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
   glutInitWindowSize( image.nx(), image.ny() );

   // Open a window 
   char title[] = "img_paint";
   glutCreateWindow( title );
   
   glClearColor( 1,1,1,1 );

   glutDisplayFunc(&cbDisplay);
   glutIdleFunc(&cbIdle);
   glutKeyboardFunc(&cbOnKeyboard);
   glutMouseFunc( &cbMouse );
   glutMotionFunc( &cbMotion );

   glutMainLoop();
   return 1;
};
