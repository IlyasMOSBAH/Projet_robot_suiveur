#include "IHM.h"     // inclut "mbed.h"

#define Cseuil 80

#define Gauche3  0x08      //0 1000
#define Gauche2  0x0c      //0 1100
#define Gauche   0x04      //0 0100
#define Avancer  0x06      //0 0110
#define Droite   0x02      //0 0010
#define Droite2  0x03      //0 0011
#define Droite3  0x01      //0 0001
#define Racourci 0x1c      //1 1100

IHM ihm;  
Timer chrono;

//Bus
BusOut Bus8led(PB_3,PA_7,PA_6,PA_5,PA_3,PA_1,PA_0,PA_2);
BusOut BusSelectMux(PA_8,PF_1,PF_0);

//Out
PwmOut MotD(PB_4);
PwmOut MotG(PB_5);

//In
DigitalIn BP0(PA_9,PullUp);
DigitalIn BP1(PA_10,PullUp);
DigitalIn BP2(PB_0,PullUp);

DigitalIn BP(PB_7,PullUp);
DigitalIn Jack(PB_6,PullUp);

AnalogIn AnaIn(PB_1);

//Variable
static int etat,k=0,tabCapt[5]={4,3,2,1,0};
static float capt[5],Vpot,Vi;

//Fonction
int lirecapteur(void);
void racourci(int etat);
void automate(int etat,float Vpot,float Vi);
void Depart(float &Vpot,float &Vi);
void fin(void);
void Test(void);

//****************************MAIN*******************************
		
int main()
{  
	
	int i,t=0;
	bool test=0;
  
	ihm.LCD_clear();
	chrono.start();
	chrono.reset();
	MotG.period_us(100);
	MotD.period_us(100);
	
	while(t<2)
	{
		if(!BP0)test=1;
		t=chrono.read();
		ihm.LCD_gotoxy(0,0);
		ihm.LCD_printf("%d",t);
	}
 
  if(test==1)Test(); 
	ihm.LCD_clear();
	ihm.LCD_gotoxy(0,0);
	ihm.LCD_printf("Mode Course");

	while(1)
	{
	  Depart(Vpot,Vi);
		etat=lirecapteur();
		Bus8led=etat;
		
	  racourci(etat);
		automate(etat,Vpot,Vi);
		fin();
	}
}

//****************************DEPART*******************************

void Depart(float &Vpot,float &Vi)
{
	  BusSelectMux=7;
	  wait_us(1);
	  while(!Jack)
	  {
      Vpot=AnaIn.read();
		  Vi=Vpot*0.7;
		  ihm.LCD_gotoxy(1,0);
		  ihm.LCD_printf("Vpot=%d%% Vi=%d%%",int(Vpot*100),int(Vi*100));
	  }	
}
void automate(int etat,float Vpot,float Vi)
{
	switch(etat)
		    {
				case Avancer:
			      MotG.write(Vpot);              
			      MotD.write(Vpot);
				    break;
					
			    case Gauche:
			      MotG.write(Vi);              
			      MotD.write(Vpot);
					  k=1;
				    break;
					
			    case Droite:
			      MotG.write(Vpot);             
			      MotD.write(Vi);
					  k=0;
				    break;
					
			    case Gauche2:
			      MotG.write(0.1);      
			      MotD.write(0.3);
					  k=1;
			      break;
					
			    case Droite2:
			      MotG.write(0.3);             
			      MotD.write(0.1);
					  k=0;
			      break;
					
			    case Gauche3:
			      MotG.write(0.0);      
			      MotD.write(0.4);
			      break;
					
			    case Droite3:
			      MotG.write(0.4);             
			      MotD.write(0.0);
			      break;
					
		    }
}

//****************************LIRECAPTEUR*******************************

int lirecapteur(void)
{
	int etat=0;
	  for(int i=0;i<5;i++)
		{
			BusSelectMux=tabCapt[i];
			wait_us(1);
			capt[i]=AnaIn.read();
			if(int(capt[i]*100)>Cseuil)etat=etat+(1<<i);
		}
	return etat;
}

//****************************RACOURCI*******************************

void racourci(int etat)
{
	if ((etat&0x1d)==Racourci)
	{
		wait_ms(15);
	  etat = lirecapteur();
		if ((etat&0x1d)==Racourci)
		{
		MotG.write(0.30);               
		MotD.write(0.30);
			
		wait(0.28);
					
		MotG.write(0);               
		MotD.write(0);
			
		wait(0.2);
			
		MotG.write(0);               
		MotD.write(0.32);
			
		wait(0.5);
			
		MotG.write(0);               
		MotD.write(0);
		
	  wait(0.1);
		}
	}
}

//****************************FIN*******************************

void fin(void)
{
	if(!BP)
		{
			MotG.write(0);            
			MotD.write(0);
			while(Jack);
		}
}

//****************************TEST*******************************

void Test(void)
{
	int i;
	bool test=0;
	
	MotG.period_us(100);
	MotD.period_us(100);
	
		ihm.LCD_clear();
		ihm.LCD_gotoxy(0,0);
		ihm.LCD_printf("Mode test");
		wait(1);
		while(BP0||Jack)
		{
			ihm.LCD_gotoxy(0,0);
			etat=0;
			for(i=0;i<5;i++)
			{
				BusSelectMux=tabCapt[i];
				wait_us(1);
				capt[i]=AnaIn.read();
				if(int(capt[i]*100)>Cseuil)etat=etat+(1<<i);
				ihm.LCD_printf("%2d ",int(capt[i]*100));
			}
			Bus8led=etat;
			BusSelectMux=7;
			wait_us(1);
		  Vpot=AnaIn.read();
			Vi=Vpot*0.7;
			ihm.LCD_gotoxy(1,0);
		  ihm.LCD_printf("Vpot=%d%% Vi=%d%%",int(Vpot*100),int(Vi*100));
			if(!BP1)
			{
				
		      Vi=Vpot*0.7;
			  	MotD.write(Vi);
					MotG.write(Vpot);
				
		      Vi=Vpot*0.7;
			  	MotG.write(Vi);
					MotD.write(Vpot);
			  
			}
			if(!BP2)
			{
				MotG.write(Vpot);
				MotD.write(Vpot);
				wait(1);
				MotG.write(0);               
		    MotD.write(0);
		    wait(1);
		    MotG.write(0);               
		    MotD.write(Vpot);
		    wait(1);
		    MotG.write(0);               
		    MotD.write(0);
			}
		  MotG.write(0);
			MotD.write(0);
		}
}
