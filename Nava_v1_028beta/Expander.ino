//-------------------------------------------------
//                  NAVA v1.x
//                  Expander
//-------------------------------------------------


void Expander() 
{
  
  while(seq.sync == EXPANDER) {
    
    #if DEBUG
      DIN_START_LOW;
      delayMicroseconds(1);
      DIN_START_HIGH;
    #endif
       
    MIDI.read();
    TriggerOffTimer();
    if (!(gateInst & 1)) SetTrigPeriod(TRIG_LENGHT);
    
    ButtonGetExpander();
    SetLedsExpander();
    
    SeqConfigurationExpander();
     
  }
  
}





void TriggerOffTimer()
{
  cur_triggerTime = TCNT2;
  
  for (byte inst = 2;  inst < 12; inst++) {
    
    if (lastDoutTrig & (1 << inst)) {
      if ((byte) (cur_triggerTime - triggerTime[inst]) > 31) {
        SetDoutTrig((~(1 << inst)) & lastDoutTrig);
        if ((lastDoutTrig & (~(1 << HH_SLCT))) == 0) {
        stepLeds = 0;
        }
      }
    }
    
  }
  
}
    
    
void ButtonGetExpander ()
{
  if (!seq.configMode) {
    
    ScanDin();
    shiftBtn = ((dinSr[2] & BTN_SHIFT) ? 1:0);
    ButtonGet (&guideBtn, dinSr[3] & BTN_GUIDE);
    ButtonGet(&bankBtn, dinSr[4] & BTN_BANK);
    ButtonGet(&muteBtn, dinSr[4] & BTN_MUTE);
    ButtonGet(&tempoBtn, dinSr[4] & BTN_TEMPO);
    ButtonGet (&stopBtn, dinSr[2] & BTN_STOP);


    ButtonGet(&encBtn, encSwState);
    
    if (bankBtn.justPressed) {
      if (muteLed) {
        bankLed = 1;
        muteLed = 0;
      }
      else {
        bankLed = !bankLed;
      }
     
      if (bankLed) {
        lcd.setCursor(0,1);
        lcd.print("      GATE      ");
      }
      else {
        lcd.setCursor(0,1);
        lcd.print("                ");
      }
    }
    
    if (muteBtn.justPressed) {
      if (bankLed) {
        bankLed = 0;
        muteLed = 1;
      }
      else {
        muteLed = !muteLed;
      }
      
      if (muteLed) {
        lcd.setCursor(0,1);
        lcd.print("      MUTE      ");
      }
      else {
        lcd.setCursor(0,1);
        lcd.print("                ");
      }
    }
    
    
    if (guideBtn.justPressed) {
      showTrigLeds = !showTrigLeds;
    }
        
    if (muteLed) {
           
      MuteButtonGet();
      if (stopBtn.justPressed) {
        muteInst ^= 1;
      }
      if (encBtn.pressed) {
        muteInst = 0;
        muteLeds = 0;
      }
    } else if (bankLed) {
           
      GateButtonGet();
      if (stopBtn.justPressed) {
        gateInst ^= 1;
      }
      if (encBtn.pressed) {
        gateInst = 0;
        gateLeds = 0;
      }
    }
    
    
    
    
  } 
}
    
    
    
void SetLedsExpander()
{
  
  
  
  
  configLed = (((tempoBtn.pressed | seq.configMode ) << 15) | (seq.setupNeedSaved << 8) | (showTrigLeds << 12) | (bankLed << 13) | (muteLed << 14)); 
  
  if (muteLed) {  
    SetDoutLed(stepLeds | muteLeds, configLed , menuLed | (muteInst & 1)); 
  }
  else if (bankLed) {
    SetDoutLed(stepLeds | gateLeds, configLed , menuLed | (gateInst & 1));
  }
  else {
    SetDoutLed(stepLeds, configLed , menuLed);
  }
  
  menuLed = ((shiftBtn << 1) | ((~(PORTA >> 2)) & 1U & showTrigLeds));
  lastStepLeds = stepLeds;
}


void SeqConfigurationExpander()
{
  
   if (shiftBtn) {
      
      if (tempoBtn.justPressed){
        seq.configMode  = TRUE;
        seq.configPage++;
        curIndex = 0;
        bankLed = 0;
        muteLed = 0;
        needLcdUpdate = TRUE;
        if (seq.configPage > MAX_CONF_PAGE){
          seq.configPage = 0;
          seq.configMode  = FALSE;
          seq.setupNeedSaved = FALSE;
          LcdUpdate();
        }
      }
    }
    
    if (seq.configMode) {
            
      ButtonGet();
      EncGet();
      
      if (encBtn.justPressed){
        curIndex++;
        if (curIndex >= MAX_CUR_POS)  curIndex = 0;
        needLcdUpdate = TRUE;
      }
      
      if (playBtn.justPressed) {
          seq.configPage = 0;
          seq.configMode  = FALSE;
          seq.setupNeedSaved = FALSE;
          needLcdUpdate = TRUE;
      }
      

      SeqConfiguration();
      if (enterBtn.justRelease) needLcdUpdate = TRUE;
      LcdUpdate();
    }
  
}



