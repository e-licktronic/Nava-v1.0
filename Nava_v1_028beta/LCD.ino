//-------------------------------------------------
//                  NAVA v1.x
//                  LCD update
//-------------------------------------------------

#include "string.h"

/////////////////////Function//////////////////////
//Initialise IO PORT and libraries
void LcdUpdate()
{
  static byte previousMode;
  //display tempo
  if (tempoBtn.pressed) {                                                         // [1.028] 
    if(curSeqMode != PTRN_PLAY && !shiftBtn && !seq.configMode) {                 // + !seq.configMode
        lcd.setCursor(11,1);
        LcdPrintTempo();
    }
  }
  //display total  accent
  /*if (enterBtn.pressed && curSeqMode == PTRN_PLAY){
   LcdPrintTotalAcc();
   }*/
  if(needLcdUpdate){
    
    needLcdUpdate = FALSE;
    if (seq.configMode)
    {
      lcd.setCursor(0,0);
      switch (seq.configPage){
      case 1:// first page
        lcd.print("syn bpm mTX mRX ");
        lcd.setCursor(cursorPos[curIndex],0);
        lcd.print(letterUpConfPage1[curIndex]);
        lcd.setCursor(0,1);
        LcdClearLine();
        lcd.setCursor(0,1);
        char  sync[2];
        strcpy_P(sync, (char*)pgm_read_word(&(nameSync[seq.sync])));
        lcd.print(sync);
        lcd.setCursor(4,1);
        lcd.print(seq.defaultBpm);
        lcd.setCursor(9,1);
        lcd.print(seq.TXchannel);
        lcd.setCursor(13,1);
        lcd.print(seq.RXchannel);
        break;
      case 2:// second page
        lcd.print("pCh mte nc. nc. ");                             // [zabox]
        lcd.setCursor(cursorPos[curIndex],0);
        lcd.print(letterUpConfPage2[curIndex]);
        lcd.setCursor(0,1);
        LcdClearLine();
        lcd.setCursor(0,1);
        char  ptrnSyncChange[2];
        strcpy_P(ptrnSyncChange, (char*)pgm_read_word(&(namePtrnChange[seq.ptrnChangeSync])));
        lcd.print(ptrnSyncChange);
        lcd.setCursor(4,1);
        char  mute[2];
        strcpy_P(mute, (char*)pgm_read_word(&(nameMute[seq.muteModeHH])));
        lcd.print(mute);
        lcd.setCursor(8,1);
        lcd.print("xxx");
        lcd.setCursor(12,1);
        lcd.print("xxx");
        break;
      }

    }
    else if (seq.sync == EXPANDER) {                                               // [1.028] Expander
      lcd.setCursor(0,0);
      lcd.print("    Expander    ");
      lcd.setCursor(0,1);
      lcd.print("                ");
    }
    
    else{
      switch (curSeqMode){
      case PTRN_PLAY:
        lcd.setCursor(0,0);
        lcd.print("  Pattern Play  ");
        lcd.setCursor(0,1);
        LcdClearLine(); 
        lcd.setCursor(2,1);
        lcd.print(char(curBank+65));
        lcd.print(curPattern - (curBank*NBR_PATTERN) + 1);                          // [zabox] step button alignement
        lcd.setCursor(9,1);
        LcdPrintTempo(); 
        previousMode = PTRN_PLAY;
        break;
      case PTRN_STEP:
      case PTRN_TAP:
        if (curInst == TOTAL_ACC){
          LcdPrintTotalAcc();
        }
        else if (shufBtn.pressed){
  /*        
          if (shiftBtn) {                                                           // [zabox] [1.027] flam
            lcd.setCursor(0,0);
            lcd.print(" Flam value     ");
            lcd.setCursor(0,1);
            LcdClearLine();
            lcd.setCursor(1,1);
            lcd.print("-");
            lcd.setCursor(4,1);
            LcdPrintLine(8);
            lcd.setCursor(14,1);
            lcd.print("+");
            lcd.setCursor(3 + pattern[ptrnBuffer].flam,1);
            lcd.print((char)219);
          }
          else {
            lcd.setCursor(0,0);
            lcd.print(" Shuffle value  ");
            lcd.setCursor(0,1);
            LcdClearLine();
            lcd.setCursor(1,1);
            lcd.print("-");
            lcd.setCursor(4,1);
            LcdPrintLine(7);
            lcd.setCursor(14,1);
            lcd.print("+");
            lcd.setCursor(3 + pattern[ptrnBuffer].shuffle,1);
            lcd.print((char)219);
          }
          
          */
          
          lcd.setCursor(0,0);
          lcd.print("Shuffle:        ");
          lcd.setCursor(9,0);
          LcdPrintLine(7);
          lcd.setCursor(8 + pattern[ptrnBuffer].shuffle, 0);
          lcd.print((char)219);
          lcd.setCursor(0,1);
          lcd.print("Flam:           ");
          lcd.setCursor(8,1);
          LcdPrintLine(8);
          lcd.setCursor(8 + pattern[ptrnBuffer].flam, 1);
          lcd.print((char)219);        
          
        }
        else if (keyboardMode){
          lcd.setCursor(0,0);
          lcd.print("idx not len oct ");
          lcd.setCursor(cursorPos[curIndex],0);
          lcd.print(letterUpExtInst[curIndex]);
          lcd.setCursor(0,1);
          LcdClearLine();
          lcd.setCursor(1,1);
          lcd.print(noteIndex + 1);                                               // [zabox] looks better
          lcd.setCursor(4,1);
          char note[2];
          strcpy_P(note, (char*)pgm_read_word(&(nameNote[pattern[ptrnBuffer].extNote[noteIndex] % 12])));
          lcd.print(note);
          lcd.print(pattern[ptrnBuffer].extNote[noteIndex] / 12);//note octave
          lcd.setCursor(9,1);
          lcd.print(pattern[ptrnBuffer].extLength + 1);
          lcd.setCursor(13,1);
          char octave[2];
          strcpy_P(octave, (char*)pgm_read_word(&(nameOct[keybOct])));
          lcd.print(octave);
        }
        else{
          lcd.setCursor(0,0);
          lcd.print("ptr len scl ins ");
          lcd.setCursor(0,1);
          LcdClearLine(); 
          lcd.setCursor(0,1);
          lcd.print(char(curBank+65));
          lcd.print(curPattern - (curBank*NBR_PATTERN) + 1);                     // [zabox] step button alignement
          lcd.setCursor(5,1);
          lcd.print(pattern[ptrnBuffer].length+1);
          lcd.setCursor(8,1);
          LcdPrintScale();
          lcd.setCursor(12,1);
          char instName[3];
          strcpy_P(instName, (char*)pgm_read_word(&(selectInstString[curInst])));
          if (curFlam) {                                                            // test
            instName[1] = instName[1] + 32;
          }
          lcd.print(instName);
        }
        previousMode = PTRN_STEP;
        break;
      case MUTE:
        if (previousMode == PTRN_STEP){
          lcd.setCursor(0,1);
          lcd.print("   ");
          lcd.setCursor(0,1);
          lcd.print(char(curBank+65));
          lcd.print(curPattern - (curBank*NBR_PATTERN) + 1);                     // [zabox] step button alignement
        }
        else if (previousMode == PTRN_PLAY){
          lcd.setCursor(2,1);
          lcd.print("   ");
          lcd.setCursor(2,1);
          lcd.print(char(curBank+65));
          lcd.print(curPattern - (curBank*NBR_PATTERN) + 1);                     // [zabox] step button alignement
        }

        break;
      case TRACK_WRITE:
        lcd.setCursor(0,0);
        lcd.print("pos ptr len num ");
        lcd.setCursor(cursorPos[curIndex],0);
        lcd.print(letterUpTrackWrite[curIndex]);
        lcd.setCursor(0,1);
        LcdClearLine();
        lcd.setCursor(0,1);
        lcd.print(trk.pos + 1);                                                 // [zabox] 
        lcd.setCursor(4,1);
        lcd.print((char)((curPattern / 16) + 65));
        lcd.print((curPattern - (((curPattern / 16)*NBR_PATTERN)) + 1));        // [zabox] step button alignement
        lcd.setCursor(8,1);
        lcd.print(track[trkBuffer].length);
        lcd.setCursor(13,1);
        lcd.print(trk.current + 1);
        previousMode = TRACK_WRITE;
        break;
      case TRACK_PLAY:
        lcd.setCursor(0,0);
        lcd.print("   Track Play   ");
        lcd.setCursor(0,1);
        LcdClearLine(); 
        lcd.setCursor(0,1);
        lcd.print("pos:");
        lcd.print(trk.pos + 1);                                                 // [zabox]
        lcd.setCursor(8,1);
        lcd.print("ptrn:");
        lcd.print((char)((curPattern / 16) + 65));
        lcd.print((curPattern - (((curPattern / 16)*NBR_PATTERN)) + 1));        // [zabox] step button alignement
         previousMode = TRACK_PLAY;
        break;
      }
    }
  }
}

//clear 16 character line-------------------------------------
void LcdClearLine()
{
  lcd.print("                ");//16 empty space to clear a line
}

//print special character for scale moniotring----------------
void LcdPrintScale()
{
  switch (pattern[ptrnBuffer].scale){
  case SCALE_32:
    lcd.write(byte(0));//1/
    lcd.write(byte(4));//32
    break;
  case  SCALE_16t:
    lcd.write(byte(0));//1/
    lcd.write(byte(1));//16
    lcd.write(byte(2));//t
    break;
  case  SCALE_16:
    lcd.write(byte(0));//1/
    lcd.write(byte(1));//16
    break;
  case  SCALE_8t:
    lcd.write(byte(0));//1/
    lcd.write(byte(3));//8
    lcd.write(byte(2));//t
    break;
  }
}

//Print tempo---------------------------------------------------------
void LcdPrintTempo()
{
  lcd.write(byte(5));
  lcd.print("-");
  if (seq.sync == MASTER)lcd.print(seq.bpm);
  else {
    lcd.print((char)219);
    lcd.print((char)219);
    lcd.print((char)219);
  }
}

//print special line--------------------------------------------------
void LcdPrintLine (byte lineSize)
{
  for (int a = 0; a < lineSize; a++){
    lcd.print((char) 161);
  }
}

//Lcd print saved !!!!
void LcdPrintSaved()
{
  lcd.setCursor(4,0);
  lcd.print("[SAVED!]");
}

//Lcd print Total Accent
void LcdPrintTotalAcc()
{
  lcd.setCursor(0,0);
  lcd.print("Total Acc value ");
  lcd.setCursor(0,1);
  LcdClearLine();
  lcd.setCursor(0,1);
  lcd.print("-");
  lcd.setCursor(1,1);
  LcdPrintLine(14);
  lcd.setCursor(15,1);
  lcd.print("+");
  lcd.setCursor(1 + pattern[ptrnBuffer].totalAcc, 1);
  lcd.print((char)219);
}

//Lcd print initialize EEprom
void  LcdPrintEEpromInit()
{
  lcd.setCursor(0,0);
  lcd.print("  init EEprom ? ");
  lcd.setCursor(0,1);
  LcdClearLine();
  lcd.setCursor(0,1);
  lcd.print("press PLAY/ENTER");

}


//Lcd print TM2 adjustement
void  LcdPrintTM2Adjust()
{
  lcd.setCursor(0,0);
  lcd.print("Adjust TM2 until");
  lcd.setCursor(0,1);
  LcdClearLine();
  lcd.setCursor(0,1);
  lcd.print("TP1 is +5V...");
}














