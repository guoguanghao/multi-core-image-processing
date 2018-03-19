
/* 
 *   Lab 1   Jiaqi Li  Guanghao Guo
 *   Image_process_single_bare
 *   Use bare metal to implement lab 1 in single core
 *   Data Flow :
 *   rgbToGray()->reisze()->brightCorrect->sobel()->printAscii()
 */


// #include "stdio.h"
#include "system.h"
#include "altera_avalon_mutex.h"
#include "sys/alt_stdio.h"
// #include "images.h"

#define MASKU1 0x10
#define MASKU2 0x08
#define MASKU3 0x04
#define MASKU4 0x02
#define MASKU0 0x01

alt_mutex_dev* mutex;





/* 
 *   Printing ascii to screen
 */

// void printAscii(unsigned char* img)
// {

// 	unsigned char* tmpP = &img[3];
// 	unsigned char* imgP = img;
// 	unsigned char sizeX = *img++, sizeY = *img++;
// 	int i,j;
// 	int sizeImg = sizeX * sizeY;
// 	unsigned char symbols[16] = {32, 46, 59, 42, 115, 116, 73, 40, 106, 117, 86, 119, 98, 82, 103, 64};
// 	*imgP++ = sizeX ;
// 	*imgP++ = sizeY ;
// 	*imgP++ = *img++;

// 	for(i = sizeImg; i > 0 ; i--) // impP -> Start of pixels. Transforming grayscale into ASCII numbers.
// 	{
// 	*imgP++ = symbols[((*imgP)>>4)];
// 	}
// 	imgP = tmpP;
// 	for(i = sizeY; i > 0; i--)
// 	{
// 		for(j = sizeX ; j > 0 ; j--)
// 		{
// 		alt_putchar(*imgP++);  // Display
// 		}
// 	alt_printf("\n");
// 	}

// }








// int main(void) {




// 	unsigned char sizeX,sizeY;
// 	unsigned int sizeImg;
// 	// unsigned char* img_array[3] = {img1_32_32, img2_32_32, img3_32_32};
// 	// char buffer[2048];
// 	// setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));	// Create a buffer for stdout, making the board not communicate to computer 
// 	unsigned char* share;
// 	alt_mutex_dev* mutex = altera_avalon_mutex_open(MUTEX_0_NAME);
// 	// char buffer[2048];
// 	// setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));
// 	while (1)
// 	{ 
		
// 		share = (unsigned char*) SHARED_ONCHIP_BASE;
// 		*share++;
		
// 		altera_avalon_mutex_lock( mutex, 1 );
// 		sizeX = share[0];
// 		sizeY = share[1];
// 		sizeImg = sizeX * sizeY;
// 		share += sizeImg + 3;
// 		sizeX = share[0];
// 		sizeY = share[1];
// 		sizeImg = sizeX * sizeY;
// 		share += sizeImg + 3;
// 		sizeX = share[0];
// 		sizeY = share[1];
// 		sizeImg = sizeX * sizeY;
// 		share += sizeImg + 3;
// 		printAscii(share);
// 		// alt_printf("hello from 2");
// 		altera_avalon_mutex_unlock( mutex );
		  

		

// 		/* Just to see that the task compiles correctly */
		


// 		/* Increment the image pointer */
// 		// current_image=(current_image+1) % number_of_images;

// 	}
 
//   return 0;
// }
int main()
{
	alt_printf("Hello from U4");
	return 0;
}