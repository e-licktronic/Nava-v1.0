//-------------------------------------------------
//                  NAVA v1.x
//                 SEQ configuration
//-------------------------------------------------

/////////////////////Function//////////////////////
void SeqConfiguration()
{
  if(seq.syncChanged){                               
    SetSeqSync();
    seq.syncChanged = FALSE;
  }
  if (seq.setupNeedSaved && enterBtn.justPressed){
    SaveSeqSetup();
    seq.setupNeedSaved = FALSE;
    LcdPrintSaved();
  }
  if (!seq.configMode) seq.setupNeedSaved = FALSE;

}



void SetSeqSync() 
{
    //Sync configuration
    switch (seq.sync){                             // [zabox] [1.028] added expander mode
    case MASTER: 
      initTrigTimer();                          
      DisconnectMidiHandleRealTime();
      DisconnectMidiHandleNote();
      TimerStart();//cf timer
      break;
    case SLAVE:
      TimerStop();
      initTrigTimer();                        
      DisconnectMidiHandleNote();
      ConnectMidiHandleRealTime();
      break;
    case EXPANDER:
      TimerStop();
      initExpTimer();                 
      DisconnectMidiHandleRealTime();
      ConnectMidiHandleNote();
      stepLeds = 0;
      configLed = 0;
      menuLed = 0;
      break;
    }
}

