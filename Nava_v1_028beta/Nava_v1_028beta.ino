//-------------------------------------------------
//                  NAVA v1.x
//                  main program
//-------------------------------------------------

/////////////////////Include/////////////////////
#include <SPI.h>
#include <LiquidCrystal.h>     // [zabox] [1.028] (still working)
//#include <NewLiquidCrystal.h>    // [zabox] [1.028] faster lcd library (https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home)
                                 //                  somehow requires the wire.h library to be present, so i modded mine with the 130 byte buffer length. old lib sill works without modification
                                 //                  lcd update down from 19ms to 5ms (1,3ms to 0,3ms in shuffle/flam update). reduces flickering
//#include <Wire.h>                // [zabox] [1.028] (wire.h/twi.h 130 byte buffer length)
#include <WireN.h>             // [zabox] [1.028]
#include "define.h"
#include "string.h"
#include <MIDI.h>

#include <MemoryFree.h>



LiquidCrystal lcd(18, 19, 20, 21, 22, 23);

////////////////////////Setup//////////////////////
void setup()
{
  InitIO();//cf Dio
  InitButtonCounter();//cf Button
  
  SetDoutTrig(0);                                           // [zabox] [1.028] no random trigger pin states at startup (bd can oscillate with open trigger)
  SetDoutLed(0, 0, 0);                                      //                 no random leds at startup
  
  initTrigTimer();                                          // [zabox] [1.028]  init 2ms trig timer
  initFlamTimer();                                          // flam
  
  lcd.begin(16, 2);                                         // [zabox] [1.028] must be executed before chreateChar with the new library
  lcd.createChar(0,font0);
  lcd.createChar(1,font1);
  lcd.createChar(2,font2);
  lcd.createChar(3,font3);
  lcd.createChar(4,font4);
  lcd.createChar(5,font5);
  

  ScanDinBoot();
  //Init EEprom-------------------------------------
  if (btnPlayStopByte == (BTN_PLAY | BTN_STOP)){
    LcdPrintEEpromInit();
    bootElapseTime = millis();
    while (1){
      ButtonGet();
      if((millis() - bootElapseTime) > BOOTLOADER_TIME) break;
      if (playBtn.pressed && enterBtn.pressed){
        InitEEprom();
        //InitEEpromTrack();//problem with init pattern 0 to 18: to be solved
        //InitSeqSetup();
        break;
      }
    }
  }
  //TM2 adjustement for velocity
  else if (btnEnterByte == BTN_ENTER){
    LcdPrintTM2Adjust();
    while (1){
      SetDacA(MAX_VEL);
    }
  }


  InitSeq();// cf Seq
  //Load default track
  LoadTrack(0);
  //Load default pattern
  LoadPattern(0);
  ptrnBuffer = !ptrnBuffer;
  InitPattern();
  SetHHPattern();
  InstToStepWord();
  SetMuxTrigMidi(RM, 0);                                    // [zabox] workaround. without the line, the first played step/instrument after power on had no sound
  SetDoutTrig(0);
  

  MIDI.begin();//Serial1.begin(MIDI_BAUD);
  //MIDI.setHandleNoteOn(HandleNoteOn);                     // [zabox] [1.028] moved bc expander mode
  //MIDI.setHandleNoteOff(HandleNoteOff);                   // 
  MIDI.setInputChannel(seq.RXchannel);
  MIDI.turnThruOff();                                       // [zabox] fixes double real time messages on midi out


#if DEBUG
  Serial.begin(115200);
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());

#endif
  sei();

  //-----------------------------------------------

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("NAVA v1.028beta ");
  lcd.setCursor(0,1);
  lcd.print("by e-licktronic ");
  delay(1000);
  LcdUpdate();                                              // [1.028] if started in expader mode

}

////////////////////////Loop///////////////////////
void loop()
{
  
  Expander();                                                             // [1.028] expander
  SetTrigPeriod(TRIG_LENGHT);
  InitMidiRealTime();
  MIDI.read();
  //SetMux();//!!!! if SetMUX() loop there is noise on HT out and a less noise on HH noise too !!!!
  ButtonGet(); 
  EncGet();
  
  if (ledUpdateCounter > 2) {                                             // [zabox] [1.028] smooth leds (combine 3 loop cycles, reduces update rate from ~220hz with running secuencer to 80hz)
    SetLeds();
    ledUpdateCounter = 0;
  } ledUpdateCounter++;
  
  SeqConfiguration();
  SeqParameter();
  KeyboardUpdate();
  LcdUpdate();
  

  

   
  
#if DEBUG
    if (stepValue) {
      if (stepValue != stepValue_old) {
       // Serial.println(stepValue, BIN);
       
       
        stepValue_old = stepValue;
      }
    }
    if (ppqn != ppqn_old) {
      //Serial.println(ppqn);
      ppqn_old = ppqn;
    }
    
    
#endif



}




























