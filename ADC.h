// ADC.h
// Runs on TM4C123
// Setup the ADC to read the signal on pin E1 using sample sequence 3
// June 14 2016

// Initialize ADC1
void ADC1_Init( void );

// Initialize ADC0
void ADC0_Init( void );

// Initialize the sample sequence
void SS_Init1( void );

// Initialize the sample sequence
void SS_Init0( void );

// Read ADC1
uint32_t ADC1_Result( void );

// Read ADC0
uint32_t ADC0_Result( void );
