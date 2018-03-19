
#include<stdio.h>
#include "images_alt.h"
#include "images.h"
// #include <math.h>
void printAscii();
void rgbToGray();
void resizeImg();
void sobel();
void brightCorrect();

#define DEBUG 1
#define PERFORMANCE 1

unsigned int sqrt_16(unsigned long M)  { 
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

// float sqrt_(float x)         
// {
//     float g=x;
//     while(abs(g*g-x)>0.0001)
//     {
//         g=(g+x/g)/2;
//     }
//     return g;
// }

// float Tsqrt(float x)//计算[0,2)范围内数的平方根
// {
//     float sum,coffe,factorial,xpower,term;
//     int i;
//     sum=0;
//     coffe=1;
//     factorial=1;
//     xpower=1;
//     term=1;
//     i=0;
//     while(abs(term)>0.000001)
//     {
//         sum+=term;
//         coffe*=(0.5-i);
//         factorial*=(i+1);
//         xpower*=(x-1);
//         term=coffe*xpower/factorial;
//         i++;
//     }
//     return sum;
    
// }

// double sqrt2(double x)//大于2的数要转化为[0,2)区间上去
// {
//     double correction=1;
//     while(x>=2)
//     {
//         x/=4;
//         correction*=2;
//     }
//     return Tsqrt(x)*correction;
// }

// float SquareRootFloat(float number) {
//     long i;
//     float x, y;
//     const float f = 1.5F;
//     x = number * 0.5F;
//     y = number;
//     i = * ( long * ) &y;
//     i = 0x5f3759df - ( i >> 1 ); //注意这一行 
//     y = * ( float * ) &i;
//     y = y* ( f - ( x * y * y ) );
//     y = y * (f - ( x * y * y ));               
//     return number * y;
// }

int main(void) {
	#if DEBUG == 1
	unsigned char* img_array[4] = {bar1,bar2,bar3,bar4 };
			
	// unsigned char* img_array[3] = {rectangle32x32, circle32x32, rectangle32x32};	
	#else if PERFORMANCE == 1
	// unsigned char* img_array[3] = {rectangle32x32, circle32x32, rectangle32x32};	
	unsigned char* img_array[3] = {img1_32_32, img2_32_32, img3_32_32};	
	#endif
	unsigned char symbols[16] = {32, 46, 59, 42, 115, 116, 73, 40, 106, 117, 86, 119, 98, 82, 103, 64};
	char buffer[8192];

	setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));
	// unsigned char test;
	// test = test - 1;
	// printf("size of unsigned char is %d \n", test);
	rgbToGray(img_array[0]);
	rgbToGray(img_array[1]); 
	rgbToGray(img_array[2]);
	rgbToGray(img_array[3]);
	
	// printf("%d", abs(-2));
	return 0;
}

void printAscii(unsigned char* img)
{
	unsigned char* tmpP = &img[3];
	unsigned char* imgP = img;
	// printf("%x ", tmpP);
	int sizeX = *img++, sizeY = *img++,i,j;
	int sizeImg = sizeX * sizeY;
	// unsigned char imgOut[sizeImg+3];
	// unsigned char* imgP = &imgOut[0];
	// tmpP = &imgOut[3];
	unsigned char symbols[16] = {32, 46, 59, 42, 115, 116, 73, 40, 106, 117, 86, 119, 98, 82, 103, 64};
	// float fl = 0.01;
	*imgP++ = sizeX ;
	*imgP++ = sizeY ;
	*imgP++ = *img++;
	// printf("%x \n", tmpP);
	// *img++;

	// printf("ascii start\n");

	// for(int i = 0; i < sizeImg; i++)
	// {
	// imgOut[i+3] = symbols[(int)((img[i+3])/16)];
	// }
	// // printf("sizeY = %d \n",imgOut[0]);

	// for(int i = 0; i < sizeY; i++)
	// 	{
	// 	for(int j=0; j< sizeX;j++)
	// 		{
	// 		printf("%c ",imgOut[j+(i*sizeX)+3]);
	// 		}
	// 	printf("\n");
	// 	}
	// 	printf("%f\n", fl);

for(i = sizeImg - 1; i >= 0 ; i--)
{
	*imgP++ = symbols[((*imgP)>>4)];
}
imgP = tmpP;
// img = tmpP;
for(i = sizeY - 1; i >= 0; i--)
	{
	for(j = sizeX - 1; j >= 0 ; j--)
		{
		// printf("%c ",*imgP++);
		putchar(*imgP++);
		// cputs(*imgP++);
		// putchar(symbols[0]);
		// tmp = imgOut[j+(i*sizeX)+3];
		}
	printf("\n");
	}
}



void rgbToGray(unsigned char* base)
{
int sizeX = *base++;
int sizeY = *base++;
int sizeImg = sizeX * sizeY;
unsigned char grayscale_img[ sizeX * sizeY + 3 ];
unsigned char* imgP = grayscale_img;

    *imgP++ = sizeX;
	*imgP++ = sizeY;
	*imgP++ = *base++;

for(int i = 0; i < sizeImg; i++)
{
	*imgP++ =    ((*base)>>4)+ ((*base++)>>2) +
                 ((*base)>>4) + ((*base++)>>1)  
                + ((*base++)>>3);
}
imgP = grayscale_img;
// for(int i=0;i<sizeY;i++) {
// 		for(int j=0;j<sizeX;j++) {
// 			printf("%d	",grayscale_img[ i * sizeX + j + 3 ]);
// 		}
// 		printf("\n");
// 	}


// printf("grayscale done\n");
// printAscii(imgP);
resizeImg(imgP);
// sobel(imgP);
// brightCorrect(imgP);

}


void resizeImg(unsigned char* base) {
	int sizeX = base[0];
	int sizeY = base[1];
	int sizeXAfResize = sizeX>>1,sizeYAfResize = sizeY>>1;
	int sizeImg = sizeX * sizeY>>2;
	unsigned char impImg[sizeImg+3];
	impImg[0] = sizeXAfResize;
	impImg[1] = sizeYAfResize;
	impImg[2] = base[2] ;
	// printf("sizex after resize %d ",impImg[0]);
	// printf("sizey after resize %d \n",impImg[1]);
	
	for(int i=0;i<sizeYAfResize;i++) {
		for(int j=0;j<sizeXAfResize;j++) {
			impImg[i * sizeXAfResize + j + 3] = (base[2 * i * sizeX + 2 * j + 3] + base[2 * i * sizeX + 2 * j + 1 + 3]
								  + base[(2 * i + 1) * sizeX  + 2 * j+3] + base[(2 * i + 1) * sizeX  + 2 * j + 1 + 3])/4;
			// printf("%d ", base[2 * i * sizeX + 2 * j + 3]);
		}
		// printf("\n");
	}
	// for(int i=0;i<sizeYAfResize;i++) {
	// 	for(int j=0;j<sizeXAfResize;j++) {
	// 		printf("%d	", impImg[ i * sizeXAfResize + j + 3 ]);
	// 	}
	// 	printf("\n");
	// }
	
	// printf("resieze done\n");
	// printAscii(impImg);
	// sobel(impImg);
	brightCorrect(impImg);
}

void brightCorrect(unsigned char* img) {
	unsigned char bmax=0,bmin=255,sig=1;
	unsigned char sizeX = img[0],sizeY = img[1];
	unsigned char imgBri[sizeX*sizeY+3];
	imgBri[0]=sizeX;
	imgBri[1]=sizeY;
	imgBri[2]=img[2];
	for(int i=0;i<sizeY*sizeX;i++) {
			imgBri[i+3] = img[i+3];
		}
	for(int i=0;i<sizeX*sizeY;i++)
	{
		if(imgBri[i+3]>bmax)
			bmax=imgBri[i+3];
		if(imgBri[i+3]<bmin)
			bmin=imgBri[i+3];
	}
	// printf("bmax = %d , bmin = %d \n",bmax,bmin);
	while(sig==1)
	{
	for(int i=0;i<sizeX*sizeY;i++)
		{
			unsigned char tmp;
			tmp = imgBri[i+3]-bmin;
			if(bmax-bmin>127)
				// tmp = imgBri[i+3];
				imgBri[i+3]=imgBri[i+3];//useless
				else if(bmax-bmin>63)
					imgBri[i+3] = tmp * 2;
					else if(bmax-bmin>31)
						imgBri[i+3] = tmp * 4;
						else if(bmax-bmin>15)
						imgBri[i+3] = tmp * 8;
						else
						imgBri[i+3] = tmp * 16;
		}
		for(int i=0;i<sizeX*sizeY;i++)
		{
		if(imgBri[i+3]>bmax)
			bmax=imgBri[i+3];
		if(imgBri[i+3]<bmin)
			bmin=imgBri[i+3];
		}
		// printf("bmax = %d , bmin = %d \n",bmax,bmin);
	if(bmax-bmin>127)
		sig = 0;
	}
	// for(int i=0;i<sizeY;i++) {
	// 	for(int j=0;j<sizeY;j++) {
	// 		printf("%d	", imgBri[i+3]);
	// 	}
	// 	printf("\n");
	// }
	// printAscii(imgBri);
	sobel(imgBri);
}

void sobel (unsigned char* base) {
	unsigned char sizeX = base[0];
	unsigned char sizeY = base[1];
	unsigned char sizeXAfSobel = sizeX - 2,sizeYAfSobel = sizeY - 2;
	unsigned char imgSobel[sizeXAfSobel*sizeYAfSobel+3];
	unsigned char* imgP = imgSobel;
	// imgSobel[0] = sizeXAfSobel;
	// imgSobel[1] = sizeYAfSobel;
	// imgSobel[2] = base[2];
	*imgP++ = sizeXAfSobel;
	*imgP++ = sizeYAfSobel;
	*imgP++ = base[2];
	unsigned char* tmpP = &base[3];
	for(int i = 0 ; i < sizeYAfSobel ; i++)
	{	
		for(int j = 0 ; j < sizeXAfSobel ; j++)
		{
			
			int xm1ym1,xym1,xp1ym1,
				xm1y       ,xp1y,
				xm1yp1,xyp1,xp1yp1;
			int gx,gy,g1;
			unsigned char g;
			

				xm1ym1=*tmpP++;
				xym1= *tmpP++;
				xp1ym1= *tmpP;
				tmpP = tmpP + sizeX - 2;
				xm1y= *tmpP++;
				*tmpP++;
				// xy= base[i*sizeX+j]; //pixel in the middle
				xp1y= *tmpP;
				tmpP = tmpP + sizeX - 2;
				xm1yp1= *tmpP++;
				xyp1= *tmpP++;
				xp1yp1= *tmpP;

				gx = xm1ym1 + (xm1y>>1) + xm1yp1 - (xp1yp1 + (xp1y>>1) + xp1ym1);
				gy = xp1ym1 + (xym1>>1) + xm1ym1 - (xp1yp1 + (xyp1>>1) + xm1yp1);
				//problem

				// g  = sqrt_16( gx * gx + gy * gy );
				g = (abs(gx) + abs(gy))>>2;
				// g =  sqrt( gx * gx + gy * gy );
				if(g>255)
				g=255;
				// g = sqrt_( gx * gx + gy * gy );
				// g  = SquareRootFloat( gx * gx + gy * gy );
				// printf("%u ",g);
				// printf("%u ",g);
				*imgP++ = g;	
				tmpP = tmpP - 2 * sizeX - 1;
		}
		tmpP = tmpP + 2;
	}
	printf("\n");
	
	imgP = imgSobel;
	// resizeImg(imgSobel);
	printAscii(imgP);
}

