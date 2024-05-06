#include <msp430.h>
#include "def_principais.h"
#include "LCD.h"

//informações para criar novos caracteres, armazenadas na memória flash
const unsigned char novos_caract[]={0b00000001,//0
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00011111,
                                            0b00011111,//1
                                            0b00010000,
                                            0b00010000,
                                            0b00010000,
                                            0b00010000,
                                            0b00010000,
                                            0b00010000,
                                            0b00011111,//2
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,//3
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00011111,//4
                                            0b00000000,
                                            0b00000000,
                                            0b00000000,
                                            0b00000000,
                                            0b00000000,
                                            0b00011111,
                                            0b00011111,//5
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00000001,
                                            0b00011111,
                                            0b00011111,//6
                                            0b00000000,
                                            0b00000000,
                                            0b00000000,
                                            0b00000000,
                                            0b00000000,
                                            0b00000000,
                                            0b00011111,//7
                                            0b00010000,
                                            0b00010000,
                                            0b00010000,
                                            0b00010000,
                                            0b00010000,
                                            0b00011111};

const unsigned char nr_grande[10][4] = { {0x01, 0x02, 0x4C, 0x00}, //nr. 0
                                                {0x20, 0x7C, 0x20, 0x7C}, //nr. 1
                                                {0x04, 0x05, 0x4C, 0x5F}, //nr. 2
                                                {0x06, 0x05, 0x5F, 0x00}, //nr. 3
                                                {0x4C, 0x00, 0x20, 0x03}, //nr. 4
                                                {0x07, 0x04, 0x5F, 0x00}, //nr. 5
                                                {0x07, 0x04, 0x4C, 0x00}, //nr. 5
                                                {0x06, 0x02, 0x20, 0x03}, //nr. 7
                                                {0x07, 0x05, 0x4C, 0x00}, //nr. 8
                                                {0x07, 0x05, 0x20, 0x03}};//nr. 9
//-----------------------------------------------------------------------------------------------------------
void cria_novos_caract()//criação dos 8 novos caracteres
{
    unsigned char i, k, j=0, n=0x40;

    for(i=0;i<8;i++)
    {
        cmd_LCD(n,0);       //endereco base para gravar novo segmento
        for(k=0;k<7;k++)
            cmd_LCD(novos_caract[k+j],1);

        cmd_LCD(0x00,1);    //apaga ultima posição do end. da CGRAM para evitar algum dado espúrio
        j += 7;
        n += 8;
    }
}
//-----------------------------------------------------------------------------------------------------------
void escreve_BIG(unsigned char end, unsigned char nr)//escreve um número grandes no LCD
{
     cmd_LCD(end,0);                                //endereco de incio de escrita (1a linha)
     cmd_LCD(nr_grande[nr][0],1);
     cmd_LCD(nr_grande[nr][1],1);
     cmd_LCD(end+64,0);                             //desloca para a 2a linha
     cmd_LCD(nr_grande[nr][2],1);
     cmd_LCD(nr_grande[nr][3],1);
}


/**
 * main.c
 */
volatile unsigned char modo = 0;
volatile unsigned char modo_2 = 0;

int main(void)
{
    //variáveis relógio
    unsigned char u_seg = 0,
            d_seg = 0 ,
            u_min = 0,
            d_min = 0,
            u_hor = 0,
            d_hor = 0;
    //variáveis Data
    unsigned char u_dia = 1,
            d_dia = 0,
            u_mes = 1,
            d_mes =0 ,
            u_ano = 1,
            d_ano=0;

    //varáveis cronometro
    unsigned char u_mseg = 0,
            d_mseg = 0,
            c_mseg = 0,
            u_sec = 0,
            d_sec = 0,
            c_sec = 0;



    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P6DIR = 0xFF;           // P6.0-P6.3 dedicados aos dados do LCD
    P3DIR = 0xFF;           // P3.2-P3.4 dedicados ao controle do LCD

    P1DIR  = 0b00000000;         // configure P1.1 as input
    P1REN |= 0b00000010;         // habilitando pull-up e pull down no P1.1
    P1OUT |= 0b00000010;         // habilita pull-up em P1.1

    P2DIR  = 0b00000000;         // configure P2.1 as input
    P2REN |= 0b00000010;         // habilitando pull-up e pull down no P2.1
    P2OUT |= 0b00000010;         // habilita pull-up em P2.1

    P1IES = 0b00000010;          // configura interrupção borda de descida
    P1IE  = 0b00000010;          // habilita interupção para P1.1

    P2IES = 0b00000010;          // configura interrupção borda de descida
    P2IE  = 0b00000010;          // habilita interupção para P2.1

    set_bit(CONTR_LCD, BkL);

    inic_LCD_4bits(); // inicializa o LCD

    cria_novos_caract();            //cria os 8 novos caracteres



    __bis_SR_register(GIE);
    while(1)
    {
          //inicia o timer do relógio

                       u_seg++;

       switch(modo){


         case 0:
               cmd_LCD(0xc4,0);
               cmd_LCD('.',1);

               cmd_LCD(0xcA,0);
               cmd_LCD('.',1);

               escreve_BIG(0x8E,u_ano);
               escreve_BIG(0x8B,d_ano);
               escreve_BIG(0x88,u_mes);
               escreve_BIG(0x85,d_mes);
               escreve_BIG(0x82,u_dia);
               escreve_BIG(0x80,d_dia);




         break;

         case 1:
             //Estrutura do Relógio/Data
             if (u_seg > 9){
                            u_seg = 0;
                            d_seg++;
                        } if(u_seg == 0 && d_seg == 6 ){
                            d_seg = 0;
                            u_min++;
                        }

                        if(u_min > 9){
                           u_min = 0;
                           d_min++;
                        } if(u_min == 0 && d_min == 6 ){
                           d_min = 0;
                           u_hor++;
                        }

                        if(u_hor > 9){
                           u_min = 0;
                           d_hor++;
                        } if(u_hor == 5 && d_hor == 2 ){
                           d_hor = 0;
                           u_hor = 0;
                           u_dia++;
                        }

                        if(u_dia > 9){
                           u_dia = 0;
                           d_dia++;
                        } if(u_dia == 1 && d_dia == 3){
                            d_dia = 0;
                            u_dia = 0;
                            u_mes++;
                        }

                        if(u_mes > 9){
                            u_mes = 0;
                            d_mes++;
                        }if(u_mes == 3 && d_mes == 1){
                            u_mes = 1;
                            d_mes = 0;
                            u_dia = 1;
                            u_ano++;
                        }

                        if(u_ano > 9){
                            u_ano = 0;
                            d_ano++;
                        }if(u_ano == 9 && d_ano == 9){
                            u_ano = 0;
                            d_ano = 0;
                        }


             cmd_LCD(0x84,0);
             cmd_LCD('.',1);
             cmd_LCD(0xc4,0);
             cmd_LCD('.',1);
             cmd_LCD(0x8A,0);
             cmd_LCD('.',1);
             cmd_LCD(0xcA,0);
             cmd_LCD('.',1);

             escreve_BIG(0x8E,u_seg);
             escreve_BIG(0x8B,d_seg);
             escreve_BIG(0x88,u_min);
             escreve_BIG(0x85,d_min);
             escreve_BIG(0x82,u_hor);
             escreve_BIG(0x80,d_hor);

         break;

         case 2:
              //Estrtura do cronometro
             switch(modo_2)
             {
                     case 0: // zera a contagem

                      u_mseg = 0, d_mseg = 0, c_mseg = 0, u_sec = 0, d_sec = 0, c_sec = 0;

                      break;

                      case 1:  // inicia a contagem

                      while(modo_2 == 1){

                                  u_mseg++;
                                  if(u_mseg > 9){
                                     u_mseg = 0;
                                     d_mseg++;
                                  }if(d_mseg > 9){
                                     d_mseg = 0;
                                     c_mseg++;
                                  }if(u_mseg == 0 && d_mseg == 0 && c_mseg == 1){

                                      u_mseg = 0;
                                      d_mseg = 0;
                                      c_mseg =0;
                                      u_sec++;

                                  }

                                  if(u_sec > 9){
                                     u_sec = 0;
                                     d_sec++;
                                  } if(d_sec > 9){
                                     d_sec = 0;
                                     c_sec++;
                                  } if(u_sec == 9 && d_sec == 9 && c_sec == 9){
                                     u_sec = 0;
                                     d_sec = 0;
                                     c_sec = 0;
                                   }
                                       cmd_LCD(0x8A,0);
                                       cmd_LCD('.',1);
                                       cmd_LCD(0xcA,0);
                                       cmd_LCD('.',1);


                                       escreve_BIG(0x8E,u_mseg);
                                       escreve_BIG(0x8B,d_mseg);
                                       escreve_BIG(0x88,u_sec);
                                       escreve_BIG(0x85,d_sec);
                                       escreve_BIG(0x83,c_sec);


                               __delay_cycles(1000); //delay para o cronometro

                      }


                       break;
                       case 2: // para a contagem
                       break;
               }
             cmd_LCD(0x8A,0);
             cmd_LCD('.',1);
             cmd_LCD(0xcA,0);
             cmd_LCD('.',1);


             escreve_BIG(0x8E,u_mseg);
             escreve_BIG(0x8B,d_mseg);
             escreve_BIG(0x88,u_sec);
             escreve_BIG(0x85,d_sec);
             escreve_BIG(0x83,c_sec);

         break;

             default:
                 break;






       }

       _delay_cycles(1000000); //delay segundos do relógio
    }
}
#pragma vector = PORT2_VECTOR
__interrupt void port_1(void) {
    if (P2IFG & 0b000000010) { // Botão de mudança de modo

        modo++;
        if (modo > 2) modo = 0;
        // Limpa o display LCD
        cmd_LCD(0x01, 0); // Limpa o display LCD
    __delay_cycles(800000);
    }

    P2IFG &= ~0b000000010; // Limpa a flag de interrupção de P1.1
}
#pragma vector = PORT1_VECTOR
__interrupt void port_2(void) {

    if (P1IFG & 0b000000010) { // Botão de mudança de modo
            modo_2++;
            if (modo_2 > 2) modo_2 = 0;
            __delay_cycles(800000);

    }

    P1IFG &= ~0b000000010; // Limpa a flag de interrupção de P2.1
}
