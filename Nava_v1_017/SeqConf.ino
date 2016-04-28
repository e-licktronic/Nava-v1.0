//-------------------------------------------------
//                  NAVA v1.x
//                 SEQ configuration
//-------------------------------------------------

/////////////////////Function//////////////////////
void SeqConfiguration()
{
  if(seq.syncChanged){
    //Sync configuration
    switch (seq.sync){
    case MASTER:
      DisconnectMidiHandleRealTime();
      TimerStart();//cf timer
      break;
    case SLAVE:
      TimerStop();
      MIDI.setHandleClock(HandleClock);
      MIDI.setHandleStart(HandleStart);
      MIDI.setHandleStop(HandleStop);
      MIDI.setHandleContinue(HandleContinue);
      break;
    }
    seq.syncChanged = FALSE;
  }
  if (seq.setupNeedSaved && enterBtn.justPressed){
    SaveSeqSetup();
    seq.setupNeedSaved = FALSE;
    LcdPrintSaved();
  }
  if (!seq.configMode) seq.setupNeedSaved = FALSE;
    
}




