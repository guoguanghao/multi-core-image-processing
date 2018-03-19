
/* 
 *   Lab 2   Jiaqi Li  Guanghao Guo
 *   Image_process_multi
 *   Implement lab 2 by multi-core
 *   
 *   cpu_3 is used for outputting ASCII
 */


// #include "stdio.h"
#include "system.h"
#include "sys/alt_stdio.h"

/* 
 *   The data in the first byte of shared onchip memory
 * 	 is used for communications between cores.
 *   It indicates the state of the system.
 * 	 First two bits indicate the number of unprocessed grayscale images.
 * 	 Next two bits indicate the number of unprocessed brightness corrected images.
 *   Next two bits indicate the number of unprocessed sobel images.
 */

#define MASKGRAY 0x03
#define MASKBRI 0x0C
#define MASKSOBEL 0x30
#define MASKRASCII 0xC0


/* 
 *   Printing ascii to screen
 */



void printAscii(unsigned char* img)
{
	// unsigned char* tmpP = &img[3];
	
	unsigned char* tmpP = &img[3];
	
	unsigned char sizeX = img[0], sizeY = img[1];
	int i,j;
	int sizeImg = sizeX * sizeY;
	unsigned char* imgP = img + sizeImg + 3;
	unsigned char symbols[16] = {32, 46, 59, 42, 115, 116, 73, 40, 106, 117, 86, 119, 98, 82, 103, 64};
	*imgP++ = sizeX ;
	*imgP++ = sizeY ;
	img += 2;
	*imgP++ = *img++;

	for(i = sizeImg; i > 0 ; i--) // impP -> Start of pixels. Transforming grayscale into ASCII numbers.
	{
	*imgP++ = symbols[((*img++)>>4)];
	}
	// imgP = tmpP + sizeImg + 3;
	// for(i = sizeY; i > 0; i--)
	// {
	// 	for(j = sizeX ; j > 0 ; j--)
	// 	{
	// 	alt_putchar(*imgP++);  // Display
	// 	}
	// alt_putchar('\n');
	// }
}






cpu3Task(unsigned char grayFIFOP) 
{
	unsigned char *share;
	unsigned char sizeX,sizeY,sobelReadyCount,asciiReadyCount;
	int sizeImg,i;
	unsigned char *asciiP;
	
	
	
	
	
	/* 
 	 *   When the number of unprocessed sobel images is more then 0,
	 *	 sobel images will be converted into ASCII.
	 *	 The resulting image will be displayed on the console.
 	 */
	share = (unsigned char*) SHARED_ONCHIP_BASE;	
	sobelReadyCount = (*share & MASKSOBEL)>>4;		
	asciiReadyCount = (*share & MASKRASCII)>>6;
	while( sobelReadyCount==0 || asciiReadyCount == 3 )						
	{
		sobelReadyCount = (*share & MASKSOBEL)>>4;
		asciiReadyCount = (*share & MASKRASCII)>>6;
	}

	*share++;

	switch(grayFIFOP) {
		case 0: 
			break;
		case 1: 
			share += 2048;
			break;
		case 2: 
			share += 2048 * 2;
			break;
		case 3: 
			share += 2048 * 3;
			break;
	}

	sizeX = share[0];
	sizeY = share[1];
	sizeImg = sizeX * sizeY;
	share += sizeImg + 3;
	sizeX = share[0];
	sizeY = share[1];
	sizeImg = sizeX * sizeY;
	share += sizeImg + 3;
	sizeX = share[0];
	sizeY = share[1];
	sizeImg = sizeX * sizeY;


	

	/*
	 * Parallel calculations in CPU3
	 */
	
	printAscii(share);
	
	/* 
	 *   Decrease the number of unprocessed sobel images,
	 *   numbers will be stored in the first byte of shared onchip memory.
 	 */

	share = (unsigned char*) SHARED_ONCHIP_BASE;
	sobelReadyCount = (*share & MASKSOBEL)>>4;
	sobelReadyCount--;
	asciiReadyCount = (*share & MASKRASCII)>>6;
	asciiReadyCount++;
	*share = (*share & (~MASKSOBEL)) | (sobelReadyCount<<4) ;
	*share = (*share & (~MASKRASCII)) | (asciiReadyCount<<6) ;
	
}

int main(void) {

	
	
	int i;
	unsigned char FIFOP = 0 ;
	while (1)
	{ 
		cpu3Task(FIFOP);	
		FIFOP=(FIFOP+1) % 4;
	}
 
  return 0;
}




