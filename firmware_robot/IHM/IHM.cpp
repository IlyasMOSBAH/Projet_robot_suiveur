#include "IHM.h"
#include "all_includes.h"
#include <stdarg.h>  // Pour va_list ... dans IHM_printf
#include <stdio.h>   // Pour vprintf et setprintf 
#define CAN_REMOTE CANType(2)
//

IHM::IHM(PinName CANRx, PinName CANTx): can(CANRx,CANTx)
{
    can.frequency(1000000);                                               /*Baud rate : kbits/s */
    can.attach(this,&IHM::can_ISR_Reader); /* Fonction d'interruption CAN*/
}
void IHM::LCD_printf(const char* format, ...)
{
    unsigned char i,j;
    va_list arg;
    va_start(arg,format);
    curseur=curseur+vsprintf(tableau_ecran+curseur%64,format,arg);
    if(curseur>31)
    {   
        for(i=32; i<curseur;i++)
        {
            tableau_ecran[i%32]=tableau_ecran[i];
        }
        curseur=curseur%32;
    }
    else
    {
        for(i=0; i<32;i++)
        {
            if(tableau_ecran[i]==0)
                tableau_ecran[i]=20;
        }   
    }
    va_end(arg);
    tableau_ecran[32]='\0';
    for(j=0; j<4; j++) {
        trame_Tx.id=LCD_CHAR0+j;
        trame_Tx.type=CANData;
        trame_Tx.len=8;
        for(i=0; i<8; i++)
            trame_Tx.data[i]=tableau_ecran[i+j*8];
        while(!can.write(trame_Tx));
        wait_us(150);
    }    
}
unsigned char JOG_valeur,mise_a_jour_JOG=0,mise_a_jour_COD=0;
void IHM::can_ISR_Reader(void)
{
    if (can.read(can_MsgRx[FIFO_ecriture])) {
        // FIFO gestion
        FIFO_ecriture=(FIFO_ecriture+1)%SIZE_FIFO;
        // myled[0] = !myled[0];
        // ledC= !ledC;
    }
}
void IHM::CAN_automate_reception(void)
{
    static signed char FIFO_lecture=0,FIFO_occupation=0,FIFO_max_occupation=0;
    FIFO_occupation=FIFO_ecriture-FIFO_lecture;
    if(FIFO_occupation<0)
        FIFO_occupation=FIFO_occupation+SIZE_FIFO;
    if(FIFO_max_occupation<FIFO_occupation)
        FIFO_max_occupation=FIFO_occupation;
    if(FIFO_occupation>SIZE_FIFO)
        while(!can.write(CANMessage(LCD_OVERFLOW,CANStandard)));
    if(FIFO_occupation!=0) {
        switch(can_MsgRx[FIFO_lecture].id) {
            case JOG_DATA:
                JOG_valeur=can_MsgRx[FIFO_lecture].data[0];
                mise_a_jour_JOG=1;
                break;
            case COD_DATA:
                COD_valeur=can_MsgRx[FIFO_lecture].data[0];
                mise_a_jour_COD=1;
                break;
             default:
                break;
        }
        FIFO_lecture=(FIFO_lecture+1)%SIZE_FIFO;
    }
}
/* FILE: IHM.c */
/****************************************************************************************/
/* MODULE NAME: IHM 16x2                                                                */
/*                                                                                      */
/* AUTHOR     : Jacques-Olivier KLEIN                                                   */
/*              Bruno LARNAUDIE                                                         */
/*                                                                                      */
/* EMAIL      : jacques-olivier.klein@u-psud.fr                                         */
/*              bruno.larnaudie@u-psud.fr                                               */
/*                                                                                      */
/* INSTITUTION: IUT de CACHAN - 9 av. de la div. Leclerc - 94230 CACHAN                 */
/*                                                                                      */
/* DATE       : 12/07/2012                                                              */
/* CPU        : FREESCALE MC9S12XS128MAA                                                */
/* BOARD      : CACHAN Xboard + IHM16x2                                                 */
/* OS         : None                                                                    */
/*                                                                                      */
/* DESCRIPTION: Fonctions d'accès au Lcd émulé passant par le bus CAN                   */
/****************************************************************************************/


/****************************************************************************************/
/* FUNCTION NAME: IHM_init                                                              */
/* DESCRIPTION  : Initialise l'afficheur IHM                                            */
/* RETURN       : VOID                                                                  */
/****************************************************************************************/



/****************************************************************************************/
/* FUNCTION NAME: IHM_gotoxy                                                            */
/* DESCRIPTION  : Modifie la position du curseur sur l'afficheur LCD de la carte IHM    */
/* RETURN       : VOID                                                                  */
/* PARAMETER  1 : UINT8 y : indice de la ligne   (0 à 1)                                */
/* PARAMETER  2 : UINT8 x : indice de la colonne (0 à 15)                               */
/****************************************************************************************/
void IHM::LCD_gotoxy (UINT8 y, UINT8 x)
{
    curseur=(y*16+x)%32;
}



/****************************************************************************************/
/* FUNCTION NAME: IHM_clear                                                             */
/* DESCRIPTION  : Efface le LCD                                                         */
/* RETURN       : VOID                                                                  */
/****************************************************************************************/
void IHM::LCD_clear(void)
{
    UINT8 i;
    curseur=0;
    for(i=0; i<32; i++)
        tableau_ecran[i]=32;
    while(!can.write(CANMessage(LCD_CLEAR,CANStandard)));
    wait_us(150);
}
/****************************************************************************************/
/* FUNCTION NAME: BAR_set                                                             */
/* DESCRIPTION  : commande les 10 LED du bargraph                                                       */
/* PARAMETER  1 : UINT16 valeur : ordre d'allumage, exprimé sur les 10 bits de droite.
                  Le bit k doit valoir 0 pour allumer la LED k, 1 pour l'éteindre.
                  Exemple :  ~(1<<9) allume la LED 9, ~0x00F allume les 4 LED 0 à 3                                                                 */
/****************************************************************************************/
void IHM::BAR_set(UINT16 valeur)
{
    trame_Tx.id=BAR_SET;
    trame_Tx.type=CANData;
    trame_Tx.len=2;
    trame_Tx.data[0]=valeur>>8;
    trame_Tx.data[1]=(UINT8)valeur;
    while(!can.write(trame_Tx));
}


/****************************************************************************************/
/* FUNCTION NAME: JOG_read                                                             */
/* DESCRIPTION  : indique la position (parmi les 8 possibles) et l'enfoncement du Jog. */
/* RETURN       : un octet UINT8 qui indique la position (RIGHT, LEFT, UP, DOWN + 4 combinaisons)
                  et l'enfoncement (PUSH) du Jog, exprimés sur 5 bits utiles.*/

/*         - Si le Jog est au repos : la fonction renvoie 0 (0x00).
         - si le Jog n'est pas au repos : 1 à 3 bits peuvent être mis à 1 dans l'octet renvoyé.
           Il est intéressant d'utiliser les masques définis en constantes.
           La signification des 8 bits de la valeur renvoyée est:
                     0   0   0   RIGHT   UP   PUSH   LEFT   DOWN
           Plusieurs bits peuvent être à 1 en même temps. Par exemple :  RIGHT + UP + PUSH
           à 1 indiquent que le Jog est en position "Nord Est " (UP+RIGHT) et qu'il est enfoncé.
           Si le Jog est simultanément enfoncé et positionné à droite, la fonction renvoie
           (0001 0100) en binaire, soit 0x14. Exprimée avec les masques, la valeur renvoyée vaut
           JOG_MSK_RIGHT | JOG_MSK_PUSH  (0x14). */
/**************************************************************************************************/
/**************************************************************************************************/
/* utiliser les Constantes symboliques de IHM.h pour interpréter la valeur renvoyée par le jog... */
/**************************************************************************************************/

UINT8 IHM::JOG_read(void)
{
    trame_Tx.id=JOG_REQ;
    trame_Tx.type=CAN_REMOTE;
    while(!can.write(trame_Tx));

    while(mise_a_jour_JOG==0)
        CAN_automate_reception();
    mise_a_jour_JOG=0;
    return JOG_valeur;

}


/***************************************************************************************************/
/* FUNCTION NAME: COD_read                                                                         */
/* DESCRIPTION  : indique la rotation effectuée par le codeur incrémental.                         */
/* RETURN       : un octet UINT8 dont la valeur entre 0 et 255 indique la rotation du
                  codeur incrémental depuis l'initialisation du codeur (cad de la carte IHM),
                  exprimée en douzièmes de tours. Le codeur dispose en effet de 12 positions
                  et la valeur retournée s'incrémente/se décrémente à chaque 12e de tour.
                  Par exemple, si le codeur a été tourné de 2 tours et demi, la valeur
                  retournée vaut 12+12+6 = 30 = 0x1E (le codeur a effectué 30 douzièmes de tour)   */
/***************************************************************************************************/

UINT8 IHM::COD_read(void)
{
    trame_Tx.id=COD_REQ;
    trame_Tx.type=CAN_REMOTE;
    while(!can.write(trame_Tx));

//    while(!can.write(CANMessage(COD_REQ,CANStandard)));
    while(mise_a_jour_COD==0)
        CAN_automate_reception();
    mise_a_jour_COD=0;
    return COD_valeur;

}

