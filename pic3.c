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
unsigned char Sekunde,Minute,Stunde;
unsigned char ad_low_byte;
unsigned char ad_high_byte;
unsigned int AD_RESULT;
unsigned char Sekunden_text[20]="Sek\0";
unsigned char Minuten_text[20]="Min\0";
unsigned char Stunden_text[20]="Std\0";
unsigned char Analog_text[20]="Analogwert= \0";
unsigned char leer[]="               ";


void lcd_printer();
void lcd_adc_update();
void lcd_time_update();

void lcd_printer()
{

}

void lcd_adc_update()
{
    


	AD_RESULT = ADRES;
	

	lcd_gotoxy(2,1);
	lcd_printf(Analog_text);
	lcd_int(AD_RESULT);
	lcd_printf (leer);

	ADCON0bits.GO = 1;
	PIR1bits.ADIF = 0;

}


void lcd_time_update()
{
	
    	// Ausgabe an LCD
	
	TMR1H = 0x80;
	TMR1L = 0x00;


	Sekunde++;
	

	if(Sekunde==60)
	{
		Sekunde = 0;
		Minute++;
			
		//Stunde rum
		if(Minute==60)
		{
			Minute = 0;
			Stunde++;
	
			if(Stunde==511)
			{
				Stunde = 0;
			}
		}
		
	}

	lcd_gotoxy(1,1);
	lcd_int(Sekunde);
	lcd_printf(Sekunden_text);
	lcd_int(Minute);
	lcd_printf(Minuten_text);
	lcd_int(Stunde);
	lcd_printf(Stunden_text);
	lcd_printf (leer);



	PIR1bits.TMR1IF = 0;		
}

void init (void)
{
	Sekunde = 0;
	Minute = 0;
	Stunde = 0;
	AD_RESULT = 0;
	ADRES = 0;
	lcd_init();
	lcd_clear();
	



	//WARUM MÃœSSEN KEINE PORTS INITIALISIERT WERDEN ?! 
	
	
	//Initialisieren des A/D-Wandlers und Timer 1
	//AD-Wandler init und gleich starten da es sonstk ein Interrupt geebn wÃ¼rde
   	ADCON0=0x81;
    ADCON1=0x8E;
	
	//TIMER_1 Config
	//-> Aufgebnstellugng von 0000 - FFFF , umsom ehr zÃ¤hlschritte umso feiner einstellbar -> feinste Einstellung
	//1 -> 16Bit 
	//0 -> unimplemented
	//0
	//0	-> SIEHE UNTEN ****
	//1 -> iwo in der Beschreibung stehts drin ....
	//0 -> external Clock
	//1 -> wo angeschlossen ? (RC0, RC1)
	//0 -> kein startwerte sollten daher auch noch nicht starten

	//****
	//TMR1H + L = TMR1 ; 0xFFFF + 1 -> INTERRUPT (1 mal mehr da auf 0 runter gezÃ¤hlt)
 	//32.768 KhZ ~ 30 mÃ¼kro Sekunden -> 32.768 ZÃ¤hlschritte pro Sekunde
	//0xFFFF +1 = 65535 +1 -> Anzahl an ZÃ¤hlschritte bis zum Interrupt
	//Es kommt normalerweise ein overflow bei 65535 + 1 -> Aber soll schon nach 32768 kommen, daher startwert = 65535 + 1 - 32768 = 32768	
	
	T1CON = 0x8F; //<<---- STIMMT DAS ?!
	
	TMR1H = 0x80;
	TMR1L = 0x00;

	//INTCON-REGISTER
	RCONbits.IPEN = 1;
	INTCONbits.GIEH = 1; //<<--WIESO MUSS NICHT MEHR KONFIGUIERT WERDEN?!
	INTCONbits.GIEL = 1;

	//PIE1-Register
	PIE1bits.ADIE = 1; //AD-Wandler darf unterbrechen
	IPR1bits.ADIP = 0; //..aber nur mit einer niedrigen Prio
	
	
	PIE1bits.TMR1IE = 1;
	IPR1bits.TMR1IP = 1;

	PIR1bits.ADIF = 0;
	ADCON0bits.GO = 1;
	PIR1bits.TMR1IF = 0;
	
	

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
	
	lcd_time_update();

}


#pragma code
#pragma interrupt low_prior_InterruptHandler

void low_prior_InterruptHandler(void)
{
	//A/D-Wandler Interrupt-Service Routine
	INTCONbits.GIEH = 0; 
	lcd_adc_update();
	INTCONbits.GIEH = 1; 	
}


void main (void)
{
	init();
	while(1);
}
//
