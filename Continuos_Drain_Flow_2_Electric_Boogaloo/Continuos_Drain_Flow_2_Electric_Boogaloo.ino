#include <Adafruit_NeoPixel.h>

#define Output_1 7 //
#define Output_2 6 //
#define Output_3 5 //
#define Output_4 4 //

#define Input_1 13 //
#define Input_2 12 //
#define Input_3 11 //
#define Input_4 10 //

#define RainLvl1 23 //
#define RainLvl2 25 //
#define RainLvl3 27 //

#define Rain_Input 9 //

#define Rain_Output1 14 //
#define Rain_Output2 15 //
#define Rain_Output3 16 //

#define Pixel_Pin 22 //
#define Num_Pixels 261
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
Time BathWait = {0,50};
Time ToiletWait = BathWait;
Time ShowerWait = BathWait;
Time SinkWait = BathWait;
//Poo Timing:
Time PipeOnePooWait = {0,60};
Time PipeTwoPooWait = PipeOnePooWait;
Time PipeThreePooWait = PipeOnePooWait;
Time PipeFourPooWait = PipeOnePooWait;
Time PooOneSpeed = {0,11};
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



bool allActive = false;


struct PixelControl {
  bool PooPath;
  bool run;
  bool start;
  int Stream;
  int Pixel;
  int Poo;
  const int End;
  const int Begin;
};

PixelControl PipeOne = {false, false, false,0,0,0,9,0};
PixelControl PipeTwo = {false, false, false,10,10,10,27,10};
PixelControl PipeThree = {false, false, false,28,28,28,40,28};
PixelControl PipeFour = {false, false, false,41,41,41,69,41};

//PipeSounds trigger
bool ToiletSound = false;
bool ShowerSound = false;
bool SinkSound = false;
bool WasherSound= false;


const int PipeOneSewer = 10;
const int PipeTwoSewer = 15;
const int PipeThreeSewer = 22;
const int PipeFourSewer = 5;
// const int PipeSewerDropSize = 2;

//Gutter Sizes and Variables
const int GutterLength = 3;
int GutterIntensity = 0;
int GutterLead = GutterLength;

//Upeer Sewers Sizes and Varialbles
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

//Color Arrays that are static
struct StaticColor {
  int RedHue[Length];
  int GreenHue[Length];
  int BlueHue[Length];
};

// StaticColor PipeLeadHue;
StaticColor RunoffLeadHue;
StaticColor RunoffLeadHueTest;

int WaveHue[Length];

int ColorHue[Length];

int StreakHue[StreakLength];

int LastPixel;
int SewersLastPixel;
int BlueSewersLastPixel;
int GreenSewersLastPixel;
int LastGutterPixel;

// struct PooDrop{
//   bool active;
//   int Pipe;
//   int Poo[StreakLength];
//   int Start[5];
//   int End[5];
//   int CurrentPath;
// };

// PooDrop PooOne = {false,0,{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},0};
// PooDrop PooTwo = {false,0,{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},0};
// PooDrop PooThree = {false,0,{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},0};
// PooDrop PooFour = {false,0,{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},0};

bool PathThreeUpdate = false;
bool PathFourUpdate = false;

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
      if(Poo[x] != 0){strip.setPixelColor(Poo[x],0,0,0);}
    }
    
    for(int x = StreakLength - 1; x > 0; x--){Poo[x] = Poo[x - 1];}

    if(Poo[0] > EndState){Poo[0]--;}
    else if (Poo[0] < EndState){Poo[0]++;}
    else if(Current < 4){
      Current++;
      Poo[0] = Path.Start[Current];
      EndState = Path.End[Current];
    }
    else if(Current == 4 && Poo[0] == Poo[StreakLength - 1]){
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
    ColorHue[x]= (200/2)+((200/2) * cos(x * (3.14/Length)));
    WaveHue[x]= (200/2)-((200/2) * sin(x * (3.14/Length)));

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
  ColorHue[Length-1] = 0;;


  for(int x=0; x < StreakLength; x++){
    StreakHue[x] = (200/2)+((200/2)*cos(x*(3.14/StreakLength)));
  }
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
  pinMode(Output_4,OUTPUT);

  // pinMode(ShowerSound,OUTPUT);
  // pinMode(ToiletSound,OUTPUT);

  pinMode(RainLvl1,OUTPUT);
  pinMode(RainLvl2,OUTPUT);
  pinMode(RainLvl3,OUTPUT);

  pinMode(Input_1,INPUT);
  pinMode(Input_2,INPUT);
  pinMode(Input_3,INPUT);
  //pinMode(Input_4,INPUT);

  pinMode(Rain_Input,INPUT);

  pinMode(Rain_Output1,OUTPUT);
  pinMode(Rain_Output2,OUTPUT);
  pinMode(Rain_Output3,OUTPUT);

  WaveUpdate();
}

void Toilet(){
  if(CurrentTime >= PipesFrame.LastTriggered + PipesFrame.Duration){
    if(digitalRead(Input_1) == HIGH && PipeOne.run == false){
      digitalWrite(Output_1,HIGH);

      if(PipeOne.start == false){
        ToiletWait.LastTriggered = CurrentTime;
        PipeOnePooWait.LastTriggered = CurrentTime;
        PipeOne.start = true;
      }
      else if(CurrentTime >= ToiletWait.LastTriggered + ToiletWait.Duration){
        digitalWrite(ToiletSound, HIGH);
        for(int x = 0; x < PipeOne.Stream-PipeOne.Begin; x++){strip.setPixelColor(PipeOne.Begin + x, 0, WaveHue[x -(Length*(x/Length))], 150);}
        if(PipeOne.Stream <= PipeOne.End){PipeOne.Stream++;}
      }
    }

    else if(PipeOne.start == true && CurrentTime >= ToiletWait.LastTriggered + ToiletWait.Duration){
      PipeOne.run = true;
      digitalWrite(ToiletSound, HIGH);

      if(PipeOne.Stream > PipeOne.Begin){
        for(int x = PipeOne.Pixel; x < PipeOne.Stream; x++){strip.setPixelColor(x, 0, WaveHue[x -(Length*(x/Length))], 150);}
        if(PipeOne.Stream <= PipeOne.End){PipeOne.Stream++;}
      }

      for(int x = 0; x < Length; x++){
        if(PipeOne.Pixel - x < PipeOne.Begin){break;}
        else if(PipeOne.Pixel - x <= PipeOne.End){strip.setPixelColor(PipeOne.Pixel - x, 0,ColorHue[x],ColorHue[x]);}
      }
      if(PipeOne.Pixel <= PipeOne.End + Length){PipeOne.Pixel++;}
      else if(PipeOne.Pixel > PipeOne.End + Length){
        PipeOne.PooPath = false;
        PipeOne.run = false;
        PipeOne.start = false;
        PipeOne.Stream = PipeOne.Begin;
        PipeOne.Pixel = PipeOne.Begin;
        PipeOne.Poo = PipeOne.Begin;
        digitalWrite(Output_1,LOW);
        digitalWrite(ToiletSound, LOW);
      }
    }

    if(PipeOne.start == true && allActive != true && CurrentTime >= PipeOnePooWait.LastTriggered + PipeOnePooWait.Duration && PipeOne.Pixel < PipeOne.End + StreakLength){
      for(int x = 0; x < StreakLength; x++){
        if(PipeOne.Poo - x < PipeOne.Begin){break;}
        else if(PipeOne.Poo - x <= PipeOne.End){strip.setPixelColor(PipeOne.Poo - x, StreakHue[x], StreakHue[x],0);}
      }

      if(PipeOne.Poo < PipeOne.End + StreakLength){PipeOne.Poo++;}
    }
  }
}

void BathTub(){
  if(CurrentTime >= PipesFrame.LastTriggered + PipesFrame.Duration){
    if(digitalRead(Input_2) == HIGH && PipeTwo.run == false){
      digitalWrite(Output_2,HIGH);

      if(PipeTwo.start == false){
        BathWait.LastTriggered = CurrentTime;
        PipeTwoPooWait.LastTriggered = CurrentTime;
        PipeTwo.start = true;
      }
      else if(CurrentTime >= BathWait.LastTriggered + BathWait.Duration){
        //digitalWrite(ShowerSound, HIGH);
        for(int x = 0; x < PipeTwo.Stream-PipeTwo.Begin; x++){strip.setPixelColor(PipeTwo.Begin + x, 0, WaveHue[x -(Length*(x/Length))], 150);}
        if(PipeTwo.Stream <= PipeTwo.End){PipeTwo.Stream++;}
      }
    }

    else if(PipeTwo.start == true && CurrentTime >= BathWait.LastTriggered + BathWait.Duration){
      PipeTwo.run = true;
      //digitalWrite(ShowerSound, LOW);
      
      if(PipeTwo.Stream > PipeTwo.Begin){
        for(int x = PipeTwo.Pixel; x < PipeTwo.Stream; x++){strip.setPixelColor(x, 0, WaveHue[x -(Length*(x/Length))], 150);}
        if(PipeTwo.Stream <= PipeTwo.End){PipeTwo.Stream++;}
      }

      for(int x = 0; x < Length; x++){
        if(PipeTwo.Pixel - x < PipeTwo.Begin){break;}
        else if(PipeTwo.Pixel - x <= PipeTwo.End){strip.setPixelColor(PipeTwo.Pixel - x, 0,ColorHue[x],ColorHue[x]);}
      }
      if(PipeTwo.Pixel <= PipeTwo.End + Length){PipeTwo.Pixel++;}
      else if(PipeTwo.Pixel > PipeTwo.End + Length){
        PipeTwo.PooPath = false;
        PipeTwo.run = false;
        PipeTwo.start = false;
        PipeTwo.Poo = PipeTwo.Begin;
        PipeTwo.Stream = PipeTwo.Begin;
        PipeTwo.Pixel = PipeTwo.Begin;
        digitalWrite(Output_2,LOW);
      }
    }

    if(PipeTwo.start == true && allActive != true && CurrentTime >= PipeTwoPooWait.LastTriggered + PipeTwoPooWait.Duration && PipeTwo.Pixel < PipeTwo.End + StreakLength){
      for(int x = 0; x < StreakLength; x++){
        if(PipeTwo.Poo - x < PipeTwo.Begin){break;}
        else if(PipeTwo.Poo - x <= PipeTwo.End){strip.setPixelColor(PipeTwo.Poo - x, StreakHue[x], StreakHue[x],0);}
      }

      if(PipeTwo.Poo < PipeTwo.End + StreakLength){PipeTwo.Poo++;}
    }
  } 
}

void Shower(){
  if(CurrentTime >= PipesFrame.LastTriggered + PipesFrame.Duration){
    if(digitalRead(Input_3) == HIGH && PipeThree.run == false){
      digitalWrite(Output_3,HIGH);

      if(PipeThree.start == false){
        ShowerWait.LastTriggered = CurrentTime;
        PipeThreePooWait.LastTriggered = CurrentTime;
        PipeThree.start = true;
      }
      else if(CurrentTime >= ShowerWait.LastTriggered + ShowerWait.Duration){
        for(int x = 0; x < PipeThree.Stream-PipeThree.Begin; x++){strip.setPixelColor(PipeThree.Begin + x, 0, WaveHue[x -(Length*(x/Length))], 150);}
        if(PipeThree.Stream <= PipeThree.End){PipeThree.Stream++;}
      }
    }

    else if(PipeThree.start == true && CurrentTime >= ShowerWait.LastTriggered + ShowerWait.Duration){
      PipeThree.run = true;

      if(PipeThree.Stream > PipeThree.Begin){
        for(int x = PipeThree.Pixel; x < PipeThree.Stream; x++){strip.setPixelColor(x, 0, WaveHue[x -(Length*(x/Length))], 150);}
        if(PipeThree.Stream <= PipeThree.End){PipeThree.Stream++;}
      }

      for(int x = 0; x < Length; x++){
        if(PipeThree.Pixel - x < PipeThree.Begin){break;}
        else if(PipeThree.Pixel - x <= PipeThree.End){strip.setPixelColor(PipeThree.Pixel - x, 0,ColorHue[x],ColorHue[x]);}
      }
      if(PipeThree.Pixel <= PipeThree.End + Length){PipeThree.Pixel++;}
      else if(PipeThree.Pixel > PipeThree.End + Length){
        PipeThree.PooPath = false;
        PipeThree.run = false;
        PipeThree.start = false;
        PipeThree.Poo = PipeThree.Begin;
        PipeThree.Stream = PipeThree.Begin;
        PipeThree.Pixel = PipeThree.Begin;
        digitalWrite(Output_3,LOW);
      }
    }

    if(PipeThree.start == true && allActive != true && CurrentTime >= PipeThreePooWait.LastTriggered + PipeThreePooWait.Duration && PipeThree.Pixel < PipeThree.End + StreakLength){
      for(int x = 0; x < StreakLength; x++){
        if(PipeThree.Poo - x < PipeThree.Begin){break;}
        else if(PipeThree.Poo - x <= PipeThree.End){strip.setPixelColor(PipeThree.Poo - x, StreakHue[x], StreakHue[x],0);}
      }

      if(PipeThree.Poo < PipeThree.End + StreakLength){PipeThree.Poo++;}
    }
  }
}

void Sink(){
  if(CurrentTime >= PipesFrame.LastTriggered + PipesFrame.Duration){
    if(digitalRead(Input_4) == HIGH && PipeFour.run == false){
      digitalWrite(Output_4,HIGH);

      if(PipeFour.start == false){
        SinkWait.LastTriggered = CurrentTime;
        PipeFourPooWait.LastTriggered = CurrentTime;
        PipeFour.start = true;
      }
      else if(CurrentTime >= SinkWait.LastTriggered + SinkWait.Duration){
        for(int x = 0; x < PipeFour.Stream-PipeFour.Begin; x++){strip.setPixelColor(PipeFour.Begin + x, 0, WaveHue[x -(Length*(x/Length))], 150);}
        if(PipeFour.Stream <= PipeFour.End){PipeFour.Stream++;}
      }
    }

    else if(PipeFour.start == true && CurrentTime >= SinkWait.LastTriggered + SinkWait.Duration){
      PipeFour.run = true;

      if(PipeFour.Stream > PipeFour.Begin){
        for(int x = PipeFour.Pixel; x < PipeFour.Stream; x++){strip.setPixelColor(x, 0, WaveHue[x -(Length*(x/Length))], 150);}
        if(PipeFour.Stream <= PipeFour.End){PipeFour.Stream++;}
      }

      for(int x = 0; x < Length; x++){
        if(PipeFour.Pixel - x < PipeFour.Begin){break;}
        else if(PipeFour.Pixel - x <= PipeFour.End){strip.setPixelColor(PipeFour.Pixel - x, 0,ColorHue[x],ColorHue[x]);}
      }
      if(PipeFour.Pixel <= PipeFour.End + Length){PipeFour.Pixel++;}
      else if(PipeFour.Pixel > PipeFour.End + Length){
        PipeFour.PooPath = false;
        PipeFour.run = false;
        PipeFour.start = false;
        PipeFour.Poo = PipeFour.Begin;
        PipeFour.Stream = PipeFour.Begin;
        PipeFour.Pixel = PipeFour.Begin;
        digitalWrite(Output_4,LOW);
      }
    }

    if(PipeFour.start == true && allActive != true && CurrentTime >= PipeFourPooWait.LastTriggered + PipeFourPooWait.Duration && PipeFour.Pixel < PipeFour.End +  StreakLength){
      for(int x = 0; x < StreakLength; x++){
        if(PipeFour.Poo - x < PipeFour.Begin){break;}
        else if(PipeFour.Poo - x <= PipeFour.End){strip.setPixelColor(PipeFour.Poo - x, StreakHue[x], StreakHue[x],0);}
      }

      if(PipeFour.Poo < PipeFour.End + StreakLength){PipeFour.Poo++;}
    }
  }
}

void Sounds(){
  if(digitalRead(Input_1) == HIGH && CurrentTime > ToiletWait.LastTriggered + ToiletWait.Duration && ToiletSound == false){
    Serial.println(1);
    ToiletSound = true;
  }
  else if(digitalRead(Input_1) == LOW && PipeOne.start == false){ToiletSound = false;}


  if(digitalRead(Input_2) == HIGH && CurrentTime > BathWait.LastTriggered + BathWait.Duration && WasherSound == false){
    Serial.println(2);
    WasherSound = true;
  }
  else if(digitalRead(Input_2) == LOW && PipeTwo.start == false){WasherSound = false;}


  if(digitalRead(Input_3) == HIGH && CurrentTime > ShowerWait.LastTriggered + ShowerWait.Duration && ShowerSound == false && PipeThree.run == false){
    Serial.println(3);
    ShowerSound = true;
  }
  else if(digitalRead(Input_3) == LOW && ShowerSound == true){
   Serial.println(5);
    ShowerSound = false;
  }


  if(digitalRead(Input_4) == HIGH && CurrentTime > SinkWait.LastTriggered + SinkWait.Duration && SinkSound == false){
    Serial.println(4);
    SinkSound = true;
  }
  else if(digitalRead(Input_4) == LOW && PipeFour.start == false){SinkSound = false;}
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

void NewPoo(){
  if(PipeOne.Poo >= PipeOne.End && PipeOne.PooPath == false){
    if(PooOne.active == false){PooOne.setup(1);}
    else if(PooTwo.active == false){PooTwo.setup(1);}
    else if(PooThree.active == false){PooThree.setup(1);}
    else if(PooFour.active == false){PooFour.setup(1);}

    PipeOne.PooPath = true;
  }
  if(PipeTwo.Poo >= PipeTwo.End && PipeTwo.PooPath == false){
    if(PooOne.active == false){PooOne.setup(2);}
    else if(PooTwo.active == false){PooTwo.setup(2);}
    else if(PooThree.active == false){PooThree.setup(2);}
    else if(PooFour.active == false){PooFour.setup(2);}

    PipeTwo.PooPath = true;
  }
  if(PipeThree.Poo >= PipeThree.End && PipeThree.PooPath == false){
    if(PooOne.active == false){PooOne.setup(3);}
    else if(PooTwo.active == false){PooTwo.setup(3);}
    else if(PooThree.active == false){PooThree.setup(3);}
    else if(PooFour.active == false){PooFour.setup(3);}

    PipeThree.PooPath = true;
  }
  if(PipeFour.Poo >= PipeFour.End && PipeFour.PooPath == false){
    if(PooOne.active == false){PooOne.setup(4);}
    else if(PooTwo.active == false){PooTwo.setup(4);}
    else if(PooThree.active == false){PooThree.setup(4);}
    else if(PooFour.active == false){PooFour.setup(4);}

    PipeFour.PooPath = true;
  }

  if(PooOne.active == true){PooOne.run();}
  if(PooTwo.active == true){PooTwo.run();}
  if(PooThree.active == true){PooThree.run();}
  if(PooFour.active == true){PooFour.run();}
}

void PathUpdate(){
  if(StormLevel == 3){
    if(FlowOutIntensity == 1 && PathThreeUpdate == false){
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

      PathThreeUpdate = true;
    }
  }
  else{
    if(Runoff == 0 && PathFourUpdate == true){
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

// void Poo(){
//   if(CurrentTime >= PooOneSpeed.LastTriggered + PooOneSpeed.Duration){
//     if(PipeOne.Poo >= PipeOne.End && PipeOne.PooPath == false){
//       if(PooOne.active != true){
//         PooOne.Start[0] = UpperSewerEnd - PipeOneSewer;
//         PooOne.Start[1] = OceanDumpStart;
//         PooOne.Start[2] = WaterTreatmentStart + LowerSewerCombine + 1;
//         PooOne.Start[3] = WaterTreatmentEnd - DrainageLength + 1;
//         PooOne.End[0] = UpperSewerEnd - LengthOfUpperSewer;
//         PooOne.End[1] = OceanDumpStart -  LowerSewerCombine;
//         PooOne.End[2] = WaterTreatmentEnd - DrainageLength;
//         PooOne.End[3] = WaterTreatmentEnd;

//         PooOne.Poo[0]=PooOne.Start[0];
//         PooOne.active = true;
//         PipeOne.PooPath = true;
//       }
//       else if(PooTwo.active != true){
//         PooTwo.Start[0] = UpperSewerEnd - PipeOneSewer;
//         PooTwo.Start[1] = OceanDumpStart;
//         PooTwo.Start[2] = WaterTreatmentStart + LowerSewerCombine + 1;
//         PooTwo.Start[3] = WaterTreatmentEnd - DrainageLength + 1;
//         PooTwo.End[0] = UpperSewerEnd - LengthOfUpperSewer;
//         PooTwo.End[1] = OceanDumpStart -  LowerSewerCombine;
//         PooTwo.End[2] = WaterTreatmentEnd - DrainageLength;
//         PooTwo.End[3] WaterTreatmentEnd;

//         PooTwo.Poo[0]=PooTwo.Start[0];
//         PooTwo.active = true;
//         PipeOne.PooPath = true;
//       }
//       else if(PooThree.active != true){
//         PooThree.Start[0] = UpperSewerEnd - PipeOneSewer;
//         PooThree.Start[1] = OceanDumpStart;
//         PooThree.Start[2] = WaterTreatmentStart + LowerSewerCombine + 1;
//         PooThree.Start[3] = WaterTreatmentEnd - DrainageLength + 1;
//         PooThree.End[0] = UpperSewerEnd - LengthOfUpperSewer;
//         PooThree.End[1] = OceanDumpStart -  LowerSewerCombine;
//         PooThree.End[2] = WaterTreatmentEnd - DrainageLength;
//         PooThree.End[3] WaterTreatmentEnd;

//         PooThree.Poo[0]=PooThree.Start[0];
//         PooThree.active = true;
//         PipeOne.PooPath = true;
//       }
//       else if(PooFour.active != true){
//         PooFour.Start[0] = UpperSewerEnd - PipeOneSewer;
//         PooFour.Start[1] = OceanDumpStart;
//         PooFour.Start[2] = WaterTreatmentStart + LowerSewerCombine + 1;
//         PooFour.Start[3] = WaterTreatmentEnd - DrainageLength + 1;
//         PooFour.End[0] = UpperSewerEnd - LengthOfUpperSewer;
//         PooFour.End[1] = OceanDumpStart -  LowerSewerCombine;
//         PooFour.End[2] = WaterTreatmentEnd - DrainageLength;
//         PooFour.End[3] WaterTreatmentEnd;

//         PooFour.Poo[0]=PooFour.Start[0];
//         PooFour.active = true;
//         PipeOne.PooPath = true;
//       }
//     }
    
//     if(PipeTwo.Poo >= PipeTwo.End && PipeTwo.PooPath == false){
//       if(PooOne.active != true){     
//         PooOne.Start[0] = UpperSewerEnd - PipeTwoSewer;
//         PooOne.Start[1] = OceanDumpStart;
//         PooOne.Start[2] = WaterTreatmentStart + LowerSewerCombine + 1;
//         PooOne.Start[3] = WaterTreatmentEnd - DrainageLength + 1;
//         PooOne.End[0] = UpperSewerEnd - LengthOfUpperSewer;
//         PooOne.End[1] = OceanDumpStart -  LowerSewerCombine;
//         PooOne.End[2] = WaterTreatmentEnd - DrainageLength;
//         PooOne.End[3] WaterTreatmentEnd;

//         PooOne.Poo[0]=PooOne.Start[0];
//         PooOne.active = true;
//         PipeTwo.PooPath = true;
//       }
//       else if(PooTwo.active != true){
//         PooTwo.Start[0] = UpperSewerEnd - PipeTwoSewer;
//         PooTwo.Start[1] = OceanDumpStart;
//         PooTwo.Start[2] = WaterTreatmentStart + LowerSewerCombine + 1;
//         PooTwo.Start[3] = WaterTreatmentEnd - DrainageLength + 1;
//         PooTwo.End[0] = UpperSewerEnd - LengthOfUpperSewer;
//         PooTwo.End[1] = OceanDumpStart -  LowerSewerCombine;
//         PooTwo.End[2] = WaterTreatmentEnd - DrainageLength;
//         PooTwo.End[3] WaterTreatmentEnd;

//         PooTwo.Poo[0]=PooTwo.Start[0];
//         PooTwo.active = true;
//         PipeTwo.PooPath = true;
//       }
//       else if(PooThree.active != true){
//         PooThree.Start[0] = UpperSewerEnd - PipeTwoSewer;
//         PooThree.Start[1] = OceanDumpStart;
//         PooThree.Start[2] = WaterTreatmentStart + LowerSewerCombine + 1;
//         PooThree.Start[3] = WaterTreatmentEnd - DrainageLength + 1;
//         PooThree.End[0] = UpperSewerEnd - LengthOfUpperSewer;
//         PooThree.End[1] = OceanDumpStart -  LowerSewerCombine;
//         PooThree.End[2] = WaterTreatmentEnd - DrainageLength;
//         PooThree.End[3] WaterTreatmentEnd;

//         PooThree.Poo[0]=PooThree.Start[0];
//         PooThree.active = true;
//         PipeTwo.PooPath = true;
//       }
//       else if(PooFour.active != true){
//         PooFour.Start[0] = UpperSewerEnd - PipeTwoSewer;
//         PooFour.Start[1] = OceanDumpStart;
//         PooFour.Start[2] = WaterTreatmentStart + LowerSewerCombine + 1;
//         PooFour.Start[3] = WaterTreatmentEnd - DrainageLength + 1;
//         PooFour.End[0] = UpperSewerEnd - LengthOfUpperSewer;
//         PooFour.End[1] = OceanDumpStart -  LowerSewerCombine;
//         PooFour.End[2] = WaterTreatmentEnd - DrainageLength;
//         PooFour.End[3] WaterTreatmentEnd;

//         PooFour.Poo[0]=PooFour.Start[0];
//         PooFour.active = true;
//         PipeTwo.PooPath = true;
//       }
//     }

//     if(PipeThree.Poo >= PipeThree.End && PipeThree.PooPath == false){
//       if(PooOne.active != true){
//         PooOne.Start[0] = UpperSewerStart + PipeThreeSewer;
//         PooOne.Start[1] = WaterTreatmentStart;
//         PooOne.Start[2] = WaterTreatmentStart + LowerSewerCombine + 1;
//         PooOne.Start[3] = WaterTreatmentEnd - DrainageLength + 1;
//         PooOne.End[0] = UpperSewerStart + LengthOfUpperSewer;
//         PooOne.End[1] = WaterTreatmentStart + LowerSewerCombine;
//         PooOne.End[2] =  WaterTreatmentEnd - DrainageLength;
//         PooOne.End[3] = WaterTreatmentEnd;

//         PooOne.Poo[0]=PooOne.Start[0];
//         PooOne.active = true;
//         PipeThree.PooPath = true;
//       }
//       else if(PooTwo.active != true){
//         PooTwo.Start[0] = UpperSewerStart + PipeThreeSewer;
//         PooTwo.Start[1] = WaterTreatmentStart;
//         PooTwo.Start[2] = WaterTreatmentEnd - DrainageLength;
//         PooTwo.End[0] = UpperSewerStart + LengthOfUpperSewer;
//         PooTwo.End[1] = WaterTreatmentEnd - DrainageLength - 1;
//         PooTwo.End[2] = WaterTreatmentEnd;

//         PooTwo.Poo[0]=PooTwo.Start[0];
//         PooTwo.active = true;
//         PipeThree.PooPath = true;
//       }
//       else if(PooThree.active != true){
//         PooThree.Start[0] = UpperSewerStart + PipeThreeSewer;
//         PooThree.Start[1] = WaterTreatmentStart;
//         PooThree.Start[2] = WaterTreatmentEnd - DrainageLength;
//         PooThree.End[0] = UpperSewerStart + LengthOfUpperSewer;
//         PooThree.End[1] = WaterTreatmentEnd - DrainageLength - 1;
//         PooThree.End[2] = WaterTreatmentEnd;

//         PooThree.Poo[0]=PooThree.Start[0];
//         PooThree.active = true;
//         PipeThree.PooPath = true;
//       }
//       else if(PooFour.active != true){
//         PooFour.Start[0] = UpperSewerStart + PipeThreeSewer;
//         PooFour.Start[1] = WaterTreatmentStart;
//         PooFour.Start[2] = WaterTreatmentEnd - DrainageLength;
//         PooFour.End[0] = UpperSewerStart + LengthOfUpperSewer;
//         PooFour.End[1] = WaterTreatmentEnd - DrainageLength - 1;
//         PooFour.End[2] = WaterTreatmentEnd;

//         PooFour.Poo[0]=PooFour.Start[0];
//         PooFour.active = true;
//         PipeThree.PooPath = true;
//       }
//     }

//     if(PipeFour.Poo >= PipeFour.End && PipeFour.PooPath == false){
//       if(PooOne.active != true){
//         PooOne.Start[0] = UpperSewerStart + PipeFourSewer;
//         PooOne.Start[1] = WaterTreatmentStart;
//         PooOne.Start[2] = WaterTreatmentStat + LowerSewerCombine + 1;
//         PooOne.Start[3] = WaterTreatmentEnd - DrainageLength + 1;
//         PooOne.End[0] = UpperSewerStart + LengthOfUpperSewer;
//         PooOne.End[1] = WaterTreatmentStat + LowerSewerCombine;
//         PooOne.End[2] = WaterTreatmentEnd - DrainageLength;
//         PooOne.End[3] = WaterTreatmentEnd;

//         PooOne.Poo[0]=PooOne.Start[0];
//         PooOne.active = true;
//         PipeFour.PooPath = true;
//       }
//       else if(PooTwo.active != true){
//         PooTwo.Start[0] = UpperSewerStart + PipeFourSewer;
//         PooTwo.Start[1] = WaterTreatmentStart;
//         PooTwo.Start[2] = WaterTreatmentEnd - DrainageLength;
//         PooTwo.End[0] = UpperSewerStart + LengthOfUpperSewer;
//         PooTwo.End[1] = WaterTreatmentEnd - DrainageLength - 1;
//         PooTwo.End[2] = WaterTreatmentEnd;

//         PooTwo.Poo[0]=PooTwo.Start[0];
//         PooTwo.active = true;
//         PipeFour.PooPath = true;
//       }
//       else if(PooThree.active != true){
//         PooThree.Start[0] = UpperSewerStart + PipeFourSewer;
//         PooThree.Start[1] = WaterTreatmentStart;
//         PooThree.Start[2] = WaterTreatmentEnd - DrainageLength;
//         PooThree.End[0] = UpperSewerStart + LengthOfUpperSewer;
//         PooThree.End[1] = WaterTreatmentEnd - DrainageLength - 1;
//         PooThree.End[2] = WaterTreatmentEnd;

//         PooThree.Poo[0]=PooThree.Start[0];
//         PooThree.active = true;
//         PipeFour.PooPath = true;
//       }
//       else if(PooFour.active != true){
//         PooFour.Start[0] = UpperSewerStart + PipeFourSewer;
//         PooFour.Start[1] = WaterTreatmentStart;
//         PooFour.Start[2] = WaterTreatmentEnd - DrainageLength;
//         PooFour.End[0] = UpperSewerStart + LengthOfUpperSewer;
//         PooFour.End[1] = WaterTreatmentEnd - DrainageLength - 1;
//         PooFour.End[2] = WaterTreatmentEnd;

//         PooFour.Poo[0]=PooFour.Start[0];
//         PooFour.active = true;
//         PipeFour.PooPath = true;
//       }
//     }


//     if(PooOne.active == true){PooOneTravel();}
//     if(PooTwo.active == true){PooTwoTravel();}
//     if(PooThree.active == true){PooThreeTravel();}
//     if(PooFour.active == true){PooFourTravel();}

//     if(PooOne.active == true && PooTwo.active == true && PooThree.active == true && PooFour.active == true){allActive = true;}
//     else {allActive = false;}

//     PooOneSpeed.LastTriggered=CurrentTime;
//   }

// }

// void PooOneTravel() {
//     for(int x = 0; x < StreakLength; x++){
//       if(PooOne.Poo[x] != 0 && PooOne.Poo[x] != PooOne.End[PooOne.CurrentPath]){strip.setPixelColor(PooOne.Poo[x],StreakHue[x],StreakHue[x],0);}
//     }

//     for(int x = StreakLength; x > 0; x--){PooOne.Poo[x] = PooOne.Poo[x-1];}

//     if(StormLevel ==3 && (PooOne.Poo[0] == PooOne.End[1] +1 || PooOne.Poo[0] == PooOne.End[1] -1)){
//       PooOne.Start[2] = OceanDumpEnd + DrainageLength;
//       PooOne.End[2] = OceanDumpEnd;
//     }
    

//     if(PooOne.Start[PooOne.CurrentPath+1] != 0){
//       if(PooOne.Poo[0] < PooOne.End[PooOne.CurrentPath]){PooOne.Poo[0]++;}
//       else if(PooOne.Poo[0] > PooOne.End[PooOne.CurrentPath]){PooOne.Poo[0]--;}
//       if(PooOne.Poo[0] == PooOne.End[PooOne.CurrentPath]){
//         PooOne.CurrentPath++;
//         PooOne.Poo[0] =PooOne.Start[PooOne.CurrentPath];
//       }
//     }
//     else{
//       if(PooOne.Poo[0] < PooOne.End[PooOne.CurrentPath]){PooOne.Poo[0]++;}
//       else if(PooOne.Poo[0] > PooOne.End[PooOne.CurrentPath]){PooOne.Poo[0]--;}
//       if(PooOne.Poo[StreakLength-1] == PooOne.End[PooOne.CurrentPath]){
//         for(int x = 0; x < StreakLength; x++){PooOne.End[x] = 0;}
//         for(int x = 0; x < StreakLength; x++){PooOne.Start[x] = 0;}
//         for(int x = 0; x < StreakLength; x++){PooOne.Poo[x] = 0;}
//         PooOne.CurrentPath = 0;
//         PooOne.active = false;
//       }
//     }


//     // if(PooOne.End[PooOne.CurrentPath] == 0 || PooOne.CurrentPath == 4){
//     //   for(int x = 0; x < StreakLength; x++){PooOne.End[x] = 0;}
//     //   for(int x = 0; x < StreakLength; x++){PooOne.Start[x] = 0;}
//     //   for(int x = 0; x < StreakLength; x++){PooOne.Poo[x] = 0;}
//     //   PooOne.CurrentPath = 0;
//     //   PooOne.active = false;
//     // }
// }

// void PooTwoTravel() {
//     for(int x = 0; x < StreakLength; x++){
//       if(PooTwo.Poo[x] != 0 && PooTwo.Poo[x] != PooTwo.End[PooTwo.CurrentPath]){strip.setPixelColor(PooTwo.Poo[x],StreakHue[x],StreakHue[x],0);}
//     }

//     for(int x = StreakLength; x > 0; x--){PooTwo.Poo[x] = PooTwo.Poo[x-1];}

//     if(StormLevel ==3 && (PooTwo.Poo[0] == PooTwo.End[1] +1 || PooTwo.Poo[0] == PooTwo.End[1] -1)){
//       PooTwo.Start[2] = OceanDumpEnd + DrainageLength;
//       PooTwo.End[2] = OceanDumpEnd;
//     }
    
//     if(PooTwo.Start[PooTwo.CurrentPath +1] != 0){
//       if(PooTwo.Poo[0] < PooTwo.End[PooTwo.CurrentPath]){PooTwo.Poo[0]++;}
//       else if(PooTwo.Poo[0] > PooTwo.End[PooTwo.CurrentPath]){PooTwo.Poo[0]--;}
//       if(PooTwo.Poo[0] == PooTwo.End[PooTwo.CurrentPath]){
//         PooTwo.CurrentPath++;
//         PooTwo.Poo[0] =PooTwo.Start[PooTwo.CurrentPath];
//       }
//     }
//     else{
//       if(PooTwo.Poo[0] < PooTwo.End[PooTwo.CurrentPath]){PooTwo.Poo[0]++;}
//       else if(PooTwo.Poo[0] > PooTwo.End[PooTwo.CurrentPath]){PooTwo.Poo[0]--;}
//       if(PooTwo.Poo[StreakLength -1] == PooTwo.End[PooTwo.CurrentPath]){
//         for(int x = 0; x < StreakLength; x++){PooTwo.End[x] = 0;}
//         for(int x = 0; x < StreakLength; x++){PooTwo.Start[x] = 0;}
//         for(int x = 0; x < StreakLength; x++){PooTwo.Poo[x] = 0;}
//         PooTwo.CurrentPath = 0;
//         PooTwo.active = false;
//       }
//     }


//     // if(PooTwo.End[PooTwo.CurrentPath] == 0 || PooTwo.CurrentPath == 4){
//     //   for(int x = 0; x < StreakLength; x++){PooTwo.End[x] = 0;}
//     //   for(int x = 0; x < StreakLength; x++){PooTwo.Start[x] = 0;}
//     //   for(int x = 0; x < StreakLength; x++){PooTwo.Poo[x] = 0;}
//     //   PooTwo.CurrentPath = 0;
//     //   PooTwo.active = false;
//     // }
// }

// void PooThreeTravel() {
//     for(int x = 0; x < StreakLength; x++){
//       if(PooThree.Poo[x] != 0 && PooThree.Poo[x] != PooThree.End[PooThree.CurrentPath]){strip.setPixelColor(PooThree.Poo[x],StreakHue[x],StreakHue[x],0);}
//     }

//     for(int x = StreakLength; x > 0; x--){PooThree.Poo[x] = PooThree.Poo[x-1];}

//     if(StormLevel ==3 && (PooThree.Poo[0] == PooThree.End[1] +1 || PooThree.Poo[0] == PooThree.End[1] -1)){
//       PooThree.Start[2] = OceanDumpEnd + DrainageLength;
//       PooThree.End[2] = OceanDumpEnd;
//     }
    

//     if(PooThree.Start[PooThree.CurrentPath +1] != 0){
//       if(PooThree.Poo[0] < PooThree.End[PooThree.CurrentPath]){PooThree.Poo[0]++;}
//       else if(PooThree.Poo[0] > PooThree.End[PooThree.CurrentPath]){PooThree.Poo[0]--;}
//       if(PooThree.Poo[0] == PooThree.End[PooThree.CurrentPath]){
//         PooThree.CurrentPath++;
//         PooThree.Poo[0] =PooThree.Start[PooThree.CurrentPath];
//       }
//     }
//     else{
//       if(PooThree.Poo[0] < PooThree.End[PooThree.CurrentPath]){PooThree.Poo[0]++;}
//       else if(PooThree.Poo[0] > PooThree.End[PooThree.CurrentPath]){PooThree.Poo[0]--;}
//       if(PooThree.Poo[StreakLength -1] == PooThree.End[PooThree.CurrentPath]){
//         for(int x = 0; x < StreakLength; x++){PooThree.End[x] = 0;}
//         for(int x = 0; x < StreakLength; x++){PooThree.Start[x] = 0;}
//         for(int x = 0; x < StreakLength; x++){PooThree.Poo[x] = 0;}
//         PooThree.CurrentPath = 0;
//         PooThree.active = false;
//       }
//     }


//     // if(PooThree.End[PooThree.CurrentPath] == 0 || PooThree.CurrentPath == 4){
//     //   for(int x = 0; x < StreakLength; x++){PooThree.End[x] = 0;}
//     //   for(int x = 0; x < StreakLength; x++){PooThree.Start[x] = 0;}
//     //   for(int x = 0; x < StreakLength; x++){PooThree.Poo[x] = 0;}
//     //   PooThree.CurrentPath = 0;
//     //   PooThree.active = false;
//     // }
// }

// void PooFourTravel() {
//     for(int x = 0; x < StreakLength; x++){
//       if(PooFour.Poo[x] != 0 && PooFour.Poo[x] != PooFour.End[PooFour.CurrentPath]){strip.setPixelColor(PooFour.Poo[x],StreakHue[x],StreakHue[x],0);}
//     }

//     for(int x = StreakLength; x > 0; x--){PooFour.Poo[x] = PooFour.Poo[x-1];}

//     if(StormLevel ==3 && (PooFour.Poo[0] == PooFour.End[1] +1 || PooFour.Poo[0] == PooFour.End[1] -1)){
//       PooFour.Start[2] = OceanDumpEnd + DrainageLength;
//       PooFour.End[2] = OceanDumpEnd;
//     }
    
//     if(PooFour.Start[PooFour.CurrentPath +1] != 0){
//       if(PooFour.Poo[0] < PooFour.End[PooFour.CurrentPath]){PooFour.Poo[0]++;}
//       else if(PooFour.Poo[0] > PooFour.End[PooFour.CurrentPath]){PooFour.Poo[0]--;}
//       if(PooFour.Poo[0] == PooFour.End[PooFour.CurrentPath]){
//         PooFour.CurrentPath++;
//         PooFour.Poo[0] =PooFour.Start[PooFour.CurrentPath];
//       }
//     }
//     else{
//       if(PooFour.Poo[0] < PooFour.End[PooFour.CurrentPath]){PooFour.Poo[0]++;}
//       else if(PooFour.Poo[0] > PooFour.End[PooFour.CurrentPath]){PooFour.Poo[0]--;}
//       if(PooFour.Poo[StreakLength -1] == PooFour.End[PooFour.CurrentPath]){
//         for(int x = 0; x < StreakLength; x++){PooFour.End[x] = 0;}
//         for(int x = 0; x < StreakLength; x++){PooFour.Start[x] = 0;}
//         for(int x = 0; x < StreakLength; x++){PooFour.Poo[x] = 0;}
//         PooFour.CurrentPath = 0;
//         PooFour.active = false;
//       }
//     }

//     // if(PooFour.End[PooFour.CurrentPath] == 0 || PooFour.CurrentPath == 4){
//     //   for(int x = 0; x < StreakLength; x++){PooFour.End[x] = 0;}
//     //   for(int x = 0; x < StreakLength; x++){PooFour.Start[x] = 0;}
//     //   for(int x = 0; x < StreakLength; x++){PooFour.Poo[x] = 0;}
//     //   PooFour.CurrentPath = 0;
//     //   PooFour.active = false;
//     // }
// }

void loop() {
  CurrentTime = millis();
  //Rain();
  //Poo();
  //UpperSewers();
  //LowerSewers();
  Toilet();
  BathTub();
  Shower();
  Sink();

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

  if(CurrentTime  >= PipesFrame.Duration + PipesFrame.LastTriggered){
    Sounds();

    LastPixel = WaveHue[Length-1];
    for(int x = Length; x >= 0; x--){
      if(x!=0){WaveHue[x]=WaveHue[x-1];} 
      else {WaveHue[x]=LastPixel;}
    }
    PipesFrame.LastTriggered = CurrentTime;
  }

  strip.show();
}
