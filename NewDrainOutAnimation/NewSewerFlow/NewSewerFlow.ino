#include <Adafruit_NeoPixel.h>

#define Output_1 2 //
#define Output_2 3 //
#define Output_3 4 //
#define Output_4 5 //

#define Input_1 13 //
#define Input_2 12 //
#define Input_3 11 //
#define Input_4 10 //

#define ToiletSound 26 //
#define ShowerSound 27 //

#define RainLvl1 51 //
#define RainLvl2 49 //
#define RainLvl3 47 //

#define Rain_Input 8 //

#define Rain_Output1 22 //
#define Rain_Output2 23 //
#define Rain_Output3 24 //

#define Pixel_Pin 9 //
#define Num_Pixels 261
#define BRIGHTNESS 255

int Runoff = 0;
int RunoffLead = 0;
bool RunoffDrain = false;
float RunoffFade = 0;
bool RunoffStart = false;

bool GutterBegin = false;
const int GutterLength = 3;

unsigned long CurrentTime;
struct Time {
  unsigned long LastTriggered;
  long Duration;
};

Time RainFrame = {0,10};
Time RainWait = {0,500};
Time SewersFrame = {0,10};
Time RunoffWait = {0,4};
Time ProjectedSpeed = {0,10};
Time SewersSpeedWait = RainWait;
Time RainHold = {0,1000};
Time GreenWaveHoldTime = {0,200};
Time BlueWaveHoldTime = {0,400};
Time StageOneStorm = {};
Time StageTwoStorm = {};
Time StageThreeStorm = {};

const int UpperSewerStart = 70;
const int UpperSewerEnd = 142;
const int LengthOfUpperSewer = 30;

const int LowerSewersStart = 143;
const int LowerSewersEnd = Num_Pixels - 12;
const int LowerSewersLength = 46;
const int DrainageLength = 14;
const int DrainageRegulator = LowerSewersStart + DrainageLength;
const int WaterTreatmentStart = LowerSewersEnd - LowerSewersLength;
const int WaterTreatmentEnd = LowerSewersEnd;
const int OceanDumpStart = LowerSewersStart + LowerSewersLength;
const int OceanDumpEnd = LowerSewersStart;
const int LowerSewerCombine = 16;

const int Length = 20;

bool RainOn = false;
int StormLevel = 0;
int ProjectedStormLevel = 0;
int StormIntensity;
bool RainPush = false;
bool Raining = false;

bool CurrentShiftRise = true;
int CurrentShift = 0;

struct ShiftingColor {
  int RedHue[Length];
  int GreenHue[Length];
  int BlueHue[Length];
  int WhiteHue[Length];
  int LastRed;
  int LastGreen;
  int LastBlue;
  int LastWhite;
};

ShiftingColor SewerWaveHue;
ShiftingColor GutterHue;
ShiftingColor FlowOutHue; 

struct StaticColor {
  int RedHue[Length];
  int GreenHue[Length];
  int BlueHue[Length];
};

// StaticColor PipeLeadHue;
StaticColor RunoffLeadHue;
StaticColor RunoffLeadHueTest;

const int StreakLength = 5;
int StreakHue[StreakLength];

int FlowOutIntensity = 0;

Adafruit_NeoPixel strip(Num_Pixels, Pixel_Pin, NEO_GRBW + NEO_KHZ800);

void WaveUpdate(){
  for(int x=0; x < Length; x++){
    SewerWaveHue.GreenHue[x]= (100/2)-((100/2) * sin(x * (3.14/Length)));
    SewerWaveHue.BlueHue[x]= (200/2)-((200/2) * sin(x * (3.14/Length)));

    RunoffLeadHue.BlueHue[x] = (20/2)+((20/2) * cos(x * ((3.14/2)/Length)));
    RunoffLeadHue.GreenHue[x] = (15/2)+((15/2) * cos(x * ((3.14/2)/Length)));

    GutterHue.BlueHue[x]= (40/2)+((40/2)*cos(x*(6.26/Length)));
    GutterHue.GreenHue[x]= (40/2)+((40/2)*cos(x*(6.26/Length)));
  }

  for(int x=0; x < StreakLength; x++){
    StreakHue[x] = (200/2)+((200/2)*cos(x*(3.14/StreakLength)));
  }
  
}

void FlowOutUpdate(){
  for(int x = 0; x < Length; x++){
    FlowOutHue.BlueHue = (FlowOutIntensity/2)-((FlowOutIntensity/2) * sin(x * (3.14/Length)));
    FlowOutHue.GreenHue = ((2*FlowOutIntensity)/2)-(((2*FlowOutInensity)/2) * sin(x * (3.14/Length)));
  }
}

void setup() {
  strip.begin();
  strip.show();
  strip.setBrightness(BRIGHTNESS);
  Serial.begin(9600);

  pinMode(Rain_Input,INPUT);

  pinMode(Rain_Output1,OUTPUT);
  pinMode(Rain_Output2,OUTPUT);
  pinMode(Rain_Output3,OUTPUT);

  WaveUpdate();
}

void Rain(){
  if(digitalRead(Rain_Input)== HIGH && RainPush == false){
    RainWait.LastTriggered = CurrentTime;
    if(ProjectedStormLevel<3 && Raining == false){ProjectedStormLevel++;}
    RainPush = true;
  }
  else if(digitalRead(Rain_Input)==LOW){RainPush = false;}

  

  if(ProjectedStormLevel == 1 || (ProjectedStormLevel < StormLevel && StormLevel == 1)){
    digitalWrite(Rain_Output1, HIGH);
    digitalWrite(Rain_Output2, LOW);
    digitalWrite(Rain_Output3, LOW);

    digitalWrite(RainLvl1, HIGH);

    ProjectedSpeed.Duration = 10;
  }
  else if(ProjectedStormLevel == 2 || (ProjectedStormLevel < StormLevel && StormLevel == 2)){
    digitalWrite(Rain_Output1, HIGH);
    digitalWrite(Rain_Output2, HIGH);
    digitalWrite(Rain_Output3, LOW);

    digitalWrite(RainLvl1, HIGH);
    digitalWrite(RainLvl2, HIGH);

    ProjectedSpeed.Duration = 9;
  }
  else if (ProjectedStormLevel== 3 || (ProjectedStormLevel < StormLevel && StormLevel == 3)){
    digitalWrite(Rain_Output1, HIGH);
    digitalWrite(Rain_Output2, HIGH);
    digitalWrite(Rain_Output3, HIGH);

    digitalWrite(RainLvl1, HIGH);
    digitalWrite(RainLvl1, HIGH);
    digitalWrite(RainLvl2,HIGH);

    ProjectedSpeed.Duration = 7;
  }
  else {
    digitalWrite(Rain_Output1, LOW);
    digitalWrite(Rain_Output2, LOW);
    digitalWrite(Rain_Output3, LOW);

    digitalWrite(RainLvl1,LOW);
    digitalWrite(RainLvl2,LOW);
    digitalWrite(RainLvl3,LOW);

    ProjectedSpeed.Duration = 10;
  }

  if(RainFrame.Duration != ProjectedSpeed.Duration){
    if(CurrentTime >= RainWait.LastTriggered + RainWait.Duration){
      if(RainFrame.Duration > ProjectedSpeed.Duration){RainFrame.Duration--;}
      else if(RainFrame.Duration < ProjectedSpeed.Duration){RainFrame.Duration++;}
    }
  }

  if(SewersFrame.Duration != ProjectedSpeed.Duration){
    if(CurrentTime >= RainWait.LastTriggered + RainWait.Duration){
      if(SewersFrame.Duration > ProjectedSpeed.Duration){SewersFrame.Duration--;}
      else if(SewersFrame.Duration < ProjectedSpeed.Duration){SewersFrame.Duration++;}
    }
  }


  if(StormLevel < ProjectedStormLevel){
    if(CurrentTime >= RainWait.LastTriggered + RainWait.Duration){
      StormLevel++;
      RainWait.LastTriggered = CurrentTime;
    }
  }
  else if(StormLevel > ProjectedStormLevel){
    if(StormLevel == 2){RainHold.Duration = 1500;}
    else {RainHold.Duration = 1000;}
    
    if(CurrentTime >= RainWait.LastTriggered + RainWait.Duration  && CurrentTime >= RainHold.LastTriggered + RainHold.Duration){
      StormLevel--;
      RainWait.LastTriggered = CurrentTime;
    }
  }
  else if(StormLevel == ProjectedStormLevel){
    RainHold.LastTriggered = CurrentTime;
    ProjectedStormLevel = 0;
  }

  if(StormLevel > 0 && ProjectedStormLevel == 0){Raining = true;}
  else{Raining = false;}

  StormIntensity = StormLevel + 1;
}

void UpperSewers(){
  if(CurrentTime >= SewersFrame.Duration + SewersFrame.LastTriggered){
    for(int x = 0; x < LengthOfUpperSewer; x++){
      if (StormLevel == 0){
        strip.setPixelColor(x+UpperSewerStart,0,0,(1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * 10);
        strip.setPixelColor(UpperSewerEnd-x,0,0,(1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * 10);
      }
      else if(StormLevel == 1){
        strip.setPixelColor(x+UpperSewerStart,0,(SewerWaveHue.GreenHue[x-(Length*(x/Length))]+1) * (CurrentShift/3),(1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)));
        strip.setPixelColor(UpperSewerEnd-x,0,(SewerWaveHue.GreenHue[x-(Length*(x/Length))]+1) * (CurrentShift/3),(1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)));
      }
      else if (StormLevel == 2){
        strip.setPixelColor(x+UpperSewerStart,0,(SewerWaveHue.GreenHue[x-(Length*(x/Length))]+1) * (CurrentShift/2),(1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * (10-(CurrentShift/2)));
        strip.setPixelColor(UpperSewerEnd-x,0,(SewerWaveHue.GreenHue[x-(Length*(x/Length))]+1) * (CurrentShift/2),(1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * (10-(CurrentShift/2)));
      }
      else if (StormLevel == 3){
        strip.setPixelColor(x+UpperSewerStart,0,(SewerWaveHue.GreenHue[x-(Length*(x/Length))]+1) * (CurrentShift),(1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * (10-(CurrentShift/2)));
        strip.setPixelColor(UpperSewerEnd-x,0,(SewerWaveHue.GreenHue[x-(Length*(x/Length))]+1) * (CurrentShift),(1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * (10-(CurrentShift/2)));
      }
    }
  }
}

void LowerSewers(){
 if(CurrentTime >= SewersFrame.Duration + SewersFrame.LastTriggered){
    for(int x = 0; x < LowerSewersLength; x++){
      strip.setPixelColor(WaterTreatmentStart + x,0,SewerWaveHue.GreenHue[x-(Length*(x/Length))]+10,10+SewerWaveHue.BlueHue[x-(Length*(x/Length))]);
      if(x <= LowerSewerCombine){strip.setPixelColor(OceanDumpStart - x,0,SewerWaveHue.GreenHue[x-(Length*(x/Length))]+10,10+SewerWaveHue.BlueHue[x-(Length*(x/Length))]);}

      if(StormLevel == 3){
        if(){ //Stage 1: Fade In [x]
          if(x < LowerSewersLength - DrainageLength && x > LowerSewerCombine){strip.setPixelColor(OceanDumpStart - x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))]);}
          if(FlowOutIntensity < 100){FlowOutIntensity++;}
          FlowOutUpdate();
        }
        else if{ // Stage 2: Flash []
          if(x < LowerSewersLength - DrainageLength && x > LowerSewerCombine){strip.setPixelColor(OceanDumpStart - x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))]);}
          else if()
        }
        else if{ // Stage 3: Flow out [x]
          if(x < LowerSewersLength - DrainageLength + Runoff && x > LowerSewerCombine){strip.setPixelColor(OceanDumpStart - x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))]);}
          for(int i = 0; i < Length; i++){
            if(Runoff - i < 0){break;}
            else if(Runoff - i < DrainageLength){strip.setPixelColor((OceanDumpEnd + DrainageLength) - (Runoff - i),0,0,0);} 
          }
          if(Runoff - Length < DrainageLength){Runoff++;}
        }
      }
      else{ //Stage 4: All flow out [x]
        if(x < LowerSewersLength - DrainageLength && x > LowerSewerCombine){strip.setPixelColor(OceanDumpStart - x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))]);}
        
        if(Runoff != RunoffLead){
          for(int i = 0; i < Length; i++){
            if(RunoffLead - i < 0){break;}
            else if(RunoffLead - i < DrainageLength){strip.setPixelColor((OceanDumpEnd + DrainageLength) - (Runoff - i),0,0,0);} 
          }
          if(RunoffLead - Length < DrainageLength){Runoff++;}
        }
        else{
          Runoff = 0;
          RunoffLead = 0;
          if(FlowIntensity > 0){FlowIntensity--;}
          FLowOutUpdate();
        }
      }
    }
  }

    return;
    if (StormLevel == 1){
      for(int x = 0; x < LowerSewersLength; x++){
          strip.setPixelColor(WaterTreatmentStart + x,0,(SewerWaveHue.GreenHue[x-(Length*(x/Length))]+1) * (CurrentShift/3),(1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)));      
          if(x < LowerSewerCombine){strip.setPixelColor(OceanDumpStart - x,0,((SewerWaveHue.GreenHue[x-(Length*(x/Length))]+1) * (CurrentShift/3)),((1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3))));}
          //else if( x < LowerSewerCombine + Runoff){strip.setPixelColor(OceanDumpStart - x,0,RunoffFade*((SewerWaveHue.GreenHue[x-(Length*(x/Length))]+1) * (CurrentShift/3))/100,RunoffFade*((1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)))/100);}
          else{strip.setPixelColor(OceanDumpStart - x,0,0,0);}
      }

      if(Runoff != 0){
        Runoff = 0;
        RunoffLead = 0;
      }
    }

    else if (StormLevel == 2){
      for(int x = 0; x < LowerSewersLength; x++){
        strip.setPixelColor(WaterTreatmentStart + x,0,(SewerWaveHue.GreenHue[x-(Length*(x/Length))]+1) * (CurrentShift/3),(1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)));      
        if( x < LowerSewerCombine + Runoff){strip.setPixelColor(OceanDumpStart - x,0,(SewerWaveHue.GreenHue[x-(Length*(x/Length))]+1) * (CurrentShift/3),(1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)));}
        else{strip.setPixelColor(OceanDumpStart - x,0,0,0);}
      }

      if(Runoff > LowerSewersLength - DrainageLength - LowerSewerCombine || RunoffLead > LowerSewersLength - DrainageLength - LowerSewerCombine){ // Trickle out to ocean
        if(RunoffLead == Runoff){RunoffLead = LowerSewersLength - DrainageLength - LowerSewerCombine;}
        else if (RunoffLead == LowerSewersLength - LowerSewerCombine + Length){RunoffLead = LowerSewersLength - DrainageLength - LowerSewerCombine;}
        else{RunoffLead++;}

        for(int x = 0; x < Length; x++){
          if(RunoffLead - x < LowerSewersLength - LowerSewerCombine - DrainageLength){break;}
          else if(RunoffLead - x < LowerSewersLength - LowerSewerCombine){strip.setPixelColor(LowerSewersStart - LowerSewerCombine - (RunoffLead-x),0,(SewerWaveHue.GreenHue[x]+1) * (CurrentShift/2),(1+SewerWaveHue.BlueHue[x]) * (10-(CurrentShift/2)));}
        }

        if(RunoffLead > LowerSewersLength - DrainageLength - LowerSewerCombine){Runoff = LowerSewersLength - DrainageLength - LowerSewerCombine;}
      }

      else if (Runoff < LowerSewersLength - DrainageLength - LowerSewerCombine){ // Fade In
        RunoffLead = LowerSewersLength - DrainageLength - LowerSewerCombine;
        Runoff = LowerSewersLength - DrainageLength - LowerSewerCombine;
        if(RunoffFade < 100){RunoffFade = RunoffFade + 5;}


        // Trickle out to DrainageRegulator
        // if(RunoffLead < (LowerSewersLength-DrainageLength-LowerSewerCombine)){RunoffLead++;}
        // for(int x = 0; x < abs(RunoffLead -Runoff); x++){strip.setPixelColor(((OceanDumpStart-LowerSewerCombine) - Runoff - x),0,(RunoffLeadHue.GreenHue[x] +1)*(CurrentShift/3),(RunoffLeadHue.BlueHue[x] +1)*(10-(CurrentShift/3)));}
        // if( (abs(RunoffLead - Runoff) > 0 && RunoffLead == (LowerSewersLength-DrainageLength-LowerSewerCombine) || (RunoffLead - Runoff) > Length)){Runoff++;}
      }
    }

    else if (StormLevel == 3){ //Flow out to Ocean
      for(int x = 0; x < LowerSewersLength; x++){
        strip.setPixelColor(WaterTreatmentStart + x,0,(SewerWaveHue.GreenHue[x-(Length*(x/Length))]+1) * (CurrentShift/3),(1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)));      
        if( x < LowerSewerCombine + Runoff){strip.setPixelColor(OceanDumpStart - x,0,(SewerWaveHue.GreenHue[x-(Length*(x/Length))]+1) * (CurrentShift/3),(1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)));}
        else{strip.setPixelColor(OceanDumpStart - x,0,0,0);}
      }

      if(Runoff < LowerSewersLength - LowerSewerCombine || Runoff != RunoffLead){
        if(Runoff < LowerSewersLength-LowerSewerCombine){RunoffLead++;}
        else{RunoffLead = Runoff;}

        for(int x = 0; x < Length; x++){
          if(RunoffLead - x < LowerSewersStart -LowerSewerCombine - DrainageLength){break;}
          else if(RunoffLead - x < LowerSewersStart -LowerSewerCombine){strip.setPixelColor((LowerSewersStart -LowerSewerCombine - (RunoffLead - x)),0,(RunoffLeadHue.GreenHue[Length-x-1] +1)*(CurrentShift),(RunoffLeadHue.BlueHue[Length-1-x] +1)*(10-(CurrentShift/2)));}
        }
        if(abs(Runoff - RunoffLead) > Length && Runoff < LowerSewersLength - LowerSewerCombine){Runoff++;}
      }
    }

    else if(StormLevel == 0) {    
      for(int x = 0; x < LowerSewersLength; x++){
          strip.setPixelColor(WaterTreatmentStart + x,0,(SewerWaveHue.GreenHue[x-(Length*(x/Length))]+1) * (CurrentShift/3),(1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)));      
          if(x < LowerSewerCombine){strip.setPixelColor(OceanDumpStart - x,0,((SewerWaveHue.GreenHue[x-(Length*(x/Length))]+1) * (CurrentShift/3)),((1+SewerWaveHue.BlueHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3))));}
          else{strip.setPixelColor(OceanDumpStart - x,0,0,0);}
      }
    }
  }
}

void loop(){
  CurrentTime = millis();
  Rain();
  //UpperSewers();
  LowerSewers();

  // if(CurrentTime  >= RainFrame.Duration + RainFrame.LastTriggered){
  //   RainGutters();
  //   RainFrame.LastTriggered = CurrentTime;
  // }

  if(CurrentTime  >= SewersFrame.Duration + SewersFrame.LastTriggered){
    SewerWaveHue.LastBlue = SewerWaveHue.BlueHue[Length-1];
    SewerWaveHue.LastGreen = SewerWaveHue.GreenHue[Length-1];

    FlowOutHue.LastBlue = FlowOutHue.BlueHue[Length-1];
    FlowOutHue.LastGreen = FlowOutHue.GreenHue[Length -1];

    for(int x = Length; x >= 0; x--){
      if(x!=0){
        SewerWaveHue.BlueHue[x]=SewerWaveHue.BlueHue[x-1];
        SewerWaveHue.GreenHue[x]=SewerWaveHue.GreenHue[x-1];

        FlowOutHue.BlueHue[x] = FlowOutHue.BlueHue[x-1];
        FlowOutHue.GreenHue[x] = FlowOutHue.GreenHue[x-1];
      } 
      else {
        SewerWaveHue.BlueHue[x]=SewerWaveHue.LastBlue;
        SewerWaveHue.GreenHue[x]=SewerWaveHue.LastGreen;

        FlowOutHue.BlueHue[x] = FlowOutHue.LastBlue;
        FlowOutHue.GreenHue[x] = FlowOutHue.LastGreen;
      }
    }

    if(StormLevel > 0){RunoffStart = true;}
    if(RunoffStart = true){
      if(CurrentTime >= BlueWaveHoldTime.LastTriggered + BlueWaveHoldTime.Duration && CurrentTime >= GreenWaveHoldTime.LastTriggered + GreenWaveHoldTime.Duration){
        if(CurrentShift < 10 && CurrentShiftRise == true){CurrentShift++;}
        else if (CurrentShift >= 10 && CurrentShiftRise == true){
          GreenWaveHoldTime.LastTriggered = CurrentTime;
          CurrentShiftRise = false;
        }
        else if (CurrentShift > 1 && CurrentShiftRise == false){CurrentShift--;}
        else if(CurrentShift <= 1 && CurrentShiftRise == false){
          BlueWaveHoldTime.LastTriggered = CurrentTime;
          CurrentShiftRise = true;
        }
      }
      if(CurrentShift == 0 && StormLevel == 0){RunoffStart = false;}
    }

    SewersFrame.LastTriggered = CurrentTime;
  }

  strip.show();}
  
