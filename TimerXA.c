// Timer0A.h
// Initialize a timer to generate an interupt
// June 14 2016

#include <stdint.h>
#include "TimerXA.h"
#include <lm4f120h5qr.h>


void TIMER0A_Init( uint32_t preset ){
  /* 11.4 Initialization and Configuration (tm4c123gh6pm.pdf,
     pg. 722) */
  
  // To use a GPTM, the appropriate TIMERn bit must be set in the
  // RCGCTIMER
  SYSCTL->RCGCTIMER |= (1<<0); // see pg. 338, place 1 in bit 0 to
			       // enable TIMER 0
  
  /* 11.4.1 One-Shot/Periodic Timer Mode */
  //1. Ensure the timer is disabled (the TnEN bit in the GPTMCTL
  //register is cleared) before making any changes.
  TIMER0->CTL &= ~(1<<0); // pg. 737
  
  //2. Write the GPTM Configuration Register (GPTMCFG) with a value of
  //0x0000.0000.
  TIMER0->CFG = 0x00000000; // 32 bit timer
  
  //3. Configure the TnMR field in the GPTM Timer n Mode Register
  //(GPTMTnMR):
  TIMER0->TAMR |= (0x2<<0); // periodic mode
  
  //4. Optionally configure the TnSNAPS, TnWOT, TnMTE, and TnCDIR bits
  // in the GPTMTnMR register to select whether to capture the value
  // of the free-running timer at time-out, use an external trigger to
  // start counting, configure an additional trigger or interrupt, and
  // count up or down.
  TIMER0->TAMR &= ~(1<<4); // count down timer
  
  //5. Load the start value into the GPTM Timer n Interval Load
  //Register (GPTMTnILR).
  TIMER0->TAILR = preset; //799999; // preset of timer
  
  //6. If interrupts are required, set the appropriate bits in the
  //GPTM Interrupt Mask Register (GPTMIMR).
  TIMER0->IMR |= (1<<0); // enable interupt, pg. 745
  // NVIC->ISER[0] |= (1<<19); // enable interupt request, pg. 142, ours is #19.
  NVIC_EnableIRQ(TIMER0A_IRQn); // same as line above but uses function in lm4f120h5qr.h

  //7. Set the TnEN bit in the GPTMCTL register to enable the timer and start counting.
  TIMER0->CTL |= (1<<0); // pg. 737
}

void TIMER1A_Init( uint32_t preset ){
  /* 11.4 Initialization and Configuration (tm4c123gh6pm.pdf,
     pg. 722) */
  SYSCTL->RCGCTIMER |= (1<<1); // provide clock to TIMER 1
  
  /* 11.4.1 One-Shot/Periodic Timer Mode */
  TIMER1->CTL &= ~(1<<0); // diable timer to do initialization
  TIMER1->CFG = 0x00000000; // 32 bit timer -> uses TIMER 1A and 1B.
  TIMER1->TAMR |= (0x2<<0); // periodic mode
  TIMER1->TAMR &= ~(1<<4); // count down timer
  TIMER1->TAILR = preset; // preset of timer -> duration depends on clock speed
  TIMER1->IMR |= (1<<0); // enable interupt, pg. 745
  NVIC_EnableIRQ(TIMER1A_IRQn); // see lm4f120h5qr.h for value
  TIMER1->CTL |= (1<<0); // re-enable
}
