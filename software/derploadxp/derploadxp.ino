#include <PinChangeInt.h>
#include <AdaEncoder.h>
#include <glcd.h>
#include <fonts/allFonts.h>
#include <Wire.h>
#include "mcp4728.h"
#include "config.h"
#include <avr/io.h>


int8_t clicks=0;
char id=0;

unsigned int state = 0;
unsigned int lastState = 0;
unsigned int running = 0;
unsigned int stateSetting = 0;
unsigned long mA = 0;
unsigned long mV = 12800;
unsigned long lastUpdate = 0;
static unsigned int startStopCountNum = 0;
static unsigned int selectCountNum = 0;
static char updateDisplay = 1;
int temp=0;

mcp4728 dac = mcp4728(0);

//GLCD TEXT AREAS
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


void setup() {
  // Initialize the GLCD
  GLCD.Init();

  //Serial.begin(9600);

 // Select the font for the default text area
  GLCD.SelectFont(System5x7);

  AdaEncoder::addEncoder('a', a_PINA, a_PINB);

  pinMode(SELECTBUTTON, INPUT);     //set the pin to input
  digitalWrite(SELECTBUTTON, HIGH);
  PCintPort::attachInterrupt(SELECTBUTTON, selectCount ,RISING);

  pinMode(STARTSTOPBUTTON, INPUT);     //set the pin to input
  digitalWrite(STARTSTOPBUTTON, HIGH);
  PCintPort::attachInterrupt(STARTSTOPBUTTON, startStopCount,RISING);

	runStop.ClearArea();
	runStop.SetFontColor(WHITE);
	runStop.print("STOP");

  dac.begin();  // initialize i2c interface
  dac.vdd(5000);
  dac.setGain(1, 1, 1, 1);

}

void loop(){
	encoder *thisEncoder;
	thisEncoder=AdaEncoder::genie(&clicks, &id);
	if((startStopCountNum+1)%2){
		running=0;
		if (thisEncoder != NULL) {
			updateDisplay=1;
			if(selectCountNum%2){
				temp+=clicks;
			}
			else{
				stateSetting+=clicks;
			}
		}
	}

	else{
		running=1;
	}

	//if((millis()-lastUpdate)>1000){
	//	updateDisplay=1;
	//}

updateDisplay=1;
	if(updateDisplay){
		update();
		setDac();
		delay(10);
		updateDisplay=0;
		//lastUpdate = millis();
	}



}

void update(){
	if(running==1){
		runStop.ClearArea();
		runStop.SetFontColor(WHITE);
		runStop.print("RUN");
	}
	else{
		runStop.ClearArea();
		runStop.SetFontColor(WHITE);
		runStop.print("STOP");
	}


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
      //unitType.ClearArea();
      //unitType.print(A);
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



      if(running==0){
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
}

void setDac(){
	float dacValue =0;

	if(running==1){
		dacValue = ((mA/CHANNEL1_SHUNT_RESISTANCE)/(CHANNEL1_GAIN*1000.00)*1.001)+6;
		if(dacValue>4095){
			dacValue=4095;
		}
		dac.setPowerDown(0, 0, 0, 0);
		dac.analogWrite(0,(uint16_t)dacValue);
	}
	else{
		dacValue = 0;
		dac.analogWrite(0,(uint16_t)dacValue);
	    dac.setPowerDown(1, 1, 1, 1);
	}

	//dac.analogWrite((uint)dac1, 0, 0, 0); // write to input register of DAC four channel (channel 0-3) together. Value 0-4095


	//printStatus();
}

void selectCount(){
  selectCountNum++;
  updateDisplay=1;
  delay(10);
}

void startStopCount(){
  startStopCountNum++;
  updateDisplay=1;
  delay(10);
}

void printStatus()
{
  Serial.println("NAME     Vref  Gain  PowerDown  Value");
  for (int channel=0; channel <= 3; channel++)
  {
    Serial.print("DAC");
    Serial.print(channel,DEC);
    Serial.print("   ");
    Serial.print("    ");
    Serial.print(dac.getVref(channel),BIN);
    Serial.print("     ");
    Serial.print(dac.getGain(channel),BIN);
    Serial.print("       ");
    Serial.print(dac.getPowerDown(channel),BIN);
    Serial.print("       ");
    Serial.println(dac.getValue(channel),DEC);

    Serial.print("EEPROM");
    Serial.print(channel,DEC);
    Serial.print("    ");
    Serial.print(dac.getVrefEp(channel),BIN);
    Serial.print("     ");
    Serial.print(dac.getGainEp(channel),BIN);
    Serial.print("       ");
    Serial.print(dac.getPowerDownEp(channel),BIN);
    Serial.print("       ");
    Serial.println(dac.getValueEp(channel),DEC);
  }
  Serial.println(" ");
}



