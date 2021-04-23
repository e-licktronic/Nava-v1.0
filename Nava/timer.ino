//-------------------------------------------------
//                  NAVA v1.x
//             Timer Configuration
//-------------------------------------------------

void TimerStart()
{
  TCCR1A = TCCR1B = 0;
  //prescale 8 => 16000000/8 = 2000000 Hz by tick
  TCCR1B |= _BV (CS11) | _BV (WGM12);
  TimerSetFrequency();
  TIMSK1 |= _BV(OCIE1A);

}

void TimerStop(void)
{
  bitWrite(TIMSK1, OCIE1A, 0);
  TCCR1A = TCCR1B = OCR1A = 0;
}

void TimerSetFrequency()
{
  // Calculates frequency for Timer1 = (BPM*ppqn)/60s
#define FREQUENCY (((seq.bpm)*(PPQN))/60)
  OCR1A =(F_CPU/ 8) / FREQUENCY;
}

