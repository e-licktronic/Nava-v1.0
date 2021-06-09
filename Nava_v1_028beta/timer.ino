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




void initTrigTimer() {                         // [zabox] [v1.028] timer 2 setup for 2ms interrupt

  TCCR2A = _BV(WGM21);                           // Set timer to ctc mode
  //TCCR2B = _BV(CS22) | (_BV(CS20);             // Set prescaler to 128
  TRIG_TIMER_STOP;
  TRIG_TIMER_ZERO;
  OCR2A = 249;                                   // Set output compare register to 2ms
  TIMSK2 = _BV(OCIE2A);                          // Enable OC interrupt
 
}


void initExpTimer() {                         // [zabox] [v1.028] timer 2 setup for 2ms interrupt

  TCCR2A = 0;                                    // Set timer to normal mode
  TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20);    // Set prescaler to 128
  TIMSK2 = 0;                                    // Enable OC interrupt
 
}


void initFlamTimer() {                        // [zabox] [v1.028] timer 3 setup for 2ms interrupt

 TCCR3A = 0;                                    // initialize
 TCCR3C = 0;                                    // initialize
 FLAM_TIMER_STOP;
 cli();
 FLAM_TIMER_ZERO;
 OCR3A = flam[1];                               // Set output compare register to 2ms
 sei();
 TIMSK3 = _BV(OCIE3A);                          // Enable OC interrupt
 
}



void setFlam() {
  if (OCR3A != flam[pattern[ptrnBuffer].flam]) {
    OCR3A = flam[pattern[ptrnBuffer].flam];                              // Set output compare register to 2m
  }
}
  
  
