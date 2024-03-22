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

unsigned long CurrentTime;
struct Time {
  unsigned long LastTriggered;
  long Duration;
};

Time RainFrame = {0,10};
Time RainWait = {0,500};
Time SewersFrame = {0,10};
Time ProjectedSpeed = {0,10};
// Time GreenWaveHoldTime = {0,200};
// Time BlueWaveHoldTime = {0,400};
Time StageOneStorm = {0,2000};
Time StageTwoStorm = {0,2000 + StageOneStorm.Duration};
Time StageThreeStorm = {0,4000 + StageTwoStorm.Duration};
Time RainHold = {0,1000};

const int GutterLength = 3;
int GutterIntensity = 0;
int GutterLead = GutterLength;

const int UpperSewerStart = 70;
const int UpperSewerEnd = 142;
const int LengthOfUpperSewer = 30;

const int LowerSewersStart = 143;
const int LowerSewersEnd = Num_Pixels - 12;
const int LowerSewersLength = 46;
const int DrainageLength = 14;
const int WaterTreatmentStart = LowerSewersEnd - LowerSewersLength;
const int WaterTreatmentEnd = LowerSewersEnd;
const int OceanDumpStart = LowerSewersStart + LowerSewersLength;
const int OceanDumpEnd = LowerSewersStart;
const int LowerSewerCombine = 16;
const int WarningLength = 6;
int WarningIntensity;
bool StormStart = false;

const int Length = 20;
const int GutterHueLength = 6;

bool RainOn = false;
int StormLevel = 0;
int ProjectedStormLevel = 0;
bool RainPush = false;
bool Raining = false;

// bool CurrentShiftRise = true;
// int CurrentShift = 0;

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

double FlowOutIntensity = 0;
//double FlowOutMultiplier[Length];
//double LastFlowOutMultiplier;

Adafruit_NeoPixel strip(Num_Pixels, Pixel_Pin, NEO_GRBW + NEO_KHZ800);

void WaveUpdate(){
  for(int x=0; x < Length; x++){
    SewerWaveHue.GreenHue[x]= (100/2)-((100/2) * sin(x * (3.14/Length)));
    SewerWaveHue.BlueHue[x]= (200/2)-((200/2) * sin(x * (3.14/Length)));

    RunoffLeadHue.BlueHue[x] = (200/2)+((200/2) * cos(x * (3.14/Length)));
    RunoffLeadHue.GreenHue[x] = (100/2)+((100/2) * cos(x * (3.14/Length)));

    FlowOutHue.BlueHue[x] = (100)-(100 * sin(x * (3.14/Length)));
    FlowOutHue.GreenHue[x] = (50)-(50 * sin(x * (3.14/Length)));
    FlowOutHue.WhiteHue[x] = (20/2)-((20/2) * cos(x*(3.14/Length))); 

    //Serial.println(FlowOutHue.WhiteHue[x]);
  }
  FlowOutHue.WhiteHue[0]=0;
  RunoffLeadHue.BlueHue[Length -1] = 0;
  RunoffLeadHue.GreenHue[Length -1] = 0;


}

void GutterFlowUpdate(int Intensity, int BlueBrightness, int GreenBrightness){
  for(int x = 0; x < GutterHueLength; x++){
    GutterHue.BlueHue[x]= (BlueBrightness/2)+((BlueBrightness/2)*cos(x*(((3.14 * Intensity))/GutterHueLength)));
    GutterHue.GreenHue[x]= (GreenBrightness/2)+((GreenBrightness/2)*cos(x*((3.14 * Intensity)/GutterHueLength)));
  }
}

// void FlowOutUpdate(){ // Not used atm
//   for(int x = 0; x< Length; x++){
//     FlowOutMultiplier[x] = (FlowOutIntensity - (FlowOutIntensity * sin(x * (3.14/Length))));
//   }
//   if(FlowOutIntensity < .5 && CurrentTime <= StageOneStorm.Duration + StageOneStorm.LastTriggered){FlowOutIntensity = FlowOutIntensity + 0.1;}
//   else if (FlowOutIntensity > 0 && CurrentTime >= StageOneStorm.Duration + StageOneStorm.LastTriggered){FlowOutIntensity = FlowOutIntensity - 0.1;}
// }

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

void Rain(){ // Could be cleaned more
  if(digitalRead(Rain_Input)== HIGH && RainPush == false){ // Rope Pull check that'll add up to three and cut off at a certain point
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


  if(StormLevel < ProjectedStormLevel){ // Increases StormLevel after a certain duration
    if(CurrentTime >= RainWait.LastTriggered + RainWait.Duration){
      StormLevel++;
      RainWait.LastTriggered = CurrentTime;
    }
  }
  else if(StormLevel > ProjectedStormLevel){    
    if((CurrentTime >= RainWait.LastTriggered + RainWait.Duration  && CurrentTime >= RainHold.LastTriggered + RainHold.Duration) || (CurrentTime >= StageThreeStorm.LastTriggered + StageThreeStorm.Duration && StormLevel == 3)){
      StormLevel--;
      RainWait.LastTriggered = CurrentTime;
      //Serial.println("Drop");
    }
  }
  else if(StormLevel == ProjectedStormLevel){
    if(StormLevel == 3){RainHold.Duration = StageThreeStorm.Duration;}
    else{RainHold.Duration = 1000;}
    
    RainHold.LastTriggered = CurrentTime;
    ProjectedStormLevel = 0;
  }

  if(StormLevel > 0 && ProjectedStormLevel == 0){Raining = true;}
  else{Raining = false;}

  //StormIntensity = StormLevel + 1;
}

void RainGutters(){
  if(StormLevel > 0){
    for(int x = 0; x < GutterLength - GutterLead; x++){ // Base Animation
      //for(int Gutter = 0; Gutter <= 4; Gutter++){strip.setPixelColor((Num_Pixels - (3*Gutter))+x,0,GutterHue[x], GutterHue[x]+5);}
      strip.setPixelColor((Num_Pixels - (GutterLength)) + x,0,GutterHue.GreenHue[x], GutterHue.BlueHue[x]+5);
      strip.setPixelColor((Num_Pixels - (GutterLength * 2)) + x,0,GutterHue.GreenHue[x], GutterHue.BlueHue[x]+5);
      strip.setPixelColor((Num_Pixels - (GutterLength * 3)) + x,0,GutterHue.GreenHue[x], GutterHue.BlueHue[x]+5);
      strip.setPixelColor((Num_Pixels - (GutterLength * 4)) + x,0,GutterHue.GreenHue[x], GutterHue.BlueHue[x]+5);
    }
    if(GutterLead > 0){GutterLead--;}
  }
  else{
    for(int x = GutterLead; x < GutterLength; x++){ // Base Animation
      //for(int Gutter = 0; Gutter <= 4; Gutter++){strip.setPixelColor((Num_Pixels - (3*Gutter))+x,0,GutterHue[x], GutterHue[x]+5);}
      strip.setPixelColor((Num_Pixels - (GutterLength)) + x,0,GutterHue.GreenHue[x], GutterHue.BlueHue[x]+5);
      strip.setPixelColor((Num_Pixels - (GutterLength * 2)) + x,0,GutterHue.GreenHue[x], GutterHue.BlueHue[x]+5);
      strip.setPixelColor((Num_Pixels - (GutterLength * 3)) + x,0,GutterHue.GreenHue[x], GutterHue.BlueHue[x]+5);
      strip.setPixelColor((Num_Pixels - (GutterLength * 4)) + x,0,GutterHue.GreenHue[x], GutterHue.BlueHue[x]+5);
    }
    for(int x = 1; x <= 4; x++){strip.setPixelColor((Num_Pixels - (GutterLength*x)) + GutterLead, 0, 0, 0);}
    if(GutterLead < GutterLength){GutterLead++;}
  }
}

void UpperSewers(){
  if(CurrentTime >= SewersFrame.Duration + SewersFrame.LastTriggered){
    for(int x = 0; x < LengthOfUpperSewer; x++){
      strip.setPixelColor(x+UpperSewerStart,0,SewerWaveHue.GreenHue[x-(Length*(x/Length))]+10,SewerWaveHue.BlueHue[x-(Length*(x/Length))]+10);
      strip.setPixelColor(UpperSewerEnd-x,0,SewerWaveHue.GreenHue[x-(Length*(x/Length))]+10,SewerWaveHue.BlueHue[x-(Length*(x/Length))]+10);
      //Serial.println(CurrentShift);
    }
  }
}

void LowerSewers(){ // Works as intended but open for tweaking
 if(CurrentTime >= SewersFrame.Duration + SewersFrame.LastTriggered){
    if(StormLevel < 3){
      for(int x = 0; x < LowerSewersLength; x++){
        strip.setPixelColor(WaterTreatmentStart + x,0,SewerWaveHue.GreenHue[x-(Length*(x/Length))]+10,10+SewerWaveHue.BlueHue[x-(Length*(x/Length))]);
        if(x <= LowerSewerCombine){strip.setPixelColor(OceanDumpStart - x,0,SewerWaveHue.GreenHue[x-(Length*(x/Length))]+10,10+SewerWaveHue.BlueHue[x-(Length*(x/Length))]);}
        else if(x <= LowerSewersLength - DrainageLength && StormStart == true){
          if(FlowOutIntensity > 0 && Runoff == RunoffLead){FlowOutIntensity = FlowOutIntensity - .005;}
          else if (FlowOutIntensity == 0){StormStart = false;}
          strip.setPixelColor(OceanDumpStart - x,0,(FlowOutHue.GreenHue[x-(Length*(x/Length))]+10) * FlowOutIntensity, (10+FlowOutHue.BlueHue[x-(Length*(x/Length))])*FlowOutIntensity);
        }
        else if(x >= LowerSewersLength - DrainageLength + RunoffLead && Runoff > 0){strip.setPixelColor(OceanDumpStart - x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))]);}
      }

      if(Runoff > 0){
        for(int x = 0; x < Length; x++){
          if(RunoffLead - x < 0){break;}
          else if (RunoffLead - x < DrainageLength){strip.setPixelColor((OceanDumpEnd + DrainageLength) - (RunoffLead - x),0,RunoffLeadHue.GreenHue[x],RunoffLeadHue.BlueHue[x]);}
        }
        if(RunoffLead < Runoff){RunoffLead++;}
        else{
          Runoff = 0;
          RunoffLead = 0;
        }
      }
    }
    else{
      if(StormStart == false){
        StageOneStorm.LastTriggered = CurrentTime;
        StageTwoStorm.LastTriggered = CurrentTime;
        StageTwoStorm.LastTriggered = CurrentTime;
        StormStart = true;
      }

      for(int x = 0; x < LowerSewersLength; x++){ // Continual Flow Animations
        strip.setPixelColor(WaterTreatmentStart + x,0,SewerWaveHue.GreenHue[x-(Length*(x/Length))]+10,10+SewerWaveHue.BlueHue[x-(Length*(x/Length))]);
        if(x <= LowerSewerCombine){strip.setPixelColor(OceanDumpStart - x,0,SewerWaveHue.GreenHue[x-(Length*(x/Length))]+10,10+SewerWaveHue.BlueHue[x-(Length*(x/Length))]);}
        else if(CurrentTime <= StageOneStorm.Duration + StageOneStorm.LastTriggered && x < LowerSewersLength - DrainageLength){
          if(FlowOutIntensity < 1){FlowOutIntensity = FlowOutIntensity + .005;}
          strip.setPixelColor(OceanDumpStart - x,0,(FlowOutHue.GreenHue[x-(Length*(x/Length))]+10) * FlowOutIntensity, (10+FlowOutHue.BlueHue[x-(Length*(x/Length))])*FlowOutIntensity);
        }
        else if(CurrentTime <= StageTwoStorm.Duration + StageTwoStorm.LastTriggered && x < LowerSewersLength - DrainageLength){
          if(x> LowerSewersLength - DrainageLength - WarningLength && x <= LowerSewersLength - DrainageLength){strip.setPixelColor(OceanDumpStart - x,0,0,0,FlowOutHue.WhiteHue[0]);}//Flash
          //if(x> LowerSewersLength - DrainageLength - WarningLength && x <= LowerSewersLength - DrainageLength){strip.setPixelColor(OceanDumpStart - x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))],FlowOutHue.WhiteHue[0]);}//Flash
          else{strip.setPixelColor(OceanDumpStart - x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))]);}//Normal
          //Serial.println(FlowOutHue.WhiteHue[0]);
        }
       else if(CurrentTime <= StageThreeStorm.Duration + StageThreeStorm.LastTriggered && x < LowerSewersLength - DrainageLength + Runoff){
          if(x> LowerSewersLength - DrainageLength - WarningLength && x <= LowerSewersLength - DrainageLength && Runoff < DrainageLength){strip.setPixelColor(OceanDumpStart - x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))],FlowOutHue.WhiteHue[0]);}//Flash
          else{strip.setPixelColor(OceanDumpStart - x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))]);}
        }
        //else if(CurrentTime <= StageThreeStorm.Duration + StageThreeStorm.LastTriggered && x < LowerSewersLength){strip.setPixelColor(OceanDumpStart - x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))]);}
      }

      if(CurrentTime <= StageOneStorm.Duration + StageOneStorm.LastTriggered) {// Stage 1: Fade In [o] ... it's good as is, but could be better and smoother
        //if(FlowOutIntensity < 1){FlowOutIntensity = FlowOutIntensity + .01;}
        //FlowOutUpdate();
      }
      else if(CurrentTime <= StageTwoStorm.Duration + StageTwoStorm.LastTriggered){ // Stage 2: FlashWarning[o] ... it's good and does not use this
        // if(WarningIntensity < Length && WarningRise == true){WarningIntensity++;}
        // else if(WarningIntensity == Length){WarningRise = false;}
        // else if(WarningIntensity > 0){WarningIntensity--;}
        // else if(WarningIntensity == 0){WarningRise = true;}
        //Serial.println(WarningIntensity);
      }
      else if(CurrentTime <= StageThreeStorm.Duration + StageThreeStorm.LastTriggered){ // Stage 3: FlowOut[o] ... it works
        for(int i = 0; i < Length; i++){
          RunoffLead = 0;
          if(Runoff - i < 0){break;}
          else if(Runoff - i < DrainageLength){strip.setPixelColor((OceanDumpEnd + DrainageLength) - (Runoff - i),0,RunoffLeadHue.GreenHue[Length - 1 - i],RunoffLeadHue.BlueHue[Length - 1 - i]);} 
        }
        if(Runoff - Length < DrainageLength){Runoff++;}
      }
    }
  }
}

void loop(){
  CurrentTime = millis();
  Rain();
  UpperSewers();
  LowerSewers();

  if(CurrentTime  >= RainFrame.Duration + RainFrame.LastTriggered){
   Serial.println(StormLevel);
    if(GutterIntensity != StormLevel){
      if(StormLevel == 1){GutterFlowUpdate(StormLevel,100,100);}
      else if (StormLevel == 2){GutterFlowUpdate(StormLevel,150,150);}
      else if (StormLevel == 3){GutterFlowUpdate(StormLevel,200,200);}
      GutterIntensity = StormLevel;
    }

    RainGutters();
    
    if(StormLevel > 0){
      GutterHue.LastBlue = GutterHue.BlueHue[GutterHueLength -1];
      GutterHue.LastGreen = GutterHue.GreenHue[GutterHueLength -1];
      for(int x = GutterHueLength; x >= 0; x--){
        if(x != 0){
          GutterHue.BlueHue[x] = GutterHue.BlueHue[x-1];
          GutterHue.GreenHue[x] = GutterHue.GreenHue[x-1];
        }
        else{
           GutterHue.BlueHue[x] = GutterHue.LastBlue;
          GutterHue.GreenHue[x] = GutterHue.LastGreen;
        }
      }
    }
  
    RainFrame.LastTriggered = CurrentTime;
  }

  if(CurrentTime  >= SewersFrame.Duration + SewersFrame.LastTriggered){
    SewerWaveHue.LastBlue = SewerWaveHue.BlueHue[Length-1];
    SewerWaveHue.LastGreen = SewerWaveHue.GreenHue[Length-1];

    FlowOutHue.LastBlue = FlowOutHue.BlueHue[Length-1];
    FlowOutHue.LastGreen = FlowOutHue.GreenHue[Length -1];
    FlowOutHue.LastWhite = FlowOutHue.WhiteHue[Length -1];


    //LastFlowOutMultiplier = FlowOutMultiplier[Length - 1];

    for(int x = Length; x >= 0; x--){
      if(x!=0){
        SewerWaveHue.BlueHue[x]=SewerWaveHue.BlueHue[x-1];
        SewerWaveHue.GreenHue[x]=SewerWaveHue.GreenHue[x-1];

        FlowOutHue.BlueHue[x] = FlowOutHue.BlueHue[x-1];
        FlowOutHue.GreenHue[x] = FlowOutHue.GreenHue[x-1];
        if(FlowOutHue.WhiteHue[0] != 0 || (CurrentTime <= StageTwoStorm.Duration + StageTwoStorm.LastTriggered && CurrentTime > StageOneStorm.Duration + StageOneStorm.LastTriggered)){FlowOutHue.WhiteHue[x] = FlowOutHue.WhiteHue[x-1];}

        //FlowOutMultiplier[x] = FlowOutMultiplier[x-1];
      } 
      else {
        SewerWaveHue.BlueHue[x]=SewerWaveHue.LastBlue;
        SewerWaveHue.GreenHue[x]=SewerWaveHue.LastGreen;

        FlowOutHue.BlueHue[x] = FlowOutHue.LastBlue;
        FlowOutHue.GreenHue[x] = FlowOutHue.LastGreen;
        if(FlowOutHue.WhiteHue[0] != 0 || (CurrentTime <= StageTwoStorm.Duration + StageTwoStorm.LastTriggered && CurrentTime > StageOneStorm.Duration + StageOneStorm.LastTriggered)){FlowOutHue.WhiteHue[x] = FlowOutHue.LastWhite;}


        //FlowOutMultiplier[x] = LastFlowOutMultiplier;
      }
    }

    // if(StormLevel > 0){RunoffStart = true;}
    // if(RunoffStart = true){
    //   if(CurrentTime >= BlueWaveHoldTime.LastTriggered + BlueWaveHoldTime.Duration && CurrentTime >= GreenWaveHoldTime.LastTriggered + GreenWaveHoldTime.Duration){
    //     if(CurrentShift < 10 && CurrentShiftRise == true){CurrentShift++;}
    //     else if (CurrentShift >= 10 && CurrentShiftRise == true){
    //       GreenWaveHoldTime.LastTriggered = CurrentTime;
    //       CurrentShiftRise = false;
    //     }
    //     else if (CurrentShift > 1 && CurrentShiftRise == false){CurrentShift--;}
    //     else if(CurrentShift <= 1 && CurrentShiftRise == false){
    //       BlueWaveHoldTime.LastTriggered = CurrentTime;
    //       CurrentShiftRise = true;
    //     }
    //   }
    //   if(CurrentShift == 0 && StormLevel == 0){RunoffStart = false;}
    // }

    SewersFrame.LastTriggered = CurrentTime;
  }

  strip.show();}
  
