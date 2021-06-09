//-------------------------------------------------
//                  NAVA v1.x
//                  Dio utility
//-------------------------------------------------

/////////////////////Function//////////////////////
//Update Velocity of each instruments---------------------------------------
void SetMux()
{      

  unsigned int temp_stepValue = stepValue & (~muteInst);                                                                      // [zabox] [1.028] solves velocity change in fading instuments (when muted) in combination with total accent
  
 //Select second Multiplexer 
  for (byte a = 0; a < 5; a++){
    
    
  //if(bitRead (stepValue, muxInst[a + 5]) || bitRead(stepValue, OH)){
    if(bitRead(temp_stepValue, muxInst[a + 5]) || (bitRead(temp_stepValue, OH) && (muxInst[a + 5] == CH ))){                 // [zabox] prevents mux from updating all velocities at a oh step
      //DeselectSecondMux();//Unselect Multiplexer
      //As CH and OH share same Mux out this code is needed
     // if (bitRead(stepValue, OH && muxInst[a + 5] == CH )){                                                                 // [zabox] bracket error, caused the ext_trig bug. took me hours to spot :/
      if (bitRead(temp_stepValue, OH) && (muxInst[a + 5] == CH )){                                                            // [zabox] correct bracketing
        SetDacA(pattern[ptrnBuffer].velocity[OH][curStep] + ((bitRead(pattern[ptrnBuffer].inst[TOTAL_ACC], curStep)) ? (pattern[ptrnBuffer].totalAcc * 4) : 0));
        PORTA = (PORTA & 0b00011111) | muxAddr[2];//Mask to clear last 3 bits of the PORTA
        SelectSecondMux();//Set the value to the multiplexer out
      }
      else  {
        SetDacA(pattern[ptrnBuffer].velocity[muxInst[a + 5]][curStep] + ((bitRead(pattern[ptrnBuffer].inst[TOTAL_ACC], curStep)) ? (pattern[ptrnBuffer].totalAcc * 4) : 0));//Set DAC Value
        //Set Multiplexer address
        PORTA = (PORTA & 0b00011111) | muxAddr[a];//Mask to clear last 3 bits of the PORTA
        SelectSecondMux();//Set the value to the multiplexer out
      }
    }
  }
  //DeselectSecondMux();//Unselect Multiplexer
  

  //Select First Multiplexer 
  for (byte a = 0; a < 5; a++){
    if(bitRead (temp_stepValue, muxInst[a])){  
      //DeselectFirstMux();//Unselect Multiplexer
      byte vel = pattern[ptrnBuffer].velocity[muxInst[a]][curStep];                   // [zabox] [1.028] flam
      if (bitRead(vel, 7)) {
        stepValueFlam |= (1 << muxInst[a]);
        vel &= 127;
      }
      SetDacA(vel + ((bitRead(pattern[ptrnBuffer].inst[TOTAL_ACC], curStep)) ? (pattern[ptrnBuffer].totalAcc * 4) : 0));//Set DAC Value 
      //Set Multiplexer address
      PORTA = (PORTA & 0b00011111) | muxAddr[a];//Mask to clear last 3 bits of the PORTA
      SelectFirstMux();//Set the value to the multiplexer out
    }
  }
  //DeselectFirstMux();//Unselect Multiplexer
  
}



void SetMuxFlam() {
 
 for (byte a = 0; a < 5; a++){
    if(bitRead (stepValueFlam, muxInst[a])){  
      //DeselectFirstMux();//Unselect Multiplexer
      byte vel = pattern[ptrnBuffer].velocity[muxInst[a]][curStep];                   // [zabox] [1.028] flam
      if (bitRead(vel, 7)) {
        vel &= 127;
      }
      SetDacA(vel + ((bitRead(pattern[ptrnBuffer].inst[TOTAL_ACC], curStep)) ? (pattern[ptrnBuffer].totalAcc * 4) : 0));//Set DAC Value 
      //Set Multiplexer address
      PORTA = (PORTA & 0b00011111) | muxAddr[a];//Mask to clear last 3 bits of the PORTA
      SelectFirstMux();//Set the value to the multiplexer out
    }
  }
  //DeselectFirstMux();//Unselect Multiplexer
}



void SetMuxTrig(int data)
{
  //cli();
  for ( byte x = 0; x < NBR_MUX; x++)
  {
    delayMicroseconds(30);//Solve HT velocity response issue
    bitWrite(MUX_INH_PORT, MUX_INH1_BIT, x);//x);
    bitWrite(MUX_INH_PORT, MUX_INH2_BIT, !x);  
    for ( byte a = 0; a < 5; a++){
      SetDacA(data);
      PORTA = dinStartState | dinClkState << 1 | 1 << 2 | (muxAddr[a] << 5);
    }
  }
  //sei();
}

void SetMuxTrigMidi(byte inst, byte velocity)
{
  //Remember velocity of each MIDI trigged instrument
  midiVelocity[inst] = velocity;

  switch (inst){
  case BD:
    //DeselectFirstMux();//Unselect Multiplexer                                                     // [1.028] this caused cuts in the digital section when a first mux instrument was played shortly after. (the other mux stays selected!) 
    SetDacA(midiVelocity[inst]);//Set DAC Value 
    //Set Multiplexer address
    PORTA = (PORTA & 0b00011111) | muxAddr[2];//Mask to clear last 3 bits of the PORTA
    SelectFirstMux();//Set the value to the multiplexer out
    break;
  case SD:
    //DeselectFirstMux();//Unselect Multiplexer
    SetDacA(midiVelocity[inst]);//Set DAC Value 
    //Set Multiplexer address
    PORTA = (PORTA & 0b00011111) | muxAddr[1];//Mask to clear last 3 bits of the PORTA
    SelectFirstMux();//Set the value to the multiplexer out
    break;
  case LT:
    //DeselectFirstMux();//Unselect Multiplexer
    SetDacA(midiVelocity[inst]);//Set DAC Value 
    //Set Multiplexer address
    PORTA = (PORTA & 0b00011111) | muxAddr[0];//Mask to clear last 3 bits of the PORTA
    SelectFirstMux();//Set the value to the multiplexer out
    break;
  case MT:
    //DeselectFirstMux();//Unselect Multiplexer
    SetDacA(midiVelocity[inst]);//Set DAC Value 
    //Set Multiplexer address
    PORTA = (PORTA & 0b00011111) | muxAddr[3];//Mask to clear last 3 bits of the PORTA
    SelectFirstMux();//Set the value to the multiplexer out
    break;
  case HT:
    //DeselectFirstMux();//Unselect Multiplexer
    SetDacA(midiVelocity[inst]);//Set DAC Value 
    //Set Multiplexer address
    PORTA = (PORTA & 0b00011111) | muxAddr[4];//Mask to clear last 3 bits of the PORTA
    SelectFirstMux();//Set the value to the multiplexer out
    break;
  case RM:
    //DeselectSecondMux();//Unselect Multiplexer
    SetDacA(midiVelocity[inst]);//Set DAC Value 
    //Set Multiplexer address
    PORTA = (PORTA & 0b00011111) | muxAddr[1];//Mask to clear last 3 bits of the PORTA
    SelectSecondMux();//Set the value to the multiplexer out
    break;
  case HC:
    //DeselectSecondMux();//Unselect Multiplexer
    SetDacA(midiVelocity[inst]);//Set DAC Value 
    //Set Multiplexer address
    PORTA = (PORTA & 0b00011111) | muxAddr[0];//Mask to clear last 3 bits of the PORTA
    SelectSecondMux();//Set the value to the multiplexer out
    break;
  case OH:
  case CH:
    //DeselectSecondMux();//Unselect Multiplexer
    SetDacA(midiVelocity[inst]);//Set DAC Value 
    //Set Multiplexer address
    PORTA = (PORTA & 0b00011111) | muxAddr[2];//Mask to clear last 3 bits of the PORTA
    SelectSecondMux();//Set the value to the multiplexer out
    break;
  case RIDE:
    //DeselectSecondMux();//Unselect Multiplexer
    SetDacA(midiVelocity[inst]);//Set DAC Value 
    //Set Multiplexer address
    PORTA = (PORTA & 0b00011111) | muxAddr[4];//Mask to clear last 3 bits of the PORTA
    SelectSecondMux();//Set the value to the multiplexer out
    break;
  case CRASH:
    //DeselectSecondMux();//Unselect Multiplexer
    SetDacA(midiVelocity[inst]);//Set DAC Value 
    //Set Multiplexer address
    PORTA = (PORTA & 0b00011111) | muxAddr[3];//Mask to clear last 3 bits of the PORTA
    SelectSecondMux();//Set the value to the multiplexer out
    break;
  }
}

//Select IC114
void SelectFirstMux(){
  delayMicroseconds(15);                                          //Wait stabilisation  [zabox] moved for better visibility, 15us are enough (4,1v/(0.55v/us) slew rate + 4us settling time = max 12us, verified with scope)
  MUX_INH1_LOW;
  delayMicroseconds(5);                                           // [zabox] charge s/h caps
  MUX_INH1_HIGH;                                                  // [1.028] moved from deselect function
}


//Select IC111
void SelectSecondMux(){
  delayMicroseconds(15);                                          //Wait stabilisation  [zabox] moved for better visibility, 15us are enough
  MUX_INH2_LOW;
  delayMicroseconds(5);                                           // [zabox] charge s/h caps
  MUX_INH2_HIGH;                                                  // [1.028] moved from deselect function
}












