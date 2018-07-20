#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "platform.h"

#define SIZE  1024
#define START_CHAR '['
#define STOP_CHAR  ']'


static const char sifive_msg[] = "\n\r\
\n\r\
                SIFIVE, INC.\n\r\
\n\r\
         5555555555555555555555555\n\r\
        5555                   5555\n\r\
       5555                     5555\n\r\
      5555                       5555\n\r\
     5555       5555555555555555555555\n\r\
    5555       555555555555555555555555\n\r\
   5555                             5555\n\r\
  5555                               5555\n\r\
 5555                                 5555\n\r\
5555555555555555555555555555          55555\n\r\
 55555           555555555           55555\n\r\
   55555           55555           55555\n\r\
     55555           5           55555\n\r\
       55555                   55555\n\r\
         55555               55555\n\r\
           55555           55555\n\r\
             55555       55555\n\r\
               55555   55555\n\r\
                 555555555\n\r\
                   55555\n\r\
                     5\n\r\
\n\r\
               Usb communication Demo \n\r\
\n\r";

static const char maxvy_msg[] = "\n\r\
\n\r\
                                      MAXVY TECHNOLOGIES.\n\r\
\n\r\
      ,;;;;`       ;;;;;    `;;;;,        ;;;;       ;;;;   `;;;   ;;;`    ;;;  :;;;     ;;;\n\r\
     ;;;;;;;;      ;;;;;.   ;;;;;,       :;;;;       ,;;;   ;;;    ;;;;    ;;;   ;;;;   ;;;\n\r\
    ;;;;;;:.;;     ;;;;;;   ;;;;;,       ;;;;;        ;;;; .;;,    .;;;    ;;,   ,;;;   ;;;\n\r\
   ;;;;;;,`;: ,    ;;;;;;   ;;;;;,       ;;;;;,       `;;; ;;;      ;;;   ,;;     ;;;; ;;;\n\r\
   ;;;`:, ;,  ;    ;;;;;;  ;;;;;;,      ,;;,;;;        ;;;;;;`      ;;;,  ;;;     `;;; ;;,\n\r\
  ,;;;;  ;,   ;    ;;;.;;: ;;;;;;,      ;;; ;;;         ;;;;;       :;;;  ;;:      ;;;;;;\n\r\
  ;;;;;`::    ;    ;;; ;;; ;;`;;;,      ;;: ;;;.        ;;;;         ;;; `;;        ;;;;.\n\r\
  :;;,::;   `;;    ;;; ;;;:;; ;;;,     ,;;  ;;;;        ;;;;;        ;;; ;;;        ;;;;\n\r\
  `;`  ;,  .;;;    ;;; ,;;;;; ;;;,     ;;;;;;;;;       ;;;;;;`       ;;;:;;;        :;;;\n\r\
   :   `  .;;;;    ;;;  ;;;;. ;;;,     ;;;;;;;;;`     `;;;;;;;       `;;;;;`        :;;;\n\r\
   ,;`   `;;;;     ;;;  ;;;;  ;;;,    .;;;;;;;;;;     ;;;  ;;;:       ;;;;;         :;;;\n\r\
    ;;.  ;;;;`     ;;;  ;;;;  ;;;,    ;;;    .;;;    ,;;:  :;;;       ;;;;;         :;;;\n\r\
     .;,;;;;       ;;;   ;;.  ;;;,    ;;;     ;;;    ;;;    ;;;;      ,;;;,         :;;;\n\r\
\n\r\
                                   USB-UART communication Demo \n\r\
\n";


static void _putc(char c) {
  while ((int32_t) UART0_REG(UART_REG_TXFIFO) < 0);
  UART0_REG(UART_REG_TXFIFO) = c;
}

int _getc(char * c){
  int32_t val = (int32_t) UART0_REG(UART_REG_RXFIFO);
  if (val > 0) {
    *c =  val & 0xFF;
    return 1;
  }
  return 0;
}

static void _puts(const char * s) {
  while (*s != '\0'){
    _putc(*s++);
  }
}


int main(void){
 
//Clock Configurations

 // Make sure the HFROSC is on before the next line:
  PRCI_REG(PRCI_HFROSCCFG) |= ROSC_EN(1);
  // Run off 16 MHz Crystal for accuracy. Note that the
  // first line is 
  PRCI_REG(PRCI_PLLCFG) = (PLL_REFSEL(1) | PLL_BYPASS(1));
  PRCI_REG(PRCI_PLLCFG) |= (PLL_SEL(1));
  // Turn off HFROSC to save power
  PRCI_REG(PRCI_HFROSCCFG) &= ~(ROSC_EN(1));
 

 // Configure UART to print
  GPIO_REG(GPIO_OUTPUT_VAL) |= IOF0_UART0_MASK;
  GPIO_REG(GPIO_OUTPUT_EN)  |= IOF0_UART0_MASK;
  GPIO_REG(GPIO_IOF_SEL)    &= ~IOF0_UART0_MASK;
  GPIO_REG(GPIO_IOF_EN)     |= IOF0_UART0_MASK;

//set 115200 Baud Rate
  UART0_REG(UART_REG_DIV) = 138;// hard code value for div register
  UART0_REG(UART_REG_TXCTRL) = UART_TXEN; // enable uart tx to transmit
  UART0_REG(UART_REG_RXCTRL) = UART_RXEN; // enable uart rx to receive

 // Wait a bit to avoid corruption on the UART.
  // (In some cases, switching to the IOF can lead
  // to output glitches, so need to let the UART
  // reciever time out and resynchronize to the real 
  // start of the stream.
  volatile int i=0;
  while(i < 10000){i++;}

 _puts(maxvy_msg);

int p = 0;
char s[SIZE]="\0";
char c[]="0";
char *store=s;

  while(1){
             if (c[0] =='0'){
               
               if(_getc(c) != 0){
                     
                   if(c[0] == START_CHAR){
                      strcat(store,c);

                       while(p == 0){
                             
                            if(_getc(c) != 0) {
                                             
                                if(c[0] != START_CHAR){

                                   if(c[0] == STOP_CHAR){    
                                     strcat(store,c);
                                     _puts("\nEcho:");
                                     _puts(store);
                                     _puts("\n");
                                     c[0] ='0';
                                     memset(store,0,sizeof(s));
                                     p = 1;
                                   }else if(c[0] != STOP_CHAR){
                                     strcat(store,c);       
                                     p = 0;
                                   }                       
                                }           
                             }
                       }
                   p = 0;
                  }else if(c[0] != START_CHAR){
                   c[0] ='0';
                  }
             }
           }      
  }


}
