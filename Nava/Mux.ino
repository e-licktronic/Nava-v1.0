//-------------------------------------------------
//                  NAVA v1.x
//                  Dio utility
//-------------------------------------------------

/////////////////////Function//////////////////////
//Update Velocity of each instruments---------------------------------------
//Must be atomic to get right instrument velocity ot NOT TO BE DEFINE!!!!
void SetMux()
{
  //cli();
  for ( byte x = 0; x < NBR_MUX; x++)
  {
    delayMicroseconds(30);//Solve HT velocity response issue
    bitWrite(MUX_INH_PORT, MUX_INH1_BIT, x);//x);
    bitWrite(MUX_INH_PORT, MUX_INH2_BIT, !x);  
    for ( byte a = 0; a < 5; a++){
      SetDacA(pattern[ptrnBuffer].velocity[muxInst[a+(5 * x)]][curStep] + ((bitRead(pattern[ptrnBuffer].inst[TOTAL_ACC], curStep)) ? (pattern[ptrnBuffer].totalAcc * 4) : 0));
      PORTA = dinStartState | dinClkState << 1 | 1 << 2 |(muxAddr[a] << 5);//need bit 2 to 1 to not disturb trig out who is on the same PORTA
      delayMicroseconds(4);
    }
  }
  //sei();
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
      delayMicroseconds(4);
    }
  }
  //sei();
}

void SetMuxTrigMidi(byte inst, byte velocity)
{
  //cli();
  midiVelocity[inst] = velocity;
  for ( byte x = 0; x < NBR_MUX; x++)
  {
    delayMicroseconds(30);//Solve HT velocity response issue
    bitWrite(MUX_INH_PORT, MUX_INH1_BIT, x);//x);
    bitWrite(MUX_INH_PORT, MUX_INH2_BIT, !x);  
    for ( byte a = 0; a < 5; a++){
      SetDacA(midiVelocity[muxInst[a+(5*x)]]);
      PORTA = dinStartState | dinClkState << 1 | 1 << 2 | (muxAddr[a] << 5);
      delayMicroseconds(4);
    }
  }
  //sei();
}
