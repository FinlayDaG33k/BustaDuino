/*
OHH SHINY :D
*/

#include <LiquidCrystal.h> 
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/atomic.h>

#define randomSeed(s) srandom(s)

// define some values used by the panel and buttons
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel

boolean gameFinished = false;
boolean cashedOut = false;
 
volatile uint32_t seed;  // These two variables can be reused in your program after the
volatile int8_t nrot;    // function CreateTrulyRandomSeed()executes in the setup() 

                         // define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
 
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
 
int read_LCD_buttons(){               // read the buttons
    adc_key_in = analogRead(0);       // read the value from the sensor 
 
    // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
    // we add approx 50 to those values and check to see if we are close
    // We make this the 1st option for speed reasons since it will be the most likely result
 
    if (adc_key_in > 1000) return btnNONE; 
 
    // For V1.1 us this threshold
    /*
    if (adc_key_in < 50)   return btnRIGHT;  
    if (adc_key_in < 250)  return btnUP; 
    if (adc_key_in < 450)  return btnDOWN; 
    if (adc_key_in < 650)  return btnLEFT; 
    if (adc_key_in < 850)  return btnSELECT; 
    */
 
   // For V1.0 comment the other threshold and use the one below:
     if (adc_key_in < 50)   return btnRIGHT;  
     if (adc_key_in < 195)  return btnUP; 
     if (adc_key_in < 380)  return btnDOWN; 
     if (adc_key_in < 555)  return btnLEFT; 
     if (adc_key_in < 790)  return btnSELECT;
 
    return btnNONE;                // when all others fail, return this.
}
 
void CreateRandomSeed()
{
  seed = 0;
  nrot = 32; // Must be at least 4, but more increased the uniformity of the produced 
             // seeds entropy.
  
  // The following five lines of code turn on the watch dog timer interrupt to create
  // the seed value
  cli();                                             
  MCUSR = 0;                                         
  _WD_CONTROL_REG |= (1<<_WD_CHANGE_BIT) | (1<<WDE); 
  _WD_CONTROL_REG = (1<<WDIE);                       
  sei();                                             
 
  while (nrot > 0);  // wait here until seed is created
 
  // The following five lines turn off the watch dog timer interrupt
  cli();                                             
  MCUSR = 0;                                         
  _WD_CONTROL_REG |= (1<<_WD_CHANGE_BIT) | (0<<WDE); 
  _WD_CONTROL_REG = (0<< WDIE);                      
  sei();                                             
}
 
ISR(WDT_vect)
{
  nrot--;
  seed = seed << 8;
  seed = seed ^ TCNT1L;
}

void setup(){
   lcd.begin(16, 2);               // start the library
   lcd.setCursor(0,0);             // set the LCD cursor   position 
   lcd.print("BustaDuino");  // print a simple message on the LCD
   CreateRandomSeed();
   randomSeed(seed);
   Serial.begin(9600);
}
  
void loop(){
  if(gameFinished == false){
    float time_to = random(1,2); //.. Aantal milliseconde om mee te beginnen
    int i;
    float current_multiplier = 0.98;
    float cashedOutMultiplier;
    int randomNumber = random(0,1000);
    if(randomNumber < 201){
      randomNumber = 0;
    }
    for(i = 0; i <= randomNumber; i++){
      switch (lcd_key){
        case btnSELECT:{
          cashedOut = true;
          cashedOutMultiplier = current_multiplier;
          break;
        }}
        lcd.clear();
        lcd.print("BustaDuino");  // print a simple message on the LCD
        lcd.setCursor(0, 1);
        if(randomNumber > 0){
          current_multiplier = current_multiplier + 0.01;
        }else{
          current_multiplier = 0;
        }
        
        lcd.print(String(current_multiplier) + "x");
        Serial.print(String(current_multiplier) + "x");
        lcd_key = read_LCD_buttons();   // read the buttons
        if(i <= randomNumber && cashedOut == true){
          lcd.clear();
          lcd.print("You Win!");
          lcd.setCursor(0, 1);
          lcd.print(String(cashedOutMultiplier) + "x");
          Serial.print(String(cashedOutMultiplier) + "x");
          gameFinished = true;
          break;
        }else if(i >= randomNumber && cashedOut == false){
          lcd.clear();
          lcd.print("BUSTED!");
          lcd.setCursor(0, 1);
          lcd.print(String(current_multiplier) + "x");
          Serial.print(String(current_multiplier) + "x");
          gameFinished = true;
        }
        delay((time_to/i) * 3000);
    }
  }
}
