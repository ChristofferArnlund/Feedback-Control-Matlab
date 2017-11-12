// ADC.h
// Runs on TM4C123

// Setup the ADC to read the signal on pins E1 and E4 using sample
// sequence 3. The ADC is triggered by a periodic timer. The ADC has
// 12 bit resolution so the result of an A-to-D conversion is a an
// integer between 0 and 2^(12) - 1 which corresponds to a voltage
// between 0 and 3.3.

// June 14 2016

#include <stdint.h>
#include "ADC.h"
#include <lm4f120h5qr.h>


/* -----------------------

ADC 1 SECTION

-------------------------*/

void ADC1_Init( void ){

  /* 13.4 Initialization and Configuration (tm4c123gh6pm.pdf,
       pg. 817)*/
  
  // In order for the ADC module to be used, the PLL must be enabled
  // and programmed to a supported crystal frequency in the RCC
  // register 

  // The default frequency 16MHz is supported - no action required

  /* 13.4.1 Module Initialization */

  // 1. Enable the ADC clock using the RCGCADC register (pg. 352)
  SYSCTL->RCGCADC = (1<<1); // ADC1

  // 2. Enable the clock to the appropriate GPIO modules via the
  //RCGCGPIO register (see page 340). To find out which GPIO ports to
  //enable, refer to “Signal Description” on page 801.
  SYSCTL->RCGCGPIO = (1<<4); // 0b010000, enable port E (ADC input)
  while((SYSCTL->PRGPIO & 0x10) != 0x10){};
  GPIOE->DIR &= ~(1<<1); // Set pin E1 as in input
    
  // 3. Set the GPIO AFSEL bits for the ADC input pins (see page
  // 671). To determine which GPIOs to configure, see Table 23-4 on
  // page 1344.
  GPIOE->AFSEL = (1<<1); // use PE1 as AIN2
    
  // 4. Configure the AINx signals to be analog inputs by clearing the
  //corresponding DEN bit in the GPIO Digital Enable (GPIODEN)
  //register (see page 682).
  GPIOE->DEN &= ~(1<<1); //Disable digial functions on PE1
  
  // 5. Disable the analog isolation circuit for all ADC input pins
  //that are to be used by writing a 1 to the appropriate bits of the
  //GPIOAMSEL register (see page 687) in the associated GPIO block.
  GPIOE->AMSEL = (1<<1); // PE1
  while((SYSCTL->PRADC & 0x00000002) != 0x00000002){};
  
  // 6. If required by the application, reconfigure the sample
  //sequencer priorities in the ADCSSPRI register. The default
  //configuration has Sample Sequencer 0 with the highest priority and
  //Sample Sequencer 3 as the lowest priority.

  ADC1->SSPRI = 0x0123;  // 8) Sequencer 3 is highest priority
  
}

void SS_Init1( void ){
  /* 13.4.2 Sample Sequencer Configuration */

  // 1. Ensure that the sample sequencer is disabled by clearing the
  // corresponding ASENn bit in the ADCACTSS register. Programming of
  // the sample sequencers is allowed without having them
  // enabled. Disabling the sequencer during programming prevents
  // erroneous execution if a trigger event were to occur during the
  // configuration process.
  ADC1->ACTSS &= ~(1<<3); // Sample sequencer 3 is disabled
  
  // 2. Configure the trigger event for the sample sequencer in the
  // ADCEMUX register.
  //ADC1->EMUX = (0xF<<12); // pg. 834, trigger event = continuous sampling
  ADC1->EMUX = ~0xF000; // pg. 834, software triggered
  
  // 3. When using a PWM generator as the trigger source, use the ADC
  // Trigger Source Select (ADCTSSEL) register to specify in which PWM
  // module the generator is located. The default register reset
  // selects PWM module 0 for all generators.
  
  // 4. For each sample in the sample sequence, configure the
  // corresponding input source in the ADCSSMUXn register.
  ADC1->SSMUX3 = 2; // use PE1 as input pin -> ANI2 -> 2, pg. 801
  
  // 5. For each sample in the sample sequence, configure the sample
  // control bits in the corresponding nibble in the ADCSSCTLn
  // register. When programming the last nibble, ensure that the END
  // bit is set. Failure to set the END bit causes unpredictable
  // behavior.
  ADC1->SSCTL3 = 0x6; // 0b00000110, pg. 876 
  
  // 6. If interrupts are to be used, set the corresponding MASK bit
  // in the ADCIM register.
  ADC1->IM = (0<<3); // A sample has completed conversion and the
		     // respective ADCSSCTL3 IEn bit is set, enabling
		     // a raw interupt - disable
  
  // 7. Enable the sample sequencer logic by setting the corresponding
  // ASENn bit in the ADCACTSS
  ADC1->ACTSS |= (1<<3); // enable sequencer

}

//------------ADC1_Result------------
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC1_Result(void){
  uint32_t result;
  ADC1->PSSI = 0x0008;            // 1) initiate SS3
  while((ADC1->RIS & 0x08)==0){}; // 2) wait for conversion done
  result = ADC1->SSFIFO3; // read ADC value
  ADC1->ISC = (1<<3); // clear interupt flag
  return result;
}


/* -----------------------

ADC 0 SECTION

-------------------------*/

void ADC0_Init( void ){

  /* 13.4.1 Module Initialization */

  // 1. Enable the ADC clock using the RCGCADC register (pg. 352)
  SYSCTL->RCGCADC |= (1<<0); // ADC0

  // 2. Enable the clock to the appropriate GPIO module
  SYSCTL->RCGCGPIO = (1<<4); // 0b010000, enable port E (ADC input)
  while((SYSCTL->PRGPIO & 0x10) != 0x10){};
  GPIOE->DIR &= ~(1<<4); // Set pin E4 as in input
    
  // 3. Set the GPIO AFSEL bits for the ADC input pins (see page
  // 671). To determine which GPIOs to configure, see Table 23-4 on
  // page 1344.
  GPIOE->AFSEL |= (1<<4); // use PE4 as AIN
    
  // 4. Configure the AINx signals to be analog inputs by clearing the
  //corresponding DEN bit in the GPIO Digital Enable (GPIODEN)
  //register (see page 682).
  GPIOE->DEN &= ~(1<<4); //Disable digial functions on PE4
  
  // 5. Disable the analog isolation circuit for all ADC input pins
  //that are to be used by writing a 1 to the appropriate bits of the
  //GPIOAMSEL register (see page 687) in the associated GPIO block.
  GPIOE->AMSEL |= (1<<4); // PE4
  while((SYSCTL->PRADC & 0x01) != 0x01){};
  
  // 6. If required by the application, reconfigure the sample
  //sequencer priorities in the ADCSSPRI register. The default
  //configuration has Sample Sequencer 0 with the highest priority and
  //Sample Sequencer 3 as the lowest priority.
  ADC0->SSPRI = 0x0123;  // 8) Sequencer 3 is highest priority
  
}

void SS_Init0( void ){

  ADC0->ACTSS &= ~(1<<3);
  ADC0->EMUX = ~0xF000;
  ADC0->SSMUX3 = 9;
  ADC0->SSCTL3 = 0x6;
  ADC0->IM = (0<<3);
  ADC0->ACTSS |= (1<<3);
}

//------------ADC0_Result------------
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC0_Result(void){
  uint32_t result;
  ADC0->PSSI = 0x0008;            // 1) initiate SS3
  while((ADC0->RIS & 0x08)==0){}; // 2) wait for conversion done
  result = ADC0->SSFIFO3; // read ADC value
  ADC0->ISC = (1<<3); // clear interupt flag
  return result;
}
