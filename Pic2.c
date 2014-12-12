// Hochschule Mannheim / Institut für Mikrocomputertechnik und Embedded Systems
//
// Versuch: PIC2 DA-Wandler durch PWM Dateiname: PIC2_PWM.c
//
// Eine am Analogeingang RA0/AN0 vorgegebene Spannung wird digitalisiert,
// der Wert AnalogIn=xxxx am LCD angezeigt
// und über eine Pulsweitenmodulation am Ausgang RC2/CCP1 ausgegeben.
// Das dort angeschlossene RC-Glied macht daraus wieder eine Analogspannung,
// die am Eingang RE2/AN7 eingelesen und als Istwert AnalogOut=yyyy angezeigt wird.
//
// 08.12.2011 (Poh) Kommentare für LCD (Prototypen in lcd.h)
// 24.05.2011 (Poh) Configuration Bit Settings, Anpassungen für NEUE_PLATINE, Includes im Projektverzeichnis
//
// Name/Gruppe:
//

#include <stdlib.h>
#include <stdio.h>

#pragma config OSC=HS,WDT=OFF,LVP=OFF // HS Oszillator, Watchdog Timer disabled, Low Voltage Programming
// Define für LCD des neuen, grünen Demo-Boards:
// #define NEUE_PLATINE // Achtung: define vor include! Bei altem braunem Demo-Board auskommentieren!

#include "p18f452.h"
#include "lcd.h" // Enthält alle Prototypen für das LCD!

void init(void);
int ad_con(void);
unsigned int x = 0, y = 0;
unsigned char Analog_text1[20]="AnalogIn=\0"; // 16 Zeichen pro Zeile
unsigned char Analog_text2[20]="AnalogOut=\0";
unsigned char leer[] =" ";


void init()
{
	lcd_init();
	lcd_clear();

	ADCON0= 0b01000001; //fosc32, off at the begining,
	ADCON1= 0b00000000; //left justified, fosc32, out at the beeginning
	TMR2 = 0;
	PR2= 0xFF; //kevin approves
	
	CCP1CON= 0b00001100; //Ist PWM hier schon an? Werden schon random Daten durchgegeben? Also erst Timer setzen?
	T2CON = 0b00000100; //für Prescale 1; für 16 müssen die letzten bits (minderwertig) auf 1x umgeändert werden
	
	// TRISC = TRISC && 0b11111011;
	TRISC = 0b11111011;
	TRISE = 0b111;
	// PORTA/TRISA muss nicht initialisiert werden, da schon ein Eingang
}


int ad_con(void)
{
		for(;;)
				{
						if(ADCON0bits.DONE = 1)  
						{
							return 1;
						}					
				}	

}


void main()
{
	init();
	while(1) 
	{
		while(!ADCON0bits.GO) 
		{ 
			// Warten, bis Wandlung fertig
			// A/D-Converter Kanalauswahl
			//Analog-Kanal 0 einlesen
			if(!ADCON0bits.CHS2 && !ADCON0bits.CHS1 && !ADCON0bits.CHS0) 
			{
					ad_con();	

					// Berechnung von x
					x = ADRES >> 6;
					// Duty Cycle für PWM einstellen
					CCP1CON = CCP1CON | (ADRESL >> 2); //CCp1con hat die LSB von 10bit PWM signal, dann ADRESL als maske verwenden!
					CCPR1L = ADRESH;
					// Channel 7 auswählen
					ADCON0bits.CHS0= 1;
					ADCON0bits.CHS1= 1;
					ADCON0bits.CHS2= 1;
			}
			// Analog-Kanal 7 einlesen
			else if(ADCON0bits.CHS2 && ADCON0bits.CHS1 && ADCON0bits.CHS0) 
			{
			
			
				//Warten bis AD-Wandler fertig ist
				ad_con();	
			
				// Berechnung von y
				y = ADRES >> 6;
				// Channel 0 auswählen
				ADCON0bits.CHS0= 0;
				ADCON0bits.CHS1= 0;
				ADCON0bits.CHS2= 0;
			}

			// Ausgabe an LCD
			ADCON1=0x0E; // RA3:RA1 wieder digital I/O für LCD
			lcd_gotoxy(1,1);
			lcd_printf(Analog_text1);
			lcd_int(x);
			lcd_printf (leer);
			lcd_gotoxy(2,1);
			lcd_printf(Analog_text2);
			lcd_int(y);
			lcd_printf(leer);
			ADCON1=0x00; // AAAA AAAA
			ADCON0bits.GO= 1; // Wandlung starten
		}
	}
}
