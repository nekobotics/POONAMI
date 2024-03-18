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

bool allActive = false;

bool GutterBegin = false;
const int GutterLength = 3;

int StormLevel = 0;
int ProjectedStormLevel = 0;
int StormIntensity;
bool RainPush = false;
bool Raining = false;

const int StreakLength = 5;

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

struct Time {
  unsigned long LastTriggered;
  long Duration;
};

Time PipesFrame = {0,5};
Time BathWait = {0,50};
Time ToiletWait = BathWait;
Time ShowerWait = BathWait;
Time SinkWait = BathWait;
Time PipeOnePooWait = {0,60};
Time PipeTwoPooWait = PipeOnePooWait;
Time PipeThreePooWait = PipeOnePooWait;
Time PipeFourPooWait = PipeOnePooWait;
Time RainFrame = {0,10};
Time RainWait = {0,500};
Time SewersFrame = {0,10};
Time RunoffWait = {0,4};
Time ProjectedSpeed = {0,10};
Time GutterSpeedWait = RainWait;
Time SewersSpeedWait = RainWait;
Time RainHold = {0,1000};
Time PooOneSpeed = {0,11};
Time GreenWaveHoldTime = {0,200};
Time BlueWaveHoldTime = {0,400};

// struct GutterPixel{
//   int Start;
//   int Pixel;
//   int Last;
// };

// GutterPixel GutterOne = {0,0,0};
// GutterPixel GutterTwo = {1,0,0};
// GutterPixel GutterThree = {2,0,0};
// GutterPixel GutterFour = {3,0,0};

  const int PipeOneSewer = 10;
  const int PipeTwoSewer = 15;
  const int PipeThreeSewer = 22;
  const int PipeFourSewer = 5;
  // const int PipeSewerDropSize = 2;

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
  const int LowerSewerCombine = 14;

const int Length = 40;


// struct ShiftingColor {
//   int RedHue[Length];
//   int GreenHue[Length];
//   int BlueHue[Length];
//   int LastRed;
//   int LastGreen;
//   int LastBlue;
// };

// ShiftingColor SewerWaveHue;
// ShiftingColor GutterHue;
// ShiftingColor PipeWaveHue;

struct StaticColor {
  int RedHue[Length];
  int GreenHue[Length];
  int BlueHue[Length];
};

// StaticColor PipeLeadHue;
StaticColor RunoffLeadHue;


int WaveHue[Length];
int BlueSewerWaveHue[Length];
int GreenSewerWaveHue[Length];

int SewerWaveHue[Length];

int GutterHue[Length];

int ColorHue[Length];

int RunoffHue[Length];

int StreakHue[StreakLength];

int LastPixel;
int SewersLastPixel;
int BlueSewersLastPixel;
int GreenSewersLastPixel;
int LastGutterPixel;

struct PooDrop{
  bool active;
  int Pipe;
  int Poo[StreakLength];
  int Start[5];
  int End[5];
  int CurrentPath;
};

PooDrop PooOne = {false,0,{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},0};
PooDrop PooTwo = {false,0,{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},0};
PooDrop PooThree = {false,0,{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},0};
PooDrop PooFour = {false,0,{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},0};

int GutterPixel = 0;
int GutterLag = 0;

unsigned long CurrentTime;
bool RainOn = false;

bool CurrentShiftRise = true;
int CurrentShift = 0;

Adafruit_NeoPixel strip(Num_Pixels, Pixel_Pin, NEO_GRBW + NEO_KHZ800);

const int RainMultiplyer = 50;

void WaveUpdate(){
  for(int x=0; x < Length; x++){
    ColorHue[x]= (200/2)+((200/2) * cos(x * (3.14/Length)));
    RunoffHue[x]= (200/2)+((200/2) * sin(x *(1.57/Length)-1.57));
    WaveHue[x]= (200/2)-((200/2) * sin(x * (3.14/Length)));
    GreenSewerWaveHue[x]= (10/2)-((10/2) * sin(x * (3.14/Length)));
    BlueSewerWaveHue[x]= (20/2)-((20/2) * sin(x * (3.14/Length)));
    SewerWaveHue[x]= (200/2)-((200/2) * sin(x * (3.14/Length)));
    GutterHue[x]= (40/2)+((40/2)*cos(x*(6.26/Length)));

    RunoffLeadHue.BlueHue[x] = (20/2)+((20/2) * cos(x * ((3.14/2)/Length)));
    RunoffLeadHue.GreenHue[x] = (10/2)+((10/2) * cos(x * ((3.14/2)/Length)));
  }

  for(int x=0; x < StreakLength; x++){
    StreakHue[x] = (200/2)+((200/2)*cos(x*(3.14/StreakLength)));
  }
  
  ColorHue[Length-1] = 0;
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

  pinMode(ShowerSound,OUTPUT);
  pinMode(ToiletSound,OUTPUT);

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
        digitalWrite(ShowerSound, HIGH);
        for(int x = 0; x < PipeTwo.Stream-PipeTwo.Begin; x++){strip.setPixelColor(PipeTwo.Begin + x, 0, WaveHue[x -(Length*(x/Length))], 150);}
        if(PipeTwo.Stream <= PipeTwo.End){PipeTwo.Stream++;}
      }
    }

    else if(PipeTwo.start == true && CurrentTime >= BathWait.LastTriggered + BathWait.Duration){
      PipeTwo.run = true;
      digitalWrite(ShowerSound, LOW);
      
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

    ProjectedSpeed.Duration = 11;
    // RainFrame.Duration = 10;
    // SewersFrame.Duration = 10;
  }
  else if(ProjectedStormLevel == 2 || (ProjectedStormLevel < StormLevel && StormLevel == 2)){
    digitalWrite(Rain_Output1, HIGH);
    digitalWrite(Rain_Output2, HIGH);
    digitalWrite(Rain_Output3, LOW);

    digitalWrite(RainLvl1, HIGH);
    digitalWrite(RainLvl2, HIGH);

    ProjectedSpeed.Duration = 9;
    // RainFrame.Duration = 9;
    // SewersFrame.Duration = 9;

  }
  else if (ProjectedStormLevel== 3 || (ProjectedStormLevel < StormLevel && StormLevel == 3)){
    digitalWrite(Rain_Output1, HIGH);
    digitalWrite(Rain_Output2, HIGH);
    digitalWrite(Rain_Output3, HIGH);

    digitalWrite(RainLvl1, HIGH);
    digitalWrite(RainLvl1, HIGH);
    digitalWrite(RainLvl2,HIGH);

    ProjectedSpeed.Duration = 7;
    // RainFrame.Duration = 7;
    // SewersFrame.Duration = 7;

  }
  else {
    digitalWrite(Rain_Output1, LOW);
    digitalWrite(Rain_Output2, LOW);
    digitalWrite(Rain_Output3, LOW);

    digitalWrite(RainLvl1,LOW);
    digitalWrite(RainLvl2,LOW);
    digitalWrite(RainLvl3,LOW);

    ProjectedSpeed.Duration = 11;
    // RainFrame.Duration = 10;
    // SewersFrame.Duration = 10;
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
    PooOneSpeed.Duration = SewersFrame.Duration;
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

void RainGutters(){
  if(StormLevel > 0){
    for(int i=1; i <= 4; i++){
      for(int x = 0; x < GutterPixel; x++){strip.setPixelColor((Num_Pixels - (3*i))+x,0,GutterHue[x]*StormLevel, (GutterHue[x]+5)*StormLevel);}
    }
    if(GutterPixel <= 3){GutterPixel++;}
    GutterBegin = true;
  }
  else if (GutterBegin == true){   
    for(int i=1; i <= 4; i++){
      for(int x = 0; x < Length; x++){
        if(GutterLag - x < 0){break;}
        else if(GutterLag - x <= GutterLength){strip.setPixelColor((Num_Pixels - (3*i))+(GutterLag-x),0,ColorHue[x],ColorHue[x]);}
      } 
    }
    GutterLag++;
    if(GutterLag > GutterLength + Length){
      GutterPixel = 0;
      GutterLag = 0;
      GutterBegin = false;
    }
  }

  LastGutterPixel = GutterHue[Length-1];
  for(int x = Length-1; x >= 0; x--){
    //Serial.println(GutterHue[x]);
    if(x!=0){GutterHue[x]=GutterHue[x-1];} 
    else {GutterHue[x]=LastGutterPixel;}
  }

}

void UpperSewers(){
  for(int x = 0; x < LengthOfUpperSewer; x++){
    if (StormLevel == 0){
      strip.setPixelColor(x+UpperSewerStart,0,0,(1+BlueSewerWaveHue[x-(Length*(x/Length))]) * 10);
      strip.setPixelColor(UpperSewerEnd-x,0,0,(1+BlueSewerWaveHue[x-(Length*(x/Length))]) * 10);
    }
    else if(StormLevel == 1){
      strip.setPixelColor(x+UpperSewerStart,0,(GreenSewerWaveHue[x-(Length*(x/Length))]+1) * (CurrentShift/3),(1+BlueSewerWaveHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)));
      strip.setPixelColor(UpperSewerEnd-x,0,(GreenSewerWaveHue[x-(Length*(x/Length))]+1) * (CurrentShift/3),(1+BlueSewerWaveHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)));
    }
    else if (StormLevel == 2){
      strip.setPixelColor(x+UpperSewerStart,0,(GreenSewerWaveHue[x-(Length*(x/Length))]+1) * (CurrentShift/2),(1+BlueSewerWaveHue[x-(Length*(x/Length))]) * (10-(CurrentShift/2)));
      strip.setPixelColor(UpperSewerEnd-x,0,(GreenSewerWaveHue[x-(Length*(x/Length))]+1) * (CurrentShift/2),(1+BlueSewerWaveHue[x-(Length*(x/Length))]) * (10-(CurrentShift/2)));
    }
    else if (StormLevel == 3){
      strip.setPixelColor(x+UpperSewerStart,0,(GreenSewerWaveHue[x-(Length*(x/Length))]+1) * (CurrentShift),(1+BlueSewerWaveHue[x-(Length*(x/Length))]) * (10-(CurrentShift/2)));
      strip.setPixelColor(UpperSewerEnd-x,0,(GreenSewerWaveHue[x-(Length*(x/Length))]+1) * (CurrentShift),(1+BlueSewerWaveHue[x-(Length*(x/Length))]) * (10-(CurrentShift/2)));
    }
    // strip.setPixelColor(x+UpperSewerStart,0,(StormLevel+1)*(SewerWaveHue[x-(Length*(x/Length))]/3)+15,20+(StormLevel*40));
    // strip.setPixelColor(UpperSewerEnd-x,0,(StormLevel+1)*(SewerWaveHue[x-(Length*(x/Length))]/3)+15,20+(StormLevel*40));
    //Serial.println(CurrentShift);
  }
}

void LowerSewers(){
  if (StormLevel == 1){
    for(int x = 0; x < LowerSewersLength; x++){
      strip.setPixelColor(WaterTreatmentStart + x,0,(GreenSewerWaveHue[x-(Length*(x/Length))]+1) * (CurrentShift/3),(1+BlueSewerWaveHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)));      
      if( x < LowerSewerCombine + Runoff){strip.setPixelColor(OceanDumpStart - x,0,RunoffFade*((GreenSewerWaveHue[x-(Length*(x/Length))]+1) * (CurrentShift/3)),RunoffFade*((1+BlueSewerWaveHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3))));}
      else{strip.setPixelColor(OceanDumpStart - x,0,0,0);}
    }

    if(RunoffLead > 0){ // fade away (See ya silverhand)
      RunoffFade = (Runoff/(LowerSewersLength - DrainageLength - LowerSewerCombine));
      if(Runoff > 0) {
        Runoff--;
        RunoffLead--;
      }
      // if(Runoff > 0){Runoff--};
      // //for(int x = 0; x < abs(RunoffLead -Runoff); x++){strip.setPixelColor((OceanDumpStart-LowerSewersCombine) - Runoff - x),0,0,0);}
      // if((abs(Runofflead - Runoff) > 0 && Runoff = 0) || abs(RunoffLead - Runoff) > Length){RunoffLead--;}
    };
  }

  else if (StormLevel == 2){
    for(int x = 0; x < LowerSewersLength; x++){
      strip.setPixelColor(WaterTreatmentStart + x,0,(GreenSewerWaveHue[x-(Length*(x/Length))]+1) * (CurrentShift/3),(1+BlueSewerWaveHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)));      
      if( x < LowerSewerCombine + Runoff){strip.setPixelColor(OceanDumpStart - x,0,(GreenSewerWaveHue[x-(Length*(x/Length))]+1) * (CurrentShift/3),(1+BlueSewerWaveHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)));}
      else{strip.setPixelColor(OceanDumpStart - x,0,0,0);}
    }

    if(RunoffLead > LowerSewersLength - DrainageLength - LowerSewerCombine && Runoff != RunoffLead){ // Trickle out to ocean
      // RunoffLead = LowerSewersLength - DrainageLength - LowerSewerCombine;
      // for(int x = 0; x < DrainageLength+Length; x++){
      //   if(RunoffLead - x < 0){break;}
      //   else if (Runoff - x <= DrainageLength){strip.setPixelColor((OceanDumpEnd - DrainageLength) - (Runoff - x),0,0,0);}
      // }
      // if(RunoffLead < LowerSewersLength + Length- LowerSewerCombine){RunoffLead++;}
      // else {RunoffLead = Runoff;};

      // if(RunoffLead > LowerSewersLength- LowerSewerCombine){Runoff = LowerSewersLength - DrainageLength - LowerSewerCombine;}

      if(Runoff > (LowerSewersLength-DrainageLength-LowerSewerCombine)){Runoff--;}
      for(int x = 0; x < abs(RunoffLead -Runoff); x++){strip.setPixelColor(((OceanDumpStart-LowerSewerCombine) - Runoff - x),0,0,0);}
      if(((abs(RunoffLead - Runoff) > 0 && Runoff == (LowerSewersLength-DrainageLength-LowerSewerCombine) || abs(RunoffLead - Runoff) > Length))){RunoffLead--;}
    }
    else if (Runoff < LowerSewersLength - DrainageLength - LowerSewerCombine){ // Trickle out to DrainageRegulator
      if(RunoffLead < (LowerSewersLength-DrainageLength-LowerSewerCombine)){RunoffLead++;}
      for(int x = 0; x < abs(RunoffLead -Runoff); x++){strip.setPixelColor(((OceanDumpStart-LowerSewerCombine) - Runoff - x),0,(RunoffLeadHue.GreenHue[x] +1)*(CurrentShift/3),(RunoffLeadHue.BlueHue[x] +1)*(10-(CurrentShift/3)));}
      if( (abs(RunoffLead - Runoff) > 0 && RunoffLead == (LowerSewersLength-DrainageLength-LowerSewerCombine) || (RunoffLead - Runoff) > Length)){Runoff++;}
    }
  }

  else if (StormLevel == 3){ //Flow out to Ocean
    for(int x = 0; x < LowerSewersLength; x++){
      strip.setPixelColor(WaterTreatmentStart + x,0,(GreenSewerWaveHue[x-(Length*(x/Length))]+1) * (CurrentShift/3),(1+BlueSewerWaveHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)));      
      if( x < LowerSewerCombine + Runoff){strip.setPixelColor(OceanDumpStart - x,0,(GreenSewerWaveHue[x-(Length*(x/Length))]+1) * (CurrentShift/3),(1+BlueSewerWaveHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)));}
      else{strip.setPixelColor(OceanDumpStart - x,0,0,0);}
    }

    if(Runoff < OceanDumpEnd){
      if(RunoffLead < LowerSewersLength-LowerSewerCombine){RunoffLead++;}
      for(int x = 0; x < abs(RunoffLead -Runoff); x++){strip.setPixelColor(((OceanDumpStart-LowerSewerCombine) - Runoff - x),0,(RunoffLeadHue.GreenHue[x] +1)*(CurrentShift/3),(RunoffLeadHue.BlueHue[x] +1)*(10-(CurrentShift/3)));}
      if((abs(RunoffLead - Runoff) > 0 && RunoffLead == LowerSewersLength-LowerSewerCombine || Length < abs(RunoffLead - Runoff))){Runoff++;}
    }
  }

  else {    
   for(int x = 0; x < LowerSewersLength; x++){
      strip.setPixelColor(WaterTreatmentStart + x,0,0,(1+BlueSewerWaveHue[x-(Length*(x/Length))]) * (10-(CurrentShift/3)));      
      if( x < LowerSewerCombine + Runoff){strip.setPixelColor(OceanDumpStart - x,0,0,(1+BlueSewerWaveHue[x-(Length*(x/Length))]) * 10);}
      else{strip.setPixelColor(OceanDumpStart - x,0,0,0);}
    }
  }

  return; // Correct code, drains correctly, works off of old animation

    if(StormLevel < 3){
      for(int x = 0; x < LowerSewersLength;x++){
        strip.setPixelColor(WaterTreatmentStart + x,0,(StormIntensity * (SewerWaveHue[x-(Length*(x/Length))]/3) + 15), (20 + StormIntensity * 40));
        if((x < LowerSewersLength - DrainageLength + Runoff && RunoffDrain == false) || x < LowerSewersLength - DrainageLength){strip.setPixelColor(OceanDumpStart - x,0,(StormIntensity * (SewerWaveHue[x-(Length*(x/Length))]/3) + 15), (20 + StormIntensity * 40));}
        else{strip.setPixelColor(OceanDumpStart - x,0,0,0);}
      }

      if(Runoff > 0){
        if(Runoff < Length){RunoffDrain = true;}
        if(CurrentTime >= RunoffWait.LastTriggered + RunoffWait.Duration){
          for(int i = 0; i < Length; i++){
            if(Runoff - i < 0){break;}
            else if(Runoff - i <= DrainageLength){strip.setPixelColor(OceanDumpEnd + (Runoff - i),0,RunoffHue[i],RunoffHue[i]);}
          }
          Runoff--;
          RunoffWait.LastTriggered = CurrentTime;
          }
       }
      else{RunoffDrain = false;}
    }

    else if (StormLevel == 3){
      for(int x = 0; x < LowerSewersLength; x++){
        strip.setPixelColor(WaterTreatmentStart + x,0,(StormIntensity * (SewerWaveHue[x-(Length*(x/Length))]/3) + 15), (20 + StormIntensity * 40));
        if(x < LowerSewersLength - DrainageLength + Runoff){strip.setPixelColor(OceanDumpStart - x,0,(StormIntensity * (SewerWaveHue[x-(Length*(x/Length))]/3) + 15), (20 + StormIntensity * 40));}
      }
      if(Runoff <= DrainageLength + Length){
        if(CurrentTime >= RunoffWait.LastTriggered + RunoffWait.Duration){
          for(int i = 0; i < Length; i++){
            if(Runoff - i < 0){break;}
            else if(Runoff - i <= DrainageLength){strip.setPixelColor(((OceanDumpEnd + DrainageLength) - (Runoff - i)), 0, RunoffHue[i], RunoffHue[i]);}
          }
        }
        Runoff++;
        RunoffWait.LastTriggered = CurrentTime;
      }
    }
}

void Poo(){
  if(CurrentTime >= PooOneSpeed.LastTriggered + PooOneSpeed.Duration){
    if(PipeOne.Poo >= PipeOne.End && PipeOne.PooPath == false){
      if(PooOne.active != true){
        PooOne.Start[0] = UpperSewerEnd - PipeOneSewer;
        PooOne.Start[1] = OceanDumpStart;
        PooOne.Start[2] = WaterTreatmentEnd - DrainageLength;
        PooOne.End[0] = UpperSewerEnd - LengthOfUpperSewer;
        PooOne.End[1] = OceanDumpEnd + DrainageLength +1;
        PooOne.End[2] = WaterTreatmentEnd;

        PooOne.Poo[0]=PooOne.Start[0];
        PooOne.active = true;
        PipeOne.PooPath = true;
      }
      else if(PooTwo.active != true){
        PooTwo.Start[0] = UpperSewerEnd - PipeOneSewer;
        PooTwo.Start[1] = OceanDumpStart;
        PooTwo.Start[2] = WaterTreatmentEnd - DrainageLength;
        PooTwo.End[0] = UpperSewerEnd - LengthOfUpperSewer;
        PooTwo.End[1] = OceanDumpEnd + DrainageLength +1;
        PooTwo.End[2] = WaterTreatmentEnd;

        PooTwo.Poo[0]=PooTwo.Start[0];
        PooTwo.active = true;
        PipeOne.PooPath = true;
      }
      else if(PooThree.active != true){
        PooThree.Start[0] = UpperSewerEnd - PipeOneSewer;
        PooThree.Start[1] = OceanDumpStart;
        PooThree.Start[2] = WaterTreatmentEnd - DrainageLength;
        PooThree.End[0] = UpperSewerEnd - LengthOfUpperSewer;
        PooThree.End[1] = OceanDumpEnd + DrainageLength +1;
        PooThree.End[2] = WaterTreatmentEnd;

        PooThree.Poo[0]=PooThree.Start[0];
        PooThree.active = true;
        PipeOne.PooPath = true;
      }
      else if(PooFour.active != true){
        PooFour.Start[0] = UpperSewerEnd - PipeOneSewer;
        PooFour.Start[1] = OceanDumpStart;
        PooFour.Start[2] = WaterTreatmentEnd - DrainageLength;
        PooFour.End[0] = UpperSewerEnd - LengthOfUpperSewer;
        PooFour.End[1] = OceanDumpEnd + DrainageLength +1;
        PooFour.End[2] = WaterTreatmentEnd;

        PooFour.Poo[0]=PooFour.Start[0];
        PooFour.active = true;
        PipeOne.PooPath = true;
      }
    }
    
    if(PipeTwo.Poo >= PipeTwo.End && PipeTwo.PooPath == false){
      if(PooOne.active != true){
        PooOne.Start[0] = UpperSewerEnd - PipeTwoSewer;
        PooOne.Start[1] = OceanDumpStart;
        PooOne.Start[2] = WaterTreatmentEnd - DrainageLength;
        PooOne.End[0] = UpperSewerEnd - LengthOfUpperSewer;
        PooOne.End[1] = OceanDumpEnd + DrainageLength +1;
        PooOne.End[2] = WaterTreatmentEnd;

        PooOne.Poo[0]=PooOne.Start[0];
        PooOne.active = true;
        PipeTwo.PooPath = true;
      }
      else if(PooTwo.active != true){
        PooTwo.Start[0] = UpperSewerEnd - PipeTwoSewer;
        PooTwo.Start[1] = OceanDumpStart;
        PooTwo.Start[2] = WaterTreatmentEnd - DrainageLength;
        PooTwo.End[0] = UpperSewerEnd - LengthOfUpperSewer;
        PooTwo.End[1] = OceanDumpEnd + DrainageLength +1;
        PooTwo.End[2] = WaterTreatmentEnd;

        PooTwo.Poo[0]=PooTwo.Start[0];
        PooTwo.active = true;
        PipeTwo.PooPath = true;
      }
      else if(PooThree.active != true){
        PooThree.Start[0] = UpperSewerEnd - PipeTwoSewer;
        PooThree.Start[1] = OceanDumpStart;
        PooThree.Start[2] = WaterTreatmentEnd - DrainageLength;
        PooThree.End[0] = UpperSewerEnd - LengthOfUpperSewer;
        PooThree.End[1] = OceanDumpEnd + DrainageLength +1;
        PooThree.End[2] = WaterTreatmentEnd;

        PooThree.Poo[0]=PooThree.Start[0];
        PooThree.active = true;
        PipeTwo.PooPath = true;
      }
      else if(PooFour.active != true){
        PooFour.Start[0] = UpperSewerEnd - PipeTwoSewer;
        PooFour.Start[1] = OceanDumpStart;
        PooFour.Start[2] = WaterTreatmentEnd - DrainageLength;
        PooFour.End[0] = UpperSewerEnd - LengthOfUpperSewer;
        PooFour.End[1] = OceanDumpEnd + DrainageLength +1;
        PooFour.End[2] = WaterTreatmentEnd;

        PooFour.Poo[0]=PooFour.Start[0];
        PooFour.active = true;
        PipeTwo.PooPath = true;
      }
    }

    if(PipeThree.Poo >= PipeThree.End && PipeThree.PooPath == false){
      if(PooOne.active != true){
        PooOne.Start[0] = UpperSewerStart + PipeThreeSewer;
        PooOne.Start[1] = WaterTreatmentStart;
        PooOne.Start[2] = WaterTreatmentEnd - DrainageLength;
        PooOne.End[0] = UpperSewerStart + LengthOfUpperSewer;
        PooOne.End[1] = WaterTreatmentEnd - DrainageLength - 1;
        PooOne.End[2] = WaterTreatmentEnd;

        PooOne.Poo[0]=PooOne.Start[0];
        PooOne.active = true;
        PipeThree.PooPath = true;
      }
      else if(PooTwo.active != true){
        PooTwo.Start[0] = UpperSewerStart + PipeThreeSewer;
        PooTwo.Start[1] = WaterTreatmentStart;
        PooTwo.Start[2] = WaterTreatmentEnd - DrainageLength;
        PooTwo.End[0] = UpperSewerStart + LengthOfUpperSewer;
        PooTwo.End[1] = WaterTreatmentEnd - DrainageLength - 1;
        PooTwo.End[2] = WaterTreatmentEnd;

        PooTwo.Poo[0]=PooTwo.Start[0];
        PooTwo.active = true;
        PipeThree.PooPath = true;
      }
      else if(PooThree.active != true){
        PooThree.Start[0] = UpperSewerStart + PipeThreeSewer;
        PooThree.Start[1] = WaterTreatmentStart;
        PooThree.Start[2] = WaterTreatmentEnd - DrainageLength;
        PooThree.End[0] = UpperSewerStart + LengthOfUpperSewer;
        PooThree.End[1] = WaterTreatmentEnd - DrainageLength - 1;
        PooThree.End[2] = WaterTreatmentEnd;

        PooThree.Poo[0]=PooThree.Start[0];
        PooThree.active = true;
        PipeThree.PooPath = true;
      }
      else if(PooFour.active != true){
        PooFour.Start[0] = UpperSewerStart + PipeThreeSewer;
        PooFour.Start[1] = WaterTreatmentStart;
        PooFour.Start[2] = WaterTreatmentEnd - DrainageLength;
        PooFour.End[0] = UpperSewerStart + LengthOfUpperSewer;
        PooFour.End[1] = WaterTreatmentEnd - DrainageLength - 1;
        PooFour.End[2] = WaterTreatmentEnd;

        PooFour.Poo[0]=PooFour.Start[0];
        PooFour.active = true;
        PipeThree.PooPath = true;
      }
    }

    if(PipeFour.Poo >= PipeFour.End && PipeFour.PooPath == false){
      if(PooOne.active != true){
        PooOne.Start[0] = UpperSewerStart + PipeFourSewer;
        PooOne.Start[1] = WaterTreatmentStart;
        PooOne.Start[2] = WaterTreatmentEnd - DrainageLength;
        PooOne.End[0] = UpperSewerStart + LengthOfUpperSewer;
        PooOne.End[1] = WaterTreatmentEnd - DrainageLength - 1;
        PooOne.End[2] = WaterTreatmentEnd;

        PooOne.Poo[0]=PooOne.Start[0];
        PooOne.active = true;
        PipeFour.PooPath = true;
      }
      else if(PooTwo.active != true){
        PooTwo.Start[0] = UpperSewerStart + PipeFourSewer;
        PooTwo.Start[1] = WaterTreatmentStart;
        PooTwo.Start[2] = WaterTreatmentEnd - DrainageLength;
        PooTwo.End[0] = UpperSewerStart + LengthOfUpperSewer;
        PooTwo.End[1] = WaterTreatmentEnd - DrainageLength - 1;
        PooTwo.End[2] = WaterTreatmentEnd;

        PooTwo.Poo[0]=PooTwo.Start[0];
        PooTwo.active = true;
        PipeFour.PooPath = true;
      }
      else if(PooThree.active != true){
        PooThree.Start[0] = UpperSewerStart + PipeFourSewer;
        PooThree.Start[1] = WaterTreatmentStart;
        PooThree.Start[2] = WaterTreatmentEnd - DrainageLength;
        PooThree.End[0] = UpperSewerStart + LengthOfUpperSewer;
        PooThree.End[1] = WaterTreatmentEnd - DrainageLength - 1;
        PooThree.End[2] = WaterTreatmentEnd;

        PooThree.Poo[0]=PooThree.Start[0];
        PooThree.active = true;
        PipeFour.PooPath = true;
      }
      else if(PooFour.active != true){
        PooFour.Start[0] = UpperSewerStart + PipeFourSewer;
        PooFour.Start[1] = WaterTreatmentStart;
        PooFour.Start[2] = WaterTreatmentEnd - DrainageLength;
        PooFour.End[0] = UpperSewerStart + LengthOfUpperSewer;
        PooFour.End[1] = WaterTreatmentEnd - DrainageLength - 1;
        PooFour.End[2] = WaterTreatmentEnd;

        PooFour.Poo[0]=PooFour.Start[0];
        PooFour.active = true;
        PipeFour.PooPath = true;
      }
    }


    if(PooOne.active == true){PooOneTravel();}
    if(PooTwo.active == true){PooTwoTravel();}
    if(PooThree.active == true){PooThreeTravel();}
    if(PooFour.active == true){PooFourTravel();}

    if(PooOne.active == true && PooTwo.active == true && PooThree.active == true && PooFour.active == true){allActive = true;}
    else {allActive = false;}

    PooOneSpeed.LastTriggered=CurrentTime;
  }

}

void PooOneTravel() {
    for(int x = 0; x < StreakLength; x++){
      if(PooOne.Poo[x] != 0 && PooOne.Poo[x] != PooOne.End[PooOne.CurrentPath]){strip.setPixelColor(PooOne.Poo[x],StreakHue[x],StreakHue[x],0);}
    }

    for(int x = StreakLength; x > 0; x--){PooOne.Poo[x] = PooOne.Poo[x-1];}

    if(StormLevel ==3 && (PooOne.Poo[0] == PooOne.End[1] +1 || PooOne.Poo[0] == PooOne.End[1] -1)){
      PooOne.Start[2] = OceanDumpEnd + DrainageLength;
      PooOne.End[2] = OceanDumpEnd;
    }
    

    if(PooOne.Start[PooOne.CurrentPath+1] != 0){
      if(PooOne.Poo[0] < PooOne.End[PooOne.CurrentPath]){PooOne.Poo[0]++;}
      else if(PooOne.Poo[0] > PooOne.End[PooOne.CurrentPath]){PooOne.Poo[0]--;}
      if(PooOne.Poo[0] == PooOne.End[PooOne.CurrentPath]){
        PooOne.CurrentPath++;
        PooOne.Poo[0] =PooOne.Start[PooOne.CurrentPath];
      }
    }
    else{
      if(PooOne.Poo[0] < PooOne.End[PooOne.CurrentPath]){PooOne.Poo[0]++;}
      else if(PooOne.Poo[0] > PooOne.End[PooOne.CurrentPath]){PooOne.Poo[0]--;}
      if(PooOne.Poo[StreakLength-1] == PooOne.End[PooOne.CurrentPath]){
        for(int x = 0; x < StreakLength; x++){PooOne.End[x] = 0;}
        for(int x = 0; x < StreakLength; x++){PooOne.Start[x] = 0;}
        for(int x = 0; x < StreakLength; x++){PooOne.Poo[x] = 0;}
        PooOne.CurrentPath = 0;
        PooOne.active = false;
      }
    }


    // if(PooOne.End[PooOne.CurrentPath] == 0 || PooOne.CurrentPath == 4){
    //   for(int x = 0; x < StreakLength; x++){PooOne.End[x] = 0;}
    //   for(int x = 0; x < StreakLength; x++){PooOne.Start[x] = 0;}
    //   for(int x = 0; x < StreakLength; x++){PooOne.Poo[x] = 0;}
    //   PooOne.CurrentPath = 0;
    //   PooOne.active = false;
    // }
}

void PooTwoTravel() {
    for(int x = 0; x < StreakLength; x++){
      if(PooTwo.Poo[x] != 0 && PooTwo.Poo[x] != PooTwo.End[PooTwo.CurrentPath]){strip.setPixelColor(PooTwo.Poo[x],StreakHue[x],StreakHue[x],0);}
    }

    for(int x = StreakLength; x > 0; x--){PooTwo.Poo[x] = PooTwo.Poo[x-1];}

    if(StormLevel ==3 && (PooTwo.Poo[0] == PooTwo.End[1] +1 || PooTwo.Poo[0] == PooTwo.End[1] -1)){
      PooTwo.Start[2] = OceanDumpEnd + DrainageLength;
      PooTwo.End[2] = OceanDumpEnd;
    }
    
    if(PooTwo.Start[PooTwo.CurrentPath +1] != 0){
      if(PooTwo.Poo[0] < PooTwo.End[PooTwo.CurrentPath]){PooTwo.Poo[0]++;}
      else if(PooTwo.Poo[0] > PooTwo.End[PooTwo.CurrentPath]){PooTwo.Poo[0]--;}
      if(PooTwo.Poo[0] == PooTwo.End[PooTwo.CurrentPath]){
        PooTwo.CurrentPath++;
        PooTwo.Poo[0] =PooTwo.Start[PooTwo.CurrentPath];
      }
    }
    else{
      if(PooTwo.Poo[0] < PooTwo.End[PooTwo.CurrentPath]){PooTwo.Poo[0]++;}
      else if(PooTwo.Poo[0] > PooTwo.End[PooTwo.CurrentPath]){PooTwo.Poo[0]--;}
      if(PooTwo.Poo[StreakLength -1] == PooTwo.End[PooTwo.CurrentPath]){
        for(int x = 0; x < StreakLength; x++){PooTwo.End[x] = 0;}
        for(int x = 0; x < StreakLength; x++){PooTwo.Start[x] = 0;}
        for(int x = 0; x < StreakLength; x++){PooTwo.Poo[x] = 0;}
        PooTwo.CurrentPath = 0;
        PooTwo.active = false;
      }
    }


    // if(PooTwo.End[PooTwo.CurrentPath] == 0 || PooTwo.CurrentPath == 4){
    //   for(int x = 0; x < StreakLength; x++){PooTwo.End[x] = 0;}
    //   for(int x = 0; x < StreakLength; x++){PooTwo.Start[x] = 0;}
    //   for(int x = 0; x < StreakLength; x++){PooTwo.Poo[x] = 0;}
    //   PooTwo.CurrentPath = 0;
    //   PooTwo.active = false;
    // }
}

void PooThreeTravel() {
    for(int x = 0; x < StreakLength; x++){
      if(PooThree.Poo[x] != 0 && PooThree.Poo[x] != PooThree.End[PooThree.CurrentPath]){strip.setPixelColor(PooThree.Poo[x],StreakHue[x],StreakHue[x],0);}
    }

    for(int x = StreakLength; x > 0; x--){PooThree.Poo[x] = PooThree.Poo[x-1];}

    if(StormLevel ==3 && (PooThree.Poo[0] == PooThree.End[1] +1 || PooThree.Poo[0] == PooThree.End[1] -1)){
      PooThree.Start[2] = OceanDumpEnd + DrainageLength;
      PooThree.End[2] = OceanDumpEnd;
    }
    

    if(PooThree.Start[PooThree.CurrentPath +1] != 0){
      if(PooThree.Poo[0] < PooThree.End[PooThree.CurrentPath]){PooThree.Poo[0]++;}
      else if(PooThree.Poo[0] > PooThree.End[PooThree.CurrentPath]){PooThree.Poo[0]--;}
      if(PooThree.Poo[0] == PooThree.End[PooThree.CurrentPath]){
        PooThree.CurrentPath++;
        PooThree.Poo[0] =PooThree.Start[PooThree.CurrentPath];
      }
    }
    else{
      if(PooThree.Poo[0] < PooThree.End[PooThree.CurrentPath]){PooThree.Poo[0]++;}
      else if(PooThree.Poo[0] > PooThree.End[PooThree.CurrentPath]){PooThree.Poo[0]--;}
      if(PooThree.Poo[StreakLength -1] == PooThree.End[PooThree.CurrentPath]){
        for(int x = 0; x < StreakLength; x++){PooThree.End[x] = 0;}
        for(int x = 0; x < StreakLength; x++){PooThree.Start[x] = 0;}
        for(int x = 0; x < StreakLength; x++){PooThree.Poo[x] = 0;}
        PooThree.CurrentPath = 0;
        PooThree.active = false;
      }
    }


    // if(PooThree.End[PooThree.CurrentPath] == 0 || PooThree.CurrentPath == 4){
    //   for(int x = 0; x < StreakLength; x++){PooThree.End[x] = 0;}
    //   for(int x = 0; x < StreakLength; x++){PooThree.Start[x] = 0;}
    //   for(int x = 0; x < StreakLength; x++){PooThree.Poo[x] = 0;}
    //   PooThree.CurrentPath = 0;
    //   PooThree.active = false;
    // }
}

void PooFourTravel() {
    for(int x = 0; x < StreakLength; x++){
      if(PooFour.Poo[x] != 0 && PooFour.Poo[x] != PooFour.End[PooFour.CurrentPath]){strip.setPixelColor(PooFour.Poo[x],StreakHue[x],StreakHue[x],0);}
    }

    for(int x = StreakLength; x > 0; x--){PooFour.Poo[x] = PooFour.Poo[x-1];}

    if(StormLevel ==3 && (PooFour.Poo[0] == PooFour.End[1] +1 || PooFour.Poo[0] == PooFour.End[1] -1)){
      PooFour.Start[2] = OceanDumpEnd + DrainageLength;
      PooFour.End[2] = OceanDumpEnd;
    }
    
    if(PooFour.Start[PooFour.CurrentPath +1] != 0){
      if(PooFour.Poo[0] < PooFour.End[PooFour.CurrentPath]){PooFour.Poo[0]++;}
      else if(PooFour.Poo[0] > PooFour.End[PooFour.CurrentPath]){PooFour.Poo[0]--;}
      if(PooFour.Poo[0] == PooFour.End[PooFour.CurrentPath]){
        PooFour.CurrentPath++;
        PooFour.Poo[0] =PooFour.Start[PooFour.CurrentPath];
      }
    }
    else{
      if(PooFour.Poo[0] < PooFour.End[PooFour.CurrentPath]){PooFour.Poo[0]++;}
      else if(PooFour.Poo[0] > PooFour.End[PooFour.CurrentPath]){PooFour.Poo[0]--;}
      if(PooFour.Poo[StreakLength -1] == PooFour.End[PooFour.CurrentPath]){
        for(int x = 0; x < StreakLength; x++){PooFour.End[x] = 0;}
        for(int x = 0; x < StreakLength; x++){PooFour.Start[x] = 0;}
        for(int x = 0; x < StreakLength; x++){PooFour.Poo[x] = 0;}
        PooFour.CurrentPath = 0;
        PooFour.active = false;
      }
    }

    // if(PooFour.End[PooFour.CurrentPath] == 0 || PooFour.CurrentPath == 4){
    //   for(int x = 0; x < StreakLength; x++){PooFour.End[x] = 0;}
    //   for(int x = 0; x < StreakLength; x++){PooFour.Start[x] = 0;}
    //   for(int x = 0; x < StreakLength; x++){PooFour.Poo[x] = 0;}
    //   PooFour.CurrentPath = 0;
    //   PooFour.active = false;
    // }
}

void loop() {
  CurrentTime = millis();
  Rain();
  //Poo();

  if(CurrentTime  >= RainFrame.Duration + RainFrame.LastTriggered){
    RainGutters();
    RainFrame.LastTriggered = CurrentTime;
  }

  if(CurrentTime  >= SewersFrame.Duration + SewersFrame.LastTriggered){
    //UpperSewers();
    LowerSewers();

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

    SewersLastPixel = SewerWaveHue[Length-1];
    BlueSewersLastPixel = BlueSewerWaveHue[Length-1];
    GreenSewersLastPixel = GreenSewerWaveHue[Length-1];

    for(int x = Length; x >= 0; x--){
      if(x!=0){
        SewerWaveHue[x]=SewerWaveHue[x-1];
        BlueSewerWaveHue[x]=BlueSewerWaveHue[x-1];
        GreenSewerWaveHue[x]=GreenSewerWaveHue[x-1];
      } 
      else {
        SewerWaveHue[x]=SewersLastPixel;
        BlueSewerWaveHue[x]=BlueSewersLastPixel;
        GreenSewerWaveHue[x]=GreenSewersLastPixel;
      }
    }
    SewersFrame.LastTriggered = CurrentTime;
  }

  if(CurrentTime  >= PipesFrame.Duration + PipesFrame.LastTriggered){
    //Toilet();
    //BathTub();
    //Shower();
    //Sink();

    LastPixel = WaveHue[Length-1];
    for(int x = Length; x >= 0; x--){
      if(x!=0){WaveHue[x]=WaveHue[x-1];} 
      else {WaveHue[x]=LastPixel;}
    }
    PipesFrame.LastTriggered = CurrentTime;
  }

  strip.show();
}
