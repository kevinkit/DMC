;Dateinamen:
;Beschreibung:
;VERSION 0.2
;
;INVERTER DOES NOT WORK PROPERLY ATM FOR THE BUZZER
;
;Entwickler: Khanh Nguyen, Kevin Höfle
; Version: 1.0 Datum:
list p=18f452
#include <p18f452.inc>
BANK0 equ 0x00
BANK1 equ 0x100
BANK2 equ 0x200
BANK3 equ 0x300
BANK4 equ 0x400
BANK5 equ 0x500
;//Bank0
;//Bank1
;//Bank2
;//Bank4
;//Bank5
;//***** Main Program *****
org 0x00
ResetVect
goto Init
org 0x08
IntVectHigh
;//No Interrupts Enabled
org 0x18
IntVectLow
;//No Interrupts Enabled
org 0x30
;//Initialisation Code - Placed at some point after IntVect
;Variablen
; BANK 0 -> Zeitschleifen, die öfters aufgerufen werden, am besten hier rein in diese Bank da diese am schnellsten zugegriffen werden
var1 equ 0x01 ;über var1 kann dann die zeit eingestellt werden
Init
;Eingägen -> 1
;Ausägnge -> 0

clrf PORTA	
movlw 0xFF ;besser alles auf Eingänge
movwf TRISA, 0
;LED-Ports //ANALOG zur A

clrf PORTB
movlw 0xf0
movwf TRISB, 0

clrf PORTC
movlw 0xfb ; ALLE BIS AUF den einen ausgang auf eingange setzen, auch wenn diese nicht gebraucht werden, da es immer besser ist diese als Eingang zu benutzen (kein strom fließt raus!)
movwf TRISC, 0

;AD-WANDLER EINSTELLUNGEN
;ADCON0 10 (fosc32) 000 (An0 soll gewandelt werden) 0 (Ad-wandler not in progress) 0 (undefined) 1 (Ad-Wandler anschalten) -> 1 00 000 0 0 1 -> 0x81
;ADCON1 1 (left justified, da die höheren 4 bits konfiguriert werden sollen) 0 (fosc32) 1110 (AN0 ist ein ANlaog und An4 ~~~warscheinlich~~~ digital, aber liegt der auch am ADWandler?! mal fragen
;1 0 0 0 1 1 1 0
movlw 0x81
movwf ADCON0, 0
movlw 0x0E
movwf ADCON1, 0
bra main
;//Main Application Code
main


tester
BTFSC PORTA, 4
BRA tester ;springt jedes mal wieder zurück wenn Bit4 nicht gesetzt ist


;Hier muss der AD-Wandler jetzt starten -> ALSO got bit setzen!
BSF ADCON0, 2, 0
;DATEN werden jetzt in ADRESSH geschrieben und müssen an die Portsweitergeleitet werden
;MOVF ADRESSH, 0, 0 ;von ADRESSH ins wreg kann weg gelassen werden dank swapf befehl

;;DIE SCHLEIFE SOLL ZEITABHÄNGIG VOM EINGESTELLTEN WERT IN ADRESSH (oder Wreg aber da is es schon getauscht)
;;den Summer invertieren
;;vorher muss geschaut werden ob der AD-Wandler fertig ist


wait_for_ad
BTFSC ADCON0, 2 ;chekcen ob auf 0
BRA wait_for_ad
;SWAPF WREG, 1, 0 ;;WREG wieder rum drehen


SWAPF ADRESH, 0,0 ;nibbles tauschen k

MOVWF PORTB, 0 ;; werte an die LED schicken
SWAPF WREG, 1,0 
BTG PORTC, RC2



;call inverter

inverter
DECFSZ WREG, 0 ;;bis wreg 0 is
bra inverter

BTG PORTC, RC2


bra main
FinishLoop
bra FinishLoop ;//Do.. Forever stops
end
