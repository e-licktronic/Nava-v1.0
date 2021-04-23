//-------------------------------------------------
//                  NAVA v1.x
//              Midi Function
//-------------------------------------------------

/////////////////////Function//////////////////////

//initialize midi real time variable
void  InitMidiRealTime()
{
  midiStart = LOW;
  midiStop = LOW;
  midiContinue = LOW;
}

//intialize note off when stop or patternChanged  or ext instrument loop back
void InitMidiNoteOff()
{
  if(midiNoteOnActive){
    midiNoteOnActive = FALSE;
    if (noteIndexCpt) MidiSendNoteOff(seq.TXchannel, pattern[ptrnBuffer].extNote[noteIndexCpt - 1]);
    else MidiSendNoteOff(seq.TXchannel, pattern[ptrnBuffer].extNote[pattern[ptrnBuffer].extLength]);
  }
}

//Send note OFF
void MidiSendNoteOff (byte channel, byte value)
{
  MIDI.sendNoteOff(value + 12, 0, channel);
  /* MidiSend(channel + 0x80);
   MidiSend(value);
   MidiSend(0x00);*/
}
//Send note ON
void MidiSendNoteOn (byte channel, byte value, byte velocity)
{
  MIDI.sendNoteOn(value + 12, velocity, channel);
  /* MidiSend(channel + 0x90);
   MidiSend(value);
   MidiSend(velocity);*/
}

//Handle clock
void HandleClock()
{
  DIN_CLK_HIGH;
  CountPPQN();//execute 4x because internal sequencer run as 96 ppqn
  CountPPQN();
  delayMicroseconds(2000);
  CountPPQN();
  CountPPQN();
  DIN_CLK_LOW;

}

//handle start
void HandleStart()
{
  midiStart = HIGH;
}

//handle stop
void HandleStop()
{
  midiStop = HIGH;
}

//handle Continue
void HandleContinue()
{
  midiContinue = HIGH;
}

//Disconned midi real time callback
void DisconnectMidiHandleRealTime()
{
  MIDI.disconnectCallbackFromType(Clock);
  MIDI.disconnectCallbackFromType(Start);
  MIDI.disconnectCallbackFromType(Stop);
  MIDI.disconnectCallbackFromType(Continue);
}

//Handle noteON
void HandleNoteOn(byte channel, byte pitch, byte velocity)
{
  if ( velocity != 0 )
  { 
    if (channel == seq.RXchannel){
      switch (pitch){
      case 35:
      case 36:
        MidiTrigOn(BD, velocity);
        break;
      case 38:
      case 40:
        MidiTrigOn(SD, velocity);
        break;
      case 41:
        MidiTrigOn(LT, velocity);
        break;
      case 45:
      case 47:
      case 48:
        MidiTrigOn(MT, velocity);
        break;
      case 50:
        MidiTrigOn(HT, velocity);
        break;
      case 37:
        MidiTrigOn(RM, velocity);
        break;
      case 39:
        MidiTrigOn(HC, velocity);
        break;
      case 42:
        MidiTrigOn(CH, velocity);
        break;
      case 46:
        MidiTrigOn(OH, velocity);
        break;
      case 51:
        MidiTrigOn(RIDE, velocity);
        break;
      case 49:
        MidiTrigOn(CRASH, velocity);
        break;
      case 56:
        TRIG_LOW;
        break;
      case 60:
      case 61:
      case 62:
      case 63:
      case 64:
      case 65:
      case 66:
      case 67:
        // Bank Select
        Serial.print("Bank Select: ");
        Serial.println(char((pitch-60) + 65));
        curBank = pitch-60;
        nextPattern = curBank * NBR_PATTERN + (curPattern % NBR_PATTERN);
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
        break;
      case 72:
      case 73:
      case 74:
      case 75:
      case 76:
      case 77:
      case 78:
      case 79:
      case 80:
      case 81:
      case 82:
      case 83:
      case 84:
      case 85:
      case 86:
      case 87:
        // Pattern Select
        Serial.print("Pattern Select: ");
        Serial.println((pitch-72) + 1);
        group.priority = FALSE;
        nextPattern = ( pitch - 72 ) + curBank * NBR_PATTERN;
        group.pos = pattern[ptrnBuffer].groupPos;
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
        break;
      }
    }
  } else {
    HandleNoteOff(channel,pitch,velocity);
  }
}

//Handle noteOFF
void HandleNoteOff(byte channel, byte pitch, byte velocity)
{
  if (channel == seq.RXchannel){
    switch (pitch){
    case 35:
    case 36:
      MidiTrigOff(BD);
      break;
    case 38:
    case 40:
      MidiTrigOff(SD);
      break;
    case 41:
      MidiTrigOff(LT);
      break;
    case 45:
    case 47:
    case 48:
      MidiTrigOff(MT);
      break;
    case 50:
      MidiTrigOff(HT);
      break;
    case 37:
      MidiTrigOff(RM);
      break;
    case 39:
      MidiTrigOff(HC);
      break;
    case 42:
      MidiTrigOff(CH);
      break;
    case 46:
      MidiTrigOff(OH);
      break;
    case 51:
      MidiTrigOff(RIDE);
      break;
    case 49:
      MidiTrigOff(CRASH);
      break;
    case 60:
      TRIG_HIGH;
      break;
    }
  }
}


//MidiTrigOn insturment
void MidiTrigOn(byte inst, byte velocity)
{
  if (instWasMidiTrigged[inst] == FALSE){
    if (inst == OH) SetMuxTrigMidi(CH, velocity);//OH and CH share same velocity
    else SetMuxTrigMidi(inst, velocity);
    if (inst == CH || inst == OH){
      SetDoutTrig(1 << HH);
      delayMicroseconds(2000);
      if (inst == OH) SetDoutTrig(0);
      else SetDoutTrig(1 << HH_SLCT);
    }
    else {
      SetDoutTrig(1 << inst);
      delayMicroseconds(2000);
      SetDoutTrig(0);
    }
    instWasMidiTrigged[inst] = TRUE;
  }
}

//MidiTrigOff insturment
void MidiTrigOff(byte inst)
{
  instWasMidiTrigged[inst] = FALSE;
}

//Midi send all note off
void SendAllNoteOff()
{
  MIDI.sendControlChange(ALL_NOTE_OFF , 0, seq.TXchannel);	
}
