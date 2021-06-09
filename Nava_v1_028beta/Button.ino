//-------------------------------------------------
//                  NAVA v1.x
//              Sequencer Button
//-------------------------------------------------

////////////////////////Function//////////////////////

void ButtonGet()
{
  ScanDin();
  byte firstByte = dinSr[2];
  byte secondByte = dinSr[3];
  byte thirdByte = dinSr[4];

  instBtn =  ((firstByte & BTN_INST) ? 1:0);
  shiftBtn = ((firstByte & BTN_SHIFT) ? 1:0);

  ButtonGet (&clearBtn, firstByte & BTN_CLEAR);
  ButtonGet (&trkBtn, secondByte & BTN_TRK);
  ButtonGet (&ptrnBtn, secondByte & BTN_PTRN);
  ButtonGet (&tapBtn, secondByte & BTN_TAP);
  ButtonGet (&dirBtn, secondByte & BTN_DIR);
  ButtonGet (&playBtn, firstByte & BTN_PLAY);
  ButtonGet (&stopBtn, firstByte & BTN_STOP);
  ButtonGet (&guideBtn, secondByte & BTN_GUIDE);
  ButtonGet(&scaleBtn, firstByte & BTN_SCALE);
  ButtonGet(&lastStepBtn, firstByte & BTN_LASTSTEP);
  ButtonGet(&shufBtn, firstByte & BTN_SHUF);
  ButtonGet(&backBtn, secondByte & BTN_BACK);
  ButtonGet(&fwdBtn, secondByte & BTN_FWD);
  ButtonGet(&tempoBtn, thirdByte & BTN_TEMPO);
  ButtonGet(&numBtn, secondByte & BTN_NUM);
  ButtonGet(&muteBtn, thirdByte & BTN_MUTE);
  ButtonGet(&bankBtn, thirdByte & BTN_BANK);
  ButtonGet(&encBtn, encSwState);
  ButtonGet(&stepsBtn, ((dinSr[1] <<8) | dinSr[0]));

  //Enter button-----------------------------------------------
  enterBtn.justRelease = 0;
  enterBtn.justPressed = 0;
  enterBtn.curState = ((thirdByte & BTN_ENTER) ? 1:0);
  if (enterBtn.curState != enterBtn.prevState){
    if (enterBtn.pressed == LOW && enterBtn.curState == HIGH){
      enterBtn.justPressed = HIGH;
      enterBtn.curTime = millis();
    }
    if (enterBtn.pressed == HIGH && enterBtn.curState == LOW){
      enterBtn.justRelease = HIGH;
      enterBtn.hold = LOW;
    }
    enterBtn.pressed = enterBtn.curState;
  }
  enterBtn.prevState = enterBtn.curState;

  if (enterBtn.pressed){
    if (millis() - enterBtn.curTime > HOLD_TIME) enterBtn.hold = HIGH;
  }

  //init step button------------------------------------------------
  for (byte a = 0; a < NBR_STEP_BTN; a++){//a = step button number
    stepBtn[a].justPressed = 0;
    stepBtn[a].justRelease = 0;
  }
}

//==========================================================================================
//==========================================================================================

//Init buttons state
void ButtonGet( Button *button, unsigned int pin)
{
  button->justRelease = 0;
  button->justPressed = 0;
  button->curState = ((pin) ? 1:0);
  if (button->curState != button->prevState){
    if (button->pressed == LOW && button->curState == HIGH) button->justPressed = HIGH;
    if (button->pressed == HIGH && button->curState == LOW) button->justRelease = HIGH;
    button->pressed = button->curState;
  }
  button->prevState = button->curState;
}

//Return a unsigned 16bits value of the steps buttons-------------------------------- 
// with two differne tmode TOGGLE and MOMENTARY
unsigned int StepButtonGet(byte mode)
{
  unsigned int data = (dinSr[1] <<8) | dinSr[0];
  static unsigned int value;
  switch (mode){
  case TOGGLE:
    for (byte a=0; a<NBR_STEP_BTN; a++){
      stepBtn[a].justPressed = 0;
      stepBtn[a].curState = bitRead(data,a);
      if (stepBtn[a].curState != stepBtn[a].prevState){
        if ((stepBtn[a].pressed == LOW) && (stepBtn[a].curState == HIGH)){
          stepBtn[a].justPressed = 1;
          stepBtn[a].counter++;//incremente step button counter
          switch (stepBtn[a].counter){
          case 1:
            bitSet (value, a);
            break;
          case 2:
            stepBtn[a].counter = 0;
            bitClear (value, a);
            break;
          }
        }      
      }
      stepBtn[a].prevState = stepBtn[a].curState;
    }
    return value; 
    break;
  case MOMENTARY:
    return data;
    break;
  }
}

unsigned int InstValueGet(unsigned int value)
{
  unsigned int reading = (dinSr[1] << 8) | dinSr[0];
  for (byte a = 0; a < NBR_STEP_BTN; a++){//a = step button number
    stepBtn[a].justPressed = 0;
    stepBtn[a].curState = bitRead(reading,a);
    if (stepBtn[a].curState != stepBtn[a].prevState){
      if ((stepBtn[a].pressed == LOW) && (stepBtn[a].curState == HIGH)){
        patternWasEdited = TRUE;
        stepBtn[a].justPressed = 1;

        //two button state
        if(curInst == OH || curInst == RIDE || curInst == CRASH || curInst == TOTAL_ACC || curInst == TRIG_OUT || curInst == EXT_INST){
          if (bitRead(value,a)) stepBtn[a].counter = 2;
          else stepBtn[a].counter = 1;
          switch (stepBtn[a].counter){
          case 1:
            bitSet (value, a);
            pattern[ptrnBuffer].velocity[curInst][a] = instVelHigh[curInst];//HIGH_VEL;
            break;
          case 2:
            stepBtn[a].counter = 0;
            bitClear (value, a);
            break;
          }
        }
        //three button state flam
        else if (curFlam) {                                                                                      // [zabox] [1.027] flam
          if (bitRead(value,a)){ 
            //if vel > MID_VEL
            if ((pattern[ptrnBuffer].velocity[curInst][a]) & 128) {
              if (((pattern[ptrnBuffer].velocity[curInst][a]) & 127) > instVelLow[curInst]) stepBtn[a].counter = 3;       // flam test, to be removed
              else stepBtn[a].counter = 2;         
            }
            else {
              stepBtn[a].counter = 3;
            }
          }
          else {
            stepBtn[a].counter = 1;
          }
          switch (stepBtn[a].counter){
            //half velocity
          case 1:                        
            bitSet (value, a);
            pattern[ptrnBuffer].velocity[curInst][a] = instVelLow[curInst] + 128;
            break;
            //full velocity
          case 2:
            pattern[ptrnBuffer].velocity[curInst][a] = instVelHigh[curInst] + 128;
            break;
            //Off
          case 3:
            stepBtn[a].counter = 0;
            bitClear (value, a);
            //pattern[ptrnBuffer].velocity[curInst][a] = 200;
            break;
          }
        }

        //three button state
        else {
          if (bitRead(value,a)){
            //if vel > MID_VEL
            if (!((pattern[ptrnBuffer].velocity[curInst][a]) & 128)) {                                                       // [zabox] [1.027] check if flam (bit7 = & 128)                        
              if ((pattern[ptrnBuffer].velocity[curInst][a]) > instVelLow[curInst]) stepBtn[a].counter = 3;      
              else stepBtn[a].counter = 2; 
            }
            else {
              stepBtn[a].counter = 3;
            }         
          }
          else {
            stepBtn[a].counter = 1;
          }
          switch (stepBtn[a].counter){
            //half velocity
          case 1:                        
            bitSet (value, a);
            pattern[ptrnBuffer].velocity[curInst][a] = instVelLow[curInst];
            break;
            //full velocity
          case 2:
            pattern[ptrnBuffer].velocity[curInst][a] = instVelHigh[curInst];
            break;
            //Off
          case 3:
            stepBtn[a].counter = 0;
            bitClear (value, a);
            //pattern[ptrnBuffer].velocity[curInst][a] = 200;
            break;
          }
        }
      }  
      stepBtn[a].prevState = stepBtn[a].curState;    
    }
  }
  return value;
}





//Mute steps buttons -----------------------------------------------------------               [1.028] new version. faster, doens't need a button counter
void MuteButtonGet()
{
  lastMuteButtons = muteButtons;
  muteButtons = (dinSr[1] << 8) | dinSr[0];

  if (muteButtons != lastMuteButtons) {                                                     // [1.028] runs only when a button is pressed (important for expander mode)
    
    for (byte a = 0; a < NBR_STEP_BTN; a++) {
      
      if (((muteButtons >> a) & 1U) && !((lastMuteButtons >> a) & 1U)) {
              
        muteInst ^= (1 << muteOut[a]);
        muteLeds ^= muteLedsOrder[a];
        
        if (seq.muteModeHH) {                                        
          if (a == 12) {
            muteInst ^= (1 << muteOut[13]);
            muteLeds ^= muteLedsOrder[13];
          }
          else if (a == 13) {
            muteInst ^= (1 << muteOut[12]);
            muteLeds ^= muteLedsOrder[12];
          }
        }      
      }      
    }
  }
}



//Mute steps buttons -----------------------------------------------------------               [1.028] gate
void GateButtonGet()
{
  lastGateButtons = gateButtons;
  //gateButtons = dinSr[1] & B11111100;
  gateButtons = (dinSr[1] << 8) | dinSr[0];

  if (gateButtons != lastGateButtons) {                                                     // [1.028] runs only when a button is pressed (important for expander mode)
    
    for (byte a = 0; a < NBR_STEP_BTN; a++) {
      
      if (((gateButtons >> a) & 1U) && !((lastGateButtons >> a) & 1U)) {
              
        gateInst ^= (1 << muteOut[a]);
        gateLeds ^= muteLedsOrder[a];
        
      }      
    }
  }
}




//return value of first pressed step button
byte FirstBitOn()
{
  static unsigned int tempStepBtnValue;
  tempStepBtnValue = StepButtonGet(MOMENTARY);
  for (int a = 0; a < NBR_BTN_STEP; a++){
    if (bitRead (tempStepBtnValue, a)){
      return a;
      break;
    }
  }
}

//return value of second pressed step button
byte SecondBitOn()
{
  static unsigned int tempStepBtnValue;
  byte counter = 0;
  byte value = 0;
  tempStepBtnValue = StepButtonGet(MOMENTARY);
  for (int a = 0; a < NBR_BTN_STEP; a++){
    if (bitRead (tempStepBtnValue, a)){
      counter++;
      if (counter == 2){
        value = a;
        break;
      }
    }
  }
  return value;
}

//return value of first just pressed step button 
//MODE => JUSTPRESSED or JUSTRELEASE
/*byte StepBtn(byte mode)
 {
 unsigned int reading = (dinSr[1] <<8) | dinSr[0];
 for (byte a = 0; a < NBR_STEP_BTN; a++){//a = step button number
 stepBtn[a].justPressed = 0;
 stepBtn[a].justRelease = 0;
 stepBtn[a].curState = bitRead(reading,a);
 if (stepBtn[a].curState != stepBtn[a].prevState){
 if (stepBtn[a].pressed == LOW && stepBtn[a].curState == HIGH){
 stepBtn[a].justPressed = 1;
 if (mode == JUSTPRESSED) return a;
 }
 if (stepBtn[a].pressed == HIGH && stepBtn[a].curState == LOW){
 stepBtn[a].justRelease = 1;
 if (mode == JUSTRELEASE) return a;
 }
 stepBtn[a].pressed = stepBtn[a].curState;
 }
 stepBtn[a].prevState = stepBtn[a].curState;
 }
 }*/

//Init buttons counter----------------------------------------------------
void InitButtonCounter()
{
  for (byte a = 0; a < NBR_STEP_BTN; a++){
    stepBtn[a].counter = 0;
  }
  stopBtn.counter = 0;
  guideBtn.counter = 0;
  scaleBtn.counter = 0;
  encBtn.counter = 0;
  muteBtn.counter = 0;
}

//Init  mute buttons counter----------------------------------------------------      [1.028] new MuteButtonGet function doesn't need this.
/*
void InitMuteBtnCounter()
{
  for (byte a=0; a<NBR_STEP_BTN; a++){
    muteStepBtn[a].counter = 0;
  }
}

*/



//Mute steps buttons -----------------------------------------------------------      [1.028] old version
/*
void MuteButtonGet()
{
  unsigned int data = (dinSr[1] <<8) | dinSr[0];
  //static unsigned int value;
  for (byte a=0; a<NBR_STEP_BTN; a++){
    muteStepBtn[a].justPressed = 0;
    muteStepBtn[a].curState = bitRead(data,a);
    if (muteStepBtn[a].curState != muteStepBtn[a].prevState){
      if ((muteStepBtn[a].pressed == LOW) && (muteStepBtn[a].curState == HIGH)){
        muteStepBtn[a].justPressed = 1;
        switch(a){
        case 0:
        case 1:
          muteStepBtn[0].counter++;//incremente step button counter
          muteStepBtn[1].counter++;//incremente step button counter
          break;
        case 2:
        case 3:
          muteStepBtn[2].counter++;//incremente step button counter
          muteStepBtn[3].counter++;//incremente step button counter
          break;
        case 4:
        case 5:
          muteStepBtn[4].counter++;//incremente step button counter
          muteStepBtn[5].counter++;//incremente step button counter
          break;
        case 6:
        case 7:
          muteStepBtn[6].counter++;//incremente step button counter
          muteStepBtn[7].counter++;//incremente step button counter
          break;
        case 8:
        case 9:
          muteStepBtn[8].counter++;//incremente step button counter
          muteStepBtn[9].counter++;//incremente step button counter
          break;
        case 10:
          muteStepBtn[10].counter++;//incremente step button counter
          break;
        case 11:
          muteStepBtn[11].counter++;//incremente step button counter
          break;
        case 12:
          muteStepBtn[12].counter++;//incremente step button counter                     // [zabox] OH/CH mute
          break;
        case 13:
          muteStepBtn[13].counter++;//incremente step button counter
          break;
        case 14:
          muteStepBtn[14].counter++;//incremente step button counter
          break;
        case 15:
          muteStepBtn[15].counter++;//incremente step button counter
          break;
        }
        switch (muteStepBtn[a].counter){
        case 1:
          bitSet (muteInst,muteOut[a]);
          muteLeds |= muteLedsOrder[a];
          
          if (seq.muteModeHH) {                                                         // [zabox] HH mute mode
            if (a == 12) {
              bitSet (muteInst,muteOut[13]);
              muteLeds |= muteLedsOrder[13];
              muteStepBtn[13].counter++;
            }
            else if (a == 13) {
              bitSet (muteInst,muteOut[12]);
              muteLeds |= muteLedsOrder[12];
              muteStepBtn[12].counter++;
            }
          }
          break;
        case 2:
          switch(a){
          case 0:
          case 1:
            muteStepBtn[0].counter = muteStepBtn[1].counter = 0;
            break;
          case 2:
          case 3:
            muteStepBtn[2].counter = muteStepBtn[3].counter = 0;
            break;
          case 4:
          case 5:
            muteStepBtn[4].counter = muteStepBtn[5].counter = 0;
            break;
          case 6:
          case 7:
            muteStepBtn[6].counter = muteStepBtn[7].counter = 0;
            break;
          case 8:
          case 9:
            muteStepBtn[8].counter = muteStepBtn[9].counter = 0;
            break;
          case 10:
            muteStepBtn[10].counter = 0;
            break;
          case 11:
            muteStepBtn[11].counter = 0;
            break;
          case 12:
            muteStepBtn[12].counter = 0;                                       // [zabox] OH/HC mute
            break;
          case 13:
            muteStepBtn[13].counter = 0;
            break;
          case 14:
            muteStepBtn[14].counter = 0;
            break;
          case 15:
            muteStepBtn[15].counter = 0;
            break;
          }
          muteStepBtn[a].counter = 0;
          bitClear (muteInst,muteOut[a]);
          muteLeds ^= muteLedsOrder[a];
          
          if (seq.muteModeHH) {                                                // [zabox] mute mode
            if (a == 12) {
              bitClear (muteInst,muteOut[13]);
              muteLeds ^= muteLedsOrder[13];
              muteStepBtn[13].counter = 0;
            }
            else if (a == 13) {
              bitClear (muteInst,muteOut[12]);
              muteLeds ^= muteLedsOrder[12];
              muteStepBtn[12].counter = 0;
            }
          }
          break;
          
        }
      }      
    }
    muteStepBtn[a].prevState = muteStepBtn[a].curState;
  }
}
*/
















