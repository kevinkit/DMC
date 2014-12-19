// Hochschule Mannheim / Institut fÃ¼r Mikrocomputertechnik und Embedded Systems
//
// Versuch: PIC4  Interrupt    Dateiname: PIC4_Interrupt.c
//
// Beschreibung:
// Es laufen zwei Tasks ab:
// Task 1: Zeituhr mit hoher PrioritÃ¤t: Timer 1 unterbricht jede Sekunde und die Variablen
// Sekunde, Minute und Stunde werden neu berechnet und an der LCD-Anzeige ausgegeben.
// Task 2: Analog-Digital-Wandler mit niederer PrioritÃ¤t: Der AD-Wandler wird ausgelesen
// und an die LCD-Anzeige ausgegeben.
// Format der LCD-Anzeige:
//   1.Zeile: xxSekyyMinzzStd
//   2.Zeile: Analogwert=aaaa
//
// 24.05.2011 (Poh) Configuration Bit Settings, Anpassungen fÃ¼r NEUE_PLATINE, Includes im Projektverzeichnis
//
// Name/Gruppe:
//

#pragma config OSC=HS,WDT=OFF,LVP=OFF  // HS Oszillator, Watchdog Timer disabled, Low Voltage Programming

// Define fÃ¼r LCD des neuen, grÃ¼nen Demo-Boards:
#define NEUE_PLATINE  // Achtung: define vor include! Bei altem braunem Demo-Board auskommentieren!
#include "p18f452.h"
#include "lcd.h"


void high_prior_InterruptHandler (void);
void low_prior_InterruptHandler (void);

/*
	extern void lcd_init_new(void );    //Initialize the LCD module per Ocular specifications
	extern void lcd_clear_new(void);	// Clear LCD display
	extern void lcd_printf_new(unsigned char*);  // Write a constant string to the LCD
	extern void lcd_gotoxy_new(unsigned char row, unsigned char column);
	extern void lcd_byte_new(char num);
	extern void lcd_int_new(int num);
	extern void lcd_putc_new(unsigned char data);
	extern void lcd_printf(unsigned char*);  // Write a constant string to the LCD
*/

unsigned char Sekunden_text[20]="Sek\0";
unsigned char Minuten_text[20]="Min\0";
unsigned char Stunden_text[20]="Std\0";
unsigned char Analog_text[20]="Analogwert= \0";
unsigned char leer[]="               ";

int i = 0;


void init (void)
{

	lcd_init();
	lcd_clear();
	TRISC = 0xFB;
	TRISB = 0xFF;
	PORTCbits.RC2 = 0;
	RCONbits.IPEN = 0;
	INTCONbits.GIE = 1;
	INTCONbits.GIEL = 0; //????? 
	
	INTCON2bits.INTEDG0 = 0;
	//INTCON2bits.INTEDG2 = 1;
	

}

#pragma code high_prior_InterruptVector = 0x08

void high_prior_InterruptVector(void)
{
	_asm
			goto high_prior_InterruptHandler
	_endasm
}


#pragma code low_prior_InterruptVector = 0x18

void low_prior_InterruptVector(void)
{
	_asm
			goto low_prior_InterruptHandler
	_endasm
}


#pragma code
#pragma interrupt high_prior_InterruptHandler

void high_prior_InterruptHandler(void)
{
	// Timer 1 Interrupt-Service Routine

	PORTCbits.RC2 = 1;
	for(i=0; i < 2; i++)
	{
		PORTBbits.RB3 = !PORTBbits.RB3;	
	}

	PORTCbits.RC2 = 0;
}


#pragma code
#pragma interrupt low_prior_InterruptHandler

void low_prior_InterruptHandler(void)
{

}


void main (void)
{
	init();
	while(1);
}

