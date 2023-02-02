/// @file IHM.h Bibliothéque de classe pour la carte l'IHM de la carte NBoard
/// @mainpage
///
///
///
/// @author Gilles Raynaud, IUT Cachan
///
/// @note
/// History
/// v2.00 05/11/2016
#include "all_includes.h"
#define SIZE_FIFO 32
#define MESSAGE_BUFFER_SIZE 20
#define JOG_MSK_PUSH   (1<<2)   // valeur renvoyée si le Jog est enfoncé (en position de repos)
#define JOG_MSK_RIGHT  (1<<4)   // valeur renvoyée si le Jog est poussé vers la droite (non enfoncé).
#define JOG_MSK_UP     (1<<3)   // valeur renvoyée si le Jog est poussé vers le haut (non enfoncé).
#define JOG_MSK_LEFT   (1<<1)   // valeur renvoyée si le Jog est poussé vers la gauche (non enfoncé).
#define JOG_MSK_DOWN   1        // valeur renvoyée si le Jog est poussé vers le bas (non enfoncé).
//
///
/// Example:
/// @code

///#include "IHM.h"
///IHM ihm;  //clase IHM
///Serial pc(SERIAL_TX, SERIAL_RX);  // I/O terminal PC 
///int main()
///{
///    unsigned char codeur=0,jog;
///    float pi=4*atan(1.0);
///    pc.printf("Hello World  \n");
///    ihm.LCD_clear();
///    ihm.LCD_printf("Hello World      ");
///    ihm.LCD_gotoxy(1,0);
///    ihm.LCD_printf("PI= %f",pi);  // test affichage float
///    ihm.BAR_set(0x3FF);
///    wait(2);
///    ihm.BAR_set(0x2AA);
///    wait(2);
///    ihm.BAR_set(0x155);
///    wait(2);
///    ihm.LCD_clear();
///    while(1) {
///        wait(0.1);
///        codeur=ihm.COD_read();      
///        jog=ihm.JOG_read();
///        ihm.BAR_set((UINT16)(codeur));
///        ihm.LCD_gotoxy(0,0);
///        ihm.LCD_printf("Jog=%02d",jog);
///        ihm.LCD_gotoxy(1,0);
///        ihm.LCD_printf("Cod=%03d",codeur);
///        pc.printf("Jog=%02d  Cod=%03d \n",jog,codeur); 
///    }
///}
/// @endcode
///
class IHM
{
public:
    /// Constructeur pour l'IHM de la carte NBoard, placé à l'extérieur du main
    /// Les paramètres peuvent être omis (ils ont tous une valeur par défaut)
    ///
    IHM(PinName CANRx=PA_11, PinName CANTx=PA_12);
    /// commande des 10 leds du bargraph
    ///
    /// Affiche la valeur passée en argument sur le bargraph
    /// @param valeur à afficher sur le bargraph
    /// @returns void
    ///
    void BAR_set(UINT16 valeur);  
    /// lecteur du codeur
    ///
    /// Retourne la valeur du codeur (0-255)
    /// @param void
    /// @returns valeur du codeur
    ///
    UINT8 COD_read(void);
    /// positionne le curseur de l'afficheur LCD
    ///
    /// @param   y : ligne (0-1)
    /// @param  x : colonne (0-15)
    /// @returns void
    ///
    void LCD_gotoxy(UINT8 y,UINT8 x);

    /// écriture formatée sur l'afficheur LCD
    ///
    /// @param  format...
    /// @returns void
    ///
    void LCD_printf(const char* format, ...);
    /// effacement de l'afficheur LCD
    ///
    /// @param  void
    /// @returns void
    ///
    void LCD_clear(void);
    /// lecture du JOG
    /// retourne la position (8 possibilités) et l'enfoncement du Jog
    /// @param  void
    /// @returns valeur JOG
    ///
    UINT8 JOG_read(void);
private:
    int COD_valeur;
    unsigned char JOG_valeur,mise_a_jour_JOG,mise_a_jour_COD;
    unsigned char FIFO_ecriture;
//    CANMessage can_MsgRx[SIZE_FIFO];
    UINT8 curseur;
    UINT8 IHM_is_running;
    CANMessage can_MsgRx[SIZE_FIFO], can_MsgTx;
//extern unsigned char Reception_CAN;
CANMessage trame_Tx;
char tableau_ecran[65],tableau_temporaire[200];
    CAN can;
    void can_ISR_Reader(void);
    void CAN_automate_reception(void);
};
