#include <PinChangeInt.h> // necessary otherwise we get undefined reference errors.
#include <AdaEncoder.h>
#include <glcd.h>
#include <fonts/allFonts.h>

#define MAXCURRENT  25000  //Max current in mA

#define a_PINA A3
#define a_PINB A4
#define STARTSTOPBUTTON A2
#define SELECTBUTTON A5
  
gText modeArea =   gText(0,0,16,1,System5x7);
gText runStop =   gText(96,0,4,1,System5x7);
gText digit1 =   gText(0,16,1,1,fixednums7x15);
gText digit2 =  gText(8,16,1,1,fixednums7x15);
gText digit3 = gText(19,16,1,1,fixednums7x15);
gText digit4 =  gText(27,16,1,1,fixednums7x15);
gText digit5 = gText(35,16,1,1,fixednums7x15);
gText unitType = gText(43,16,1,1,fixednums7x15);

gText digit1B =   gText(51,16,1,1,fixednums7x15);
gText digit2B =  gText(59,16,1,1,fixednums7x15);
gText digit3B = gText(67,16,1,1,fixednums7x15);
gText digit4B =  gText(75,16,1,1,fixednums7x15);
gText unitTypeB = gText(83,16,1,1,fixednums7x15);

int8_t clicks=0;
char id=0;

unsigned int state = 0;
unsigned int lastState = 0;
unsigned int toggle = 1;
unsigned int stateSetting = 0;
unsigned long mA = 0;
unsigned long mV = 12800;
unsigned int startStopCountNum = 0;
unsigned int selectCountNum = 0;
int temp=0;

void setup() {
  // Initialize the GLCD 
  GLCD.Init();

 // Select the font for the default text area
  GLCD.SelectFont(System5x7);
  
  Serial.begin(115200);
  Serial.println("test");
  
  AdaEncoder::addEncoder('a', a_PINA, a_PINB);
  
  pinMode(SELECTBUTTON, INPUT);     //set the pin to input
  digitalWrite(SELECTBUTTON, HIGH);
  PCintPort::attachInterrupt(SELECTBUTTON, selectCount ,RISING);
  
  pinMode(STARTSTOPBUTTON, INPUT);     //set the pin to input
  digitalWrite(STARTSTOPBUTTON, HIGH);
  PCintPort::attachInterrupt(STARTSTOPBUTTON, startStopCount,RISING);

}

void loop() {
   if(startStopCountNum%2){
         runStop.ClearArea();
         runStop.SetFontColor(WHITE);
         runStop.print("RUN");
         toggle=0;
   }else{
         runStop.ClearArea();
         runStop.SetFontColor(WHITE);
         runStop.print("STOP");
         toggle=1;

   }
  
  encoder *thisEncoder;
  thisEncoder=AdaEncoder::genie(&clicks, &id);
  if (thisEncoder != NULL) {
    Serial.print(id); Serial.print(':');
    if (clicks > 0) {
      if(toggle){
        if(selectCountNum%2){
          temp=1;
        }else{
           stateSetting++;
        }
      }
    }
    if (clicks < 0) {
      if(toggle){
        if(selectCountNum%2){
          temp=-1;
        }else{
           stateSetting--;
        }
      }
    }
  }
//switch state for mode
  switch(state){
    case 0:
      modeArea.ClearArea();
      

      //13 CHARS MAX
      modeArea.print("C/C Mode");
      digit1.ClearArea();
      digit1.print(mA/ 10000 % 10);
      digit2.ClearArea();
      digit2.print(mA/ 1000 % 10);
      digit3.ClearArea();
      digit3.print(mA/ 100 % 10);
      digit4.ClearArea();
      digit4.print(mA/ 10 % 10);
      digit5.ClearArea();
      digit5.print(mA % 10);
      unitType.ClearArea();
      unitType.print("A");
      /*
      digit1B.ClearArea();
      digit1B.print(mV/ 10000 % 10);
      digit2B.ClearArea();
      digit2B.print(mV/ 1000 % 10);
      digit3B.ClearArea();
      digit3B.print(mV/ 100 % 10);
      digit4B.ClearArea();
      digit4B.print(mV/ 10 % 10);
      unitTypeB.ClearArea();
      unitTypeB.print('V');
      */
      GLCD.FillRect( 16, 28, 1, 1);
      

      
      if(toggle){
        switch(stateSetting%6){
          case 0:
            modeArea.SetFontColor(BLACK);
            digit1.SetFontColor(BLACK);
            digit2.SetFontColor(BLACK);
            digit3.SetFontColor(BLACK);
            digit4.SetFontColor(BLACK);
            digit5.SetFontColor(BLACK);
          break;
          case 1:
            mA+=temp*10000;
            temp=0;
            modeArea.SetFontColor(WHITE);
            digit1.SetFontColor(WHITE);
            digit2.SetFontColor(BLACK);
            digit3.SetFontColor(BLACK);
            digit4.SetFontColor(BLACK);
            digit5.SetFontColor(BLACK);
            break;                 
          case 2:
            mA+=temp*1000;
            temp=0;
            modeArea.SetFontColor(WHITE);
            digit1.SetFontColor(BLACK);
            digit2.SetFontColor(WHITE);
            digit3.SetFontColor(BLACK);
            digit4.SetFontColor(BLACK);
            digit5.SetFontColor(BLACK);
            break;         
          case 3:
            mA+=temp*100;
            temp=0;
            modeArea.SetFontColor(WHITE);
            digit1.SetFontColor(BLACK);
            digit2.SetFontColor(BLACK);
            digit3.SetFontColor(WHITE);
            digit4.SetFontColor(BLACK);
            digit5.SetFontColor(BLACK);
            break;          
          case 4:
            mA+=temp*10;
            temp=0;
            modeArea.SetFontColor(WHITE);
            digit1.SetFontColor(BLACK);
            digit2.SetFontColor(BLACK);
            digit3.SetFontColor(BLACK);
            digit4.SetFontColor(WHITE);
            digit5.SetFontColor(BLACK);
            break;          
          case 5: 
            mA+=temp;
            temp=0;
            modeArea.SetFontColor(WHITE);
            digit1.SetFontColor(BLACK);
            digit2.SetFontColor(BLACK);
            digit3.SetFontColor(BLACK);
            digit4.SetFontColor(BLACK);
            digit5.SetFontColor(WHITE);
            break;          
        }
        
        if(mA>MAXCURRENT)
          mA=MAXCURRENT;
        
      }else{
            modeArea.SetFontColor(WHITE);
            digit1.SetFontColor(BLACK);
            digit2.SetFontColor(BLACK);
            digit3.SetFontColor(BLACK);
            digit4.SetFontColor(BLACK);
            digit5.SetFontColor(BLACK);
      }
      break;
  }
  
  
  delay(50);
  //stateSetting++;
  //mA+=9;
}

void selectCount(){
  selectCountNum++;
  delay(5);
}

void startStopCount(){
  startStopCountNum++;
  delay(5);
}



