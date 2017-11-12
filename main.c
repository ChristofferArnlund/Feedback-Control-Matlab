#include <lm4f120h5qr.h>
#include "ADC.h"
#include "PLL.h"
#include "PWM.h"
#include "TimerXA.h"

#define FLAG_NONE 0x00000000
#define FLAG_READ_ADC 0x00000001
#define PWM_PERIOD 1000
#define ADC_PERIOD 160000
     
/* Results of discretization. Enter your discretized controller gains
   here */
#define b1 0.0f
#define b0 0.0f
#define a1 0.0f
#define a0 0.0f

volatile uint8_t flags = FLAG_NONE;
volatile static uint32_t adc1Result = 0;
volatile static uint32_t adc0Result = 0;

static float controlSignal[] = {0,0};
static float errorSignal[] = {0,0};
static float referenceSignal = 0;
static float outputSignal = 0;
static float dutycycle = 0;

void Timer0A_Handler( void ); // handle interupt from timer


int main(){
  /* Initializations */
  PLL_Init(Bus16MHz);                  // Set processor clock speed

  ADC1_Init();                         // ADC1 on pin PE1 (0 to 3.3.V)
				       // - feedback

  SS_Init1();                          // software triggered sampling

  ADC0_Init();                         // ADC0 on pin PE4 (0 to 3.3.V)
				       // - reference

  SS_Init0();                          // software triggered sampling
  
  TIMER0A_Init(ADC_PERIOD-1);            // Timer initiates ADC read at
				       // 100Hz

  PWM0A_Init(PWM_PERIOD, 0);         // initialize PWMA, 0% duty - Pin
				     // PB6
  
  PWM0B_Init(PWM_PERIOD, 0);         // initialize PWMB, 0% duty - Pin
				     // PB7
  
  /* Main control loop */
  while(1){
    if(flags == FLAG_READ_ADC){

      adc1Result = ADC1_Result(); // read feedback
      outputSignal = 3.3 * adc1Result / 4095; // in volts
      
      adc0Result = ADC0_Result(); // read reference
      referenceSignal = 3.3 * adc0Result / 4095; // in volts

      
      //update error signals
      errorSignal[1] = errorSignal[0];
      errorSignal[0] = referenceSignal - outputSignal;

      //update control signal
      controlSignal[1] = controlSignal[0];
      controlSignal[0] = (b0*errorSignal[0] + b1*errorSignal[1] - a1*controlSignal[1])/a0;

      // convert control signal (+/- 3.3) to PWM duty cycle & direction
      dutycycle = controlSignal[0]/3.3;
      
      // the following code is not the most efficient
      if(dutycycle > 0){
        PWM0B_Duty(0);
	if(dutycycle >= 1){
	  PWM0A_Duty(PWM_PERIOD - 1); // +ve saturation
	}else{
	  PWM0A_Duty((uint16_t)(PWM_PERIOD*dutycycle)); // WARNING :
							// converting
							// float to
							// int may
							// cause
							// overflow
      }
      }else{
        PWM0A_Duty(0);
	if(dutycycle <= -1){
	  PWM0B_Duty(PWM_PERIOD - 1); // -ve saturation
	}else{
	  PWM0B_Duty((uint16_t)(-PWM_PERIOD*dutycycle)); // WARNING :
							 // converting
							 // float to
							 // int may
							 // cause
							 // overflow
	}
    }
    }
      
      // Clear ADC conversion flag
      flags = FLAG_NONE;
  }
  return 0;
}

void TIMER0A_Handler( void ){
  flags =  FLAG_READ_ADC;
  TIMER0->ICR |= (1<<0); // reset timer, pg. 754
}
