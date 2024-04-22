#include <Adafruit_NeoPixel.h>

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
#define Num_Pixels 395
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
//RainTiming
Time RainFrame = {0,10};
Time RainWait = {0,500};
Time ProjectedSpeed = {0,10};
Time RainHold = {0,1000};
//SewersTiming
Time SewersFrame = {0,10};
Time StageOneStorm = {0,2000};
Time StageTwoStorm = {0,2000 + StageOneStorm.Duration};
Time StageThreeStorm = {0,4000 + StageTwoStorm.Duration};

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
ShiftingColor SewerWaveHue;
ShiftingColor FlowOutHue; 


struct StaticColor {
  int RedHue[Length];
  int GreenHue[Length];
  int BlueHue[Length];
};

StaticColor PipeLeadHue;
StaticColor RunoffLeadHue;
StaticColor RunoffLeadHueTest;

int StreakHue[StreakLength];

//Upper Sewer Lengths:
  const int LeftUpperSewerStart = 264;
  const int LeftUpperSewerEnd = 293;
  const int RightUpperSewerStart = 234;
  const int RightUpperSewerEnd = 263;
//

//Lower Sewer Lengths:
  const int OceanDrainStart = 292;
  const int OceanDrainEnd = 336;
  const int WaterTreatmentStart = 337;
  const int WaterTreatmentEnd = 381;
  int SewerCombineLength = 17;//Stage 1
  int CentralSewerLength = 29;//Stage 2
  int SewerOutLength = 46; //Stage 3

  int StageTwoStart = WaterTreatmentStart + SewerCombineLength + 1;
  int StageTwoEnd = WaterTreatmentStart + CentralSewerLength;
  int StageTwoStormStart = OceanDrainStart + SewerCombineLength + 1;
  int StageTwoStormEnd = OceanDrainStart + CentralSewerLength;

  int StageThreeStart = WaterTreatmentStart + CentralSewerLength + 1;
  int StageThreeEnd = WaterTreatmentEnd;
  int StageThreeStormStart = OceanDrainStart + CentralSewerLength + 1;
  int StageThreeStormEnd = OceanDrainEnd;

  const int WarningLength = 6;
  int WarningIntensity;
  bool StormStart = false;
  double FlowOutIntensity = 0;
//

struct PooPaths{
  int Start[4];
  int End[4];
  int PipeType;
};

PooPaths ToiletPoo[3] = {
  {{LeftUpperSewerStart + 4, OceanDrainStart, StageTwoStart, StageThreeStart},{LeftUpperSewerEnd, OceanDrainStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},1},
  {{LeftUpperSewerStart + 25, OceanDrainStart, StageTwoStart, StageThreeStart},{LeftUpperSewerEnd, OceanDrainStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},1},
  {{RightUpperSewerStart + 9, WaterTreatmentStart, StageTwoStart, StageThreeStart},{RightUpperSewerEnd, WaterTreatmentStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},1}
};
PooPaths WasherPoo[2] = {
  {{LeftUpperSewerStart + 12, OceanDrainStart, StageTwoStart, StageThreeStart},{LeftUpperSewerEnd, OceanDrainStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},2},
  {{RightUpperSewerStart + 26, WaterTreatmentStart, StageTwoStart, StageThreeStart},{RightUpperSewerEnd, WaterTreatmentStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},2}
};
PooPaths ShowerPoo[3] = {
  {{LeftUpperSewerStart + 24, OceanDrainStart, StageTwoStart, StageThreeStart},{LeftUpperSewerEnd, OceanDrainStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},3},
  {{LeftUpperSewerStart + 26, OceanDrainStart, StageTwoStart, StageThreeStart},{LeftUpperSewerEnd, OceanDrainStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},3},
  {{RightUpperSewerStart + 2, WaterTreatmentStart, StageTwoStart, StageThreeStart},{RightUpperSewerEnd, WaterTreatmentStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},3}
};
PooPaths SinkPoo[3] = {
  {{LeftUpperSewerStart + 5, OceanDrainStart, StageTwoStart, StageThreeStart},{LeftUpperSewerEnd, OceanDrainStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},4},
  {{RightUpperSewerStart + 27, WaterTreatmentStart, StageTwoStart, StageThreeStart},{RightUpperSewerEnd, WaterTreatmentStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},4},
  {{RightUpperSewerStart + 14, WaterTreatmentStart, StageTwoStart, StageThreeStart} ,{RightUpperSewerEnd, WaterTreatmentStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},4}
};

const int NumPoos = 11;

struct PooControls{
  int Poo[StreakLength];
  int Start;
  int End;
  int CurrentPath;
  int CurrentPoo;
  int CurrentPooType;
  bool active;

  void Update(int PooType, int PooNumber,int PathNumber){
    active = true;
    CurrentPoo = PooNumber;
    CurrentPooType = PooType;

    if(PooType == 1){
      Start = ToiletPoo[CurrentPoo].Start[CurrentPath];
      End = ToiletPoo[CurrentPoo].End[CurrentPath];
      Poo[0] = Start;
    }

    else if(PooType == 2){
      Start = WasherPoo[CurrentPoo].Start[CurrentPath];
      End = WasherPoo[CurrentPoo].End[CurrentPath];
      Poo[0] = Start;
    }

    else if(PooType == 3){
      Start = ShowerPoo[CurrentPoo].Start[CurrentPath];
      End = ShowerPoo[CurrentPoo].End[CurrentPath];
      Poo[0] = Start;
    }
    
    else if(PooType == 4){
      Start = SinkPoo[CurrentPoo].Start[CurrentPath];
      End = SinkPoo[CurrentPoo].End[CurrentPath];
      Poo[0] = Start;
    }
  }

  void Run(){
    if(CurrentTime >= SewersFrame.LastTriggered + SewersFrame.Duration){
      for(int i=0; i < StreakLength; i++){strip.setPixelColor(Poo[i],StreakHue[i],StreakHue[i],0);}

      for(int i = StreakLength - 1; i > 0; i--){Poo[i] = Poo[i-1];}
      
      if(Poo[0] < End){Poo[0]++;}
      else{
        if(CurrentPath < 3){
          CurrentPath++;
          Update(CurrentPooType,CurrentPoo, CurrentPath);
        }

        else if(CurrentPath == 3 && Poo[StreakLength-1] == End){
          active = false;
          CurrentPath = 0;
          memset(Poo,0,sizeof(Poo));
          Start = 0;
          End = 0;
        }
      }
    }
  }
};

PooControls Poo[NumPoos];

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
  bool PooTriggered[3];
  int CurrentPipe;
  int Trigger;
  int PipeStart[3];
  int PipeLength[3];
  int Lead[3];
  int Lag[3];
  int PooPath[3][StreakLength];
  Time PipeWait = {0,50};
  Time PooWait = {0,60};


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
          for(int i = 0; i < StreakLength; i++){PooPath[x][i]=0;}

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

  void PooFlow(){
    for(int x = 0; x < 3; x++){
      if(PooTriggered[x] == false && PipeLength[x] != 0){
        for(int i = 0; i < StreakLength; i++){strip.setPixelColor(PipeStart[x]+PooPath[x][i],StreakHue[i],StreakHue[i],0);}
        if(PooPath[x][0] != PipeLength[x]){PooPath[x][0]++;}
        else if(PooPath[x][StreakLength - 1] == PipeLength[x]){
          for(int i = 0; i < NumPoos; i++){
            if(Poo[i].active == false){
              Poo[i].Update(CurrentPipe,x,2);
              break;
            }
          }
        }
        for(int i = StreakLength - 1; i > 0; i--){PooPath[x][i] = PooPath[x][i-1];}
      }
    }
  }

  void Run(){
    if(begin == false && CordRelease == false){
      Serial.println(CurrentPipe);
      PipeWait.LastTriggered = CurrentTime;
      PooWait.LastTriggered = CurrentTime;
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
    if(CurrentTime >= PooWait.Duration + PooWait.LastTriggered){
      PooFlow();
    }
  }
};

Pipes Pipes[NumPipes];


//Lower Sewers Runoff Variables
int Runoff = 0;
int RunoffLead = 0;
bool RunoffDrain = false;
float RunoffFade = 0;
bool RunoffStart = false;

// Rain Variables
// bool RainOn = false;
// int StormLevel = 0;
// int ProjectedStormLevel = 0;
// bool RainPush = false;
// bool Raining = false;

// struct PooPaths{
//   int Start[5];
//   int End[5];
//   int PipeType;
// };

// PooPaths ToiletPoo[3] = {
//   {{Toilet.PipeStart[0], LeftUpperSewerStart + 4, OceanDrainStart, StageTwoStart, StageThreeStart},{Toilet.PipeStart[0] + Toilet.PipeLength[0], LeftUpperSewerEnd, OceanDrainStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},1},
//   {{Toilet.PipeStart[1], LeftUpperSewerStart + 25, OceanDrainStart, StageTwoStart, StageThreeStart},{Toilet.PipeStart[1] + Toilet.PipeLength[1], LeftUpperSewerEnd, OceanDrainStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},1},
//   {{Toilet.PipeStart[2], RightUpperSewerStart + 9, WaterTreatmentStart, StageTwoStart, StageThreeStart},{Toilet.PipeStart[2] + Toilet.PipeLength[2], RightUpperSewerEnd, WaterTreatmentStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},1}
// };
// PooPaths WasherPoo[2] = {
//   {{Washer.PipeStart[0], LeftUpperSewerStart + 12, OceanDrainStart, StageTwoStart, StageThreeStart},{Washer.PipeStart[0] + Washer.PipeLength[0], LeftUpperSewerEnd, OceanDrainStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},2},
//   {{Washer.PipeStart[1], RightUpperSewerStart + 26, WaterTreatmentStart, StageTwoStart, StageThreeStart},{Washer.PipeStart[1] + Washer.PipeLength[1], RightUpperSewerEnd, WaterTreatmentStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},2}
// };
// PooPaths ShowerPoo[3] = {
//   {{Shower.PipeStart[0], LeftUpperSewerStart + 24, OceanDrainStart, StageTwoStart, StageThreeStart},{Shower.PipeStart[0] + Shower.PipeLength[0], LeftUpperSewerEnd, OceanDrainStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},3},
//   {{Shower.PipeStart[1], LeftUpperSewerStart + 26, OceanDrainStart, StageTwoStart, StageThreeStart},{Shower.PipeStart[1] + Shower.PipeLength[1], LeftUpperSewerEnd, OceanDrainStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},3},
//   {{Shower.PipeStart[2], RightUpperSewerStart + 2, WaterTreatmentStart, StageTwoStart, StageThreeStart},{Shower.PipeStart[2] + Shower.PipeLength[2], RightUpperSewerEnd, WaterTreatmentStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},3}
// };
// PooPaths SinkPoo[3] = {
//   {{Sink.PipeStart[0], LeftUpperSewerStart + 5, OceanDrainStart, StageTwoStart, StageThreeStart},{Sink.PipeStart[0] + Sink.PipeLength[0], LeftUpperSewerEnd, OceanDrainStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},4},
//   {{Sink.PipeStart[1], RightUpperSewerStart + 27, WaterTreatmentStart, StageTwoStart, StageThreeStart},{Sink.PipeStart[1] + Sink.PipeLength[1], RightUpperSewerEnd, WaterTreatmentStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},4},
//   {{Sink.PipeStart[2], RightUpperSewerStart + 14, WaterTreatmentStart, StageTwoStart, StageThreeStart} ,{Sink.PipeStart[2] + Sink.PipeLength[2], RightUpperSewerEnd, WaterTreatmentStart + SewerCombineLength, StageTwoEnd, StageThreeEnd},4}
// };

//const int NumPoos = 4;

// struct PooControls{
//   int Poo[3][StreakLength];
//   int Start[3][5];
//   int End[3][5];
//   int CurrentPath[3];
//   bool active[3];
//   bool Running;
//   bool begin;
//   Time PooWait = {0,600};
//   Time PooFrame[3];
//   int CurrentPoo;

//   void Setup(int PooType){
//     Running = true;
//     CurrentPoo = PooType;

//     if(PooType == 1){
//       for(int x = 0; x < 3; x++){
//         for(int i = 0; i < 5; i++){
//           Start[x][i] = ToiletPoo[x].Start[i];
//           End[x][i] = ToiletPoo[x].End[i];
//         }
//         active[x] = true;
//         Poo[x][0] = Start[x][0];
//       }
//     }

//     else if(PooType == 2){
//       for(int x = 0; x < 2; x++){
//         for(int i = 0; i < 5; i++){
//           Start[x][i] = WasherPoo[x].Start[i];
//           End[x][i] = WasherPoo[x].End[i];
//         }
//         active[x] = true;
//         Poo[x][0] = Start[x][0];
//       }
//     }

//     else if(PooType == 3){
//       for(int x = 0; x < 3; x++){
//         for(int i = 0; i < 5; i++){
//           Start[x][i] = ShowerPoo[x].Start[i];
//           End[x][i] = ShowerPoo[x].End[i];
//         }
//         active[x] = true;
//         Poo[x][0] = Start[x][0];
//       }
//     }
    
//     else if(PooType == 4){
//       for(int x = 0; x < 3; x++){
//         for(int i = 0; i < 5; i++){
//           Start[x][i] = SinkPoo[x].Start[i];
//           End[x][i] = SinkPoo[x].End[i];
//         }
//         active[x] = true;
//         Poo[x][0] = Start[x][0];
//       }
//     }

//   }

//   // void update(int CurrentPoo, int CurrentPhase){
//   //   if(CurrentPoo == 1){
//   //     if(End[CurrentPoo][CurrentPhase] != ToiletPoo[CurrentPoo].End[CurrentPhase]){
//   //       for(int x = 0; x < 5; x++){End[CurrentPoo][x] = ToiletPoo[CurrentPoo].End[x];}
//   //     }
//   //   }
//   //   else if(CurrentPoo == 2){
//   //     if(End[CurrentPoo][CurrentPhase] != WasherPoo[CurrentPoo].End[CurrentPhase]){
//   //       for(int x = 0; x < 5; x++){End[CurrentPoo][x] = WasherPoo[CurrentPoo].End[x];}
//   //     }
//   //   }
//   //   else if(CurrentPoo == 3){
//   //     if(End[CurrentPoo][CurrentPhase] != ShowerPoo[CurrentPoo].End[CurrentPhase]){
//   //       for(int x = 0; x < 5; x++){End[CurrentPoo][x] = ShowerPoo[CurrentPoo].End[x];}
//   //     }
//   //   }
//   //   else if(CurrentPoo == 4){
//   //     if(End[CurrentPoo][CurrentPhase] != SinkPoo[CurrentPoo].End[CurrentPhase]){
//   //       for(int x = 0; x < 5; x++){End[CurrentPoo][x] = SinkPoo[CurrentPoo].End[x];}
//   //     }
//   //   }
//   // }

//   void Flow(){
//     for(int x = 0; x < 3; x++){
//       if(CurrentPath[x] == 0){PooFrame[x].Duration = PipesFrame.Duration;}
//       else{PooFrame[x].Duration = SewersFrame.Duration;}

//       if(active[x] == true && CurrentTime >= PooFrame[x].LastTriggered + PooFrame[x].Duration){

//         for(int i=0; i < StreakLength; i++){
//           strip.setPixelColor(Poo[x][i],StreakHue[i],StreakHue[i],0);
//         }

//         for(int i = StreakLength - 1; i > 0; i--){
//           Poo[x][i] = Poo[x][i-1];
//         }
        
//         if(Poo[x][0] < End[x][CurrentPath[x]]){
//           Poo[x][0]++;
//         }
//         else{
//           if(CurrentPath[x] < 4){
//             CurrentPath[x]++;
//             Poo[x][0] = Start[x][CurrentPath[x]];
//           }

//           if(CurrentPath[x] == 4 && Poo[x][StreakLength-1] == End[x][4]){
//             active[x] = false;
//             CurrentPath[x] = 0;
//             for (int i = 0; i < StreakLength; i++){Poo[x][i] = 0;}
//             for(int i = 0; i < 5; i++){
//               Start[x][i] = 0;
//               End[x][i] = 0;
//               }
//           }
//         }
//       }
//     }
    

//     if(Poo[0][0] + Poo[1][0] + Poo[2][0] == 0){
//       Running = false;
//       begin = false;
//     }
//   }

//   void Run(){
//     if(begin == false){
//       PooWait.LastTriggered = CurrentTime;
//       begin = true;
//     }

//     if(CurrentTime >= PooWait.LastTriggered + PooWait.Duration){
//       Flow();
//     }

//   }
// };

// struct PooControls{
//   int Poo[StreakLength];
//   int Start;
//   int End;
//   int CurrentPath;
//   int CurrentPoo;
//   bool active;

//   void Update(int PooNumber, PathNumber){
//     active = true;
//     CurrentPoo = PooNumber;

//     if(PooType == 1){
//       Start = ToiletPoo[CurrentPoo].Start[CurrentPath];
//       End = ToiletPoo[CurrentPoo].End[CurrentPath];
//       Poo[0] = Start;
//     }

//     else if(PooType == 2){
//       Start = WasherPoo[CurrentPoo].Start[CurrentPath];
//       End = WasherPoo[CurrentPoo].End[CurrentPath];
//       Poo[0] = Start;
//     }

//     else if(PooType == 3){
//       Start = ShowerPoo[CurrentPoo].Start[CurrentPath];
//       End = ShowerPoo[CurrentPoo].End[CurrentPath];
//       Poo[0] = Start;
//     }
    
//     else if(PooType == 4){
//       Start = SinkPoo[CurrentPoo].Start[CurrentPath];
//       End = SinkPoo[CurrentPoo].End[CurrentPath];
//       Poo[0] = Start;
//     }
//   }

//   void Run(){
//     for(int i=0; i < StreakLength; i++){strip.setPixelColor(Poo[i],StreakHue[i],StreakHue[i],0);}

//     for(int i = StreakLength - 1; i > 0; i--){Poo[i] = Poo[i-1];}
    
//     if(Poo[0] < End){Poo[0]++;}
//     else{
//       if(CurrentPath < 4){
//         CurrentPath++;
//         Update(CurrentPoo, CurrentPath);
//       }

//       else if(CurrentPath[x] == 4 && Poo[StreakLength-1] == End){
//         active[x] = false;
//         CurrentPath[x] = 0;
//         for (int i = 0; i < StreakLength; i++){Poo[x][i] = 0;}
//         for(int i = 0; i < 5; i++){
//           Start[x][i] = 0;
//           End[x][i] = 0;
//           }
//       }
//     }
//   }
// };
//PooControls Poo[NumPoos];


//______________________________________________________________________//
void WaveUpdate(){
  for(int x=0; x < Length; x++){
    PipeHue.BlueHue[x] = (200/2)-((200/2) * sin(x * (3.14/Length)));
    PipeHue.GreenHue[x] = (200/2)-((200/2) * sin(x * (3.14/Length)));

    SewerWaveHue.GreenHue[x]= (100/2)-((100/2) * sin(x * (3.14/Length)));
    SewerWaveHue.BlueHue[x]= (200/2)-((200/2) * sin(x * (3.14/Length)));

    PipeLeadHue.GreenHue[x] = (200/2)+((200/2) * cos(x * (3.14/Length)));
    PipeLeadHue.BlueHue[x] = (200/2)+((200/2) * cos(x * (3.14/Length)));

    FlowOutHue.BlueHue[x] = (100)-(100 * sin(x * (3.14/Length)));
    FlowOutHue.GreenHue[x] = (50)-(50 * sin(x * (3.14/Length)));
    FlowOutHue.WhiteHue[x] = (20/2)-((20/2) * cos(x*(3.14/Length))); 
  }

  FlowOutHue.WhiteHue[0]=0;
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

    // for(int x = 0; x < NumPoos; x++){
    //   if(Poo[x].Running == false){
    //     Poo[x].Setup(1);
    //     break;
    //   }
    // }
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

    // for(int x = 0; x < NumPoos; x++){
    //   if(Poo[x].Running == false){
    //     Poo[x].Setup(2);
    //     break;
    //   }
    // }
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

    // for(int x = 0; x < NumPoos; x++){
    //   if(Poo[x].Running == false){
    //     Poo[x].Setup(3);
    //     break;
    //   }
    // }
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

    // for(int x = 0; x < NumPoos; x++){
    //   if(Poo[x].Running == false){
    //     Poo[x].Setup(4);
    //     break;
    //   }
    // }
  }
  else if(digitalRead(Input_4) == LOW && PullFour == true){PullFour = false;}

  if(CurrentTime >= PipesFrame.Duration + PipesFrame.LastTriggered){
    for(int x=0; x < NumPipes; x++){
      if(Pipes[x].active == true){Pipes[x].Run();}
    }
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

void Sewers(){
  if(CurrentTime >= SewersFrame.Duration + SewersFrame.LastTriggered){
    for(int x = RightUpperSewerStart; x < LeftUpperSewerEnd; x++){
      strip.setPixelColor(x,0,SewerWaveHue.GreenHue[x-(Length*(x/Length))]+10,SewerWaveHue.BlueHue[x-(Length*(x/Length))]+10);
    }

    for(int x = WaterTreatmentStart; x < WaterTreatmentEnd; x++){strip.setPixelColor(x,0,SewerWaveHue.GreenHue[x-(Length*(x/Length))]+10,10+SewerWaveHue.BlueHue[x-(Length*(x/Length))]);}

    if(StormLevel < 3){
      for(int x = OceanDrainStart; x < OceanDrainEnd; x++){
        if(x < StageTwoStormStart){strip.setPixelColor(x,0,SewerWaveHue.GreenHue[x-(Length*(x/Length))]+10,10+SewerWaveHue.BlueHue[x-(Length*(x/Length))]);}
        else if(x > StageTwoStormStart && x < StageTwoStormEnd && StormStart == true){
          if(FlowOutIntensity > 0 && Runoff == RunoffLead){FlowOutIntensity = FlowOutIntensity - .005;}
          else if (FlowOutIntensity == 0){StormStart = false;}
          strip.setPixelColor(x,0,(FlowOutHue.GreenHue[x-(Length*(x/Length))]+10) * FlowOutIntensity, (10+FlowOutHue.BlueHue[x-(Length*(x/Length))])*FlowOutIntensity);
        }
        else if(x >= StageThreeStormStart + RunoffLead && Runoff > 0){strip.setPixelColor(x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))]);}
      }

      if(Runoff > 0){
        for(int x = 0; x < Length; x++){
          if(RunoffLead - x < 0){break;}
          else if (RunoffLead - x < StageThreeStormEnd - StageThreeStormStart){strip.setPixelColor(StageThreeStormStart + (RunoffLead - x),0,RunoffLeadHue.GreenHue[x],RunoffLeadHue.BlueHue[x]);}
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

      for(int x = OceanDrainStart; x < OceanDrainEnd; x++){ // Continual Flow Animations
        if(x < StageTwoStormStart){strip.setPixelColor(x,0,SewerWaveHue.GreenHue[x-(Length*(x/Length))]+10,10+SewerWaveHue.BlueHue[x-(Length*(x/Length))]);}
        else if(CurrentTime <= StageOneStorm.Duration + StageOneStorm.LastTriggered && x < StageTwoStormEnd){
          if(FlowOutIntensity < 1){FlowOutIntensity = FlowOutIntensity + .005;}
          strip.setPixelColor(x,0,(FlowOutHue.GreenHue[x-(Length*(x/Length))]+10) * FlowOutIntensity, (10+FlowOutHue.BlueHue[x-(Length*(x/Length))])*FlowOutIntensity);
        }
        else if(CurrentTime <= StageTwoStorm.Duration + StageTwoStorm.LastTriggered && x < StageTwoStormEnd){
          if(x> StageTwoStormEnd - WarningLength && x <= StageTwoStormEnd){strip.setPixelColor(x,0,0,0,FlowOutHue.WhiteHue[0]);}//Flash
          else{strip.setPixelColor(x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))]);}//Normal
        }
       else if(CurrentTime <= StageThreeStorm.Duration + StageThreeStorm.LastTriggered && x < StageThreeStormStart + Runoff){
          if(x> StageTwoStormEnd - WarningLength && x <= StageTwoStormEnd && Runoff < StageThreeStormEnd - StageThreeStormStart){strip.setPixelColor(x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))],FlowOutHue.WhiteHue[0]);}//Flash
          else{strip.setPixelColor(x,0,FlowOutHue.GreenHue[x-(Length*(x/Length))]+10,10+FlowOutHue.BlueHue[x-(Length*(x/Length))]);}
        }
      }

      if(CurrentTime <= StageOneStorm.Duration + StageOneStorm.LastTriggered) {// Stage 1: Fade In [o] ... it's good as is, but could be better and smoother
        //if(FlowOutIntensity < 1){FlowOutIntensity = FlowOutIntensity + .01;}
        //FlowOutUpdate();
      }
      else if(CurrentTime <= StageThreeStorm.Duration + StageThreeStorm.LastTriggered){ // Stage 3: FlowOut[o] ... it works
        for(int i = 0; i < Length; i++){
          RunoffLead = 0;
          if(Runoff - i < 0){break;}
          else if(Runoff - i < StageThreeStormEnd - StageThreeStormStart){strip.setPixelColor(StageThreeStormStart + (Runoff - i),0,RunoffLeadHue.GreenHue[Length - 1 - i],RunoffLeadHue.BlueHue[Length - 1 - i]);} 
        }
        if(Runoff - Length < StageThreeStormEnd - StageThreeStormStart){Runoff++;}
      }
    }

    for(int x=0; x < NumPoos; x++){
      if(Poo[x].active == true){
        Poo[x].Run();
      }
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  CurrentTime = millis();

  if(CurrentTime  >= SewersFrame.Duration + SewersFrame.LastTriggered){
    Sewers();
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
  }

  if(CurrentTime >= PipesFrame.LastTriggered + PipesFrame.Duration){
    PipesRun();

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
