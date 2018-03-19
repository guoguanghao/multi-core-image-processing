/* 
 *   Lab 1   Jiaqi Li  Guanghao Guo
 *   Image_process_single_rtos
 *   Use rtos to implement lab
 *   Data Flow :
 *   rgbToGray()->reisze()->brightCorrect->sobel()->printAscii()
 */


#include <stdio.h>
#include "altera_avalon_performance_counter.h"
#include "includes.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "sys/alt_alarm.h"
#include "system.h"
#include "io.h"
#include "images.h"

#define DEBUG 0

#define HW_TIMER_PERIOD 100 /* 100ms */

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];
OS_STK    task3_stk[TASK_STACKSIZE];
OS_STK    task4_stk[TASK_STACKSIZE];
OS_STK    task5_stk[TASK_STACKSIZE];
OS_STK    StartTask_Stack[TASK_STACKSIZE]; 

/* Definition of Task Priorities */

#define STARTTASK_PRIO      1
#define TASK1_PRIORITY      10
#define TASK2_PRIORITY      11
#define TASK3_PRIORITY      12
#define TASK4_PRIORITY      13
#define TASK5_PRIORITY      14

/* Definition of Task Periods (ms) */
#define TASK1_PERIOD 10000

#define rgbToGrayS 1
#define resizeImgS 2
#define brightCorrectS 3
#define sobelS 4
#define printAsciiS 5


void printAscii();
void rgbToGray();
void resizeImg();
void sobel();
void brightCorrect();

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
	unsigned char* tmpP = &img[3];
	unsigned char* imgP = img;
	int sizeX = *img++, sizeY = *img++,i,j;
	int sizeImg = sizeX * sizeY;
	unsigned char symbols[16] = {32, 46, 59, 42, 115, 116, 73, 40, 106, 117, 86, 119, 98, 82, 103, 64};
	*imgP++ = sizeX ;
	*imgP++ = sizeY ;
	*imgP++ = *img++;

	for(i = sizeImg - 1; i >= 0 ; i--) //impP -> Start of pixels. Transforming grayscale into ASCII numbers.
	{
	*imgP++ = symbols[((*imgP)>>4)];
	}
	imgP = tmpP;
	for(i = sizeY - 1; i >= 0; i--)
	{
		for(j = sizeX - 1; j >= 0 ; j--)
		{
		putchar(*imgP++); // Display
		}
	printf("\n");
	}

}
/* 
 *   Transform a rgb imagie into gray image
 */
void rgbToGray(unsigned char* base)
{
	unsigned char sizeX = *base++;
	unsigned char sizeY = *base++;
	int sizeImg = sizeX * sizeY,i;
	unsigned char* share;
	share = (unsigned char*) SHARED_ONCHIP_BASE;

    *share++ = sizeX;
	*share++ = sizeY;
	*share++ = *base++;

	for(i = sizeImg-1; i >= 0; i--)  // grayscale = (4/16 + 1/16) * imgR + (8/16 + 1/16) * imgG + 1/8 * imgB 
	{
		//*share++ = ((*base++)>>2)
    	//         + ((*base++)>>1) 
    	//         + ((*base++)>>3); // Less accurate but faster
		*share++ = ((*base)>>4) + ((*base++)>>2)
            	 + ((*base)>>4) + ((*base++)>>1) 
        		 + ((*base++)>>3);
}

}

/* 
 *   Adjust the size of an image
 */

void resizeImg(unsigned char* base) {
	unsigned char sizeX = base[0];
	unsigned char sizeY = base[1];
	unsigned char sizeXAfResize = sizeX>>1,sizeYAfResize = sizeY>>1;
	int sizeImg = sizeX * sizeY>>2,i,j;
	base[0] = sizeXAfResize;
	base[1] = sizeYAfResize;
	
	for(i = 0;i<sizeYAfResize;i++) {
		for(j = 0;j<sizeXAfResize;j++) {	//  (x + x_+_1 + x_next_row + x_next_row_+_1) / 4  merging 4 pixels
			int tmp = 2 * i * sizeX;
			base[i * sizeXAfResize + j + 3] = (base[tmp + 2 * j + 3] + base[tmp + 2 * j + 1 + 3]
								  			 + base[tmp + sizeX  + 2 * j+3] + base[tmp + sizeX  + 2 * j + 1 + 3])>>2;
		}
	}	
}

/* 
 *   Adjust the brightness of an image
 */

void brightCorrect(unsigned char* img) {
	int bmax=0,bmin=255,sig=1,i,j;
	int sizeX = img[0],sizeY = img[1];
	img[0]=sizeX;
	img[1]=sizeY;
	for(i = 0;i<sizeX*sizeY;i++)
	{
		
		if(img[i+3]>bmax)
			bmax=img[i+3];
		if(img[i+3]<bmin)
			bmin=img[i+3];
	}
	if(bmax-bmin>127)
		sig = 0;
	while(sig==1)				//   Make error = maximum_brightness - minimum_brightness
	{							//   If error > 127, pixel values will not change
	for(i = 0;i<sizeX*sizeY;i++)	 // If error > 63, pixel values become 2 * (own_value - minimum_brightness)
		{							 // If error > 31, pixel values become 4 * (own_value - minimum_brightness)
			unsigned char tmp,error; //If error > 15, pixel values become 8 * (own_value - minimum_brightness)
			tmp = img[i+3]-bmin;	 //If error < 15, pixel values become 16 * (own_value - minimum_brightness)
			error = bmax-bmin;
			if(error>127)
				sig = 0;//useless
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
}

/* 
 *   Apply sobel algorithm on an image
 */

void sobel (unsigned char* base) {
	unsigned char sizeX = base[0],i,j;
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
		{							//  tmpP↓
			int xm1ym1,xym1,xp1ym1,	// |(x-1,y-1) (x,y-1) (x+1,y-1)|
				xm1y       ,xp1y,	// |(x-1,y)   (x,y)   (x+1,y)  |
				xm1yp1,xyp1,xp1yp1;	// |(x-1,y+1) (x,y+1) (x+1,y+1)|
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

				g = (abs(gx) + abs (gy))>>2;	// 	g = √(gx^2 + gy^2) ≈ |gx| + |gy|
				if(g>255)
				g=255;
				*imgP++ = g;	
				tmpP = tmpP - 2 * sizeX - 1;
		}
		tmpP = tmpP + 2;
	}
}




void sram2sm_p3(unsigned char* base)
{
	int x, y;
	unsigned char* shared;

	// shared = (unsigned char*) SHARED_ONCHIP_BASE;

	int size_x = *base++;
	int size_y = *base++;
	int max_col= *base++;
	*shared++  = size_x;
	*shared++  = size_y;
	*shared++  = max_col;
	printf("The image is: %d x %d!! \n", size_x, size_y);
	for(y = 0; y < size_y; y++)
	for(x = 0; x < size_x; x++)
	{
		*shared++ = *base++; 	// R
		*shared++ = *base++;	// G
		*shared++ = *base++;	// B
	}
}

/*
 * Global variables
 */
int delay; // Delay of HW-timer 

/*
 * ISR for HW Timer
 */
alt_u32 alarm_handler(void* context)
{
  OSTmrSignal(); /* Signals a 'tick' to the SW timers */
  
  return delay;
}

// Semaphores
OS_EVENT *Task1TmrSem;
OS_EVENT *Task2Sem;
OS_EVENT *Task3Sem;
OS_EVENT *Task4Sem;
OS_EVENT *Task5Sem;



void task1(void* pdata)
{
	unsigned char err;
	unsigned char value=0;
	char current_image=0;
	
	#if DEBUG == 1
	/* Sequence of images for testing if the system functions properly */
	char number_of_images=10;
	unsigned char* img_array[10] = {img1_24_24, img1_32_22, img1_32_32, img1_40_28, img1_40_40, 
			img2_24_24, img2_32_22, img2_32_32, img2_40_28, img2_40_40 };
	#else
	/* Sequence of images for measuring performance */
	char number_of_images=3;
	unsigned char* img_array[3] = {img1_32_32, img2_32_32, img3_32_32};
	#endif

	while (1)
	{ 
		/* Extract the x and y dimensions of the picture */
		unsigned char i = *img_array[current_image];
		unsigned char j = *(img_array[current_image]+1);

		PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
		PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, rgbToGrayS);		
		/* Measurement here */

    	rgbToGray(img_array[current_image]);
		PERF_END(PERFORMANCE_COUNTER_0_BASE, rgbToGrayS);
		/* Just to see that the task compiles correctly */
		IOWR_ALTERA_AVALON_PIO_DATA(LEDS_GREEN_BASE,value++);
		OSSemPost(Task2Sem);
		OSSemPend(Task1TmrSem, 0, &err);

		/* Increment the image pointer */
		current_image=(current_image+1) % number_of_images;

	}
}

void task2(void* pdata) {
	unsigned char err;
	unsigned char* share;
	share = (unsigned char*) SHARED_ONCHIP_BASE;
	while(1)
	{
	OSSemPend(Task2Sem, 0, &err);
	PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, resizeImgS);		
		/* Measurement here */	
	resizeImg(share);
	PERF_END(PERFORMANCE_COUNTER_0_BASE, resizeImgS);
	// printf("hello from task2\n");
	
	OSSemPost(Task3Sem);
	}
}
void task3(void* pdata) {
	unsigned char err;
	unsigned char* share;
	share = (unsigned char*) SHARED_ONCHIP_BASE;
	while(1)
	{
		
	OSSemPend(Task3Sem, 0, &err);
	PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, brightCorrectS);		
		/* Measurement here */	
	brightCorrect(share);
	PERF_END(PERFORMANCE_COUNTER_0_BASE, brightCorrectS);
	
	
	OSSemPost(Task4Sem);
	}
}
void task4(void* pdata) {
	unsigned char err;
	unsigned char* share;
	share = (unsigned char*) SHARED_ONCHIP_BASE;
	while(1)
	{
	OSSemPend(Task4Sem, 0, &err);
	PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, sobelS);		
		/* Measurement here */	
	sobel(share);
	PERF_END(PERFORMANCE_COUNTER_0_BASE, sobelS);
	
	
	OSSemPost(Task5Sem);
	}
}
void task5(void* pdata) {
	unsigned char err;
	unsigned char* share;
	
	char buffer[2048];
  	setvbuf(stdout, buffer, _IOFBF, sizeof(buffer)); 
	while(1)
	{
	OSSemPend(Task5Sem, 0, &err);
	PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, printAsciiS);		
		/* Measurement here */
	share = (unsigned char*) SHARED_ONCHIP_BASE;
	unsigned char sizeX = share[0];
	unsigned char sizeY = share[1];
	share = share + sizeX * sizeY + 3;	
	printAscii(share);
	PERF_END(PERFORMANCE_COUNTER_0_BASE, printAsciiS);
	
	  

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
	// OSSemPend(Task5Sem, 0, &err);
	OSSemPost(Task1TmrSem);		
	}
}

void StartTask(void* pdata)
{
  unsigned char err;
  void* context;

  static alt_alarm alarm;     /* Is needed for timer ISR function */
  
  /* Base resolution for SW timer : HW_TIMER_PERIOD ms */
  delay = alt_ticks_per_second() * HW_TIMER_PERIOD / 1000; 
  printf("delay in ticks %d\n", delay);

  /* 
   * Create Hardware Timer with a period of 'delay' 
   */
  if (alt_alarm_start (&alarm,
      delay,
      alarm_handler,
      context) < 0)
      {
          printf("No system clock available!n");
      }



   /*
   * Creation of Kernel Objects
   */

  Task1TmrSem = OSSemCreate(1); // 5 semaphores for communication between tasks
  Task2Sem = OSSemCreate(0);	// Task1Sem is initilized as 1 so it can always be the start
  Task3Sem = OSSemCreate(0);
  Task4Sem = OSSemCreate(0);
  Task5Sem = OSSemCreate(0);  

  /*
   * Create statistics task
   */

  OSStatInit();

  /* 
   * Creating Tasks in the system 
   */

  err=OSTaskCreateExt(task1,
                  NULL,
                  (void *)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("Task1 created\n");
    }
   }  
   err=OSTaskCreateExt(task2,
                  NULL,
                  (void *)&task2_stk[TASK_STACKSIZE-1],
                  TASK2_PRIORITY,
                  TASK2_PRIORITY,
                  task2_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("Task2 created\n");
    }
   }  
   err=OSTaskCreateExt(task3,
                  NULL,
                  (void *)&task3_stk[TASK_STACKSIZE-1],
                  TASK3_PRIORITY,
                  TASK3_PRIORITY,
                  task3_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("Task3 created\n");
    }
   }  
   err=OSTaskCreateExt(task4,
                  NULL,
                  (void *)&task4_stk[TASK_STACKSIZE-1],
                  TASK4_PRIORITY,
                  TASK4_PRIORITY,
                  task4_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("Task4 created\n");
    }
   }  
   err=OSTaskCreateExt(task5,
                  NULL,
                  (void *)&task5_stk[TASK_STACKSIZE-1],
                  TASK5_PRIORITY,
                  TASK5_PRIORITY,
                  task5_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("Task5 created\n");
    }
   }  

  printf("All Tasks and Kernel Objects generated!\n");

  /* Task deletes itself */

  OSTaskDel(OS_PRIO_SELF);
}


int main(void) {

  printf("MicroC/OS-II-Vesion: %1.2f\n", (double) OSVersion()/100.0);
  
  

  OSTaskCreateExt(
	 StartTask, // Pointer to task code
         NULL,      // Pointer to argument that is
                    // passed to task
         (void *)&StartTask_Stack[TASK_STACKSIZE-1], // Pointer to top
						     // of task stack 
         STARTTASK_PRIO,
         STARTTASK_PRIO,
         (void *)&StartTask_Stack[0],
         TASK_STACKSIZE,
         (void *) 0,  
         OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
         
  OSStart();
  
  return 0;
}
