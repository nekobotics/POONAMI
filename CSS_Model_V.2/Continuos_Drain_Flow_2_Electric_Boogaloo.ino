#include <Adafruit_NeoPixel.h>

#define Output_1 7 //
#define Output_2 6 //
#define Output_3 5 //
#define Output_4 4 //

#define Input_1 13 //
#define Input_2 12 //
#define Input_3 11 //
#define Input_4 10 //

#define RainLvl1 14 //
#define RainLvl2 15 //
#define RainLvl3 16 //

#define Rain_Input 9 //

#define Rain_Output1 23 //
#define Rain_Output2 27 //
#define Rain_Output3 25 //

#define Pixel_Pin 22 //
#define Num_Pixels 429
#define BRIGHTNESS 255

Adafruit_NeoPixel strip(Num_Pixels, Pixel_Pin, NEO_GRBW + NEO_KHZ800);

//Animation Lengths
const int Length = 20;
const int GutterHueLength = 6;
const int StreakLength = 5;

//Timing:
unsigned long CurrentTime;
struct Time {
  unsigned long LastTriggered;
  long Duration;
};
//PipeTiming
Time PipesFrame = {0,5};
//Poo Timing:
Time PooFrame = {0,11};
//RainTiming
Time RainFrame = {0,10};
Time RainWait = {0,500};
Time ProjectedSpeed = {0,10};
Time RainHold = {0,1000};
//SewersTiming
Time SewersFrame = {0,10};
// Time GreenWaveHoldTime = {0,200};
// Time BlueWaveHoldTime = {0,400};
Time StageOneStorm = {0,2000};
Time StageTwoStorm = {0,2000 + StageOneStorm.Duration};
Time StageThreeStorm = {0,4000 + StageTwoStorm.Duration};

//checks to make sure that each cord triggers once
bool PullOne;
bool PullTwo;
bool PullThree;
bool PullFour;


//bool allActive = false;


// struct PixelControl {
//   bool PooPath;
//   bool run;
//   bool start;
//   int Stream;
//   int Pixel;
//   int Poo;
//   const int End;
//   const int Begin;
// };

// PixelControl PipeOne = {false, false, false,0,0,0,9,0};
// PixelControl PipeTwo = {false, false, false,10,10,10,27,10};
// PixelControl PipeThree = {false, false, false,28,28,28,40,28};
// PixelControl PipeFour = {false, false, false,41,41,41,69,41};

//PipeSounds trigger
bool ToiletSound = false;
bool ShowerSound = false;
bool SinkSound = false;
bool WasherSound= false;

//Gutter Sizes and Variables
const int GutterLength = 3;
int GutterIntensity = 0;
int GutterLead = GutterLength;

//Upper Sewers Sizes and Varialbles
const int PipeOneSewer = 10;
const int PipeTwoSewer = 15;
const int PipeThreeSewer = 22;
const int PipeFourSewer = 5;
const int UpperSewerStart = 70;
const int UpperSewerEnd = 142;
const int LengthOfUpperSewer = 30;

//Lower Sewers Sizes and Variables
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
double FlowOutIntensity = 0;

//Lower Sewers Runoff Variables
int Runoff = 0;
int RunoffLead = 0;
bool RunoffDrain = false;
float RunoffFade = 0;
bool RunoffStart = false;

// Rain Variables
bool RainOn = false;
int StormLevel = 0;
int ProjectedStormLevel = 0;
bool RainPush = false;
bool Raining = false;

//Color arrays that move
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
ShiftingColor PipeHue;

//Color Arrays that are static
struct StaticColor {
  int RedHue[Length];
  int GreenHue[Length];
  int BlueHue[Length];
};

StaticColor PipeLeadHue;
StaticColor RunoffLeadHue;
StaticColor RunoffLeadHueTest;

// int WaveHue[Length];

// int ColorHue[Length];

int StreakHue[StreakLength];

int LastPixel;
int SewersLastPixel;
int BlueSewersLastPixel;
int GreenSewersLastPixel;
int LastGutterPixel;

//Size and starting pixel of all the pipes
struct Pipe {
  const int Start;
  const int Length;
  bool active;
};

Pipe Toilet = {0,9,false};
Pipe Washer = {10,17,false};
Pipe Shower = {28,12, false};
Pipe Sink = {41,28,false};

//Full variables and functions for each run of a pipe
struct PipeFlow {
  int CurrentPipe;
  bool PooBegin;
  bool sound;
  bool active;
  bool begin;
  bool StartDraining;
  bool SewersBegin;
  int Apartment;
  int Trigger;
  int PipeStart;
  int PipeLength;
  int DrainPixel;
  int StreamPixel;
  int PooPixel;
  Time PipeWait = {0,50};
  Time PooWait = {0,60};
  
  void setup(int Pipe){
    if(Pipe == 1){
      Toilet.active = true;
      PipeStart = Toilet.Start;
      PipeLength = Toilet.Length;
      Trigger = Input_1;
      Apartment = Output_1;
    }
    else if(Pipe == 2){
      Washer.active = true;
      PipeStart = Washer.Start;
      PipeLength = Washer.Length;
      Trigger = Input_2;
      Apartment = Output_2;
    }
    else if(Pipe == 3){
      Shower.active = true;
      PipeStart = Shower.Start;
      PipeLength = Shower.Length;
      Trigger = Input_3;
      Apartment = Output_3;
    }
    else if(Pipe == 4){
      Sink.active = true;
      PipeStart = Sink.Start;
      PipeLength = Sink.Length;
      Trigger = Input_4;
      Apartment = Output_4;
    }

    CurrentPipe = Pipe;
  }
  
  void Flow(){
    if(sound == false){
      Serial.println(CurrentPipe);
      sound = true;
    }
    
    for(int x = 0; x < StreamPixel; x++){
      if(x > StreamPixel - (Length - 1) && x <= PipeLength){strip.setPixelColor(PipeStart + x, 0,PipeLeadHue.GreenHue[StreamPixel-x],PipeLeadHue.BlueHue[StreamPixel-x]);} // Leading Trail
      else if(x <= PipeLength){strip.setPixelColor(PipeStart + x, 0, PipeHue.GreenHue[x - (Length * (x/Length))] + 10, PipeHue.BlueHue[x - (Length * (x/Length))] + 10);} // Flow
    }

    if(StreamPixel < PipeLength + Length){StreamPixel++;}
  }
  
  void Drain(){
    StartDraining = true;
  
    for(int x = 0; x < Length; x++){
      if(StreamPixel - x <= PipeLength && StreamPixel - x > 0){strip.setPixelColor(PipeStart + (StreamPixel - x), 0,PipeLeadHue.GreenHue[Length-1-x],PipeLeadHue.BlueHue[Length- 1 - x]);} // Leading Trail
    }

    for(int x = DrainPixel; x < StreakLength; x++){ //Flow
      if(x <= PipeLength){strip.setPixelColor(PipeStart + x, 0,PipeHue.GreenHue[x - (Length * (x/Length))] + 10, PipeHue.BlueHue[x - (Length * (x/Length))] + 10);}
    }
    
    for(int x = 0; x < Length; x++){
      if(DrainPixel - x < 0){break;}
      else if (DrainPixel - x <= PipeLength){strip.setPixelColor(PipeStart + (DrainPixel-x), 0,PipeLeadHue.GreenHue[x], PipeLeadHue.GreenHue[x]);} // Drain Tail
    }

    if(StreamPixel <= PipeLength + Length + 1){StreamPixel++;}
    if(DrainPixel < StreamPixel && StreamPixel > Length){DrainPixel++;}
    else if(DrainPixel == StreamPixel){ // reset
      active = false;
      StartDraining = false;
      begin = false;
      PooBegin = false;
      DrainPixel = 0;
      StreamPixel = 0;
      PooPixel = 0;
      PipeStart = 0;
      PipeLength = 0;
      Trigger = 0;
      CurrentPipe = 0;
      digitalWrite(Apartment, LOW);
    }

    if(DrainPixel > Length){ // Turn off Active Pipe
      if(PipeStart == Toilet.Start){Toilet.active = false;}
      else if(PipeStart == Washer.Start){Washer.active = false;}
      else if(PipeStart == Shower.Start){Shower.active = false;}
      else if(PipeStart == Sink.Start){Sink.active = false;}

      if(sound == true){
        if(CurrentPipe == 3){Serial.println(5);}
        sound = false;
      }
    } 
  }

  void Poo(){
    for(int x = 0; x < StreakLength; x++){
      if(PooPixel - x < 0){break;}
      else if(PooPixel - x < PipeLength){strip.setPixelColor(PipeStart + (PooPixel - x),StreakHue[x],StreakHue[x],0);}
    }
    if(PooPixel < PipeLength + StreakLength){PooPixel++;}
    else if(PooBegin == false){
      SewersBegin = true;
      PooBegin = true;
    }
  }

  void run(){
    if(begin == false){
      PipeWait.LastTriggered = CurrentTime;
      PooWait.LastTriggered = CurrentTime;
      begin = true;
    }
    
    if(digitalRead(Trigger) == HIGH && StartDraining == false){
      digitalWrite(Apartment, HIGH);
      if(CurrentTime >= PipeWait.LastTriggered + PipeWait.Duration){Flow();}
    }

    else if((digitalRead(Trigger) == LOW || StartDraining == true) && CurrentTime >= PipeWait.LastTriggered + PipeWait.Duration){Drain();}

    if(CurrentTime > PooWait.LastTriggered + PooWait.Duration){Poo();}
  }
};

PipeFlow PipeOne;
PipeFlow PipeTwo;
PipeFlow PipeThree;
PipeFlow PipeFour;


//Update checks for if the Path for poo has changed(The total path is segemented into 4 chunks, See pg.0 of your notes)
bool PathThreeUpdate = false;
bool PathFourUpdate = false;

//The four different base paths the poo can take
struct PooPaths{
  int Start[4];
  int End[4];
};

PooPaths FromToilet = {
  {UpperSewerEnd - PipeOneSewer, OceanDumpStart,WaterTreatmentStart + LowerSewerCombine + 1,WaterTreatmentEnd - DrainageLength + 1},
  {UpperSewerEnd - LengthOfUpperSewer, OceanDumpStart - LowerSewerCombine, WaterTreatmentEnd - DrainageLength, WaterTreatmentEnd}
};
PooPaths FromWasher = {
  {UpperSewerEnd - PipeTwoSewer, OceanDumpStart, WaterTreatmentStart + LowerSewerCombine + 1, WaterTreatmentEnd - DrainageLength + 1},
  {UpperSewerEnd - LengthOfUpperSewer, OceanDumpStart -  LowerSewerCombine, WaterTreatmentEnd - DrainageLength, WaterTreatmentEnd}
};
PooPaths FromShower = {
  {UpperSewerStart + PipeThreeSewer, WaterTreatmentStart, WaterTreatmentStart + LowerSewerCombine + 1, WaterTreatmentEnd - DrainageLength + 1},
  {UpperSewerStart + LengthOfUpperSewer, WaterTreatmentStart + LowerSewerCombine, WaterTreatmentEnd - DrainageLength, WaterTreatmentEnd}
};
PooPaths FromSink = {
  {UpperSewerStart + PipeFourSewer, WaterTreatmentStart, WaterTreatmentStart + LowerSewerCombine + 1, WaterTreatmentEnd - DrainageLength + 1},
  {UpperSewerStart + LengthOfUpperSewer, WaterTreatmentStart + LowerSewerCombine, WaterTreatmentEnd - DrainageLength, WaterTreatmentEnd}
};


//Poo Runner
struct NewPoo{
  bool active;
  int Poo[StreakLength];
  int EndState;
  int PipePath;
  PooPaths Path;
  int Current;

  void setup(int SetPath){
    if(SetPath == 1){
      Path = FromToilet;
    }
    else if(SetPath == 2){
      Path = FromWasher;
    }
    else if(SetPath == 3){
      Path = FromShower;
    }
    else if(SetPath == 4){
      Path = FromSink;
    }

    PipePath = SetPath;
    Poo[0] = Path.Start[0];
    EndState = Path.End[0];
    active = true;
  }

  void run(){ 
    for(int x = 0; x < StreakLength; x++){
      if(Poo[x] != 0){strip.setPixelColor(Poo[x],StreakHue[x],StreakHue[x],0);}
    }
    
    for(int x = StreakLength - 1; x > 0; x--){Poo[x] = Poo[x - 1];}

    if(Poo[0] > EndState){Poo[0]--;}
    else if (Poo[0] < EndState){Poo[0]++;}
    else if(Current < 3){
      Current++;
      Poo[0] = Path.Start[Current];
      EndState = Path.End[Current];
    }
    else if(Current == 3){
      PipePath = 0;
      EndState = 0;
      Current = 0;
      for(int x = 0; x < StreakLength; x++){Poo[x] = 0;}
      for(int x = 0; x < 4; x++){
        Path.Start[x] = 0;
        Path.End[x] = 0;
      }
      active = false;
    }
  }

  void update(){
    if(PipePath == 1){
      Path = FromToilet;
    }
    else if(PipePath == 2){
      Path = FromWasher;
    }
    else if(PipePath == 3){
      Path = FromShower;
    }
    else if(PipePath == 4){
      Path = FromSink;
    }
  }
};

NewPoo PooOne;
NewPoo PooTwo;
NewPoo PooThree;
NewPoo PooFour;


// bool CurrentShiftRise = true;
// int CurrentShift = 0;

const int RainMultiplyer = 50;

void WaveUpdate(){
  for(int x=0; x < Length; x++){
    PipeHue.BlueHue[x] = (200/2)-((200/2) * sin(x * (3.14/Length)));
    PipeHue.GreenHue[x] = (200/2)-((200/2) * sin(x * (3.14/Length)));

    PipeLeadHue.GreenHue[x] = (200/2)+((200/2) * cos(x * (3.14/Length)));
    PipeLeadHue.BlueHue[x] = (200/2)+((200/2) * cos(x * (3.14/Length)));

    SewerWaveHue.GreenHue[x]= (100/2)-((100/2) * sin(x * (3.14/Length)));
    SewerWaveHue.BlueHue[x]= (200/2)-((200/2) * sin(x * (3.14/Length)));

    RunoffLeadHue.BlueHue[x] = (200/2)+((200/2) * cos(x * (3.14/Length)));
    RunoffLeadHue.GreenHue[x] = (100/2)+((100/2) * cos(x * (3.14/Length)));

    FlowOutHue.BlueHue[x] = (100)-(100 * sin(x * (3.14/Length)));
    FlowOutHue.GreenHue[x] = (50)-(50 * sin(x * (3.14/Length)));
    FlowOutHue.WhiteHue[x] = (20/2)-((20/2) * cos(x*(3.14/Length))); 

  }

  FlowOutHue.WhiteHue[0]=0;
  RunoffLeadHue.BlueHue[Length -1] = 0;
  RunoffLeadHue.GreenHue[Length -1] = 0;
  PipeLeadHue.GreenHue[Length-1] = 0;
  PipeLeadHue.BlueHue[Length-1] = 0; 


  for(int x=0; x < StreakLength; x++){
    StreakHue[x] = (200/2)+((200/2)*cos(x*(3.14/StreakLength)));
  }
  StreakHue[StreakLength - 1] = 0;
}

void GutterFlowUpdate(int Intensity, int BlueBrightness, int GreenBrightness){
  for(int x = 0; x < GutterHueLength; x++){
    GutterHue.BlueHue[x]= (BlueBrightness/2)+((BlueBrightness/2)*cos(x*(((3.14 * Intensity))/GutterHueLength)));
    GutterHue.GreenHue[x]= (GreenBrightness/2)+((GreenBrightness/2)*cos(x*((3.14 * Intensity)/GutterHueLength)));
  }
}

void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.show();
  strip.setBrightness(BRIGHTNESS);
  Serial.begin(9600);

  pinMode(Output_1,OUTPUT);
  pinMode(Output_2,OUTPUT);
  pinMode(Output_3,OUTPUT);
  //pinMode(Output_4,OUTPUT);

  // pinMode(ShowerSound,OUTPUT);
  // pinMode(ToiletSound,OUTPUT);

  pinMode(RainLvl1,OUTPUT);
  pinMode(RainLvl2,OUTPUT);
  pinMode(RainLvl3,OUTPUT);

  pinMode(Input_1,INPUT);
  pinMode(Input_2,INPUT);
  pinMode(Input_3,INPUT);
  pinMode(Input_4,INPUT);

  pinMode(Rain_Input,INPUT);

  pinMode(Rain_Output1,OUTPUT);
  pinMode(Rain_Output2,OUTPUT);
  pinMode(Rain_Output3,OUTPUT);

  WaveUpdate();
}

void PipeControl(){
  if(CurrentTime >= PipesFrame.LastTriggered + PipesFrame.Duration){
    if(digitalRead(Input_1)==HIGH && PullOne == false && Toilet.active == false){
      if(PipeOne.active == false){
        PipeOne.setup(1);
        PipeOne.active = true;
      }
      else if(PipeTwo.active == false){
        PipeTwo.setup(1);
        PipeTwo.active = true;
      }
      else if(PipeThree.active == false){
        PipeThree.setup(1);
        PipeThree.active = true;
      }
      else if(PipeFour.active == false){
        PipeFour.setup(1);
        PipeFour.active = true;
      }

      PullOne = true;
    }
    else if (digitalRead(Input_1) == LOW && PullOne == true){PullOne = false;}

    if(digitalRead(Input_2)==HIGH && PullTwo == false && Washer.active == false){
      if(PipeOne.active == false){
        PipeOne.setup(2);
        PipeOne.active = true;
      }
      else if(PipeTwo.active == false){
        PipeTwo.setup(2);
        PipeTwo.active = true;
      }
      else if(PipeThree.active == false){
        PipeThree.setup(2);
        PipeThree.active = true;
      }
      else if(PipeFour.active == false){
        PipeFour.setup(2);
        PipeFour.active = true;
      }

      PullTwo = true;
    }
    else if (digitalRead(Input_2) == LOW && PullTwo == true){PullTwo = false;}

    if(digitalRead(Input_3)==HIGH && PullThree == false && Shower.active == false){
      if(PipeOne.active == false){
        PipeOne.setup(3);
        PipeOne.active = true;
      }
      else if(PipeTwo.active == false){
        PipeTwo.setup(3);
        PipeTwo.active = true;
      }
      else if(PipeThree.active == false){
        PipeThree.setup(3);
        PipeThree.active = true;
      }
      else if(PipeFour.active == false){
        PipeFour.setup(3);
        PipeFour.active = true;
      }

      PullThree = true;
    }
    else if (digitalRead(Input_3) == LOW && PullThree == true){PullThree = false;}
    

    if(digitalRead(Input_4)==HIGH && PullFour == false && Sink.active == false){
      if(PipeOne.active == false){
        PipeOne.setup(4);
        PipeOne.active = true;
      }
      else if(PipeTwo.active == false){
        PipeTwo.setup(4);
        PipeTwo.active = true;
      }
      else if(PipeThree.active == false){
        PipeThree.setup(4);
        PipeThree.active = true;
      }
      else if(PipeFour.active == false){
        PipeFour.setup(4);
        PipeFour.active = true;
      }

      PullFour = true;
    }
    else if (digitalRead(Input_4) == LOW && PullFour == true){PullFour = false;}

    if(PipeOne.active == true){PipeOne.run();}
    if(PipeTwo.active == true){PipeTwo.run();}
    if(PipeThree.active == true){PipeThree.run();}
    if(PipeFour.active == true){PipeFour.run();}
  }
}

void Rain(){ // Could be cleaned more
  if(digitalRead(Rain_Input)== HIGH && RainPush == false){ // Rope Pull check that'll add up to three and cut off at a certain point
    RainWait.LastTriggered = CurrentTime;
    if(ProjectedStormLevel<3 && Raining == false){ProjectedStormLevel++;}
    RainPush = true;
  }
  else if(digitalRead(Rain_Input)==LOW){RainPush = false;}

  

  if(ProjectedStormLevel == 1 || (ProjectedStormLevel < StormLevel && StormLevel == 1)){
    Serial.println(1);
    digitalWrite(Rain_Output1, HIGH);
    digitalWrite(Rain_Output2, LOW);
    digitalWrite(Rain_Output3, LOW);

    digitalWrite(RainLvl1, HIGH);

    ProjectedSpeed.Duration = 10;
  }
  else if(ProjectedStormLevel == 2 || (ProjectedStormLevel < StormLevel && StormLevel == 2)){
    Serial.println(2);
    digitalWrite(Rain_Output1, HIGH);
    digitalWrite(Rain_Output2, HIGH);
    digitalWrite(Rain_Output3, LOW);

    digitalWrite(RainLvl1, HIGH);
    digitalWrite(RainLvl2, HIGH);

    ProjectedSpeed.Duration = 9;
  }
  else if (ProjectedStormLevel== 3 || (ProjectedStormLevel < StormLevel && StormLevel == 3)){
    Serial.println(3);
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
          else{strip.setPixelColor(OceanDumpStart - x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))]);}//Normal
        }
       else if(CurrentTime <= StageThreeStorm.Duration + StageThreeStorm.LastTriggered && x < LowerSewersLength - DrainageLength + Runoff){
          if(x> LowerSewersLength - DrainageLength - WarningLength && x <= LowerSewersLength - DrainageLength && Runoff < DrainageLength){strip.setPixelColor(OceanDumpStart - x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))],FlowOutHue.WhiteHue[0]);}//Flash
          else{strip.setPixelColor(OceanDumpStart - x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))]);}
        }
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

void PooControl(){
  if(CurrentTime >= SewersFrame.Duration + SewersFrame.LastTriggered){
    PathUpdate();

    if(PipeOne.SewersBegin == true){
      if(PooOne.active == false){PooOne.setup(PipeOne.CurrentPipe);}
      else if(PooTwo.active == false){PooTwo.setup(PipeOne.CurrentPipe);}
      else if(PooThree.active == false){PooThree.setup(PipeOne.CurrentPipe);}
      else if(PooFour.active == false){PooFour.setup(PipeOne.CurrentPipe);}

      PipeOne.SewersBegin = false;
    }

    if(PipeTwo.SewersBegin == true){
      if(PooOne.active == false){PooOne.setup(PipeTwo.CurrentPipe);}
      else if(PooTwo.active == false){PooTwo.setup(PipeTwo.CurrentPipe);}
      else if(PooThree.active == false){PooThree.setup(PipeTwo.CurrentPipe);}
      else if(PooFour.active == false){PooFour.setup(PipeTwo.CurrentPipe);}

      PipeTwo.SewersBegin = false;
    }

    if(PipeThree.SewersBegin == true){
      if(PooOne.active == false){PooOne.setup(PipeThree.CurrentPipe);}
      else if(PooTwo.active == false){PooTwo.setup(PipeThree.CurrentPipe);}
      else if(PooThree.active == false){PooThree.setup(PipeThree.CurrentPipe);}
      else if(PooFour.active == false){PooFour.setup(PipeThree.CurrentPipe);}

      PipeThree.SewersBegin = false;
    }

    if(PipeFour.SewersBegin == true){
      if(PooOne.active == false){PooOne.setup(PipeFour.CurrentPipe);}
      else if(PooTwo.active == false){PooTwo.setup(PipeFour.CurrentPipe);}
      else if(PooThree.active == false){PooThree.setup(PipeFour.CurrentPipe);}
      else if(PooFour.active == false){PooFour.setup(PipeFour.CurrentPipe);}

      PipeFour.SewersBegin = false;
    }

    if(PooOne.active == true){PooOne.run();}
    else if(PooTwo.active == true){PooTwo.run();}
    else if(PooThree.active == true){PooThree.run();}
    else if(PooFour.active == true){PooFour.run();}

    //PooFrame.LastTriggered = CurrentTime;
  }
}

void PathUpdate(){
  if(StormLevel == 3){
    if(FlowOutIntensity > 0.5 && PathThreeUpdate == false){
      FromToilet.Start[2] = FromToilet.End[1] - 1;
      FromToilet.End[2] = OceanDumpEnd + DrainageLength;

      FromWasher.Start[2] = FromWasher.End[1] - 1;
      FromWasher.End[2] = OceanDumpEnd + DrainageLength;

      PathThreeUpdate = true;
    }
    else if(Runoff >= DrainageLength && PathFourUpdate == false){
      FromToilet.Start[3] = OceanDumpEnd + DrainageLength - 1;
      FromToilet.End[3] = OceanDumpEnd;

      FromWasher.Start[3] = OceanDumpEnd + DrainageLength - 1;
      FromWasher.End[3] = OceanDumpEnd;

      FromShower.Start[3] = OceanDumpEnd + DrainageLength - 1;
      FromShower.End[3] = OceanDumpEnd;

      FromSink.Start[3] = OceanDumpEnd + DrainageLength - 1;
      FromSink.End[3] = OceanDumpEnd;

      PathFourUpdate = true;
    }
  }
  else{
    if(RunoffLead > 0 && PathFourUpdate == true){
      FromToilet.Start[3] = WaterTreatmentEnd - DrainageLength + 1;
      FromToilet.End[3] = WaterTreatmentEnd;

      FromWasher.Start[3] = WaterTreatmentEnd - DrainageLength + 1;
      FromWasher.End[3] = WaterTreatmentEnd;

      FromShower.Start[3] = WaterTreatmentEnd - DrainageLength + 1;
      FromShower.End[3] = WaterTreatmentEnd;

      FromSink.Start[3] = WaterTreatmentEnd - DrainageLength + 1;
      FromSink.End[3] = WaterTreatmentEnd;

      PathFourUpdate = false;
    }
    else if(FlowOutIntensity < 1 && PathThreeUpdate == true){
      FromToilet.Start[2] = WaterTreatmentStart + LowerSewerCombine + 1;
      FromToilet.End[2] = WaterTreatmentEnd - DrainageLength;

      FromWasher.Start[2] = WaterTreatmentStart + LowerSewerCombine + 1;
      FromWasher.End[2] = WaterTreatmentEnd - DrainageLength;

      PathThreeUpdate = false;
    }
  }

  if(PooOne.active == true){PooOne.update();}
  if(PooTwo.active == true){PooTwo.update();}
  if(PooThree.active == true){PooThree.update();}
  if(PooFour.active == true){PooFour.update();}
  
}

void loop() {
  CurrentTime = millis();
  Rain();
  PipeControl();
  UpperSewers();
  LowerSewers();
  PooControl();


  if(CurrentTime  >= RainFrame.Duration + RainFrame.LastTriggered){
   //Serial.println(StormLevel);
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

 if(CurrentTime >= PipesFrame.LastTriggered + PipesFrame.Duration){
    PipeHue.LastBlue = PipeHue.BlueHue[Length-1];
    PipeHue.LastGreen = PipeHue.GreenHue[Length-1];
    
    for(int x = Length -1; x >= 0; x--){
      if(x!= 0){
        PipeHue.BlueHue[x] = PipeHue.BlueHue[x-1];
        PipeHue.GreenHue[x] = PipeHue.GreenHue[x-1];
      }
      else{
        PipeHue.BlueHue[x] = PipeHue.LastBlue;
        PipeHue.GreenHue[x] = PipeHue.LastGreen;
      }
    }
    PipesFrame.LastTriggered = CurrentTime;
  }

  strip.show();
}
