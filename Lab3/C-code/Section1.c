// gcc -o Section1 Section1.c allocate.c randlib.c solve.c tiff.c mysubroutine.c 
// ./Section1 img22gd2.tif 2

#include <math.h>
#include "tiff.h"
#include "allocate.h"
#include "randlib.h"
#include "typeutil.h"
#include "mysubroutine.h"

void error(char *name);

int main (int argc, char **argv)
{
  FILE *fp;
  struct TIFF_img input_img, seg_img;
  struct pixel s;
  struct pixel c[4];
  double **img1;
  double T;
  int connect_num;
  int ClassLabel = 0;
  int neighbor_num;
  int32_t i,j;

  if ( argc != 3 ) error( argv[0] );

  T = atof(argv[2]); // third input 
  /* open image file */
  if ( ( fp = fopen ( argv[1], "rb" ) ) == NULL ) {
    fprintf ( stderr, "cannot open file %s\n", argv[1] );
    exit ( 1 );
  }

  /* read image */
  if ( read_TIFF ( fp, &input_img ) ) {
    fprintf ( stderr, "error reading file %s\n", argv[1] );
    exit ( 1 );
  }

  /* close image file */
  fclose ( fp );

  if ( input_img.TIFF_type != 'g' ) {
    fprintf ( stderr, "error:  image must be monochrome\n" );
    exit ( 1 );
  }

  printf("T: %f\n", T);
  /* Allocate image of double precision floats
  Extend the image for filtering*/
  img1 = (double **)get_img(input_img.width,input_img.height,sizeof(double));

  /* to allocate a full color image use type 'c' */
  get_TIFF ( &seg_img, input_img.height, input_img.width, 'g' );

  for(i = 0; i<input_img.height; i++)
    for(j = 0; j<input_img.width; j++){
      // seg_img.mono[i][j] = 0;
      seg_img.mono[i][j] = 255; // set to be white
    }
    printf("image dimensions: height:%d  width: %d\n",input_img.height, input_img.width );

    s.m = 45;
    s.n = 67;

    ConnectedSet(s,T,input_img.mono,input_img.width,input_img.height,
      ClassLabel,seg_img.mono, &connect_num);
    printf("Connected number: %d\n",connect_num);

  /* open image file */
    if ( ( fp = fopen ( "seg_img.tif", "wb" ) ) == NULL ) {
      fprintf ( stderr, "cannot open file seg_img.tif\n");
      exit ( 1 );
    }

  /* write image */
    if ( write_TIFF ( fp, &seg_img ) ) {
      fprintf ( stderr, "error writing TIFF file %s\n", argv[2] );
      exit ( 1 );
    }

  /* close image file */
    fclose ( fp );

  /* de-allocate space which was used for the images */
    free_TIFF ( &(input_img) );
    free_TIFF ( &(seg_img) );
    free_img( (void**)img1 );

    return(0);
  }

  void error(char *name)
  {
    printf("usage:  %s  image.tiff \n\n",name);
    printf("this program reads in a 24-bit color TIFF image.\n");
    printf("It then horizontally filters the green component, adds noise,\n");
    printf("and writes out the result as an 8-bit image\n");
    printf("with the name 'green.tiff'.\n");
    printf("It also generates an 8-bit color image,\n");
    printf("that swaps red and green components from the input image");
    exit(1);
  }