#include <MKL25Z4.h>
#include "Functions.h"


int initialize(void);	/*initializes switch interrupt for PTA16 and timer interrupt for LCD*/
void PORTA_IRQHandler(void);
void SysTick_Handler(void);
int ADC_Func(void);

void Saw_Tooth_Wave(void);
void Square_Wave(void);
void Rectified_Sine_Wave(void);

int print_num(int x);

volatile int wavetype = 0;  /*global variable for the wavetypes*/
int ADC_result; /*global variable for adc output to be used in both in functions and systick interrupt*/
volatile int wavetypeUpdated = 0;

int main(void){
	
	LCD_init();
	LCD_command(0x01);
	initialize();
	/*
	while(1){
		
	if (wavetype == 0){
		
		while(wavetype == 0){
		Delay(300000);
		}
	}
	else if(wavetype == 1){
		Square_Wave();
	}
	else if(wavetype == 2){
		Square_Wave();
		}
	else if(wavetype == 3){
		Saw_Tooth_Wave();
		
	 }*/
	 
	 while (1) {
        /* Check if the wavetype has been updated*/
        if (wavetypeUpdated) {
            wavetypeUpdated = 0;  // Reset the flag
            
            // Handle the different waveform types
            if (wavetype == 0) {
                while(wavetype == 0){
									Delay(300000);
								}
            } else if (wavetype == 1) {
								Square_Wave();
								}
            } else if (wavetype == 2) {
                Saw_Tooth_Wave();
            } else if (wavetype == 3) {
								Rectified_Sine_Wave();
							
        }
    }

}
int initialize(void){
	
	 __disable_irq();           /* disable all IRQs */
	
	SIM->SCGC5 |= 0x400;  /*enable port B for output*/
	PORTB->PCR[1] = 0X100; /* make PTB1 GPIO*/
	PORTB->PCR[2] = 0X100; /* make PTB2 GPIO*/
	PORTB->PCR[3] = 0X100; /* make PTB3 GPIO*/
	PTB->PDDR |= 0x0E ;/*make PTB1,2,3 outputs*/
	
	SIM->SCGC5 |= 0x200; /*enable clock A*/
	PORTA->PCR[16] |= 0x100; /*make PTA16 GPIO*/
	PORTA->PCR[16] |= 0x002; /*enable pull down for PTA16*/ 
	PTA->PDDR &= ~0x10000;	/*make PTA16 input*/
	
	PORTA->PCR[16] &= ~0xF0000; /* clear interrupt selection */
	PORTA->PCR[16] |= 0xA0000; /*interrupt at rising edge*/
	
	SysTick->LOAD = (41940000/16) - 1; /*to make 2 second delay*/
	SysTick->CTRL = 3;			/*clock is divided by 16 make such a long delay*/
	
	NVIC->ISER[0] |= 0x40000000;	 /* enable INT30 (bit 30 of ISER[0]) */
	 __enable_irq();             /* global enable IRQs */
	
	/*------ADC CONFIGURATION-----------------------------------------*/
	SIM->SCGC5 |= 0x2000; /*PTE20 will be used as ADC input*/
	PORTE->PCR[20] = 0;  /*Pin take analog input*/
	
	SIM->SCGC6 |= 0x8000000; /*ADC0 channel will be used.*/
	ADC0->SC2 &= ~0x40; /*software triggering is selected*/
	/*clock for adc is divided by 4, long sample time, 12-bit, bus clock*/
	ADC0->CFG1 = 0x40 | 0x10 | 0x04 | 0x00;
	
	/*------DAC CONFIGURATION------------------------------------------*/
	SIM->SCGC6 |= 0x80000000; /*Clocking DAC*/
	DAC0->C1 = 0;   /*Disabling buffers*/
	DAC0->C0 = 0x80 | 0x20; /*enable DAC and software trigger*/
	
	return 0;
}
void PORTA_IRQHandler(void){
	/*
	wavetype++;
	
	if (wavetype == 1){
		LCD_command(0x01);
		print_fnc("SQUARE");
		
	}
	else if(wavetype == 2){
		LCD_command(0x01);
		print_fnc("Saw Tooth");*/
	/*	Saw_Tooth_Wave(); *//*
	}
	else if(wavetype == 3){
		LCD_command(0x01);
		print_fnc("Rectified Sine");
	}
	else{
		wavetype = 0;
	}

	PORTA->ISFR |= 00010000; *//*Clear Interrupt Flag*/
	
	__disable_irq();
    
    wavetype++;
    
    if (wavetype > 3) {
        wavetype = 0;
    }
    
    /* Set the flag to indicate wavetype update*/
    wavetypeUpdated = 1;
    
    /* Clear the interrupt flag*/
    PORTA->ISFR |= 0x00010000;
    
    /*Re-enable interrupts*/
    __enable_irq();
	
}

void SysTick_Handler(void){
	
	/*int ADC_result;*/
	int V_result;
	/*
	static int i;
	i++;
	
	if (i % 2 == 0){
		LCD_command(0x01);
		print_fnc("Current F:360Hz");
	}
	else{
		LCD_command(0x01);
		print_fnc("Current F:50Hz");*
	}*/
	/*ADC_result = ADC_Func();*/
	
	V_result = ((ADC_result + 1)*3.3)*1000/4096;
		
		LCD_command(0x01);
		print_fnc("Currrent F:360Hz");
		LCD_command(0xC0);
		print_fnc("Currrent V:");
		print_num(V_result);

	
}

int ADC_Func(void){
	
	int ADC_code;
	/*int V_result;*/
		
		ADC0->SC1[0] = 0; /*Starting conversion*/
		while(!(ADC0->SC1[0] & 0x80)){} /*wait for conversion to complete*/
		ADC_code = ADC0->R[0];
			
	/*V_result = ((ADC_code + 1)*3.3)*1000/4096;
		
		LCD_command(0x01);
		print_fnc("Currrent F:360Hz");
		LCD_command(0xC0);
		print_fnc("Currrent V:");
		print_num(V_result);*/
		
		return ADC_code; 
}

int print_num(int x){
	
	int num_array[4];
	int i;
	char char_array[4];
	
	for (i = 0; i < 4; i++){
	 num_array[i] = x % 10;
	 char_array[i] = num_array[i] + 0x30;
		x = x / 10;	
	}
	
	LCD_data(char_array[3]);
	LCD_data('.');
	LCD_data(char_array[2]);
	LCD_data(char_array[1]);
	LCD_data(char_array[0]);
}

void Saw_Tooth_Wave(void){
	int i;
	/*int ADC_result;*/
	float ratio;
	int value;
	/*
	ADC_result_saw = ADC_Func();
	ratio = ADC_result_saw / 4096.0;
	value = i*ratio;*/
	
	while(wavetype == 2){
		
		for(i = 0; i < 0x1000; i+= 0x0100){
			
			ADC_result = ADC_Func();
			ratio = ADC_result / 4096.0;
			value = i*ratio;
			
			DAC0->DAT[0].DATL = (value) & 0xFF;
			DAC0->DAT[0].DATH = (value >> 8) & 0x0F;
			Delay(215); /*generating 360Hz*/
		}
	}
	
}

void Square_Wave(void){
	int i;
	float ratio;
	int value1,value2;

	while(wavetype == 1){
		for (i = 0; i < 4096; i+= 256){
			
			ADC_result = ADC_Func();
			ratio = ADC_result / 4096.0;
			value1 = 4096.0*ratio;
			value2 = 0;
				
			if (i < 2048){
				
			DAC0->DAT[0].DATL = (value1) & 0xFF;
			DAC0->DAT[0].DATH = (value1 >> 8) & 0x0F;
			Delay(195);
				
			}
			else if(i >= 2048){
				
			DAC0->DAT[0].DATL = (value2) & 0xFF;
			DAC0->DAT[0].DATH = (value2 >> 8) & 0x0F;
			Delay(195);
				
			}
		}
	}
}

void Rectified_Sine_Wave(void){
	
	int i;
	float ratio;
	int value;

	
	const static int sine_array[] = {0, 799, 1567, 2275, 2896, 3405, 3784,\
		4017, 4096, 4017, 3784, 3405, 2896, 2275, 1567, 799, 0, 0, 0, 0, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


	while(wavetype == 3){
		for (i = 0; i < 32; i+=1){
			
			ADC_result = ADC_Func();
			ratio = ADC_result / 4096.0;
			
			value = sine_array[i]*ratio;
			
			if( i < 16){
			DAC0->DAT[0].DATL = (value) & 0xFF;
			DAC0->DAT[0].DATH = (value >> 8) & 0x0F;
			Delay(50);
			}
			else if (i >= 16){
			DAC0->DAT[0].DATL = (0) & 0xFF;
			DAC0->DAT[0].DATH = (0 >> 8) & 0x0F;
			Delay(50);
			}
		}
	}
}
