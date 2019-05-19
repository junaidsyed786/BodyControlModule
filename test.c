#define mask_var 1
#define LIGHT1 PD1
#define PIR1 PD2
#define ONE 1 


///////////////////////functions///////////////////////////////
void CorrectVoicecommandReceived();
void VoiceCommandNotReceived();
void WrongVoiceCommandReceived();
void DoorLock();
void gpio_init();
void timer_init();
void clk_source();
void anti();
///////////////////////////variables////////////////////////////
int correctvoicecommand = 0;
int wrongvoicecommand = 0;
unsigned int counter = 0;
unsigned int password=0;
unsigned int key=0;
///////////////////////main program///////////////////////////////
int main()
{
  gpio_init();
  Int_init();
 
  while(1)
  {
    anti();
    if(correctvoicecommand==0 && wrongvoicecommand==0)
    {
    	PORTD &= ~(1<<PD7);
    	VoiceCommandNotReceived();//LED will toggle //DC motor off
    }
    
    else if(correctvoicecommand==1 && wrongvoicecommand==0)
    {
       
         CorrectVoicecommandReceived();//LED will be steady on
         if(PINB & (1<<PB4))
         {
         	DoorLock();//Dc motor on
         }
         else
         {
           PORTD &= ~(mask_var<<PD7);//Dc motor off
         }
    }
        
    else if(correctvoicecommand==0 && wrongvoicecommand==1 )
    {
    
      WrongVoiceCommandReceived();//LED off Dc motor off
      
    }
    else if(correctvoicecommand==1 && wrongvoicecommand==1){
        WrongVoiceCommandReceived();// VoiceCommandNotReceived();//LED off Dc motor off
        
    }
      }
  return 0;
}

//////////////////////////end of main//////////////////////////


/////////////////////gpio initialization function////////////////
void gpio_init()
{
  //voice control doors
  DDRB |= (mask_var<<PB4); // LED 
  PORTB &=~ (mask_var<<PB4);
 
  DDRB &=~(mask_var<<PB3); //Switch 2
  
  DDRD &= ~(mask_var<<PD4); // Switch 1
  
  DDRD |=(mask_var<<PD7); // MOTOR
  PORTD &= ~(mask_var<<PD7);
  
  //In Cabin Lighting
  DDRD |= (1<<LIGHT1);      // LED output for first PIR
  PORTD &= ~(1<<LIGHT1);   // turn LED off at first - interrupt will turn on/off
  DDRD &= ~(1<<PIR1);    // set PIR pin as input
  
  //AntiTheft
  DDRD&=~(1<<3); //CLEAR IP1
  PORTD|=(1<<3);
  DDRB&=~(ONE<<PB0);
  DDRB|=(ONE<<PB5);//output dc,buzzer
  PORTB&=~(ONE<<PB5);
  DDRB|=(ONE<<PB2);
  PORTB&=~(ONE<<PB2);
}
///////////////////////////end of gpio initialization function//////////////

void anti()
{
    if(key==1)
    {
     if(password==1)
      {
       PORTB|=(ONE<<PB5);
       PORTB&=~(ONE<<PB2);
      }
      else
        PORTB|=(ONE<<PB2);
    }
       else
       {
         _delay_ms(5000);
        PORTB&=~(ONE<<PB5);
         PORTB&=~(ONE<<PB2);
       }

}


/////////////////////////Interrupt initialization function////////////////
int Int_init()
{
   
  sei();
  //Voice Control PCINT3,PCINT20
  PCICR |= (1<<PCIE2);
  PCMSK2 |=(1<<PCINT20);
  
  PCICR |= (1<<PCIE0);
  PCMSK0 |= (1<<PCINT3);
  timer_init();
  
  //setup external interrupt INT0
  EICRA = 0x00;         // init
  EICRA |= (1<<ISC00);      // interrupt sense control: any logic change on INT0
  EIMSK |= (1<<INT0);      // turn extern interrupt on
  
  //AntiTheft set up
  EICRA|=(ONE<<ISC10);  
  EIMSK|=(ONE<<INT1);
  PCICR|=(1<<PCIE1);
  PCMSK1|=(1<<PCINT8);
}
////////////////////////end of Interrupt initialization function////////////////


//////////////////////////timer initialization function////////////////

void timer_init(){
  
TCCR0A &= ~(1<< WGM00);
TCCR0A &= ~(1<< WGM01);
TCCR0B &= ~(1<< WGM02);
TIMSK0 |= (1<<TOIE0);
TCNT0 = 0x00;
clk_source();    
 
}
////////////////////end of timer initialization function//////////////


////////////////clock select function///////////////////////////
void clk_source()
{

  TCCR0B &= ~(1<<CS01);
  TCCR0B |= ((1<<CS00) | (1<<CS02)); 

}
////////////////////end of clock select function/////////////////



//////////////////clock select function//////////
void CorrectVoicecommandReceived(){
  
  PORTB |=(mask_var<<PB4);
  counter = 0;
}
///////////////////////end of clock select function///////////////



///////////////////door lock function///////////////////
void DoorLock()
{
  PORTD |= (1<<PD7);
}
////////////////////end of door lock function////////////////


/////////////////////voice command not received function//////////
void VoiceCommandNotReceived(){
   PORTB |=(mask_var<<PB4);
  _delay_ms(200);
   PORTB &=~(mask_var<<PB4);
    _delay_ms(200);
   PORTD &= ~(1<<PD7);
}
/////////////////end of voice command not received function/////////////


////////////////////////wrong voice command received function////////
void WrongVoiceCommandReceived(){

   PORTD &= ~(mask_var<<PD7);
   PORTB &= ~(mask_var<<PB4);
}
////////////////////////end of wrong voice command received function//////////////////



/////////////////Interrupt subroutine//////////////////////////
//Interrupt for VoiceControl
ISR(PCINT2_vect){ // Switch 1 ISR

  correctvoicecommand ^= 1;
}

ISR(PCINT0_vect) // Switch 2 ISR
{ 
  wrongvoicecommand ^= 1;
}
ISR(TIMER0_OVF_vect)
{
  counter++;
}
//Interrupt for InCabinLighting
ISR(INT0_vect)
{
   PORTD ^= (1<<LIGHT1);      //toggle LED
}
//Interrupt for AntiTheft
ISR(INT1_vect)
{
 key=!key;
}
ISR(PCINT1_vect)
{
  password=!password;
  
}
