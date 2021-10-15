/*
 * File:   mainTarea4.c
 * Author: Wu Cocozza Devandas Sousa
 * Microprocesadores y Microcontroladores IIS2021
 * Created on 14 de octubre de 2021, 09:53 PM
 */
//FUSES
// CONFIG1L
#pragma config PLLDIV = 2               // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC1_PLL2       // System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 1               // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = INTOSC_HS         // Oscillator Selection bits (Internal oscillator, HS oscillator used by USB (INTHS))
#pragma config FCMEN = OFF              // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF               // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = ON                // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOR = OFF                // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 3                 // Brown-out Reset Voltage bits (Minimum setting 2.05V)
#pragma config VREGEN = OFF             // USB Voltage Regulator Enable bit (USB voltage regulator disabled)

// CONFIG2H
#pragma config WDT = OFF                // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 0                // Watchdog Timer Postscale Select bits (1:1)

// CONFIG3H
#pragma config CCP2MX = ON              // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON              // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF            // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON               // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON              // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF                // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config ICPRT = OFF              // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF              // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF                // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF                // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF                // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF                // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF                // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF                // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF               // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF               // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF               // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF               // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF               // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF               // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF               // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF              // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF              // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF              // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF              // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF              //  Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

#include <xc.h>

#include<pic18f4550.h>

#define _XTAL_FREQ 8000000UL            //  Frecuencia de reloj en 8MHz

////////////////////////////////////////////////////////////////////////////
// VARIABLES INTERNAS
////////////////////////////////////////////////////////////////////////////

    int cont = 0;                       //  Contador del numero de cajas
    int timer = 0;                      //  Contador de numero de overflows del timer.
    int stop = 0;                       //  Toggle del stop
    
void isr(void);

void main(void) {
    
    OSCCONbits.IRCF = 0b111;            //  Frecuencia del oscilador interno a 8MHz

    ////////////////////////////////////////////////////////////////////////////
    // SETUP DE PUERTOS
    ////////////////////////////////////////////////////////////////////////////

    ADCON1 = 0b00001111;                //  Los bits del 0-3 nos indican que patillas son analogicas y cuales digitales, en este caso todas son digitales
                                        //  El bit 4 nos indica la configuracion del voltaje de referencia, en este caso es VDD
                                        //  El bit 5 nos indica otra configuracion del voltaje de referencia, en este caso es Vss
                                        //  Los bits 6 y 7 no estan implementados

    TRISB = 0b00000111;                 //  Todos los puertos son salidas excepto en 0, 1 y 2 que son entradas, son stop, entrada y salida

    TRISD = 0b00000000;                 //  Todos los puertos son salidas, aca se encuentran los LEDs y la Alarma

    LATBbits.LB0 = 0;                   //  Entrada de interrupcion de Stop
    LATBbits.LB1 = 0;                   //  Sensor de Entrada de cajas
    LATBbits.LB2 = 0;                   //  Sensor de Salida de cajas

    LATDbits.LD0 = 0;                   //  LED1
    LATDbits.LD1 = 0;                   //  LED2
    LATDbits.LD2 = 0;                   //  LED3
    LATDbits.LD3 = 0;                   //  LED4
    LATDbits.LD4 = 0;                   //  Alarma

    ////////////////////////////////////////////////////////////////////////////
    // SETUP GENERAL DE INTERRUPCIONES
    ////////////////////////////////////////////////////////////////////////////

    RCONbits.IPEN     = 0;              //  No tenemos prioridad de interrupciones
    INTCONbits.GIEH   = 1;              //  Permite interrupciones no enmascaradas
    INTCONbits.GIEL   = 1;              //  Permite interrupciones perifericas no enmascaradas

    INTCONbits.TMR0IE = 1;              //  Interrupciones por overflow activadas porque queremos contar overflows para obtener los 5segs de actualizacion de LEDS

    INTCON2bits.INTEDG0 = 1;            //  Sensible a Flanco de subida
    INTCON2bits.INTEDG1 = 1;            //  Sensible a Flanco de subida
    INTCON2bits.INTEDG2 = 1;            //  Sensible a Flanco de subid
    
    INTCONbits.INT0IE   = 1;            //  Interrupcion INT0 Enabled
    INTCON3bits.INT1IE  = 1;            //  Interrupcion INT1 Enabled
    INTCON3bits.INT2IE  = 1;            //  Interrupcion INT2 Enabled

    ////////////////////////////////////////////////////////////////////////////
    // SETUP DE TIMER
    ////////////////////////////////////////////////////////////////////////////

    T0CONbits.T08BIT = 0;               //  Define si el timer es de 8 o 16 bits, en este caso es de 16 bits
    T0CONbits.T0CS   = 0;               //  El reloj fuente de Timer0 es el interno CLKO
    T0CONbits.T0SE   = 0;               //  Seleccion de flanco del timer0, el timer0 incrementa en el flanco positivo T0CKl
    T0CONbits.PSA    = 1;               //  No tenemos preescalamiento

    T0CONbits.TMR0ON = 1;               //  Enable,  Timer0 activo

    ////////////////////////////////////////////////////////////////////////////
    // CICLO PRINCIPAL
    ////////////////////////////////////////////////////////////////////////////

    while(1){                           //  while True no tiene nada adentro, No usamos el ciclo principal
    }

}

void __interrupt() isr(void){

    ////////////////////////////////////////////////////////////////////////////
    // INTERRUPCION STOP
    ////////////////////////////////////////////////////////////////////////////

    if (INTCONbits.INT0IF == 1){        //  Se da la interrupcion INT0, que es la entrada de parada STOP
        stop = ~stop;                   //  Se intercambia el valor de stop
        INTCONbits.INT0IF = 0;          //  Se baja la bandera
    }

    ////////////////////////////////////////////////////////////////////////////
    // INTERRUPCION OVERFLOW DEL TIMER
    ////////////////////////////////////////////////////////////////////////////

    if (INTCONbits.TMR0IF == 1){        //  Se da overflow del timer0
        timer++;                        //  Aumenta el valor del timer
        INTCONbits.TMR0IF = 0;          //  Reset de flag

        if (timer >= 152) {             //  Se dan o han pasado mas de 5 segs
            
            if (stop == 0){             //  Verifica que no este activa la parada
            
            if (INTCONbits.INT0IF == 0){            //  Verifica si no tenemos interrupcion INT0

                if (cont >= 0) {                    //  Verifica que el numero de cajas sea 0 o mayor
                    LATDbits.LD4 = 0;               //  Se baja la alarma si cumple lo anterior
                    switch(cont){                   //  Manejo de LEDs segun numero de cajas
                        case 0:
                            LATDbits.LD0 = 0;       //  LED1 OFF
                            LATDbits.LD1 = 0;       //  LED2 OFF
                            LATDbits.LD2 = 0;       //  LED3 OFF
                            LATDbits.LD3 = 0;       //  LED4 OFF
                            break;
                        case 1:
                            LATDbits.LD0 = 1;       //  LED1 ON
                            LATDbits.LD1 = 0;       //  LED2 OFF
                            LATDbits.LD2 = 0;       //  LED3 OFF
                            LATDbits.LD3 = 0;       //  LED4 OFF
                            break;
                        case 2:
                            LATDbits.LD0 = 0;       //  LED1 OFF
                            LATDbits.LD1 = 1;       //  LED2 ON
                            LATDbits.LD2 = 0;       //  LED3 OFF
                            LATDbits.LD3 = 0;       //  LED4 OFF
                            break;
                        case 3:
                            LATDbits.LD0 = 0;       //  LED1 OFF
                            LATDbits.LD1 = 0;       //  LED2 OFF
                            LATDbits.LD2 = 1;       //  LED3 ON
                            LATDbits.LD3 = 0;       //  LED4 OFF
                            break;
                        case 4:
                            LATDbits.LD0 = 0;       //  LED1 OFF
                            LATDbits.LD1 = 0;       //  LED2 OFF
                            LATDbits.LD2 = 0;       //  LED3 OFF
                            LATDbits.LD3 = 1;       //  LED4 ON
                            break;
                        default:
                            LATDbits.LD0 = 1;       //  LED1 ON
                            LATDbits.LD1 = 1;       //  LED2 ON
                            LATDbits.LD2 = 1;       //  LED3 ON
                            LATDbits.LD3 = 1;       //  LED4 ON
                            break;
                       }
                    }
                }    
            }
        timer = 0;                                  //  Si se dan los 5s entonces debemos empezar a contar de nuevo, se pone en 0 el contador de overflows
            
        
    }

    ////////////////////////////////////////////////////////////////////////////
    // INTERRUPCION ENTRADA DE CAJA
    ////////////////////////////////////////////////////////////////////////////

        if (INTCON3bits.INT1IF == 1 && stop == 0 ){             //  Interrupcion que entra una caja y ademas no tenemos parada
            cont++;                                             //  Aumentamos el numero de cajas contadas
            INTCON3bits.INT1IF = 0;                             //  Reset de la bandera
        }

    ////////////////////////////////////////////////////////////////////////////
    // INTERRUPCION SALIDA DE CAJA
    ////////////////////////////////////////////////////////////////////////////

        if (INTCON3bits.INT2IF == 1 && stop == 0 ){             //  Interrupcion que sale una caja y ademas no tenemos parada
            cont--;                                             //  Disminuimos el numero de cajas contadas
            INTCON3bits.INT2IF = 0;                             //  Reset de la bandera
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    // INTERRUPCION POR CASO SIN SENTIDO, CAJAS NEGATIVAS
    ////////////////////////////////////////////////////////////////////////////

    if (cont < 0){              //  Cajas negattivas, todos los leds apagados excepto la alarma.
        LATDbits.LD0 = 0;       //  LED1 OFF
        LATDbits.LD1 = 0;       //  LED2 OFF
        LATDbits.LD2 = 0;       //  LED3 OFF
        LATDbits.LD3 = 0;       //  LED4 OFF
        LATDbits.LD4 = 1;       //  Alarma ON
    }
}