//-------------------------------------------------
//                  NAVA v1.x
//                 keyboard mode
//-------------------------------------------------

/////////////////////Function//////////////////////
void KeyboardUpdate()
{
  if (isRunning && keyboardMode){
    keyboardMode = 0;
    needLcdUpdate = TRUE;
  }
  
  if (numBtn.justPressed && curInst == EXT_INST && curSeqMode == PTRN_STEP){
    keyboardMode = !keyboardMode;
    //stop sequencer when keyboard mode ON
    if (keyboardMode){
      InitMidiNoteOff();
      isStop = TRUE;
      isRunning = FALSE;
   //   MIDI.sendRealTime(Stop);//;MidiSend(STOP_CMD);
      stopBtn.counter = 1;//like a push on stop button
    }
    needLcdUpdate = TRUE;
  }

  /////////////////////////////KeyboardMode//////////////////////////////
  if(keyboardMode)
  {
    if (scaleBtn.justPressed){
      keybOct++;
      if (keybOct >= MAX_OCT) keybOct = MAX_OCT - 1;
      needLcdUpdate = TRUE;
    }
    if (lastStepBtn.justPressed){
      keybOct--;
      if (keybOct < 0 || keybOct > MAX_OCT - 1) keybOct = 0;
      needLcdUpdate = TRUE;
    }
    if (backBtn.justPressed){
      noteIndex--;
      if( noteIndex < 0 || noteIndex > MAX_EXT_INST_NOTE - 1) noteIndex = 0;
      MidiSendNoteOn(seq.TXchannel, pattern[ptrnBuffer].extNote[noteIndex], HIGH_VEL);
      needLcdUpdate = TRUE;
    }
    if (backBtn.justRelease)MidiSendNoteOff(seq.TXchannel, pattern[ptrnBuffer].extNote[noteIndex]);

    if (fwdBtn.justPressed){
      noteIndex++;
      if( noteIndex >= MAX_EXT_INST_NOTE ) noteIndex = MAX_EXT_INST_NOTE;
      MidiSendNoteOn(seq.TXchannel, pattern[ptrnBuffer].extNote[noteIndex], HIGH_VEL);
      needLcdUpdate = TRUE;
    }
    if (fwdBtn.justRelease)MidiSendNoteOff(seq.TXchannel, pattern[ptrnBuffer].extNote[noteIndex]);

    if(clearBtn.justPressed){
      noteIndex = 0;
      needLcdUpdate = TRUE;
    }

    if (isStop){
      for (byte a = 0; a < NBR_STEP_BTN; a++){
        stepBtn[a].curState = bitRead(readButtonState,a);
        if (stepBtn[a].curState != stepBtn[a].prevState){
          if ((stepBtn[a].pressed == LOW) && (stepBtn[a].curState == HIGH)){
            pattern[ptrnBuffer].extNote[noteIndex] = a + (12* keybOct);
            MidiSendNoteOn(seq.TXchannel, a + 12*keybOct, HIGH_VEL);
            noteIndex++;
            patternWasEdited = TRUE;
            if(noteIndex > MAX_EXT_INST_NOTE) noteIndex = MAX_EXT_INST_NOTE;
            pattern[ptrnBuffer].extLength = noteIndex - 1;
            if(pattern[ptrnBuffer].extLength > MAX_EXT_INST_NOTE)pattern[ptrnBuffer].extLength = 0;
            needLcdUpdate = TRUE;
          }  
          if ((stepBtn[a].pressed == HIGH) && (stepBtn[a].curState == LOW)){
            MidiSendNoteOff(seq.TXchannel, a + 12*keybOct);    
          }
          stepBtn[a].pressed = stepBtn[a].curState;
        }
        stepBtn[a].prevState = stepBtn[a].curState;
      }  
    }
  }
}








