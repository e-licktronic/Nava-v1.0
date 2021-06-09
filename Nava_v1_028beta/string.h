//-------------------------------------------------
//                  NAVA v1.x
//                  LCD String
// Strings to be stored as Program (not in RAM, but in Flash)
//-------------------------------------------------

#ifndef string_h
#define string_h



prog_char txt_INST0[] PROGMEM   ="TRG";
prog_char txt_INST1[] PROGMEM   ="   ";
prog_char txt_INST2[] PROGMEM   =" HT";
prog_char txt_INST3[] PROGMEM   ="RIM";
prog_char txt_INST4[] PROGMEM   ="HCL";
prog_char txt_INST5[] PROGMEM   ="   ";
prog_char txt_INST6[] PROGMEM   ="RID";
prog_char txt_INST7[] PROGMEM   ="CRH";
prog_char txt_INST8[] PROGMEM   =" BD";
prog_char txt_INST9[] PROGMEM   =" SD";
prog_char txt_INST10[] PROGMEM  =" LT";
prog_char txt_INST11[] PROGMEM  =" MT";
prog_char txt_INST12[] PROGMEM  ="ACC";
prog_char txt_INST13[] PROGMEM  ="EXT";
prog_char txt_INST14[] PROGMEM  =" CH";
prog_char txt_INST15[] PROGMEM  =" OH";

prog_char txt_OCT0[] PROGMEM   ="-3";
prog_char txt_OCT1[] PROGMEM   ="-2";
prog_char txt_OCT2[] PROGMEM   ="-1";
prog_char txt_OCT3[] PROGMEM   ="+0";
prog_char txt_OCT4[] PROGMEM   ="+1";
prog_char txt_OCT5[] PROGMEM   ="+2";
prog_char txt_OCT6[] PROGMEM   ="+3";
prog_char txt_OCT7[] PROGMEM   ="+4";

prog_char txt_NOTE0[] PROGMEM   =" C";
prog_char txt_NOTE1[] PROGMEM   ="C#";
prog_char txt_NOTE2[] PROGMEM   =" D";
prog_char txt_NOTE3[] PROGMEM   ="D#";
prog_char txt_NOTE4[] PROGMEM   =" E";
prog_char txt_NOTE5[] PROGMEM   =" F";
prog_char txt_NOTE6[] PROGMEM   ="F#";
prog_char txt_NOTE7[] PROGMEM   =" G";
prog_char txt_NOTE8[] PROGMEM   ="G#";
prog_char txt_NOTE9[] PROGMEM   =" A";
prog_char txt_NOTE10[] PROGMEM   ="A#";
prog_char txt_NOTE11[] PROGMEM   =" B";

prog_char txt_SYNC0[] PROGMEM   ="MST";
prog_char txt_SYNC1[] PROGMEM   ="SLV";
prog_char txt_SYNC2[] PROGMEM   ="EXP";                                        // [zabox] [1.028] 

prog_char txt_PTRNSYNCCHANGE[] PROGMEM   ="SYN";
prog_char txt_PTRNFREECHANGE[] PROGMEM   ="FRE";

prog_char txt_MUTE0[] PROGMEM   ="C/O";                                        // [zabox] HH mute mode
prog_char txt_MUTE1[] PROGMEM   ="HH";                                         //

//synchro name----------------------------------------------------
PROGMEM const char *nameSync[]={
  txt_SYNC0, txt_SYNC1, txt_SYNC2};                                            // [zabox] [1.028]
  
  //Pattern change sync name--------------------------------------
PROGMEM const char *namePtrnChange[]={
  txt_PTRNFREECHANGE, txt_PTRNSYNCCHANGE};
  
//Mute Mode-------------------------------------------------------            // [zabox] HH mute mode
PROGMEM const char *nameMute[]={
  txt_MUTE0, txt_MUTE1};
  
//instrument name-------------------------------------------------
PROGMEM const char *selectInstString[]={
  txt_INST0,txt_INST1,txt_INST2,txt_INST3,
  txt_INST4,txt_INST5,txt_INST6,txt_INST7,
  txt_INST8,txt_INST9,txt_INST10,txt_INST11,
  txt_INST12,txt_INST13,txt_INST14,txt_INST15};

//Octaves names------------------------------------------------------
PROGMEM const char *nameOct[] = { 
 txt_OCT0,txt_OCT1,txt_OCT2,txt_OCT3,txt_OCT4,txt_OCT5,txt_OCT6,txt_OCT7 };

//Notes names----------------------------------------------------
PROGMEM const char *nameNote[] = { 
  txt_NOTE0,txt_NOTE1,txt_NOTE2,txt_NOTE3,txt_NOTE4,txt_NOTE5,txt_NOTE6,txt_NOTE7,txt_NOTE8,txt_NOTE9,txt_NOTE10,txt_NOTE11};

//Special character-----------------------------------------------
byte font0[8] = {
  0x10, 0x10, 0x11, 0x12, 0x04, 0x08, 0x10, 0x00};// 1/  font
byte font1[8] = {
  0x00, 0x00, 0x17, 0x14, 0x17, 0x15, 0x17, 0x00};// small 16 font
byte font2[8] = {
  0x00, 0x00, 0x10, 0x18, 0x10, 0x10, 0x0C, 0x00};//small T font
byte font3[8] = {
  0x00, 0x00, 0x0E, 0x0A, 0x0E, 0x0A, 0x0E, 0x00};//small 8 font
byte font4[8] = {
  0x00, 0x00, 0x1B, 0x09, 0x1B, 0x0A, 0x1B, 0x00};// Small 32 font
byte font5[8] = {
  0x01, 0x01, 0x01, 0x01, 0x01, 0x0F, 0x1F, 0x0E};//noir tempo

#endif//end if string_h







