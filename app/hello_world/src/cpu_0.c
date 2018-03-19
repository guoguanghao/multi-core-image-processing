#include "altera_avalon_performance_counter.h"
#include "system.h"
#include <stdio.h>

#define SECTION_1 1
#define SECTION_2 2
#define SECTION_3 3

int fac5, fac10, fac15;

long factorial(int n)
{
  int c;
  long result = 1;
 
  for (c = 1; c <= n; c++)
    result = result * c;
 
  return result;
}

int main()
{ 
  // printf("Hello from Nios II!\n");

  // /* Reset Performance Counter */
  // PERF_RESET(PERFORMANCE_COUNTER_0_BASE);  

  // /* Start Measuring */
  // PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);

  // /* Section 1 */
  // PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
  // fac5 = factorial(5);
  // PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
 
  // /* Section 2 */
  // PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_2);
  // fac10 = factorial(10);
  // PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_2);

  // /* Section 3 */
  // PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_3);
  // fac15 = factorial(15);
  // PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_3);

  // /* End Measuring */
  // PERF_STOP_MEASURING(PERFORMANCE_COUNTER_0_BASE);

  // /* Print measurement report */
  // perf_print_formatted_report
	//  (PERFORMANCE_COUNTER_0_BASE,            
	//   ALT_CPU_FREQ,                 // defined in "system.h"
	//   3,                            // How many sections to print
	//   "fac5", "fac10", "fac15");    // Display-name of section(s).

  // printf("\n");

  // printf("factorial(5) = %x (hexadecimal)\n",fac5);
  // printf("factorial(10) = %x (hexadecimal)\n",fac10);
  // printf("factorial(15) = %x (hexadecimal)\n",fac15);

  // /* Event loop never exits. */
  // while (1);

    unsigned int *intP;
    unsigned char *charP,arr[5]={1,2,3,4,5},*share,i;
    charP = arr;
    share = (unsigned char*) SHARED_ONCHIP_BASE;
    for(i=0;i<5;i++)
    {
      *share++ = *charP++;
    }
    share = (unsigned char*) SHARED_ONCHIP_BASE;
    printf("share0: %x\n",*share);
	printf("share1: %x\n",share[1]);
	printf("share2: %x\n",share[2]);
	printf("share3: %x\n",share[3]);
	intP = (share);
	printf("intP: %x\n",*intP);
  while(1);

  return 0;
}
