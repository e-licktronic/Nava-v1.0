//-------------------------------------------------
//                  NAVA v1.x
//                  BPM
//-------------------------------------------------

/////////////////////Function//////////////////////
//Timer interrupt
ISR(TIMER1_COMPA_vect) {
  CountPPQN();
}


ISR(TIMER2_COMPA_vect) {     // [zabox] [v1.028] 2ms trig off isr. improves led flickering a lot and improves external instrument midi out lag

  TRIG_TIMER_STOP;           // [zabox] one shot
  TRIG_TIMER_ZERO;           // [zabox] reset
  SetDoutTrig(tempDoutTrig);
}  
  


ISR(TIMER3_COMPA_vect) {       // [zabox] flam

  FLAM_TIMER_STOP;
  FLAM_TIMER_ZERO;

  SetMuxFlam();
 
  SetDoutTrig(stepValueFlam & (~muteInst) | tempDoutTrig);//Send TempDoutTrig too to prevet tick noise on HH circuit
  
  TRIG_TIMER_START;
  stepValueFlam = 0;
  
} 
  
  
  

//Tick proceed each pulse
void CountPPQN()
{
  blinkVeryFast =! blinkVeryFast;
  if (ppqn % (PPQN/2) == 0) blinkTempo = !blinkTempo;
  if (ppqn  % (pattern[ptrnBuffer].scale/2) == 0) blinkFast = LOW;
 // if (ppqn % 4 == 0) MIDI.sendRealTime(Clock);      //MidiSend(CLOCK_CMD);//as NAVA seq is 96ppqn we need to send clock each 4 internal ppqn
  if (ppqn % 4 == 0) {                                
    while (!(UCSR1A & (1 << UDRE1))) {};                                            // [zabox] directly adressing the uart fixes the midi clock lag
    UDR1 = CLOCK_CMD; //Tick
  }

  if (seq.sync == MASTER){                                                          // [zabox] has to be 0/2 for the correct phase
    if (ppqn % 4 == 0){
      DIN_CLK_HIGH;                                                               
      dinClkState = HIGH;
    }
    else if (ppqn % 4 == 2) {
      DIN_CLK_LOW;
      dinClkState = LOW;
    }    
  }
  

  if (isRunning)
  {
    if (ppqn % pattern[ptrnBuffer].scale == (pattern[ptrnBuffer].scale/2)) tapStepCount++;

    if (tapStepCount > pattern[ptrnBuffer].length) tapStepCount = 0;

    // Initialize the step value for trigger and gate value for cv gate track
    stepValue = 0;

    if (ppqn % pattern[ptrnBuffer].scale == 0) stepChanged = TRUE;//Step changed ?

    if (((ppqn + shuffle[(pattern[ptrnBuffer].shuffle)-1][shufPolarity]) % pattern[ptrnBuffer].scale == 0) && stepChanged)
    {//Each Step
      stepChanged = FALSE;//flag that we already trig this step
      shufPolarity = !shufPolarity;
      blinkFast = HIGH;

      //sequencer direction-----------
      switch (seq.dir){
      case FORWARD:
        curStep = stepCount;
        break;
      case BACKWARD:
        curStep = pattern[ptrnBuffer].length - stepCount;
        break;
      case PING_PONG:
        if (curStep == pattern[ptrnBuffer].length && changeDir == 1) changeDir = 0;
        else if (curStep == 0 && changeDir == 0)  changeDir = 1;
        if (changeDir) curStep = stepCount;
        else curStep = pattern[ptrnBuffer].length - stepCount;
        break;
      case RANDOM:
        curStep = random(0, 16);
        break;
      }

      //Set step value to be trigged
      for (byte z = 0; z < NBR_INST; z++)
      {
        stepValue |= (bitRead(pattern[ptrnBuffer].inst[z], curStep)) << z;
      }
      //Set stepvalue depending metronome
      stepValue |= (bitRead(metronome,curStep) << RM);
      
      setFlam();                                                                                    // [zabox] [1.027] if changed, update flam interval
      
      if (stepValue){
        SetMux();
        int temp_muteInst = muteInst;                                                               // [zabox] OH/CH mute select
        if (bitRead(stepValue, CH) && bitRead(muteInst, CH)) {                                      //
          temp_muteInst |= (1 << HH);                                                               //  
        }   
        else if (bitRead(stepValue, OH) && bitRead(muteInst, OH)) {                                 //  
          temp_muteInst |= (1 << HH);                                                               //
        }
        
        if (bitRead(pattern[ptrnBuffer].inst[CH], curStep) && !bitRead(muteInst, CH)) tempDoutTrig = B10;//CH trig                        // [zabox] + check if OH/CH mute
        else if (bitRead(pattern[ptrnBuffer].inst[OH], curStep) && !bitRead(muteInst, OH)) tempDoutTrig = 0;// OH trig                    // [zabox] + check if OH/CH mute
        
        SetDoutTrig((stepValue) & (~temp_muteInst) | (tempDoutTrig));//Send TempDoutTrig too to prevet tick noise on HH circuit
        
        TRIG_TIMER_START;        // [zabox] [1.028] start trigger off timer
        
        if (stepValueFlam) {
          FLAM_TIMER_START;
        } 
        
      }
      if (bitRead(pattern[ptrnBuffer].inst[TRIG_OUT], curStep)){
        TRIG_LOW;//Trigout
        trigCounterStart = TRUE;
      }

      //Trig external instrument-------------------------------------
      if (bitRead(pattern[ptrnBuffer].inst[EXT_INST], curStep))
      {
        InitMidiNoteOff();
        MidiSendNoteOn(seq.TXchannel, pattern[ptrnBuffer].extNote[noteIndexCpt], HIGH_VEL);
        midiNoteOnActive = TRUE;
        noteIndexCpt++;//incremente external inst note index
      }
      if (noteIndexCpt > pattern[ptrnBuffer].extLength){
        noteIndexCpt = 0;
      }
      
/*      
      delayMicroseconds(2000);
      if (bitRead(pattern[ptrnBuffer].inst[CH], curStep) && !bitRead(muteInst, CH)) tempDoutTrig = B10;//CH trig                        // [zabox] + check if OH/CH mute               // [zabox v1.028] now handled inside timer2 isr
      else if (bitRead(pattern[ptrnBuffer].inst[OH], curStep) && !bitRead(muteInst, OH)) tempDoutTrig = 0;// OH trig                    // [zabox] + check if OH/CH mute
      SetDoutTrig(tempDoutTrig);
*/      
      
      //TRIG_HIGH;
      //ResetDoutTrig();
      stepCount++;

    }
    if (stepCount > pattern[ptrnBuffer].length){// && (ppqn % 24 == pattern[ptrnBuffer].scale - 1))
      endMeasure = TRUE;
      trackPosNeedIncremante = TRUE;                                                
      stepCount = 0;
      //In pattern play mode this peace of code execute in the PPQNCount function
      if(nextPatternReady && curSeqMode == PTRN_PLAY){
        nextPatternReady = FALSE;
        keybOct = DEFAULT_OCT;
        noteIndex = 0;
        InitMidiNoteOff();
        ptrnBuffer = !ptrnBuffer;
        InitPattern();//SHOULD BE REMOVED WHEN EEPROM WILL BE INITIALIZED
        SetHHPattern();
        InstToStepWord();
      }
    }
//    if (ppqn % pattern[ptrnBuffer].scale == 4 && stepCount == 0){ 
//      endMeasure = FALSE;
//    }
  
  }
  
  ppqn++;                                                                 // [1.028] more consistent to run the counter from 0-95
  if (ppqn >= PPQN) ppqn = 0;
  
}

//
void Metronome(boolean state)
{
  if (state){
    metronome = 0x1111;//trig RM every time
    pattern[ptrnBuffer].velocity[RM][0] = 127;
    pattern[ptrnBuffer].velocity[RM][4] = 30;
    pattern[ptrnBuffer].velocity[RM][8] = 30;
    pattern[ptrnBuffer].velocity[RM][12] = 30;
  }
  else{
    metronome = 0;
    pattern[ptrnBuffer].velocity[RM][0] = HIGH_VEL;
    pattern[ptrnBuffer].velocity[RM][4] = HIGH_VEL;
    pattern[ptrnBuffer].velocity[RM][8] = HIGH_VEL;
    pattern[ptrnBuffer].velocity[RM][12] = HIGH_VEL;
  }
}






























