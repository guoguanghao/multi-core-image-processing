

/* 
 *   Lab 2   Jiaqi Li  Guanghao Guo
 *   Image_process_multi
 *   Implement lab 2 by multi-core
 *   
 *   cpu_2 is used for doing sobel calculation.
 */


// #include <stdio.h>
#include "system.h"
#include "altera_avalon_mutex.h"
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

// /* 
//  *   Printing ascii to screen
//  */

void printAscii(unsigned char* img)
{

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
 *   Apply sobel algorithm on an image
 */



void sobel (unsigned char* base) {

		
	unsigned char sizeX = base[0];
	int i,j,tmp;
	unsigned char sizeY = base[1];
	unsigned char sizeXAfSobel = sizeX - 2,sizeYAfSobel = sizeY - 2;
	unsigned char *imgP;
	// unsigned char sobelImg[sizeXAfSobel*sizeYAfSobel+3];
	imgP = base;
	imgP += sizeX * sizeY + 3;
	// imgP = sobelImg;
	*imgP++ = sizeXAfSobel;
	*imgP++ = sizeYAfSobel;
	*imgP++ = base[2];
	unsigned char* tmpP = &base[3];
	
	/*  tmpP↓
	 *	|(x-1,y-1) (x,y-1) (x+1,y-1)|
	 *  |(x-1,y)   (x,y)   (x+1,y)  |
	 *  |(x-1,y+1) (x,y+1) (x+1,y+1)|
	 * 	Pointer begins with pointing at (x-1,y-1)
	 */  

	for(i=1;i<sizeY-1;i++)
	{	
		for( j=1; j<sizeX-1;j++)
		{							  
			int xm1ym1,xym1,xp1ym1, 
				xm1y       ,xp1y,	
				xm1yp1,xyp1,xp1yp1;	 
			int gx,gy,g,g1;
			

				xm1ym1=*tmpP++;
				xym1= *tmpP++;
				xp1ym1= *tmpP;
				tmpP = tmpP + sizeX - 2;
				xm1y= *tmpP++;
				*tmpP++;
				// xy: pixel in the middle
				xp1y= *tmpP;
				tmpP = tmpP + sizeX - 2;
				xm1yp1= *tmpP++;
				xyp1= *tmpP++;
				xp1yp1= *tmpP;

				gx = xm1ym1 + (xm1y>>1) + xm1yp1 - (xp1yp1 + (xp1y>>1) + xp1ym1);
				gy = xp1ym1 + (xym1>>1) + xm1ym1 - (xp1yp1 + (xyp1>>1) + xm1yp1);
				//problem
				g = (abs(gx) + abs (gy))>>2; // 	g = √(gx^2 + gy^2) ≈ |gx| + |gy|
				if(g>255)
				g=255;
				*imgP++ = g;	
				tmpP = tmpP - 2 * sizeX - 1;
		}
		tmpP = tmpP + 2;
	}
	// printf("\n");
	// imgP = sobelImg;
	imgP = base;
	imgP = imgP + sizeX * sizeY + 3;
	// printAscii(imgP);
}

cpu2Task(unsigned char grayFIFOP) 
{
<<<<<<< HEAD
	unsigned char state;
	unsigned char sizeX,sizeY,sizeXAfResize,sizeYAfResize,maxBri;
	int sizeImg,i,j;
	alt_mutex_dev* mutex = altera_avalon_mutex_open(MUTEX_0_NAME);
 	printf("Hello from cpu_2???!\n");
 	unsigned char* sharedGrayImgP,sharedResizeImg,imgP,tmpP;
  while (TRUE) { 
	  	sharedGrayImgP = (unsigned char*) SHARED_ONCHIP_BASE;
		/*
		 * state judgement here
		 */
		altera_avalon_mutex_lock( mutex, 1 );
		state = *sharedGrayImgP;
		altera_avalon_mutex_unlock( mutex );
		//while((~(state & MASKU2)) != ~(MASKU2))
		while(state & MASKU2 == MASKU2)//revise 1 by gguo
		{
			altera_avalon_mutex_lock( mutex, 1 );
			/*
 		 	 * Access a shared resource here.
 			 */
			state = *sharedGrayImgP;
			altera_avalon_mutex_unlock( mutex );
		}
		*sharedGrayImgP++;
		tmpP = sharedGrayImgP;//store the current pointer 
		altera_avalon_mutex_lock( mutex, 1 );
		/*
 		* Access a shared resource here, read from shared memory which is wrote by grayscaled function
 		*/
	  	sizeX = *sharedGrayImgP++;
		sizeY = *sharedGrayImgP++;
		maxBri = *sharedGrayImgP++;
		sizeImg = sizeX * sizeY;
		// int grayImg[sizeImg + 3];
		// imgP = grayImg;
		unsigned char imgP[sizeImg + 3];//revise 2 by gguo
		sharedGrayImgP = tmpP;//reuse stored pointer
		for(i=0;i<sizeImg+3;i++)
		{
			*imgP++ = *sharedGrayImgP++;
		}
		altera_avalon_mutex_unlock( mutex );
		/*
		*change the cpu2 state to has been read
		*/
		share = (unsigned char*) SHARED_ONCHIP_BASE;
		altera_avalon_mutex_lock( mutex, 1 );
		*share = *share | MASKU2;
		state = *share;
		altera_avalon_mutex_unlock( mutex );
		/*
		*resize image
		*/
		sizeXAfResize = sizeX>>1,sizeYAfResize = sizeY>>1;
		sizeImg = sizeXAfResize * sizeYAfResize;
		unsigned char resizeImg[sizeImg+3];
		//imgP = resizeImg;
		resizeImg[0] = sizeXAfResize;
		resizeImg[1] = sizeYAfResize;
		resizeImg[2] = maxBri;
		for(i = 0;i<sizeYAfResize;i++) {
			for(j = 0;j<sizeXAfResize;j++) {
				int tmp = 2 * i * sizeX;
				resizeImg[i * sizeXAfResize + j + 3] = (imgP[tmp + 2 * j + 3] + imgP[tmp + 2 * j + 1 + 3]
								  			 	+ imgP[tmp + sizeX  + 2 * j+3] + imgP[tmp + sizeX  + 2 * j + 1 + 3])>>2;
			}
		}
	//	imgP = resizeImg;
    // printf("Hello from cpu_1???!\n");
		sharedResizeImg = sharedGrayImgP;
		sharedGrayImgP = (unsigned char*) SHARED_ONCHIP_BASE;
		altera_avalon_mutex_lock( mutex, 1 );
		state = *sharedGrayImgP;//get the current state from shared memory
		altera_avalon_mutex_unlock( mutex );
		while ((state & MASKU3)!=MASKU3)
		//while((~(state & MASKU3)) != ~(MASKU3))
		{
			altera_avalon_mutex_lock( mutex, 1 );
			/*
 		 	 * Access a shared resource here.
 			 */
			state = *sharedGrayImgP;
			altera_avalon_mutex_unlock( mutex );
		}
		altera_avalon_mutex_lock( mutex, 1 );
  //   share = (unsigned char*) SHARED_ONCHIP_BASE;
		for(i=0;i<sizeImg+3;i++)
		{
			*sharedResizeImgP++ = *resizeImg++;
		}
		// delay(500);
		/* release the lock */
		altera_avalon_mutex_unlock( mutex );
		/*
		*change the cpu3 state  unread
		*/
		share = (unsigned char*) SHARED_ONCHIP_BASE;
		altera_avalon_mutex_lock( mutex, 1 );
		*share = *share & (~MASKU3);
		state = *share;
		altera_avalon_mutex_unlock( mutex );
		
=======
	unsigned char *share,state,sobelReadyCount,brightReadyCount;
	share = (unsigned char*) SHARED_ONCHIP_BASE;
	unsigned char sizeX,sizeY;
	int sizeImg,i;
	unsigned char *sobelP;
	
	/* 
 	 *   When the number of unprocessed sobel images is less than 3
	 *	 and the number of unprocessed brightness images is more then 0,
	 *	 sobel algorithm will be applied on brightness images.
	 *	 The resulting image will be stored into different memory positions.
 	 */
	
>>>>>>> Kaki

	share = (unsigned char*) SHARED_ONCHIP_BASE;
	// state = *share;

	sobelReadyCount = (*share & MASKSOBEL)>>4;
	brightReadyCount = (*share & MASKBRI)>>2;

	
	while(sobelReadyCount==3||brightReadyCount==0)
	{
		sobelReadyCount = (*share & MASKSOBEL)>>4;
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
	

	sizeX = share[0];
	sizeY = share[1];
	sizeImg = sizeX * sizeY;
	share += sizeImg + 3;
	sizeX = share[0];
	sizeY = share[1];
	sizeImg = sizeX * sizeY;

	/*
	 * Parallel calculations in CPU1
	 */

	sobelP = share;
	sobel(sobelP);
	
	
	/* 
	 *   Decrease the number of unprocessed brightness images and increase
	 * 	 the number of unprocessed sobel images,
	 *   numbers will be stored in the first byte of shared onchip memory.
 	 */

	share = (unsigned char*) SHARED_ONCHIP_BASE;
	sobelReadyCount = (*share & MASKSOBEL)>>4;
	brightReadyCount = (*share & MASKBRI)>>2;
	sobelReadyCount++;
	brightReadyCount--;
	*share = (*share & (~MASKSOBEL)) | (sobelReadyCount<<4) ;
	*share = (*share & (~MASKBRI)) | (brightReadyCount<<2) ;
	
}

int main(void) {

	
	
	int i;
	unsigned char FIFOP = 0;
	while (1)
	{ 
		cpu2Task(FIFOP);	
		FIFOP=(FIFOP+1) % 4;
	}
 
  return 0;
}
