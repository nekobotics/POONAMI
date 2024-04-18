#include <Adafruit_NeoPixel.h>

#define Input_1 13 //
#define Input_2 12 //
#define Input_3 11 //
#define Input_4 10 //

#define Rain_Input 9 //

#define Rain_Output1 23 //
#define Rain_Output2 27 //
#define Rain_Output3 25 //

#define Pixel_Pin 22 //
#define Num_Pixels 234
#define BRIGHTNESS 255

Adafruit_NeoPixel strip(Num_Pixels, Pixel_Pin, NEO_GRBW + NEO_KHZ800);

//checks to make sure that each cord triggers once
bool PullOne = false;
bool PullTwo = false;
bool PullThree = false;
bool PullFour = false;

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


//PipeSounds trigger
bool ToiletSound = false;
bool ShowerSound = false;
bool SinkSound = false;
bool WasherSound= false;

//Gutter Sizes and Variables
const int GutterLength = 3;
int GutterIntensity = 0;
int GutterLead = GutterLength;

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

ShiftingColor PipeHue;
ShiftingColor GutterHue;

struct StaticColor {
  int RedHue[Length];
  int GreenHue[Length];
  int BlueHue[Length];
};

StaticColor PipeLeadHue;

int StreakHue[StreakLength];

struct PipePaths{
  int PipeStart[3];
  int PipeLength[3];
  int Trigger;
  bool active;
};

PipePaths Toilet = {{0,86,169}, {18,24,29}, Input_1};
PipePaths Washer = {{48,141,0}, {11,16,0}, Input_2};
PipePaths Shower = {{60,111,199}, {25,16,34}, Input_3};
PipePaths Sink = {{19,128,158}, {28,12,10}, Input_4};

const int NumPipes = 4;

//Pipes with 
struct Pipes{
  bool CordRelease;
  bool begin;
  bool active;
  int CurrentPipe;
  int Trigger;
  int PipeStart[3];
  int PipeLength[3];
  int Lead[3];
  int Lag[3];
  Time PipeWait = {0,50};

  void Setup(int Pipe){
    if(Pipe == 1){
      CurrentPipe = Pipe;
      Trigger = Toilet.Trigger;
      Toilet.active = true;
      for(int x= 0; x < 3; x++){
        PipeStart[x] = Toilet.PipeStart[x];
        PipeLength[x] = Toilet.PipeLength[x];
      }
    }

    else if(Pipe == 2){
      CurrentPipe = Pipe;
      Trigger = Washer.Trigger;
      Washer.active = true;
      for(int x= 0; x < 3; x++){
        PipeStart[x] = Washer.PipeStart[x];
        PipeLength[x] = Washer.PipeLength[x];
      }
    }

    else if(Pipe == 3){
      CurrentPipe = Pipe;
      Trigger = Shower.Trigger;
      Shower.active = true;
      for(int x= 0; x < 3; x++){
        PipeStart[x] = Shower.PipeStart[x];
        PipeLength[x] = Shower.PipeLength[x];
      }
    }

    else if(Pipe == 4){
      CurrentPipe = Pipe;
      Trigger = Sink.Trigger;
      Sink.active = true;
      for(int x= 0; x < 3; x++){
        PipeStart[x] = Sink.PipeStart[x];
        PipeLength[x] = Sink.PipeLength[x];
      }
    }

    active = true;
  }
  
  void Flow(){
    for(int x = 0; x < 3; x++){
      if(PipeLength[x] != 0){
        for(int i = 0; i < Lead[x]; i++){
          if(i > Lead[x] - Length && i <= PipeLength[x]){strip.setPixelColor(PipeStart[x] + i, 0,PipeLeadHue.GreenHue[Length-(Lead[x] -i)],PipeLeadHue.BlueHue[Length-(Lead[x] -i)]);}
          else if(i <= PipeLength[x]){strip.setPixelColor(PipeStart[x] + i, 0, PipeHue.GreenHue[i - (Length * (i/Length))] + 10, PipeHue.BlueHue[i - (Length * (i/Length))] + 10);}
        }
        
        if(Lead[x] != PipeLength[x] + Length){Lead[x]++;}
      }
    }
  }

  void Drain(){
    for(int x = 0; x < 3; x++){
      if(PipeLength[x] != 0){
        //Lead Head
        if(Lead[x] < PipeLength[x] + Length){
          for(int i = 0; i < Length; i++){
            if(Lead[x] - i <= PipeLength[x] && Lead[x] - i >= 0){strip.setPixelColor(PipeStart[x] + (Lead[x] - i), 0,PipeLeadHue.GreenHue[Length-i-1],PipeLeadHue.BlueHue[Length-i-1]);}
          }
          Lead[x]++;
        }

        //Adds Flow
        for(int i = Lag[x]; i < Lead[x]; i++){
          if(i <= PipeLength[x]){strip.setPixelColor(PipeStart[x] + i, 0, PipeHue.GreenHue[i - (Length * (i/Length))] + 10, PipeHue.BlueHue[i - (Length * (i/Length))] + 10);}
        }

        //Lag tail
        for(int i = 0; i < Length; i++){
          if(Lag[x] - i < 0){break;}
          else if (Lag[x] - i <= PipeLength[x]){strip.setPixelColor(PipeStart[x] + (Lag[x]-i), 0,PipeLeadHue.GreenHue[i], PipeLeadHue.GreenHue[i]);}
        }


        if(Lead[x] > Length && Lag[x] - Length != PipeLength[x]){Lag[x]++;}
        
        if (Lag[x] - Length == PipeLength[x]){
          Lag[x] = 0;
          Lead[x] = 0;
          PipeLength[x] = 0;
          PipeStart[x] = 0; 
          // memset(Lag,0,sizeof(Lag));
          // memset(Lead,0,sizeof(Lead));
          // memset(PipeLength,0,sizeof(PipeLength));
          // memset(PipeStart,0,sizeof(PipeStart));
        }

        if(Lead[0] + Lead[1] + Lead[2] == 0){
          CordRelease = false;
          active = false;
          begin = false;
        }


        if(Lag[0] > Length){
          if(CurrentPipe == 1){
            Toilet.active = false;
          }
          else if (CurrentPipe == 2){
            Washer.active = false;
          }
          else if (CurrentPipe == 3){
            Shower.active = false;
          }
          else if (CurrentPipe == 4){
            Sink.active = false;
          }
        }
      }
    }
  }

  void Run(){
    if(begin == false && CordRelease == false){
      Serial.println(CurrentPipe);
      PipeWait.LastTriggered = CurrentTime;
      begin = true;
    }

    if(CurrentTime >= PipeWait.Duration + PipeWait.LastTriggered && CurrentTime >= PipesFrame.Duration + PipesFrame.LastTriggered){
      if(digitalRead(Trigger) == HIGH && CordRelease == false){
        Flow();
      }

      else{
        if(CordRelease == false){
          CordRelease = true;
          if(CurrentPipe == 3){Serial.println(5);}
        }
        Drain();
      }
    }
  }
};

Pipes Pipes[NumPipes];

void WaveUpdate(){
  for(int x=0; x < Length; x++){
    PipeHue.BlueHue[x] = (200/2)-((200/2) * sin(x * (3.14/Length)));
    PipeHue.GreenHue[x] = (200/2)-((200/2) * sin(x * (3.14/Length)));

    PipeLeadHue.GreenHue[x] = (200/2)+((200/2) * cos(x * (3.14/Length)));
    PipeLeadHue.BlueHue[x] = (200/2)+((200/2) * cos(x * (3.14/Length)));
  }

  PipeLeadHue.GreenHue[Length-1] = 0;
  PipeLeadHue.BlueHue[Length-1] = 0; 


  for(int x=0; x < StreakLength; x++){
    StreakHue[x] = (200/2)+((200/2)*cos(x*(3.14/StreakLength)));
  }
  StreakHue[StreakLength - 1] = 0;
}

void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.show();
  strip.setBrightness(BRIGHTNESS);
  Serial.begin(9600);

  pinMode(Input_1,INPUT);
  pinMode(Input_2,INPUT);
  pinMode(Input_3,INPUT);
  pinMode(Input_4,INPUT);

  WaveUpdate();
}

void PipesRun(){
  if(digitalRead(Input_1) == HIGH && PullOne == false && Toilet.active == false){
    PullOne = true;
    for(int x = 0; x < NumPipes; x++){
      if(Pipes[x].active == false){
        Pipes[x].Setup(1);
        break;
      }
    }
  }
  else if(digitalRead(Input_1) == LOW && PullOne == true){PullOne = false;}


  if(digitalRead(Input_2) == HIGH && PullTwo == false && Washer.active == false){
    PullTwo = true;
    for(int x = 0; x < NumPipes; x++){
      if(Pipes[x].active == false){
        Pipes[x].Setup(2);
        break;
      }
    }
  }
  else if(digitalRead(Input_2) == LOW && PullTwo == true){PullTwo = false;}


  if(digitalRead(Input_3) == HIGH && PullThree == false && Shower.active == false){
    PullThree = true;
    for(int x = 0; x < NumPipes; x++){
      if(Pipes[x].active == false){
        Pipes[x].Setup(3);
        break;
      }
    }
  }
  else if(digitalRead(Input_3) == LOW && PullThree == true){PullThree = false;}


  if(digitalRead(Input_4) == HIGH && PullFour == false && Sink.active == false){
    PullFour = true;
    for(int x = 0; x < NumPipes; x++){
      if(Pipes[x].active == false){
        Pipes[x].Setup(4);
        break;
      }
    }
  }
  else if(digitalRead(Input_4) == LOW && PullFour == true){PullFour = false;}

  if(CurrentTime >= PipesFrame.Duration + PipesFrame.LastTriggered){
    // if(Pipes[0].active == true){Pipes[0].Run();}
    // if(Pipes[1].active == true){Pipes[1].Run();}
    // if(Pipes[2].active == true){Pipes[2].Run();}
    // if(Pipes[3].active == true){Pipes[3].Run();}

    for(int x=0; x < NumPipes; x++){
      if(Pipes[x].active == true){Pipes[x].Run();}
    }
 }
}

void loop() {
  // put your main code here, to run repeatedly:
  CurrentTime = millis();
  PipesRun();
  // if(Pipes[0].active == false && digitalRead(Input_3) == HIGH){Pipes[0].Setup(3);}
  // else{Pipes[0].Run();}

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
