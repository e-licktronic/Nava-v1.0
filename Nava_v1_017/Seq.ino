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
    if (curIndex >= MAX_CUR_POS) curIndex = 0;
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
    //trackPosNeedIncremante = TRUE;
    //init Groupe pattern  position
    /*group.pos = 0;
     nextPattern = group.firstPattern + group.pos;
     if(curPattern != nextPattern) selectedPatternChanged = TRUE;
     trackPosNeedIncremante = FALSE;
     needLcdUpdate = TRUE;*/

    MIDI.sendRealTime(Start);//;MidiSend(START_CMD);
    DIN_START_HIGH;
    dinStartState = HIGH;
    DIN_CLK_HIGH;
    dinClkState = HIGH;
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
    if (trkBtn){
      needLcdUpdate = TRUE;
      curSeqMode = TRACK_WRITE;
      keyboardMode = FALSE;
      seq.configMode  = FALSE;
    }
    if (ptrnBtn) {
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
      if (seq.configPage > MAX_CONF_PAGE){
        seq.configPage = 0;
        seq.configMode  = FALSE;
      }
      needLcdUpdate = TRUE;
    }
  }
  //-------------------Shift button released------------------------------
  else {
    if (trkBtn){
      curSeqMode = TRACK_PLAY;
      needLcdUpdate = TRUE;
      keyboardMode = FALSE;
      seq.configMode  = FALSE;
    }
    // if (backBtn.justPressed) ;//back  track postion
    //if (fwdBtn.justPressed) ;//foward track postion
    if (numBtn.pressed) ;//select Track number
    if (ptrnBtn){
      curSeqMode = PTRN_PLAY;
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

    //-------------------Select instrument------------------------------
    //Match with trig shift register out (cf schematic)
    if(readButtonState == 0) doublePush = 0; //init double push if all step button  released 
    if (instBtn && readButtonState){
      needLcdUpdate = TRUE;
      keyboardMode = FALSE;
      switch(FirstBitOn()){
      case BD_BTN:
      case BD_LOW_BTN:
        curInst = BD;
        break;
      case SD_BTN:
      case SD_LOW_BTN:
        curInst = SD;
        break;
      case LT_BTN:
      case LT_LOW_BTN:
        curInst = LT;
        break;
      case MT_BTN:
      case MT_LOW_BTN:
        curInst = MT ;
        break;
      case HT_BTN:
      case HT_LOW_BTN:
        curInst = HT;
        break;
      case RM_BTN:
        curInst = RM;
        break;
      case HC_BTN:
        curInst = HC;
        break;
      case CH_BTN:
      case CH_LOW_BTN:
        if (doublePush == 0){
          curInst = CH;
        }
        break;
      case RIDE_BTN:
        curInst = RIDE;
        break;
      case CRASH_BTN:
        curInst = CRASH;
        break;
      }
      if (readButtonState == OH_BTN){
        curInst = OH;
        doublePush = 1;
      }
    }
    if (instBtn && enterBtn.pressed){
      curInst = TOTAL_ACC;
      needLcdUpdate = TRUE;
    }
    if (instBtn && stopBtn.pressed){
      curInst = TRIG_OUT;
      needLcdUpdate = TRUE;
    }
    if (shiftBtn && guideBtn.pressed){
      curInst = EXT_INST;
      needLcdUpdate = TRUE;
    }

    //-------------------Clear Button------------------------------
    if (clearBtn && !keyboardMode && curSeqMode != PTRN_TAP){
      patternWasEdited = TRUE;
      if (isRunning){
        bitClear (pattern[ptrnBuffer].inst[curInst], curStep);
        if (curInst == CH) pattern[ptrnBuffer].velocity[CH][curStep] = instVelHigh[HH];//update HH velocity that OH is trigged correctly
      }
      else{//clear full pattern
        for (int a = 0; a < NBR_INST; a++){
          pattern[ptrnBuffer].inst[a] = 0;
        }
        pattern[ptrnBuffer].shuffle = DEFAULT_SHUF;
        pattern[ptrnBuffer].length = NBR_STEP - 1;
        pattern[ptrnBuffer].scale = SCALE_16;
        keybOct = DEFAULT_OCT;
        needLcdUpdate = TRUE;
      }
    }

    //-------------------shuffle Button------------------------------
    if (shufBtn.justPressed || shufBtn.justRelease) needLcdUpdate = TRUE;

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
    /////////////////////////////STEP EDIT///////////////////////////
    if (curSeqMode == PTRN_STEP){
      if (stepsBtn.justRelease) doublePush = FALSE;
      if(!lastStepBtn.pressed && !instBtn && !keyboardMode){
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
      if (trackPosNeedIncremante && group.length ){//&& stepCount > 0)
        group.pos++;
        if (group.pos > group.length) group.pos = 0;
        nextPattern = group.firstPattern + group.pos;
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
        trackPosNeedIncremante = FALSE;
        needLcdUpdate = TRUE;
      }
    }
    //////////////////////////////TAP EDIT///////////////////////////////
    else if (curSeqMode == PTRN_TAP)
    {
      if (clearBtn){
        bitClear (pattern[ptrnBuffer].inst[curInst], curStep);
        if (curInst == CH) pattern[ptrnBuffer].velocity[CH][curStep] = HIGH_VEL;//update HH velocity that OH is trigged correctly
        patternWasEdited = TRUE;
      } 
      if (!lastStepBtn.pressed && !instBtn)
      {
        if (readButtonState == OH_BTN) doublePush = 1;
        else if (readButtonState == 0) doublePush = 0;

        for (byte a = 0; a < NBR_STEP_BTN; a++){
          stepBtn[a].curState = bitRead(readButtonState,a);


          if (stepBtn[a].curState != stepBtn[a].prevState){
            if ((stepBtn[a].pressed == LOW) && (stepBtn[a].curState == HIGH)){
              int tempVel;
              switch (a){
              case BD_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[BD];
                tempVel = instVelHigh[BD];
                break;
              case BD_LOW_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelLow[BD];
                tempVel = instVelLow[BD];
                break;
              case SD_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[SD];
                tempVel = instVelHigh[SD];
                break;
              case SD_LOW_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelLow[SD];
                tempVel = instVelLow[SD];
                break;
              case LT_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[LT];
                tempVel = instVelHigh[LT];
                break;
              case LT_LOW_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelLow[LT];
                tempVel = instVelLow[LT];
                break;
              case MT_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[MT];
                tempVel = instVelHigh[MT];
                break;
              case MT_LOW_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelLow[MT];
                tempVel = instVelLow[MT];
                break;
              case HT_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[HT];
                tempVel = instVelHigh[HT];
                break;
              case HT_LOW_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelLow[HT];
                tempVel = instVelLow[HT];
                break;
              case CH_BTN:
                if (!doublePush){
                  if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[CH];
                  tempVel = instVelHigh[CH];
                }
                break;
              case CH_LOW_BTN:
                if (!doublePush){
                  if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelLow[CH];
                  tempVel = instVelLow[CH];
                }
                break;
              case RM_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[RM];
                tempVel = instVelHigh[RM];
                break;
              case HC_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[HC];
                tempVel = instVelHigh[HC];
                break;
              case CRASH_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[CRASH];
                tempVel = instVelHigh[CRASH];
                break;
              case RIDE_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[RIDE];
                tempVel = instVelHigh[RIDE];
                break;
              }
              if (doublePush){
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[OH];
                tempVel = instVelHigh[OH];
              }

              /*if ( a == BD_LOW_BTN || a == SD_LOW_BTN || a == LT_LOW_BTN || a == MT_LOW_BTN || a == HT_LOW_BTN || a == CH_LOW_BTN && !doublePush){
               //Set MID_VEL velocity if low pressed button
               if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = MID_VEL;
               tempVel = MID_VEL;
               }
               else{//Set HIGH_VEL velocity if low pressed button
               if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = HIGH_VEL;
               tempVel = HIGH_VEL;
               }*/
              //Set velocity for manual trig !!TO BE UPTADED!!
              for (int x =0; x<16; x++){ 
                SetMuxTrig(tempVel);
              }

              if( a == CH_LOW_BTN || a == CH_BTN || readButtonState == OH_BTN){
                SetDoutTrig(1 << HH);
                delayMicroseconds(2000);
                if (readButtonState == OH_BTN){
                  SetDoutTrig(0);
                  if(isRunning)  bitSet(tempInst[OH],tapStepCount);
                }
                else {
                  SetDoutTrig(1 << HH_SLCT);
                  if(isRunning) bitSet(tempInst[CH],tapStepCount);
                }
              }
              else{
                SetDoutTrig(1 << instOut[a]);
                delayMicroseconds(2000);
                SetDoutTrig(0);
                if(isRunning) bitSet(tempInst[instOut[a]],tapStepCount);
              }
            }      
          }
          stepBtn[a].prevState = stepBtn[a].curState;
        }//END FOR LOOP
      }
      if (endMeasure){//Update pattern at the end of measure to not get a double trig
        for (int inst = 0; inst < NBR_INST; inst++){
          if (tempInst[inst])//if instruments was edited
          {
            pattern[ptrnBuffer].inst[inst] |= tempInst[inst];
            tempInst[inst] = 0;// init tempInst
            patternWasEdited = TRUE;
          }
        }
      }
    }//END IF PTRN_TAP MODE
  }//ENDIF MODE EDIT

  //=======================================================================================
  //////////////////////////MODE PATTERN PLAY.../////////////////////////////////

  if (curSeqMode == PTRN_PLAY){
    //-------------------------------select pattern-----------------------------------
    if (stepsBtn.justRelease) doublePush = FALSE;
    if (readButtonState){
      if (bankBtn.pressed){
        if(FirstBitOn() >= MAX_BANK) curBank = MAX_BANK;
        else curBank = FirstBitOn();
        nextPattern = curBank * NBR_PATTERN + (curPattern % NBR_PATTERN);
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      }
      else{
        if (SecondBitOn())
        {
          group.length = SecondBitOn() - FirstBitOn();
          nextPattern = group.firstPattern = FirstBitOn() + curBank * NBR_PATTERN;
          doublePush = TRUE;
          group.priority = TRUE;
        }
        else if (!doublePush){
          group.priority = FALSE;
          nextPattern = FirstBitOn() + curBank * NBR_PATTERN;
          group.pos = pattern[ptrnBuffer].groupPos;
        }
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      }
    }

    //--------------------------------sequencer run direction-----------------------
    if (shiftBtn && dirBtn.justPressed){
      if (seq.dir++ >= MAX_SEQ_DIR) seq.dir = FORWARD;
    }
    //--------------------------------pattern next update---------------------------
    /*if (group.length){
     nextPattern = group.firstPattern;
     if(curPattern != nextPattern) selectedPatternChanged = TRUE;
     needLcdUpdate = TRUE;
     }*/

    if (trackPosNeedIncremante && group.length){// && stepCount > 0)
      group.pos++;
      if (group.pos > group.length) group.pos = 0;
      nextPattern = group.firstPattern + group.pos;
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      trackPosNeedIncremante = FALSE;
      needLcdUpdate = TRUE;
    }
    else if (trackPosNeedIncremante && group.length == 0){
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      trackPosNeedIncremante = FALSE;
    }
    /*if (patternNeedSwitchBuffer){
     patternNeedSwitchBuffer = FALSE;
     if(curPattern != nextPattern) selectedPatternChanged = TRUE;
     }*/
  }//END IF MODE PLAY

  //===================================================================================================================================

  //===================================================================================================================================

  //////////////////////////MODE TRACK WRITE/////////////////////////////////

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
    if (clearBtn){
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
      if (backBtn.justPressed){
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
      //insert a pattern
      if (fwdBtn.justPressed){
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

    //////////////////////////MODE TRACK PLAY/////////////////////////////////
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
    if (clearBtn){
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

  //=======================================================================================
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
      InitMuteBtnCounter();
    }
  }  
}

















































