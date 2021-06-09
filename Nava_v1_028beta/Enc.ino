//-------------------------------------------------
//                  NAVA v1.x
//                 Encoder function
//-------------------------------------------------

////////////////////////Function//////////////////////

void EncGet()
{

  
  //////////////////////////////////TOTAL ACCENT////////////////////////////////////
  if (curInst == TOTAL_ACC && curSeqMode == PTRN_STEP){
    pattern[ptrnBuffer].totalAcc = EncGet(pattern[ptrnBuffer].totalAcc, 1);
    pattern[ptrnBuffer].totalAcc = constrain(pattern[ptrnBuffer].totalAcc, 0, 13);
    static byte prevTotalAcc;
    if (pattern[ptrnBuffer].totalAcc != prevTotalAcc){
      prevTotalAcc = pattern[ptrnBuffer].totalAcc;
      patternWasEdited = TRUE;
      needLcdUpdate = TRUE;
    }
  }
  ///////////////////////////////////TRACK WRITE////////////////////////////////////
  else if (curSeqMode == TRACK_WRITE && !tempoBtn.pressed && !seq.configMode){
    switch (curIndex){
      //track position
    case 0://track position
      if(instBtn) trk.pos = EncGet(trk.pos, 10);
      else trk.pos = EncGet(trk.pos, 1);
      trk.pos = constrain(trk.pos, 0, 999);
      static unsigned int prevTrkPos;
      if (trk.pos != prevTrkPos){
        prevTrkPos = trk.pos;
        nextPattern = track[trkBuffer].patternNbr[trk.pos];
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
        needLcdUpdate = TRUE;
        break;
      case 1:// track pattern
        if(instBtn) nextPattern = EncGet(nextPattern, 16);
        else nextPattern = EncGet(nextPattern, 1);
        nextPattern = constrain(nextPattern, 0, MAX_PTRN - 1);
        static unsigned int prevNextPattern;
        if (nextPattern != prevNextPattern){
          prevNextPattern = nextPattern;
          if(curPattern != nextPattern) selectedPatternChanged = TRUE;
          needLcdUpdate = TRUE;
        }
        break;
      case 2://track length
        if(instBtn) track[trkBuffer].length = EncGet(track[trkBuffer].length, 10);
        else track[trkBuffer].length = EncGet(track[trkBuffer].length, 1);
        track[trkBuffer].length = constrain(track[trkBuffer].length, 0, 999);
        static unsigned int prevTrkLength;
        if (track[trkBuffer].length != prevTrkLength){
          prevTrkLength = track[trkBuffer].length;
          trackNeedSaved = TRUE;
          needLcdUpdate = TRUE;
        }
        break;
      case 3://track number
        //toDo
        break;
      }
    }
  }

  ///////////////////////////////////KEYBOARD MODE////////////////////////////////////
  else if (keyboardMode){
    switch (curIndex){
      //track position
    case 0://external instrument note index
      if(instBtn) noteIndex = EncGet(noteIndex, 10);
      else noteIndex = EncGet(noteIndex, 1);
      noteIndex = constrain(noteIndex, 0, 99);
      static unsigned int prevNoteIndex;
      if (noteIndex != prevNoteIndex){
        prevNoteIndex = noteIndex;
        needLcdUpdate = TRUE;
        break;
      case 1://external instrument note
        if(instBtn) pattern[ptrnBuffer].extNote[noteIndex] = EncGet(pattern[ptrnBuffer].extNote[noteIndex], 12);
        else pattern[ptrnBuffer].extNote[noteIndex] = EncGet(pattern[ptrnBuffer].extNote[noteIndex], 1);
        pattern[ptrnBuffer].extNote[noteIndex] = constrain(pattern[ptrnBuffer].extNote[noteIndex], 0, 127);
        static unsigned int prevExtNote;
        if (pattern[ptrnBuffer].extNote[noteIndex] != prevExtNote){
          prevExtNote = pattern[ptrnBuffer].extNote[noteIndex];
          needLcdUpdate = TRUE;
        }
        break;
      case 2://external instrument notes stack length
        if(instBtn) pattern[ptrnBuffer].extLength = EncGet(pattern[ptrnBuffer].extLength, 10);
        else pattern[ptrnBuffer].extLength = EncGet(pattern[ptrnBuffer].extLength, 1);
        pattern[ptrnBuffer].extLength = constrain(pattern[ptrnBuffer].extLength, 0, 99);
        static unsigned int prevExtLength;
        if (pattern[ptrnBuffer].extLength != prevExtLength){
          prevExtLength = pattern[ptrnBuffer].extLength;
          patternNeedSaved = TRUE;
          needLcdUpdate = TRUE;
        }
        break;
      case 3://octave
        keybOct = EncGet(keybOct, 1);
        keybOct = constrain(keybOct, 0, 7);
        static unsigned int prevKeybOct;
        if (keybOct != prevKeybOct){
          prevKeybOct = keybOct;
          needLcdUpdate = TRUE;
        }
        break;
      }
    }
  }
  ///////////////////////////////////CONFIG MODE////////////////////////////////////
  else if (seq.configMode){                                                               //  [zabox] rewrite for two complete pages & no wrong encoder updates
    
    if (seq.configPage == 1) {    
      
  //---------------------Page 1----------------------------------------------------
    
    switch (curIndex){
      //track position
      case 0:
      
        seq.sync = EncGet(seq.sync, 1);                       //sync select
        seq.sync = constrain(seq.sync, 0, 2);
        static byte prevSeqSync;
        if (seq.sync != prevSeqSync){
          prevSeqSync = seq.sync;
          seq.syncChanged = TRUE;
          seq.setupNeedSaved = TRUE;
          needLcdUpdate = TRUE;
        }
        break;
      case 1:
  
        seq.defaultBpm = EncGet(seq.defaultBpm, 1);
        seq.defaultBpm = constrain(seq.defaultBpm, MIN_BPM, MAX_BPM);
        static unsigned int prevDefaultBpm;
        if (seq.defaultBpm != prevDefaultBpm){
          prevDefaultBpm = seq.defaultBpm;
          seq.setupNeedSaved = TRUE;
          needLcdUpdate = TRUE;
        }
        break;
      case 2:
  
        seq.TXchannel = EncGet(seq.TXchannel, 1);
        seq.TXchannel = constrain(seq.TXchannel, 1, 16);
        static unsigned int prevTX;
        if (seq.TXchannel != prevTX){
          prevTX = seq.TXchannel;
          seq.setupNeedSaved = TRUE;
          needLcdUpdate = TRUE;
        }
        break;
      case 3:
        seq.RXchannel = EncGet(seq.RXchannel, 1);
        seq.RXchannel = constrain(seq.RXchannel, 1, 16);
        static unsigned int prevRX;
        if (seq.RXchannel != prevRX){
          prevRX = seq.RXchannel;
          MIDI.setInputChannel(seq.RXchannel);
          seq.setupNeedSaved = TRUE;
          needLcdUpdate = TRUE;
        }
        break;
      }
    }
    else if (seq.configPage == 2) {
            
  //---------------------Page 2----------------------------------------------------
    
    switch (curIndex){
      //track position
      case 0:
        seq.ptrnChangeSync = EncGet(seq.ptrnChangeSync, 1);               //pattern change sync select
        seq.ptrnChangeSync = constrain(seq.ptrnChangeSync, 0, 1);
        static boolean prevPtrnSyncChange;
        if (seq.ptrnChangeSync != prevPtrnSyncChange){
          prevPtrnSyncChange = seq.ptrnChangeSync;
          // seq.syncChanged = TRUE;
          seq.setupNeedSaved = TRUE;
          needLcdUpdate = TRUE;
        }
        break;
      case 1:
        seq.muteModeHH = EncGet(seq.muteModeHH, 1);                              // [zabox]
        seq.muteModeHH = constrain(seq.muteModeHH, 0, 1);
        static boolean prev_muteModeHH;
        if (seq.muteModeHH != prev_muteModeHH){
          prev_muteModeHH = seq.muteModeHH;
          seq.setupNeedSaved = TRUE;
          needLcdUpdate = TRUE;
        }
        break;
      case 2:
  
        break;
      case 3:
        
        break;
      }
    }
    
    
  }
  else{
    seq.bpm = EncGet(seq.bpm,1);
    if (seq.bpm <= MIN_BPM) seq.bpm = MIN_BPM;
    if (seq.bpm >= MAX_BPM) seq.bpm = MAX_BPM;
    static unsigned int curBpm;
    if( seq.bpm != curBpm)
    {
      curBpm = seq.bpm;
      TimerSetFrequency();
      if (curSeqMode != PTRN_STEP || tempoBtn.pressed) needLcdUpdate = TRUE;
    }
  }
}


//Get encoder value--------------------------------------------------------
int EncGet(int value, int dif)
{
  encoder_A = PINB & B1;// Read encoder pins
  encoder_B = PINB & B10;
  if((!encoder_A) && (encoder_A_prev)){
    // A has gone from high to low 
    if(encoder_B) {
      // B is high so clockwise   
      value += dif;
    }   
    else {
      if(instBtn){
        switch (curIndex){
        case 0:
        case 2:
          if (value >= 10) value -= dif;
          break;
        case 1:
          if (value >= 16) value -= dif;
          break;
        }
      }
      else{
        if (value != 0) value -= dif;  // to not go under 0
      }
    }   
  }   
  encoder_A_prev = encoder_A;     // Store value of A for next time 
  return value;
}  





















