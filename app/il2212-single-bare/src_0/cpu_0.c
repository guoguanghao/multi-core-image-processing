
/* 
 *   Lab 1   Jiaqi Li  Guanghao Guo
 *   Image_process_single_bare
 *   Use bare metal to implement lab 1 in single core
 *   Data Flow :
 *   rgbToGray()->reisze()->brightCorrect->sobel()->printAscii()
 */


#include <stdio.h>
#include "altera_avalon_performance_counter.h"
#include "altera_avalon_pio_regs.h"
#include "system.h"
#include "io.h"
#include "images.h"

#define DEBUG 1

#define HW_TIMER_PERIOD 100 /* 100ms */

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048


/* Definition of Task Priorities */

#define STARTTASK_PRIO      1
#define TASK1_PRIORITY      10

/* Definition of Task Periods (ms) */
#define TASK1_PERIOD 100

#define rgbToGrayS 1
#define resizeImgS 2
#define brightCorrectS 3
#define sobelS 4
#define printAsciiS 5


unsigned int sqrt_16(unsigned long M)  { // Backup simple sqrt function
	unsigned int N, i;        
	unsigned long tmp, ttp;          
	if (M == 0)                        
	return 0;  
		N = 0;         
	tmp = (M >> 30);               
	M <<= 2;       
 	if (tmp > 1)                  
 	{            
		 N ++;                           
		tmp -= N;       
	}         
	for (i=15; i>0; i--)           
	{           
		N <<= 1;                         
		tmp <<= 2;            
		tmp += (M >> 30);              
 		ttp = N;            
		ttp = (ttp<<1)+1;             
		M <<= 2;            
		if (tmp >= ttp)                
		{               
 			tmp -= ttp;               
			N ++;           
		}       
 	}         
	return N;  
}


/* 
 *   Printing ascii to screen
 */

void printAscii(unsigned char* img)
{
	PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, printAsciiS);
		/* Measurement here */

	unsigned char* tmpP = &img[3];
	unsigned char* imgP = img;
	unsigned char sizeX = *img++, sizeY = *img++;
	int i,j;
	int sizeImg = sizeX * sizeY;
	unsigned char symbols[16] = {32, 46, 59, 42, 115, 116, 73, 40, 106, 117, 86, 119, 98, 82, 103, 64};
	*imgP++ = sizeX ;
	*imgP++ = sizeY ;
	*imgP++ = *img++;

	for(i = sizeImg; i > 0 ; i--) // impP -> Start of pixels. Transforming grayscale into ASCII numbers.
	{
	*imgP++ = symbols[((*imgP)>>4)];
	}
	imgP = tmpP;
	for(i = sizeY; i > 0; i--)
	{
		for(j = sizeX ; j > 0 ; j--)
		{
		putchar(*imgP++);  // Display
		}
	printf("\n");
	}
	PERF_END(PERFORMANCE_COUNTER_0_BASE, printAsciiS);

}
/* 
 *   Transform a rgb imagie into gray image
 */
void rgbToGray(unsigned char* base)
{
PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, rgbToGrayS);
		/* Measurement here */


	unsigned char sizeX = *base++;
	unsigned char sizeY = *base++;
	int sizeImg = sizeX * sizeY,i;
	unsigned char* share;
	share = (unsigned char*) SHARED_ONCHIP_BASE;

    *share++ = sizeX;
	*share++ = sizeY;
	*share++ = *base++;

	for(i = sizeImg-1; i >= 0; i--)   // grayscale = (4/16 + 1/16) * imgR + (8/16 + 1/16) * imgG + 1/8 * imgB 
	{
		*share++ =  ((*base)>>4) + ((*base++)>>2)
    	         + ((*base)>>4) + ((*base++)>>1) 
        	     + ((*base++)>>3);
			//    ((*base++)>>2)
            //  + ((*base++)>>1)   
        	//  + ((*base++)>>3);// Less accurate but faster
	}

	share = (unsigned char*) SHARED_ONCHIP_BASE;
	PERF_END(PERFORMANCE_COUNTER_0_BASE, rgbToGrayS);
	resizeImg(share);
}

/* 
 *   Adjust the size of an image
 */

void resizeImg(unsigned char* base) {
	PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, resizeImgS);
		/* Measurement here */

		
	unsigned char sizeX = base[0];
	unsigned char sizeY = base[1];
	unsigned char sizeXAfResize = sizeX>>1,sizeYAfResize = sizeY>>1;
	int sizeImg = sizeX * sizeY>>2,i,j;
	base[0] = sizeXAfResize;
	base[1] = sizeYAfResize;
	
	for(i = 0;i<sizeYAfResize;i++) {
		for(j = 0;j<sizeXAfResize;j++) {		//  (x + x_+_1 + x_next_row + x_next_row_+_1) / 4  merging 4 pixels
			int tmp = 2 * i * sizeX;
			base[i * sizeXAfResize + j + 3] = (base[tmp + 2 * j + 3] + base[tmp + 2 * j + 1 + 3]
								  			 + base[tmp + sizeX  + 2 * j+3] + base[tmp + sizeX  + 2 * j + 1 + 3])>>2;
		}
	}


	PERF_END(PERFORMANCE_COUNTER_0_BASE, resizeImgS);
	brightCorrect(base);
}

/* 
 *   Adjust the brightness of an image
 */

void brightCorrect(unsigned char* img) {
	PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, brightCorrectS);
		/* Measurement here */

	unsigned char bmax=0,bmin=255,sig=1;
	unsigned char sizeX = img[0],sizeY = img[1];
	int i,j;
	img[0]=sizeX;
	img[1]=sizeY;
	for(i = 0;i<sizeX*sizeY;i++)  // Look for maximum and minimum brightness
	{
		
		if(img[i+3]>bmax)
			bmax=img[i+3];
		if(img[i+3]<bmin)
			bmin=img[i+3];
	}
	if(bmax-bmin>127)
		sig = 0;
	while(sig==1)		//   Make error = maximum_brightness - minimum_brightness
	{					//   If error > 127, pixel values will not change
	for(i = 0;i<sizeX*sizeY;i++) // If error > 63, pixel values become 2 * (own_value - minimum_brightness)
		{						 // If error > 31, pixel values become 4 * (own_value - minimum_brightness)
			unsigned char tmp,error; //If error > 15, pixel values become 8 * (own_value - minimum_brightness)
			tmp = img[i+3]-bmin;	//If error < 15, pixel values become 16 * (own_value - minimum_brightness)
			error = bmax-bmin;
			if(error>127)
				sig = 0;
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
	PERF_END(PERFORMANCE_COUNTER_0_BASE, brightCorrectS);

	sobel(img);
}

/* 
 *   Apply sobel algorithm on an image
 */

void sobel (unsigned char* base) {
	PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, sobelS);
		/* Measurement here */

		
	unsigned char sizeX = base[0];
	int i,j,tmp;
	unsigned char sizeY = base[1];
	unsigned char sizeXAfSobel = sizeX - 2,sizeYAfSobel = sizeY - 2;
	unsigned char* imgP;
	imgP = (unsigned char*) SHARED_ONCHIP_BASE;
	imgP = imgP + sizeX * sizeY + 3;
	*imgP++ = sizeXAfSobel;
	*imgP++ = sizeYAfSobel;
	*imgP++ = base[2];
	unsigned char* tmpP = &base[3];
	
	for(i=1;i<sizeY-1;i++)
	{	
		for( j=1; j<sizeX-1;j++)
		{							  //  tmpP↓
			int xm1ym1,xym1,xp1ym1,   // |(x-1,y-1) (x,y-1) (x+1,y-1)|
				xm1y       ,xp1y,	  // |(x-1,y)   (x,y)   (x+1,y)  |
				xm1yp1,xyp1,xp1yp1;	  // |(x-1,y+1) (x,y+1) (x+1,y+1)|
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
	printf("\n");
	imgP = (unsigned char*) SHARED_ONCHIP_BASE;
	imgP = imgP + sizeX * sizeY + 3;
	PERF_END(PERFORMANCE_COUNTER_0_BASE, sobelS);
	printAscii(imgP);
}








int main(void) {



unsigned char value=0;
	unsigned char current_image=0;
	
	// #if DEBUG == 1
	// /* Sequence of images for testing if the system functions properly */
	// char number_of_images=10;
	// unsigned char* img_array[10] = {img1_24_24, img1_32_22, img1_32_32, img1_40_28, img1_40_40, 
	// 		img2_24_24, img2_32_22, img2_32_32, img2_40_28, img2_40_40 };
	// #else
	/* Sequence of images for measuring performance */
	unsigned char number_of_images=3;
	unsigned char* img_array[3] = {img1_32_32, img2_32_32, img3_32_32};
	char buffer[2048];
	setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));	// Create a buffer for stdout, making the board not communicate to computer 
	// #endif											// every time printf/putchar is called.
	// char number_of_images=3;

	while (1)
	{ 
		/* Extract the x and y dimensions of the picture */
		// unsigned char i = *img_array[current_image];
		// unsigned char j = *(img_array[current_image]+1);

		PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
		PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
		
		rgbToGray(img_array[current_image]);

		  

		/* Print report */
		perf_print_formatted_report
		(PERFORMANCE_COUNTER_0_BASE,            
		ALT_CPU_FREQ,        // defined in "system.h"
		5,                   // How many sections to print
		"rgbToGray",        // Display-name of section(s).
		"resizeImg",
		"brightCorrect",
		"sobel",
		"printAscii"
		);

		/* Just to see that the task compiles correctly */
		IOWR_ALTERA_AVALON_PIO_DATA(LEDS_GREEN_BASE,value++);


		/* Increment the image pointer */
		current_image=(current_image+1) % number_of_images;

	}
 
  return 0;
}
