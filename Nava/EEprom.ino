//-------------------------------------------------
//                  NAVA v1.x
//                   EEprom 1024K
//
//Pattern size:
//  -16 instruments with 2 bytes each = 32 bytes
//  -16 steps of 16 instruments velocity (0 to 127 so 1 byte) = 256 bytes
//  -100 notes ext instrument  = 128 bytes// to do
//  -Setup pattern, each 1 byte: Scale, Shuffle, Flam, Length, ext length, group position, group length and 25 more parameters = 32 b
//  PATTERN_SIZE = 448 bytes x 128 = 57344 bytes(7 pages of 64 bytes)
//  OFFSET_PATTERN = 0
//
//Track size:
//  -each track is 1022 pattern = 1022 bytes  OFFICALY 1000 patterns by to got a multiple of 64 bytes page
//  -track lenght = 2 bytes   the last two byte in "track[trkBuffer].patternNbr[trk.pos]"
//  TRACK_SIZE = 1024 bytes x 16 = 16384 bytes
//  OFFSET_TRACK = MAX_PTRN * 448 = 57344 bytes
//
//Setup size:
//  -OFFSET_SETUP = TRACK_OFFSET + (TRACK_SIZE * MAX_TRACK) = 73728;
//  -SETUP_SIZE = 64 bytes  only 4 bytes used NOW...
//
//
//Total 1M bits EEprom size: 131072 bytes
//-------------------------------------------------

#define PTRN_SIZE          (unsigned long)(448)
#define PTRN_OFFSET        (unsigned long)(0)
#define PTRN_TRIG_SIZE     (unsigned long)(32)
#define PTRN_TRIG_OFFSET   (PTRN_OFFSET + PTRN_TRIG_SIZE)
#define PTRN_SETUP_SIZE    (unsigned long)(32)
#define PTRN_SETUP_OFFSET  (PTRN_TRIG_OFFSET + PTRN_SETUP_SIZE)
#define PTRN_EXT_SIZE      (unsigned long)(128)
#define PTRN_EXT_OFFSET    (PTRN_SETUP_OFFSET + PTRN_EXT_SIZE)
#define PTRN_VEL_SIZE      (unsigned long)(256)
#define PTRN_VEL_OFFSET    (PTRN_EXT_OFFSET + PTRN_VEL_SIZE)
#define TRACK_SIZE         (unsigned long)(1024)
#define TRACK_OFFSET       (PTRN_SIZE * MAX_PTRN)
#define OFFSET_SETUP       (TRACK_OFFSET + (TRACK_SIZE * MAX_TRACK))
#define SETUP_SIZE         (unsigned long)(64)
#define OFFSET_GROUP       (unsigned long)(37) //save only .groupPos and .groupLength
#define GROUP_SIZE         (unsigned long)(2)
#define MAX_PAGE_SIZE      (unsigned long)(64)
#define HRDW_ADDRESS       0x50
#define HRDW_ADDRESS_UP    (0x50 | B100)
#define DELAY_WR           5

////////////////////////Function//////////////////////

// Non Blocking Delay
void Ndelay(unsigned long delay_ms)
{
  unsigned long time_now = millis();
  unsigned long period = time_now;
  while ( period < time_now + delay_ms )
  {
    period=millis();
  }
//  Serial.print("Ndelay: ");
//  Serial.println(period - time_now);
}

//save pattern
void SavePattern(byte patternNbr)
{
  unsigned long adress = (unsigned long)(PTRN_OFFSET + patternNbr * PTRN_SIZE);
  WireBeginTX(adress); 
  // Serial.println(adress);
  //TRIG-----------------------------------------------
  for (byte i = 0; i < NBR_INST; i++){ 
    byte lowbyte =  (pattern[ptrnBuffer].inst[i] & 0xFF);
    byte highbyte = (pattern[ptrnBuffer].inst[i] >> 8) & 0xFF;
    Wire.write((byte)(lowbyte)); 
    Wire.write((byte)(highbyte));
  }//32 bytes

  //SETUP-----------------------------------------------
  Wire.write((byte)(pattern[ptrnBuffer].length));
  Wire.write((byte)(pattern[ptrnBuffer].scale));
  Wire.write((byte)(pattern[ptrnBuffer].shuffle));
  Wire.write((byte)(pattern[ptrnBuffer].flam));
  Wire.write((byte)(pattern[ptrnBuffer].extLength));
  Wire.write((byte)(pattern[ptrnBuffer].groupPos));
  Wire.write((byte)(pattern[ptrnBuffer].groupLength));
  Wire.write((byte)(pattern[ptrnBuffer].totalAcc));

  for(int a =0; a < 24; a++){
    Wire.write( (byte)(0));//unused parameters
  }//32 bytes

  Wire.endTransmission();//end page transmission
  Ndelay(DELAY_WR);//delay between each write page

  //EXT INST-----------------------------------------------
  for(int nbrPage = 0; nbrPage < 2; nbrPage++){
    adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_SETUP_OFFSET);
    //Serial.println(adress);
    WireBeginTX(adress);
    for (byte j = 0; j < MAX_PAGE_SIZE; j++){
      Wire.write((byte)(pattern[ptrnBuffer].extNote[j + (MAX_PAGE_SIZE * nbrPage)] & 0xFF));
    }//64 bytes  fisrt page
    Wire.endTransmission();//end page transmission
    Ndelay(DELAY_WR);//delay between each write page
  }//2 * 64 bytes = 128 bytes

  //VELOCITY-----------------------------------------------
  for(int nbrPage = 0; nbrPage < 4; nbrPage++){
    adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_EXT_OFFSET);
    //Serial.println(adress);
    WireBeginTX(adress);
    for (byte i = 0; i < 4; i++){//loop as many instrument for a page
      for (byte j = 0; j < NBR_STEP; j++){
        Wire.write((byte)(pattern[ptrnBuffer].velocity[i + 4*nbrPage][j] & 0xFF)); 
      }
    }
    Wire.endTransmission();//end of 64 bytes transfer
    Ndelay(DELAY_WR);//delay between each write page
  }//4 * 64 bytes = 256 bytes
}

//Load Pattern
void LoadPattern(byte patternNbr)
{
  unsigned long adress = (unsigned long)(PTRN_OFFSET + patternNbr * PTRN_SIZE);
  WireBeginTX(adress); 
  Wire.endTransmission();
  Wire.requestFrom(HRDW_ADDRESS,MAX_PAGE_SIZE); //request a 64 bytes page
  //TRIG-----------------------------------------------
  for(int i =0; i<NBR_INST;i++){
    pattern[!ptrnBuffer].inst[i] = (unsigned long)((Wire.read() & 0xFF) | (( Wire.read() << 8) & 0xFF00));
    // Serial.println(Wire.read());
  }
  //SETUP-----------------------------------------------
  pattern[!ptrnBuffer].length = Wire.read();
  pattern[!ptrnBuffer].scale = Wire.read();
  pattern[!ptrnBuffer].shuffle = Wire.read();
  pattern[!ptrnBuffer].flam = Wire.read();
  pattern[!ptrnBuffer].extLength = Wire.read();
  pattern[!ptrnBuffer].groupPos = Wire.read();
  pattern[!ptrnBuffer].groupLength = Wire.read();
  pattern[!ptrnBuffer].totalAcc = Wire.read();
  for(int a = 0; a < 24; a++){
    Wire.read();
  }
  //EXT INST-----------------------------------------------
  for(int nbrPage = 0; nbrPage < 2; nbrPage++){
    adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_SETUP_OFFSET);
    WireBeginTX(adress);
    Wire.endTransmission();
    Wire.requestFrom(HRDW_ADDRESS,MAX_PAGE_SIZE); //request of  64 bytes

    for (byte j = 0; j < MAX_PAGE_SIZE; j++){
      pattern[!ptrnBuffer].extNote[j + (MAX_PAGE_SIZE * nbrPage) ] = Wire.read();
    }
  }
  //VELOCITY-----------------------------------------------
  for(int nbrPage = 0; nbrPage < 4; nbrPage++){
    adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_EXT_OFFSET);
    WireBeginTX(adress);
    Wire.endTransmission();
    Wire.requestFrom(HRDW_ADDRESS,MAX_PAGE_SIZE); //request of  64 bytes
    for (byte i = 0; i < 4; i++){//loop as many instrument for a page
      for (byte j = 0; j < NBR_STEP; j++){
        pattern[!ptrnBuffer].velocity[i + 4*nbrPage][j] = (Wire.read() & 0xFF);
      }
    }
  }
}

//Track save
void SaveTrack(byte trackNbr)
{
  byte lowbyte =  (byte)(track[trkBuffer].length & 0xFF);
  byte highbyte = (byte)((track[trkBuffer].length >> 8) & 0xFF);
  track[trkBuffer].patternNbr[1022] = lowbyte;
  track[trkBuffer].patternNbr[1023] = highbyte;

  unsigned long adress;
  for(int nbrPage = 0; nbrPage < TRACK_SIZE/MAX_PAGE_SIZE; nbrPage++){
    adress = (unsigned long)(PTRN_OFFSET + (trackNbr * TRACK_SIZE) + (MAX_PAGE_SIZE * nbrPage) + TRACK_OFFSET);
    WireBeginTX(adress);
    for (byte i = 0; i < MAX_PAGE_SIZE; i++){//loop as many instrument for a page
      Wire.write((byte)(track[trkBuffer].patternNbr[i + (MAX_PAGE_SIZE * nbrPage)] & 0xFF)); 
    }
    Wire.endTransmission();//end of 64 bytes transfer
    Ndelay(DELAY_WR);//delay between each write page
  }

}

//Load track
void LoadTrack(byte trackNbr)
{
  unsigned long adress;
  for(int nbrPage = 0; nbrPage < TRACK_SIZE/MAX_PAGE_SIZE; nbrPage++){
    adress = (unsigned long)(PTRN_OFFSET + (trackNbr * TRACK_SIZE) + (MAX_PAGE_SIZE * nbrPage) + TRACK_OFFSET);
    WireBeginTX(adress);
    Wire.endTransmission();
    if (adress > 65535) Wire.requestFrom(HRDW_ADDRESS_UP,MAX_PAGE_SIZE); //request of  64 bytes
    else Wire.requestFrom(HRDW_ADDRESS,MAX_PAGE_SIZE); //request of  64 bytes
    for (byte i = 0; i < MAX_PAGE_SIZE; i++){//loop as many instrument for a page
      track[trkBuffer].patternNbr[i + (MAX_PAGE_SIZE * nbrPage)] = (Wire.read() & 0xFF); 
    }
  }
  byte lowbyte = (byte)track[trkBuffer].patternNbr[1022];
  byte highbyte = (byte)track[trkBuffer].patternNbr[1023];
  track[trkBuffer].length =  (unsigned long)(lowbyte | highbyte << 8);
}

//Save Setup
void SaveSeqSetup()
{
  unsigned long adress = (unsigned long)(OFFSET_SETUP);
  WireBeginTX(adress);
  Wire.write((byte)(seq.sync)); 
  Wire.write((byte)(seq.defaultBpm));
  Wire.write((byte)(seq.TXchannel));
  Wire.write((byte)(seq.RXchannel));
  Wire.write((byte)(seq.EXTchannel));
  Wire.write((byte)(seq.patternSync));
  int ret=Wire.endTransmission();//end page transmission

  Ndelay(DELAY_WR);//delay between each write page
}

//Load Setup
void LoadSeqSetup()
{
  unsigned long adress = (unsigned long)(OFFSET_SETUP);
  WireBeginTX(adress); 
  Wire.endTransmission();
  Wire.requestFrom(HRDW_ADDRESS_UP,SETUP_SIZE); //
  seq.sync = (Wire.read() & 0xFF);
  seq.sync = constrain(seq.sync, 0, 1);
  seq.defaultBpm = (Wire.read() & 0xFF);
  seq.defaultBpm = constrain(seq.defaultBpm, MIN_BPM, MAX_BPM);
  seq.TXchannel = (Wire.read() & 0xFF);
  seq.TXchannel = constrain(seq.TXchannel, 1 ,16);
  seq.RXchannel = (Wire.read() & 0xFF);
  seq.RXchannel = constrain(seq.RXchannel, 1 ,16);
  seq.EXTchannel= (Wire.read() & 0xFF);
  seq.EXTchannel= constrain(seq.EXTchannel, 1, 16);
  seq.patternSync = (Wire.read() & 0xFF);
  seq.patternSync = constrain(seq.patternSync, 0, 1);
}

//Save pattern group
void SavePatternGroup(byte firstPattern, byte length)
{
  for (int a = 0; a <= length ; a++){
    unsigned long adress = (unsigned long)(PTRN_OFFSET + ((firstPattern + a) * PTRN_SIZE) + OFFSET_GROUP);
    WireBeginTX(adress);
    /* Serial.print("adresse=");
     Serial.println(adress);
     Serial.print("groupPos=");
     Serial.println(a);
     Serial.print("groupLeght=");
     Serial.println(length);*/
    Wire.write((byte)(a)); //pattern[ptrnBuffer].groupPos
    Wire.write((byte)(length));//pattern[ptrnBuffer].groupLength
    Wire.endTransmission();//end page transmission
    Ndelay(DELAY_WR);//delay between each write page
  }
}

//Clear pattern group
void ClearPatternGroup(byte firstPattern, byte length)
{
  for (int a = 0; a <= length ; a++){
    unsigned long adress = (unsigned long)(PTRN_OFFSET + ((firstPattern + a) * PTRN_SIZE) + OFFSET_GROUP);
    WireBeginTX(adress);
    Wire.write((byte)(0)); //pattern[ptrnBuffer].groupPos
    Wire.write((byte)(0));//pattern[ptrnBuffer].groupLength
    Wire.endTransmission();//end page transmission
    Ndelay(DELAY_WR);//delay between each write page
  }
}

//Load pattern group type => POSITION = 0 or LENGTH = 1
byte LoadPatternGroup(byte patternNum, byte type)
{
  unsigned long adress = (unsigned long)(PTRN_OFFSET + (patternNum * PTRN_SIZE) + OFFSET_GROUP + type);
  WireBeginTX(adress);
  Wire.endTransmission();
  Wire.requestFrom(HRDW_ADDRESS,(unsigned long)(1)); //
  byte data = (Wire.read() & 0xFF);
  return data;
}


//==================================================================================================
//==================================================================================================
//init pattern in the EEprom
void InitEEprom()
{
  unsigned long adress;
Serial.println("InitEEprom");

  //Pattern init
  for (byte nbrPattern = 0; nbrPattern < MAX_PTRN ; nbrPattern++)
  {
    adress = (unsigned long)(PTRN_OFFSET + nbrPattern * PTRN_SIZE);
    WireBeginTX(adress); 
    // Serial.println(adress);
    //TRIG-----------------------------------------------
    for (byte i = 0; i < NBR_INST; i++){ 
      Wire.write((byte)(0)); 
      Wire.write((byte)(0));
    }//32 bytes

    //SETUP-----------------------------------------------
    Wire.write((byte)(DEFAULT_LEN - 1));
    Wire.write((byte)(DEFAULT_SCALE));
    Wire.write((byte)(DEFAULT_SHUF));
    Wire.write((byte)(0));
    Wire.write((byte)(0));
    Wire.write((byte)(0));
    Wire.write((byte)(0));
    Wire.write((byte)(0));
    for(int a = 0; a < 24; a++){
      //Wire.write( (byte)(0));//unused parameters
    }//32 bytes

    Wire.endTransmission();//end page transmission
    Ndelay(DELAY_WR);//delay between each write page

    //EXT INST-----------------------------------------------
    for(int nbrPage = 0; nbrPage < 2; nbrPage++){
      adress = (unsigned long)(PTRN_OFFSET + (nbrPattern * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_SETUP_OFFSET);
      //Serial.println(adress);
      WireBeginTX(adress);
      for (byte j = 0; j < MAX_PAGE_SIZE; j++){
        Wire.write((byte)(0));
      }//64 bytes -> fisrt page
      Wire.endTransmission();//end page transmission
      Ndelay(DELAY_WR);//delay between each write page
    }//2 * 64 bytes = 128 bytes

    //VELOCITY-----------------------------------------------
    for(int nbrPage = 0; nbrPage < 4; nbrPage++){
      adress = (unsigned long)(PTRN_OFFSET + (nbrPattern * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_EXT_OFFSET);
      //Serial.println(adress);
      WireBeginTX(adress);
      for (byte i = 0; i < 4; i++){//loop as many instrument for a page
        for (byte j = 0; j < NBR_STEP; j++){
          Wire.write((byte)(0)); 
        }
      }
      Wire.endTransmission();//end of 64 bytes transfer
      Ndelay(DELAY_WR);//delay between each write page
    }//4 * 64 bytes = 256 bytes
    static unsigned int tempInitLeds;
    tempInitLeds |= bitSet(tempInitLeds, (nbrPattern / 8));
    SetDoutLed(tempInitLeds, 0, 0);
    //delay(10);
  }
 /* Serial.print("patrn offset add=");
  Serial.println(adress);*/
  
  //Track Init
  for (unsigned long trackNbr = 0; trackNbr < MAX_TRACK; trackNbr++){
    for(unsigned long nbrPage = 0; nbrPage < (TRACK_SIZE/MAX_PAGE_SIZE); nbrPage++){// to init 1024 byte track size we need 16 pages of 64 bytes
      adress = (unsigned long)(PTRN_OFFSET + (trackNbr * TRACK_SIZE) + (MAX_PAGE_SIZE * nbrPage) + TRACK_OFFSET);
      WireBeginTX(adress);
      for (byte i = 0; i < MAX_PAGE_SIZE; i++){//loop as many instrument for a page
        Wire.write((byte)(0)); 
      }
      Wire.endTransmission();//end of 64 bytes transfer
      Ndelay(DELAY_WR);//delay between each write page
    }
    static unsigned int tempInitLeds;
    tempInitLeds |= bitSet(tempInitLeds, trackNbr);
    SetDoutLed(tempInitLeds, 0, 0);
   //delay(10);
  }
 /* Serial.print("track offset add=");
  Serial.println(TRACK_OFFSET);*/
  
  //Setup init
  adress = (unsigned long)(OFFSET_SETUP);
  WireBeginTX(adress);
  Wire.write((byte)(MASTER));//seq.sync)); 
  Wire.write((byte)(DEFAULT_BPM));//seq.defaultBpm));
  Wire.write((byte)(1));//seq.TXchannel));
  Wire.write((byte)(1));//seq.RXchannel));
  Wire.write((byte)(2));//seq.EXTchannel));
  Wire.write((byte)(0));//seq.patternSync));
  Wire.endTransmission();//end page transmission
  Ndelay(DELAY_WR);//delay between each write page
  /*Serial.print("setup offset add=");
  Serial.println((unsigned long)(TRACK_OFFSET + (TRACK_SIZE * MAX_TRACK)));*/
}

//init track in the eeprom //GOT SOME ISSUES: PATTERN 0 TO 18 ARE NOT INTIALIZE CORRECTLY ?!!//
/*void InitEEpromTrack()
{
  unsigned long adress;
  for (unsigned long trackNbr = 1; trackNbr < MAX_TRACK; trackNbr++){
    for(unsigned long nbrPage = 0; nbrPage < (TRACK_SIZE/MAX_PAGE_SIZE); nbrPage++){// to init 1024 byte track size we need 16 pages of 64 bytes
      adress = (unsigned long)(PTRN_OFFSET + (trackNbr * TRACK_SIZE) + (MAX_PAGE_SIZE * nbrPage) + TRACK_OFFSET);
      WireBeginTX(adress);
      for (byte i = 0; i < MAX_PAGE_SIZE; i++){//loop as many instrument for a page
        Wire.write((byte)(0)); 
      }
      Wire.endTransmission();//end of 64 bytes transfer
      delay(DELAY_WR);//delay between each write page
    }
    static unsigned int tempInitLeds;
    tempInitLeds |= bitSet(tempInitLeds, trackNbr);
    SetDoutLed(tempInitLeds, 0, 0);
   //delay(10);
  }
}

//Init sequencer setup
void InitSeqSetup()
{
  unsigned long adress = (unsigned long)(OFFSET_SETUP);
  WireBeginTX(adress);
  Wire.write((byte)(MASTER));//seq.sync)); 
  Wire.write((byte)(DEFAULT_BPM));//seq.defaultBpm));
  Wire.write((byte)(1));//seq.TXchannel));
  Wire.write((byte)(1));//seq.RXchannel));

  Wire.endTransmission();//end page transmission
  delay(DELAY_WR);//delay between each write page
}*/

//wire begin
void WireBeginTX(unsigned long address)
{
  byte hardwareAddress;
  if (address > 65535) hardwareAddress = HRDW_ADDRESS_UP;
  else hardwareAddress = HRDW_ADDRESS;
  
  Wire.beginTransmission(hardwareAddress);
  Wire.write((byte)(address >> 8));
  Wire.write((byte)(address & 0xFF));
}
