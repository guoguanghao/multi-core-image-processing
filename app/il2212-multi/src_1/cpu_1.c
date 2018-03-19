
/* 
 *   Lab 2   Jiaqi Li  Guanghao Guo
 *   Image_process_multi
 *   Implement lab 2 by multi-core
 *   
 *   cpu_1 is used for resizing and brightness correcting.
 */


// #include <stdio.h>
#include "sys/alt_stdio.h"
#include "system.h"

#define DEBUG 1

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
	
	unsigned char sizeX = *img++, sizeY = *img++;
	int sizeImg = sizeX * sizeY;
	unsigned char ascImg[ sizeImg + 3 ];
	unsigned char* imgP = ascImg;
	int i,j;
	unsigned char symbols[16] = {32, 46, 59, 42, 115, 116, 73, 40, 106, 117, 86, 119, 98, 82, 103, 64};
	*imgP++ = sizeX ;
	*imgP++ = sizeY ;
	*imgP++ = *img++;

	for(i = sizeImg; i > 0 ; i--) // impP -> Start of pixels. Transforming grayscale into ASCII numbers.
	{
	*imgP++ = symbols[((*img++)>>4)];
	}
	imgP = &ascImg[3];
	for(i = sizeY; i > 0; i--)
	{
		for(j = sizeX ; j > 0 ; j--)
		{
		alt_putchar(*imgP++);  // Display
		}
	alt_printf("\n");
	}
}



/* 
 *   Adjust the size of an image
 */
void resizeImg(unsigned char* base) {

		
	unsigned char sizeX = base[0],state;
	unsigned char sizeY = base[1];
	int sizeImg = sizeX * sizeY,i,j;
	unsigned char *resizeImg;
	resizeImg = base + sizeImg + 3;
	unsigned char sizeXAfResize = sizeX>>1,sizeYAfResize = sizeY>>1;
	resizeImg[0] = sizeXAfResize;
	resizeImg[1] = sizeYAfResize;
	
	for(i = 0;i<sizeYAfResize;i++) {
		for(j = 0;j<sizeXAfResize;j++) {		//  (x + x_+_1 + x_next_row + x_next_row_+_1) / 4  merging 4 pixels
			int tmp = 2 * i * sizeX;
			resizeImg[i * sizeXAfResize + j + 3] = (base[tmp + 2 * j + 3] + base[tmp + 2 * j + 1 + 3]
								  			+ base[tmp + sizeX  + 2 * j+3] + base[tmp + sizeX  + 2 * j + 1 + 3])>>2;
		}
	}
	// printAscii(base);
	// printAscii(share);
}

<<<<<<< HEAD
		imgP = img_array[current_image];
		// if(count!=1)
		// while(state==??????);    revise1 by gguo
		
		// PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
		// PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
		// PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, rgbToGrayS);
		/* Measurement here */
		sizeX = *imgP++;
		sizeY = *imgP++;
		sizeImg = sizeX * sizeY;
		unsigned char grayImg[sizeImg+3];
		grayP = grayImg;
=======
/* 
 *   Adjust the brightness of an image
 */

void brightCorrect(unsigned char* img) 
{
	unsigned char bmax=0,bmin=255,sig=1;
	
	unsigned char sizeX = img[0];
	unsigned char sizeY = img[1];
	int sizeImg = sizeX * sizeY;
	int i,j;
	for(i = 0;i<sizeX*sizeY;i++)  // Look for maximum and minimum brightness
	{
>>>>>>> Kaki
		
		if(img[i+3]>bmax)
			bmax=img[i+3];
		if(img[i+3]<bmin)
			bmin=img[i+3];
	}
	if(bmax-bmin>127)
	{
		sig = 0;	
	}
	while(sig==1)		//   Make error = maximum_brightness - minimum_brightness
	{					//   If error > 127, pixel values will not change
	for(i = 0;i<sizeX*sizeY;i++) // If error > 63, pixel values become 2 * (own_value - minimum_brightness)
		{						 // If error > 31, pixel values become 4 * (own_value - minimum_brightness)
			unsigned char tmp,error; //If error > 15, pixel values become 8 * (own_value - minimum_brightness)
			tmp = img[i+3]-bmin;	//If error < 15, pixel values become 16 * (own_value - minimum_brightness)
			error = bmax-bmin;
			if(error>127)
				{
				sig = 0;
				}
				else if(error>63)
					img[i+3] = tmp << 1;
					else if(error>31)
						img[i+3] = tmp << 2;
						else if(error>15)
						img[i+3] = tmp << 3;
						else
						img[i+3] = tmp << 4;
		}
		for(i = 0;i<sizeX*sizeY;i++)	// Search for maximum and minimum brightness 
		{								//  after adjusting brightness levels until 
		if(img[i+3]>bmax)				//  error larger than 127
			bmax=img[i+3];
		if(img[i+3]<bmin)
			bmin=img[i+3];
		}
	if(bmax-bmin>127)
		sig = 0;
	}
	img[2]=bmax;
	// printAscii(share);
}

cpu1Task(unsigned char grayFIFOP) 
{
	unsigned char *share,state,*tmpP;
	
	unsigned char sizeX,sizeY,grayReadyCount,brightReadyCount;
	int sizeImg,i;
	share = (unsigned char*) SHARED_ONCHIP_BASE;

	/* 
 	 *   When the number of unprocessed brightness images is less than 3
	 *	 and the number of unprocessed grayscale images is more then 0,
	 *	 grayscale images will be resized and corrected the brightness.
	 *	 The resulting image will be stored into different memory positions.
 	 */

	grayReadyCount = *share & MASKGRAY;
	brightReadyCount = (*share & MASKBRI)>>2;

	
	while(grayReadyCount==0||brightReadyCount==3)
	{
		grayReadyCount = *share & MASKGRAY;
		brightReadyCount = (*share & MASKBRI)>>2;
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

<<<<<<< HEAD
		share = (unsigned char*) SHARED_ONCHIP_BASE;
		/*
		*change the CPU2 state to unread
		*/
		altera_avalon_mutex_lock( mutex, 1 );
		*share = *share & (~MASKU2);
		state = *share;
		altera_avalon_mutex_unlock( mutex );
		// printAscii(share);
=======
	
>>>>>>> Kaki

	sizeX = share[0];
	sizeY = share[1];
	sizeImg = sizeX * sizeY;
	unsigned char *brightImgP;
	brightImgP = share + sizeImg + 3;

	/*
	 * Parallel calculations in CPU1
	 */
	resizeImg(share);
	brightCorrect(brightImgP);

	/* 
	 *   Decrease the number of unprocessed grayscale images and increase
	 * 	 the number of unprocessed brightness corrected images,
	 *   numbers will be stored in the first byte of shared onchip memory.
 	 */

	share = (unsigned char*) SHARED_ONCHIP_BASE;
	grayReadyCount = *share & MASKGRAY;
	grayReadyCount--;
	brightReadyCount = (*share & MASKBRI)>>2;
	brightReadyCount++;
	*share = (*share & (~MASKGRAY)) | grayReadyCount ;
	*share = (*share & (~MASKBRI)) | (brightReadyCount<<2);
	
	
}

int main(void) {

	
	
	int i;
	unsigned char FIFOP=0;
	while (1)
	{ 
		cpu1Task(FIFOP);	
		FIFOP = ( FIFOP + 1 ) % 4;
	}
 
  return 0;
}
