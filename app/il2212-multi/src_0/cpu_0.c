
/* 
 *   Lab 2   Jiaqi Li  Guanghao Guo
 *   Image_process_multi
 *   Implement lab 2 by multi-core
 *   
 *   cpu_0 is used for converint RGB images into grayscale images.
 */


#include <stdio.h>
#include "altera_avalon_performance_counter.h"
#include "altera_avalon_pio_regs.h"
#include "system.h"
#include "io.h"
#include "images.h"

#define DEBUG 0

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

#define rgbToGrayS 1





/* 
 *   Printing ascii to screen
 */

void printAscii(unsigned char* img)
{
	unsigned char sizeX = *img++, sizeY = *img++;
	int sizeImg = sizeX * sizeY;
	int i,j;
	unsigned char symbols[16] = {32, 46, 59, 42, 115, 116, 73, 40, 106, 117, 86, 119, 98, 82, 103, 64};
	*img++;

	for(i = sizeY; i > 0; i--)
	{
		for(j = sizeX ; j > 0 ; j--)
		{
		putchar(*img++);  // Display
		}
	printf("\n");
	}

}

/*
 * Store ASCII to SRAM
 */


void storeASCII (unsigned int *asciiPointer, unsigned char whichASCII) {

	unsigned char *share,sizeX,sizeY,count=0,*tmpP;
	unsigned int sizeImg,i;
	unsigned int *intP;
	share = (unsigned char*) SHARED_ONCHIP_BASE;
	*share++;

	switch(whichASCII) {
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
	/*
	 * Navigate to ASCII images
	 */
	sizeX = share[0];
	sizeY = share[1];
	sizeImg = sizeX * sizeY + 3 ;
	share += sizeImg;
	sizeX = share[0];
	sizeY = share[1];
	sizeImg = sizeX * sizeY + 3 ;
	share += sizeImg;
	sizeX = share[0];
	sizeY = share[1];
	sizeImg = sizeX * sizeY + 3 ;
	share += sizeImg;
	/*
	 * Use integer to store data in order to decrese the time for reading and writing.
	 */
	intP = (unsigned int*) share;
	
	for(i=0 ;i < 50 ;i++)
	{
	*asciiPointer++ = *intP++;
	}
	
}





int main(void) {



	unsigned int count=0;
	unsigned char value=0;
	unsigned char current_image=0;
	
	
	unsigned char number_of_images=4,grayFIFOP=0;
	unsigned char* img_array[4] = {bar1,bar2,bar3,bar4};
	unsigned char* imgP;

	unsigned char sizeX,sizeY,state,grayReadyCount,asciiReadyCount,number_of_ascii=0;
	int sizeImg,i;
	unsigned char* share,*asciiP;
	unsigned int asciiImg[51];
	share = (unsigned char*) SHARED_ONCHIP_BASE;
	*share++ = 0;
	*share = 0;
	
	

	while (1)
	{ 

	/* 
 	 *   Counting the time needed for processing 300 images.
 	 */
		if(count==0)
		{
		PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
		PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
		}
		/* Measurement here */
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, rgbToGrayS);
		count++;

		

		imgP = img_array[current_image];
		sizeX = *imgP++;
		sizeY = *imgP++;
		sizeImg = sizeX * sizeY;
		
		/* 
 		 *   When the number of unprocessed grayscale images is less than 3,
		 *	 RGB images will be processed into grayscale images
		 *	 and stored into different memory positions.
 		 */

		share = (unsigned char*) SHARED_ONCHIP_BASE;
		grayReadyCount = *share & MASKGRAY;
		asciiReadyCount = (*share & MASKRASCII)>>6;
		while( grayReadyCount == 3 )
		{
			grayReadyCount = *share & MASKGRAY;
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

		
    	share[0] = sizeX;
		share[1] = sizeY;
		share[2] = *imgP++;
		
		for(i = 0; i < sizeImg ; i++)   // grayscale = (4/16 + 1/16) * imgR + (8/16 + 1/16) * imgG + 1/8 * imgB 
		{
			share[ i + 3 ] =    ((*imgP++)>>2)
    	         	   		 + ((*imgP)>>4) + ((*imgP++)>>1) 
        	 	       		 + ((*imgP++)>>3);
		}

		share = (unsigned char*) SHARED_ONCHIP_BASE;
		asciiReadyCount = (*share & MASKRASCII)>>6;


		/*
	 	 * Store ASCII images into SRAM.
		 * Number of unstored ASCII images decreases.
	 	 */
		if( asciiReadyCount>0)
		{
			storeASCII(asciiImg, number_of_ascii);
			number_of_ascii = (number_of_ascii + 1) % 4;
			share = (unsigned char*) SHARED_ONCHIP_BASE;
			asciiReadyCount = (*share & MASKRASCII)>>6;
			asciiReadyCount--;
			*share = (*share & (~MASKRASCII)) | (asciiReadyCount<<6) ;
			if(DEBUG == 1)
			{
				asciiP =(unsigned char*) asciiImg;
				asciiP += 2;
				printAscii(asciiP);
			}
		}
		
		/* 
 		 *   Number of grayscale images that are waiting for being processed
		 *   will be stored in the first byte of shared onchip memory.
		 * 	 Number of unprocessed grayscale images increases
 		 */

		share = (unsigned char*) SHARED_ONCHIP_BASE;
		grayReadyCount = *share & MASKGRAY;
		grayReadyCount++;
		*share = (*share & (~MASKGRAY)) | grayReadyCount;
		

		
		

		/* Increment the image pointer */
		current_image=(current_image+1) % number_of_images;
		grayFIFOP=(grayFIFOP+1) % 4;
		
		PERF_END(PERFORMANCE_COUNTER_0_BASE, rgbToGrayS);
		if(count==320)
		{
		
		/* Print report */
		perf_print_formatted_report
		(PERFORMANCE_COUNTER_0_BASE,            
		ALT_CPU_FREQ,        // defined in "system.h"
		1,                   // How many sections to print
		"Process Images"		     // Display-name of section(s).
		);
		count = 0;
		}
		/* Just to see that the task compiles correctly */
		IOWR_ALTERA_AVALON_PIO_DATA(LEDS_GREEN_BASE,value++);


		
	}
 
  return 0;
}
