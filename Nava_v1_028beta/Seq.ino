//-------------------------------------------------
//                  NAVA v1.x
//                 SEQ Parameter
//-------------------------------------------------

/////////////////////Function//////////////////////
void SeqParameter()
{  
  readButtonState = StepButtonGet(MOMENTARY);
  //can not access config when isRunning
  if (isRunning && seq.configMode){
    seq.configMode = FALSE;
    needLcdUpdate = TRUE;
  }
  if (curSeqMode != MUTE) muteBtn.counter = 0;
  if (!seq.configMode) seq.configPage = 0;

  //-------------------Encoder button---------------------------
  if(encBtn.justPressed){
    curIndex++;
    if (curIndex >= MAX_CUR_POS)  curIndex = 0;
    needLcdUpdate = TRUE;
  }
  //-------------------play button---------------------------
  if (playBtn.justPressed || midiStart){
    isRunning = TRUE;
    isStop = FALSE;
    ppqn = 0;
    stepCount = 0;
    tapStepCount = 0;
    changeDir = 1;//restart Forward
    stopBtn.counter = 0;
    shufPolarity = 0;//Init shuffle polarity
    noteIndexCpt = 0;//init ext instrument note index counter
    blinkTempo = 0;                                                               // [zabox] looks more consistent
    //trackPosNeedIncremante = TRUE;
    //init Groupe pattern  position
    /*group.pos = 0;
     nextPattern = group.firstPattern + group.pos;
     if(curPattern != nextPattern) selectedPatternChanged = TRUE;
     trackPosNeedIncremante = FALSE;
     needLcdUpdate = TRUE;*/

    MIDI.sendRealTime(Start);  //;MidiSend(START_CMD);
      DIN_START_HIGH;
      dinStartState = HIGH;
//    DIN_CLK_HIGH;                                                               // [1.028] redundant
//    dinClkState = HIGH;
  }

  //-------------------stop button------------------------------
  if ((stopBtn.justPressed && !instBtn) || midiStop || midiContinue){
    //Init Midi note off
    SendAllNoteOff();//InitMidiNoteOff();
    if (midiStop) stopBtn.counter = 0;
    else if (midiContinue) stopBtn.counter = 1;
    stopBtn.counter++;
    switch (stopBtn.counter){
    case 1:
      isStop = TRUE;
      isRunning = FALSE;
      MIDI.sendRealTime(Stop);//;MidiSend(STOP_CMD);
      DIN_START_LOW;
      dinStartState = LOW;
      break;
    case 2:
      isStop = FALSE;
      isRunning = TRUE;
      stopBtn.counter = 0;
      ppqn = 0;
      MIDI.sendRealTime(Continue);//MidiSend(CONTINU_CMD);
      DIN_START_HIGH;
      dinStartState = HIGH;
      break;
    }
  }

  //-------------------Shift button pressed------------------------------
  if (shiftBtn){
    if (trkBtn.justPressed){
      needLcdUpdate = TRUE;
      curSeqMode = TRACK_WRITE;
      keyboardMode = FALSE;
      seq.configMode  = FALSE;
    }
    if (ptrnBtn.justPressed) {
      needLcdUpdate = TRUE;
      curSeqMode = PTRN_STEP;
      seq.configMode  = FALSE;
      trackNeedSaved = FALSE;
    }
    if (tapBtn.justPressed){
      curSeqMode = PTRN_TAP;
      needLcdUpdate = TRUE;
      keyboardMode = FALSE;
      seq.configMode  = FALSE;
      trackNeedSaved = FALSE;
    }
    if (bankBtn.justPressed){
      CopyPatternToBuffer(curPattern);
    } // copy current pattern to the buffer
    if (muteBtn.justPressed){
      PasteBufferToPattern(curPattern);
      patternWasEdited = TRUE;
    } //paste copy buffered pattern to the current pattern number

    //sequencer configuration page
    if (tempoBtn.justPressed && !isRunning){
      seq.configMode  = TRUE;
      seq.configPage++;
      curIndex = 0;
      if (seq.configPage > MAX_CONF_PAGE){
        seq.configPage = 0;
        seq.configMode  = FALSE;
      }
      needLcdUpdate = TRUE;
    }
  }
  //-------------------Shift button released------------------------------
  else {
    if (trkBtn.justPressed){
      curSeqMode = TRACK_PLAY;
      needLcdUpdate = TRUE;
      keyboardMode = FALSE;
      seq.configMode  = FALSE;
    }
    // if (backBtn.justPressed) ;//back  track postion
    //if (fwdBtn.justPressed) ;//foward track postion
    if (numBtn.pressed) ;//select Track number
    if (ptrnBtn.justPressed){
      if (curSeqMode == PTRN_STEP) curSeqMode = PTRN_PLAY;
      else curSeqMode = PTRN_STEP;
      needLcdUpdate = TRUE;
      keyboardMode = FALSE;
      seq.configMode  = FALSE;
      trackNeedSaved = FALSE;
    }
    if (tapBtn.justPressed) ShiftLeftPattern();
    if (dirBtn.justPressed) ShiftRightPattern();
    if (guideBtn.justPressed){
      guideBtn.counter++;
      switch (guideBtn.counter){
      case 1:
        Metronome(TRUE);
        break;
      case 2:
        Metronome(FALSE);
        guideBtn.counter = 0;
        break;
      }
    }
    //if (bankBtn && readButtonState) curBank = FirstBitOn();
    if (muteBtn.justPressed){
      muteBtn.counter++;
      switch (muteBtn.counter){
      case 1:
        prevSeqMode = curSeqMode;
        curSeqMode = MUTE; //paste copy buffered pattern to the current pattern number
        break;
      case 2:
        curSeqMode = prevSeqMode;
        muteBtn.counter = 0;
        break;
      }
    }
    if (curSeqMode != MUTE) muteBtn.counter = 0;
    if (!seq.configMode) seq.configPage = 0;
    if (tempoBtn.justRelease) needLcdUpdate = TRUE;
  }

  //==============================================================================
  //////////////////////////MODE PATTERN EDIT/////////////////////////////////

  if (curSeqMode == PTRN_STEP || curSeqMode == PTRN_TAP){

    static boolean curInstChanged;//flag that curInstchanged to not update LCD more than one tiem

    //-------------------Select instrument------------------------------
    //Match with trig shift register out (cf schematic)
    if(readButtonState == 0) doublePush = 0; //init double push if all step button  released 

    if (instBtn && readButtonState){          // [zabox] [1.027] added flam
      curInstChanged = TRUE;
      keyboardMode = FALSE;
      switch(FirstBitOn()){
      case BD_BTN:
      case BD_LOW_BTN:
        curInst = BD;
        if (doublePush == 0){
          curFlam = 0;
        }
        break;
      case SD_BTN:
      case SD_LOW_BTN:
        curInst = SD;
        if (doublePush == 0){
          curFlam = 0;
        }
        break;
      case LT_BTN:
      case LT_LOW_BTN:
        curInst = LT;
        if (doublePush == 0){
          curFlam = 0;
        }
        break;
      case MT_BTN:
      case MT_LOW_BTN:
        curInst = MT;
        if (doublePush == 0){
          curFlam = 0;
        }
        break;
      case HT_BTN:
      case HT_LOW_BTN:
        curInst = HT;
        if (doublePush == 0){
          curFlam = 0;
        }
        break;
      case RM_BTN:
        curInst = RM;
        curFlam = 0;
        break;
      case HC_BTN:
        curInst = HC;
        curFlam = 0;
        break;
      case CH_BTN:
      case CH_LOW_BTN:
        curFlam = 0;
        if (doublePush == 0){
          curInst = CH;
        }
        break;
      case RIDE_BTN:
        curInst = RIDE;
        curFlam = 0;
        break;
      case CRASH_BTN:
        curInst = CRASH;
        curFlam = 0;
        break;
      }
      //if (readButtonState == OH_BTN){
      //  curInst = OH;
      //  doublePush = 1;
      //}
      switch (readButtonState) {
        case BD_F_BTN:
          curFlam = 1;
          doublePush = 1;
        break;
        case SD_F_BTN:
          curFlam = 1;
          doublePush = 1;
        break;
        case LT_F_BTN:
          curFlam = 1;
          doublePush = 1;
        break;
        case MT_F_BTN:
          curFlam = 1;
          doublePush = 1;
        break;
        case HT_F_BTN:
          curFlam = 1;
          doublePush = 1;
        break;
        case OH_BTN:
          curInst = OH;
          doublePush = 1;
        break;
      }
        
        
        
        
    }
    if (curInstChanged && stepsBtn.justRelease){
      needLcdUpdate = TRUE;
      curInstChanged = FALSE;
    }
    if (instBtn && enterBtn.justPressed){
      curInst = TOTAL_ACC;
      needLcdUpdate = TRUE;
    }
    if (instBtn && stopBtn.justPressed){
      curInst = TRIG_OUT;
      needLcdUpdate = TRUE;
    }
    if (shiftBtn && guideBtn.justPressed){
      curInst = EXT_INST;
      needLcdUpdate = TRUE;
    }

    //-------------------Clear Button------------------------------
    if (clearBtn.pressed && !keyboardMode && curSeqMode != PTRN_TAP && isRunning){
      
      if (clearBtn.justPressed)  prev_muteInst = muteInst;                                                    // [zabox] save mute state
      
      muteInst |= (1 << curInst);                                                                             // [zabox] mute current instrument while holding clear
      
      bitClear (pattern[ptrnBuffer].inst[curInst], curStep);
      pattern[ptrnBuffer].velocity[curInst][curStep] = instVelLow[curInst];
      if (curInst == CH) pattern[ptrnBuffer].velocity[CH][curStep] = instVelHigh[HH];//update HH velocity that OH is trigged correctly    
      patternWasEdited = TRUE;   
    }
    
    if (clearBtn.justRelease) muteInst = prev_muteInst;                                                       // [zabox] unmute

    
    if (clearBtn.justPressed && !keyboardMode && curSeqMode != PTRN_TAP && !isRunning){
      //clear full pattern
      for (int a = 0; a < NBR_INST; a++){
        pattern[ptrnBuffer].inst[a] = 0;
      }
      //init all intrument velocity
      for (int b = 0; b < NBR_STEP; b++){
        pattern[ptrnBuffer].velocity[BD][b] = instVelLow[BD];
        pattern[ptrnBuffer].velocity[SD][b] = instVelLow[SD];
        pattern[ptrnBuffer].velocity[LT][b] = instVelLow[LT];
        pattern[ptrnBuffer].velocity[MT][b] = instVelLow[MT];
        pattern[ptrnBuffer].velocity[HT][b] = instVelLow[HT];
        pattern[ptrnBuffer].velocity[RM][b] = instVelLow[RM];
        pattern[ptrnBuffer].velocity[HC][b] = instVelLow[HC];
        pattern[ptrnBuffer].velocity[RIDE][b] = instVelLow[RIDE];
        pattern[ptrnBuffer].velocity[CRASH][b] = instVelLow[CRASH];
      }
      pattern[ptrnBuffer].shuffle = DEFAULT_SHUF;
      pattern[ptrnBuffer].flam = DEFAULT_FLAM;                                            // [1.028] flam
      pattern[ptrnBuffer].length = NBR_STEP - 1;
      pattern[ptrnBuffer].scale = SCALE_16;
      keybOct = DEFAULT_OCT;
      patternWasEdited = TRUE;
      needLcdUpdate = TRUE;
    }


    //-------------------shuffle Button------------------------------                          [zabox] test
    if (shufBtn.justPressed || shufBtn.justRelease) needLcdUpdate = TRUE;
    
    if (shufBtn.pressed) {
      
      if (readButtonState) {
        if (FirstBitOn() < 7) {
          pattern[ptrnBuffer].shuffle = FirstBitOn() + 1;
          if (pattern[ptrnBuffer].shuffle != prevShuf){
            
            lcd.setCursor(8 + prevShuf, 0);                                        // [zabox] [1.027] flam
            lcd.print((char) 161);                                                 //
            
            prevShuf = pattern[ptrnBuffer].shuffle;
            
            lcd.setCursor(8 + prevShuf, 0);                                        // [zabox] [1.027] flam
            lcd.print((char)219);                                                  //
            
            patternWasEdited = TRUE;
          }
        }  
        else if (FirstBitOn() > 7) {
          pattern[ptrnBuffer].flam = FirstBitOn() - 8;
          if (pattern[ptrnBuffer].flam != prevFlam){
            
            lcd.setCursor(8 + prevFlam, 1);                                        // [zabox] [1.027] flam
            lcd.print((char) 161);                                                 //
            
            prevFlam = pattern[ptrnBuffer].flam;
            
            lcd.setCursor(8 + prevFlam, 1);                                        // [zabox] [1.027] flam
            lcd.print((char)219);                                                  //
            
            patternWasEdited = TRUE;
          }
        }
      }
    }
      
    
    
    
    
    

    //-------------------scale button------------------------------
    if (scaleBtn.justPressed && !keyboardMode){
      needLcdUpdate = TRUE;
      patternWasEdited = TRUE;
      scaleBtn.counter++;
      if (scaleBtn.counter == 4) scaleBtn.counter = 0;
      switch (scaleBtn.counter){
      case 0:
        pattern[ptrnBuffer].scale =  SCALE_16;// 1/16
        break;
      case 1:
        pattern[ptrnBuffer].scale =  SCALE_32;// 1/32
        break;
      case 2:
        pattern[ptrnBuffer].scale =  SCALE_8t;// 1/8t 
        break;
      case 3:
        pattern[ptrnBuffer].scale =  SCALE_16t;// 1/16t 
        break;
      }
    } 

    //-------------------last step button------------------------------
    if (lastStepBtn.pressed && readButtonState) {
      pattern[ptrnBuffer].length = FirstBitOn();
      needLcdUpdate = TRUE;
      patternWasEdited = TRUE;
    }
    //-------------------Steps buttons------------------------------
    /////////////////////////////STEP EDIT////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (curSeqMode == PTRN_STEP){
      if (stepsBtn.justRelease) doublePush = FALSE; 
      if(!lastStepBtn.pressed && !instBtn && !keyboardMode && !shufBtn.pressed){                                      // [zabox] test
        if (isRunning)
        {
          pattern[ptrnBuffer].inst[curInst] = InstValueGet(pattern[ptrnBuffer].inst[curInst]);//cf InstValueGet()
        }
        else if (!isRunning)
        {//Return pattern number
          if (stepsBtn.pressed){
            if (bankBtn.pressed){
              if(FirstBitOn() >= MAX_BANK) curBank = MAX_BANK;
              else curBank = FirstBitOn();
              nextPattern = curBank * NBR_PATTERN + (curPattern % NBR_PATTERN);
              if(curPattern != nextPattern) selectedPatternChanged = TRUE;
              group.length = 0;
            }
            else{//pattern group edit------------------------------------------------------
              if (SecondBitOn())
              {
                trackPosNeedIncremante = FALSE;                                                               // [zabox] fixes group bug
                group.length = SecondBitOn() - FirstBitOn();
                nextPattern = group.firstPattern = FirstBitOn() + curBank * NBR_PATTERN;
                doublePush = TRUE;
                group.priority = TRUE;
                //Store groupe in eeprom
                if(enterBtn.justPressed){
                  group.priority = FALSE;
                  byte tempLength;
                  byte tempPos;
                  //Test if one the  selected pattern is already in a Group
                  for (int a = 0; a <= group.length; a++){
                    tempLength = LoadPatternGroup(group.firstPattern + a, LENGTH);
                    if (tempLength){
                      tempPos = LoadPatternGroup(group.firstPattern + a, POSITION);
                      ClearPatternGroup(group.firstPattern + a - tempPos, tempLength);
                    }
                  }
                  SavePatternGroup(group.firstPattern, group.length);
                }
              }
              else if (!doublePush){
                group.priority = FALSE;
                nextPattern = FirstBitOn() + curBank * NBR_PATTERN;
                if(enterBtn.justPressed){
                  ClearPatternGroup(nextPattern - pattern[ptrnBuffer].groupPos, pattern[ptrnBuffer].groupLength);
                  group.length = 0;
                }
                group.pos = pattern[ptrnBuffer].groupPos;
              }
              if(curPattern != nextPattern) selectedPatternChanged = TRUE;
            }
          }
        }
      }
      /*if (trackPosNeedIncremante && group.length ){//&& stepCount > 0)
       group.pos++;
       if (group.pos > group.length) group.pos = 0;
       nextPattern = group.firstPattern + group.pos;
       if(curPattern != nextPattern) selectedPatternChanged = TRUE;
       trackPosNeedIncremante = FALSE;
       needLcdUpdate = TRUE;
       }*/
    }
    //////////////////////////////TAP EDIT///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    else if (curSeqMode == PTRN_TAP)
    {
      //static byte lastHHtrigged;// remmeber last OH or CH was trigged to prevent OH noise when trigget other instruments


      
      if (clearBtn.pressed){                                                                           // [zabox] new function in tap mode: hold clear + inst in tap mode
      
        if (clearBtn.justPressed)  prev_muteInst = muteInst;                                           // [zabox] save mute state
        if (readButtonState) {
          
        byte clearInst;                                                         
          
        switch(FirstBitOn()){
        case BD_BTN:
        case BD_LOW_BTN:
          clearInst = BD;
          break;
        case SD_BTN:
        case SD_LOW_BTN:
          clearInst = SD;
          break;
        case LT_BTN:
        case LT_LOW_BTN:
          clearInst = LT;
          break;
        case MT_BTN:
        case MT_LOW_BTN:
          clearInst = MT ;
          break;
        case HT_BTN:
        case HT_LOW_BTN:
          clearInst = HT;
          break;
        case RM_BTN:
          clearInst = RM;
          break;
        case HC_BTN:
          clearInst = HC;
          break;
        case CH_BTN:
        case CH_LOW_BTN:
          if (doublePush == 0){
            clearInst = CH;
          }
          break;
        case RIDE_BTN:
          clearInst = RIDE;
          break;
        case CRASH_BTN:
          clearInst = CRASH;
          break;
        }
        if (readButtonState == OH_BTN){
          clearInst = OH;
          doublePush = 1;
        }
            
                                                 
        muteInst = prev_muteInst | (1 << clearInst);                                                            // [zabox] mute inst
      
        bitClear (pattern[ptrnBuffer].inst[clearInst], curStep);
        pattern[ptrnBuffer].velocity[clearInst][curStep] = instVelLow[clearInst];                               // [zabox] was missing here
        if (clearInst == CH) pattern[ptrnBuffer].velocity[CH][curStep] = HIGH_VEL;        //update HH velocity that OH is trigged correctly
        patternWasEdited = TRUE;
        }
        else if (muteInst != prev_muteInst) muteInst = prev_muteInst;                                           // [zabox] unmute 
      }
      
      if (clearBtn.justRelease) muteInst = prev_muteInst;                                                       // [zabox] unmute
      
      if (!lastStepBtn.pressed && !instBtn && !clearBtn.pressed && !shufBtn.pressed)                            // [zabox] test
      {
        static boolean doublePushOH;
        static byte tempVel[16];//store temp instrument velocity
        static byte triggedInst;

        if (bitRead(readButtonState,12) && bitRead(readButtonState,13)) doublePushOH = 1;
        else doublePushOH = 0;

        for (byte a = 0; a < NBR_STEP_BTN; a++){
          stepBtn[a].curState = bitRead(readButtonState,a);

          if (stepBtn[a].curState != stepBtn[a].prevState){
            if ((stepBtn[a].pressed == LOW) && (stepBtn[a].curState == HIGH)){
              //the "a" button is pressed NOW!
              switch (a){
              case BD_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[BD];
                tempVel[BD] = instVelHigh[BD];
                triggedInst = BD;
                break;
              case BD_LOW_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelLow[BD];
                tempVel[BD] = instVelLow[BD];
                triggedInst = BD;
                break;
              case SD_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[SD];
                tempVel[SD] = instVelHigh[SD];
                triggedInst = SD;
                break;
              case SD_LOW_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelLow[SD];
                tempVel[SD] = instVelLow[SD];
                triggedInst = SD;
                break;
              case LT_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[LT];
                tempVel[LT] = instVelHigh[LT];
                triggedInst = LT;
                break;
              case LT_LOW_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelLow[LT];
                tempVel[LT] = instVelLow[LT];
                triggedInst = LT;
                break;
              case MT_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[MT];
                tempVel[MT] = instVelHigh[MT];
                triggedInst = MT;
                break;
              case MT_LOW_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelLow[MT];
                tempVel[MT] = instVelLow[MT];
                triggedInst = MT;
                break;
              case HT_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[HT];
                tempVel[HT] = instVelHigh[HT];
                triggedInst = HT;
                break;
              case HT_LOW_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelLow[HT];
                tempVel[HT] = instVelLow[HT];
                triggedInst = HT;
                break;
              case CH_BTN:
                if (!doublePushOH){
                  if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[CH];
                  tempVel[CH] = instVelHigh[CH];
                  triggedInst = CH;
                }
                break;
              case CH_LOW_BTN:
                if (!doublePushOH){
                  if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelLow[CH];
                  tempVel[CH] = instVelLow[CH];
                  triggedInst = CH;
                }
                break;
              case RM_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[RM];
                tempVel[RM] = instVelHigh[RM];
                triggedInst = RM;
                break;
              case HC_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[HC];
                tempVel[HC] = instVelHigh[HC];
                triggedInst = HC;
                break;
              case CRASH_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[CRASH];
                tempVel[CRASH] = instVelHigh[CRASH];
                triggedInst = CRASH;
                break;
              case RIDE_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[RIDE];
                tempVel[RIDE] = instVelHigh[RIDE];
                triggedInst = RIDE;
                break;
              }
              if (doublePushOH){
            //  if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[OH];
                if (isRunning) pattern[ptrnBuffer].velocity[OH][tapStepCount] = instVelHigh[OH];                     // [zabox] fixes oh low velocity bug in tap mode (dim leds, no sound after mux fix)
                tempVel[OH] = instVelHigh[OH];
                triggedInst = OH;
              }

              //-----SET Velocity Values-----//
              SetMuxTrigMidi(triggedInst, tempVel[triggedInst]);

              if( a == CH_LOW_BTN || a == CH_BTN || readButtonState == OH_BTN){
                //If OH is tapped
                if (doublePushOH){
                  lastHHtrigged = 0;
                  if(isRunning){
                    bitSet(tempInst[OH],tapStepCount);
                    bitClear(tempInst[CH],tapStepCount);
                  }
                }
                //If CH is tapped
                else {
                  lastHHtrigged = B10;
                  if(isRunning){
                    bitSet(tempInst[CH],tapStepCount);
                    bitClear(tempInst[OH],tapStepCount);
                  }
                }
                while (TCCR2B) {};                                                            // [zabox] [1.028] wait until the last trigger is low again (checks if timer2 is running)
                SetDoutTrig((1 << HH) | lastHHtrigged);
              }
              else{
                while (TCCR2B) {};                                                            // [zabox] [1.028] wait until the last trigger is low again (checks if timer2 is running)
                //SetDoutTrig(1 << instOut[a] | lastHHtrigged);
                SetDoutTrig((1 << instOut[a]) | (lastDoutTrig & B11));                        // [zabox] [1.027] fixes hh cuts when tapping other instruments  
                if(isRunning) bitSet(tempInst[instOut[a]],tapStepCount);
              }
              delayMicroseconds(2000);
              //SetDoutTrig(lastHHtrigged);
              SetDoutTrig(lastDoutTrig & B11);                                                // [zabox] [1.027] fixes hh cuts when tapping other instruments  
            }      
          }
          stepBtn[a].prevState = stepBtn[a].curState;
        }//END FOR LOOP
      }

      if (endMeasure){//Update pattern at the end of measure to not get a double trig
        for (int inst = 0; inst < NBR_INST; inst++){
          if (tempInst[OH]){
            unsigned int i = tempInst[OH] & pattern[ptrnBuffer].inst[CH];
            pattern[ptrnBuffer].inst[CH] ^= i;
            pattern[ptrnBuffer].inst[OH] |= tempInst[OH];
            tempInst[OH] = 0;// init tempInst
            patternWasEdited = TRUE;
          }
          else if (tempInst[CH]){
            unsigned int i = tempInst[CH] & pattern[ptrnBuffer].inst[OH];
            pattern[ptrnBuffer].inst[OH] ^= i;
            pattern[ptrnBuffer].inst[CH] |= tempInst[CH];
            tempInst[CH] = 0;// init tempInst
            patternWasEdited = TRUE;
          }
          else if (tempInst[inst])//if instruments was edited
          {
            pattern[ptrnBuffer].inst[inst] |= tempInst[inst];
            tempInst[inst] = 0;// init tempInst
            patternWasEdited = TRUE;
          }
        }
      }
    }//END IF PTRN_TAP MODE
  }//ENDIF MODE EDIT

  //======================================================================================================================================================================================================================================
  //////////////////////////MODE PATTERN PLAY...//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (curSeqMode == PTRN_PLAY){
    //-------------------------------select pattern-----------------------------------
    if (stepsBtn.justRelease) doublePush = FALSE;
    if (readButtonState){
      if (bankBtn.pressed){
        if(FirstBitOn() >= MAX_BANK) curBank = MAX_BANK;
        else curBank = FirstBitOn();
        nextPattern = curBank * NBR_PATTERN + (curPattern % NBR_PATTERN);
        group.length = 0;//should be 0 to play the right next pattern
        if((curPattern != nextPattern) && !isRunning) selectedPatternChanged = TRUE;
      }
      else{
        //Group selected
        if (SecondBitOn())
        {
          group.length = SecondBitOn() - FirstBitOn();
          nextPattern = group.firstPattern = FirstBitOn() + curBank * NBR_PATTERN;
          if (isRunning && (seq.ptrnChangeSync == SYNC)) {                                                    // [zabox] fixes group bug while running
            group.pos = group.length;                                                                           
          }
          doublePush = TRUE;
          group.priority = TRUE;
        }
        //Only one pattern selected
        else if (!doublePush){
          group.priority = FALSE;
          group.length = 0;//should be 0 to play the right next pattern
          nextPattern = FirstBitOn() + curBank * NBR_PATTERN;
          group.pos = pattern[ptrnBuffer].groupPos;
        }
        if(curPattern != nextPattern && stepsBtn.justPressed) {                                                // [zabox] [1.027] fixes pattern change bug in slave mode
          if(seq.ptrnChangeSync == SYNC){
            if (isRunning) needLcdUpdate = TRUE;//selected pattern changed so we need to update display 
            else selectedPatternChanged = TRUE;
          }
          if(seq.ptrnChangeSync == FREE){
            selectedPatternChanged = TRUE;
          }
        }
      }
    }

    //--------------------------------sequencer run direction-----------------------
    if (shiftBtn && dirBtn.justPressed){
      if (seq.dir++ >= MAX_SEQ_DIR) seq.dir = FORWARD;
    }

    //--------------------------------pattern next update---------------------------
    /* if (trackPosNeedIncremante && group.length ){// && stepCount > 0)
     if (group.pos > group.length) group.pos = 0;
     nextPattern = group.firstPattern + group.pos;
     group.pos++;
     if(curPattern != nextPattern) selectedPatternChanged = TRUE;
     trackPosNeedIncremante = FALSE;
     needLcdUpdate = TRUE;
     }
     else*/    if (trackPosNeedIncremante && group.length == 0){
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      trackPosNeedIncremante = FALSE;
    }

  }//END IF MODE PLAY PATTERN

  //===================================================================================================================================

  //===================================================================================================================================

  //////////////////////////MODE TRACK WRITE/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (curSeqMode == TRACK_WRITE){
    //-------------------------------select pattern-----------------------------------
    if (readButtonState){

      if (bankBtn.pressed){
        if(FirstBitOn() >= MAX_BANK) curBank = MAX_BANK;
        else curBank = FirstBitOn();
        nextPattern = curBank * NBR_PATTERN + (curPattern % NBR_PATTERN);
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      }
      else if(numBtn.pressed){
        trk.next = FirstBitOn();
        selectedTrackChanged = TRUE;
        needLcdUpdate = TRUE;
      }
      else{
        nextPattern = FirstBitOn() + curBank * NBR_PATTERN;
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      }
    }
    //decremente track position
    if (backBtn.justPressed){
      trk.pos--;
      if (trk.pos < 0 || trk.pos > MAX_PTRN_TRACK) trk.pos = 0;
      nextPattern = track[trkBuffer].patternNbr[trk.pos];
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      needLcdUpdate = TRUE;
    }
    //incremente track position
    if (fwdBtn.justPressed){
      trk.pos++;
      if (trk.pos > MAX_PTRN_TRACK) trk.pos = MAX_PTRN_TRACK;
      nextPattern = track[trkBuffer].patternNbr[trk.pos];
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      needLcdUpdate = TRUE;
    }
    //go to first measure
    if (clearBtn.justPressed){
      trk.pos = 0;
      nextPattern = track[trkBuffer].patternNbr[trk.pos];
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      needLcdUpdate = TRUE;
    }
    if(shiftBtn){
      //go to last measure
      if (numBtn.pressed){
        trk.pos = track[trkBuffer].length;
        nextPattern = track[trkBuffer].patternNbr[trk.pos];
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
        needLcdUpdate = TRUE;
      }
      //delete current pattern in the current position
   // if(backBtn.justPressed) {
      if (backBtn.justPressed && track[trkBuffer].length){                                                  // [zabox] fixes crashs when deleting the last track pos
        if (trk.pos < (track[trkBuffer].length - 1)) {                                                      // [zabox] delete only valid track pos
        for (int a = trk.pos + 1; a < track[trkBuffer].length; a++){         
          track[trkBuffer].patternNbr[a] = track[trkBuffer].patternNbr[a + 1]; 
        }
        trk.pos +=1;//to stay in the same position
        track[trkBuffer].length = track[trkBuffer].length - 1;//decremente length by 1 du to deleted pattern
        nextPattern = track[trkBuffer].patternNbr[trk.pos];
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
        trackNeedSaved = TRUE;
        needLcdUpdate = TRUE;
        }
        else {
          trk.pos +=1;//to stay in the same position
        }
      }
      //insert a pattern
      if (fwdBtn.justPressed) {                                                                              
        if (trk.pos < (track[trkBuffer].length + 1)) {                                                     // [zabox] insert only inside track
        for (int a = track[trkBuffer].length + 1; a >= trk.pos; a--){                                             
          track[trkBuffer].patternNbr[a] = track[trkBuffer].patternNbr[a - 1]; 
        }
        trk.pos -=1;//to stay in the same position
        track[trkBuffer].patternNbr[trk.pos] = curPattern;
        track[trkBuffer].length = track[trkBuffer].length + 1;//decremente length by 1 du to deleted pattern
        nextPattern = track[trkBuffer].patternNbr[trk.pos];
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
        trackNeedSaved = TRUE;
        needLcdUpdate = TRUE;
        }
        else {
          trk.pos -=1;//to stay in the same position
        }
      }
    }//end shift

    //write selected pattern in the current track position
    if (enterBtn.justRelease && !trackJustSaved){
      track[trkBuffer].patternNbr[trk.pos] = curPattern;
      trk.pos++;
      if (trk.pos > MAX_PTRN_TRACK) trk.pos = MAX_PTRN_TRACK;
      nextPattern = track[trkBuffer].patternNbr[trk.pos];
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      track[trkBuffer].length = trk.pos;
      trackNeedSaved = TRUE;
      needLcdUpdate = TRUE;
    }
  }//END IF MODE TRACK WRITE

    //////////////////////////MODE TRACK PLAY/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (curSeqMode == TRACK_PLAY)
  {
    if (trackPosNeedIncremante){//(endMeasure)
      trk.pos++;
      if (trk.pos >= track[trkBuffer].length) trk.pos = 0;
      nextPattern = track[trkBuffer].patternNbr[trk.pos];
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      trackPosNeedIncremante = FALSE;
      needLcdUpdate = TRUE;

    }
    //go to first measure
    if (clearBtn.justPressed){
      trk.pos = 0;
      nextPattern = track[trkBuffer].patternNbr[trk.pos];
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      needLcdUpdate = TRUE;
    }
    if(readButtonState){
      trk.next = FirstBitOn();
      selectedTrackChanged = TRUE;
      needLcdUpdate = TRUE;
    }
  }

  //======================================================================================================================================================================================================================================================================
  //------------------------------------Update pattern track load/save-----------------------
  if (selectedTrackChanged)
  {
    selectedTrackChanged = FALSE;
    needLcdUpdate = TRUE;
    trackNeedSaved = FALSE;
    LoadTrack(trk.next);
    trk.current = trk.next;
    nextPattern = track[trkBuffer].patternNbr[trk.pos];
    if(curPattern != nextPattern) selectedPatternChanged = TRUE;
    //trkBuffer = !trkBuffer;
  }
  if (trackNeedSaved && enterBtn.hold)
  {
    trackNeedSaved = FALSE;
    SaveTrack(trk.current);
    LcdPrintSaved();
    trackJustSaved = TRUE;
    timeSinceSaved = millis();
  }
  //this function is to not incremente trk.pos when released enterBtn after Saved track
  if (millis() - timeSinceSaved > HOLD_TIME){                                               
    trackJustSaved = FALSE;
  }

  if (selectedPatternChanged)
  {
    //Serial.println("changed!!");
    selectedPatternChanged = FALSE;
    needLcdUpdate = TRUE;//selected pattern changed so we need to update display
    patternNeedSaved = FALSE;
    LoadPattern(nextPattern);
    curPattern = nextPattern;
    nextPatternReady = TRUE;
  }

  if(nextPatternReady){///In pattern play mode this peace of code execute in the PPQ Count function
    //Serial.println("Ready!!");
    //if ((isRunning && endMeasure) || !isRunning ){//|| (curSeqMode != PTRN_PLAY))
    // Serial.println("endMeasure!!");
    nextPatternReady = FALSE;
    keybOct = DEFAULT_OCT;
    noteIndex = 0;
    InitMidiNoteOff();
    ptrnBuffer = !ptrnBuffer;
    //Serial.println("switched!!");
    InitPattern();//SHOULD BE REMOVED WHEN EEPROM WILL BE INITIALIZED
    SetHHPattern();
    InstToStepWord();
  }

  if (patternWasEdited)
  {//update Pattern
    patternWasEdited = FALSE;
    SetHHPattern();
    InstToStepWord();
    patternNeedSaved = TRUE;
    // Serial.println("patternupdated");
  }

  if (patternNeedSaved && enterBtn.justPressed && !instBtn)
  {
    patternNeedSaved = FALSE;
    SavePattern(curPattern);//pattern saved
    LcdPrintSaved();
  }

  if (enterBtn.justRelease) needLcdUpdate = TRUE;

  //////////////////////////MODE MUTE//////////////////////////////////////

  if (curSeqMode == MUTE){
    MuteButtonGet();
    if (encBtn.pressed){
      muteInst = 0;
      muteLeds = 0;                                                                   // [1.028] new MuteButtonGet function
      //InitMuteBtnCounter();                                                         //   
    }
  } 

  //We still incremente pattern group in those mode
  if (curSeqMode == MUTE || curSeqMode == PTRN_PLAY || curSeqMode == PTRN_STEP ){
    if (trackPosNeedIncremante && group.length ){//&& stepCount > 0)
      group.pos++;
      if (group.pos > group.length) group.pos = 0;
      nextPattern = group.firstPattern + group.pos;
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      trackPosNeedIncremante = FALSE;
      needLcdUpdate = TRUE;
    } 
  }
}






