#include <Adafruit_NeoPixel.h>

#define Output_1 2
#define Output_2 3
#define Output_3 4
#define Output_4 5

#define Input_1 10
#define Input_2 11
#define Input_3 12
#define Input_4 0

#define Rain_Input 8

#define Rain_Output1 22
#define Rain_Output2 23
#define Rain_Output3 24

#define Pixel_Pin 9
#define Num_Pixels 261
#define BRIGHTNESS 200

int Runoff = 0;

bool GutterBegin = false;
const int GutterLength = 3;

int StormLevel = 0;
int ProjectedStormLevel = 0;
bool RainPush = false;

const int StreakLength = 5;

struct PixelControl {
  bool run;
  bool start;
  int Stream;
  int Pixel;
  const int End;
  const int Begin;
};

PixelControl PipeOne = {false, false,0,0,9,0};
PixelControl PipeTwo = {false, false,10,10,27,10};
PixelControl PipeThree = {false, false,28,28,40,28};
PixelControl PipeFour = {false, false,41,41,69,41};

struct Time {
  unsigned long LastTriggered;
  long Duration;
};

Time PipesFrame = {0,5};
Time BathWait = {0,50};
Time ToiletWait = BathWait;
Time ShowerWait = BathWait;
Time SinkWait = BathWait;
Time RainFrame = {0,10};
Time RainWait = {0,500};
Time SewersFrame = {0,10};
Time RunoffWait = {0,6};
Time ProjectedSpeed = {0,10};
Time GutterSpeedWait = RainWait;
Time SewersSpeedWait = RainWait;
Time RainHold = {0,5000};
Time PooOneSpeed = {0,11};

// struct GutterPixel{
//   int Start;
//   int Pixel;
//   int Last;
// };

// GutterPixel GutterOne = {0,0,0};
// GutterPixel GutterTwo = {1,0,0};
// GutterPixel GutterThree = {2,0,0};
// GutterPixel GutterFour = {3,0,0};

  const int PipeOneSewer = 11;
  const int PipeTwoSewer = 17;
  const int PipeThreeSewer = 24;
  const int PipeFourSewer = 6;
  // const int PipeSewerDropSize = 2;

  const int UpperSewerStart = 70;
  const int UpperSewerEnd = 143;
  const int LengthOfUpperSewer = 30;

  const int LowerSewersStart = 144;
  const int LowerSewersEnd = Num_Pixels - 12;
  const int LowerSewersLength = 45;
  const int DrainageRegulator = LowerSewersLength - 14;

const int Length = 20;

int WaveHue[Length];
int SewerWaveHue[Length];
int GutterHue[Length];
int ColorHue[Length];
int RunoffHue[Length];
int StreakHue[StreakLength];
int LastPixel;
int SewersLastPixel;
int LastGutterPixel;

struct PooDrop{
  bool active;
  int Poo[StreakLength];
  int Start[5];
  int End[5];
  int CurrentPath;
};

PooDrop PooOne = {false,{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},0};

int GutterPixel = 0;
int GutterLag = 0;

unsigned long CurrentTime;
bool RainOn = false;

Adafruit_NeoPixel strip(Num_Pixels, Pixel_Pin, NEO_GRBW + NEO_KHZ800);

void WaveUpdate(){
  for(int x=0; x < Length; x++){
    ColorHue[x]= (150/2)+((150/2) * cos(x * (3.14/Length)));
    RunoffHue[x]= (150/2)+((150/2) * sin(x *(3.14/Length)-1.57));
    WaveHue[x]= (150/2)-((150/2) * sin(x * (3.14/Length)));
    SewerWaveHue[x]= (150/2)-((150/2) * sin(x * (3.14/Length)));
    GutterHue[x]= (30/2)+((30/2)*cos(x*(6.26/Length)));
  }

  for(int x=0; x < StreakLength; x++){
    StreakHue[x] = (150/2)+((150/2)*cos(x*(3.14/StreakLength)));
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
        PipeOne.start = true;
      }
      else if(CurrentTime >= ToiletWait.LastTriggered + ToiletWait.Duration){
        for(int x = 0; x < PipeOne.Stream-PipeOne.Begin; x++){strip.setPixelColor(PipeOne.Begin + x, 0, WaveHue[x -(Length*(x/Length))], 150);}
        if(PipeOne.Stream <= PipeOne.End){PipeOne.Stream++;}
      }
    }

    else if(PipeOne.start == true && CurrentTime >= ToiletWait.LastTriggered + ToiletWait.Duration){
      PipeOne.run = true;

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
        PipeOne.run = false;
        PipeOne.start = false;
        PipeOne.Stream = PipeOne.Begin;
        PipeOne.Pixel = PipeOne.Begin;
        digitalWrite(Output_1,LOW);
      }
    }
  }
}

void BathTub(){
  if(CurrentTime >= PipesFrame.LastTriggered + PipesFrame.Duration){
    if(digitalRead(Input_2) == HIGH && PipeTwo.run == false){
      digitalWrite(Output_2,HIGH);

      if(PipeTwo.start == false){
        BathWait.LastTriggered = CurrentTime;
        PipeTwo.start = true;
      }
      else if(CurrentTime >= BathWait.LastTriggered + BathWait.Duration){
        for(int x = 0; x < PipeTwo.Stream-PipeTwo.Begin; x++){strip.setPixelColor(PipeTwo.Begin + x, 0, WaveHue[x -(Length*(x/Length))], 150);}
        if(PipeTwo.Stream <= PipeTwo.End){PipeTwo.Stream++;}
      }
    }

    else if(PipeTwo.start == true && CurrentTime >= BathWait.LastTriggered + BathWait.Duration){
      PipeTwo.run = true;

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
        PipeTwo.run = false;
        PipeTwo.start = false;
        PipeTwo.Stream = PipeTwo.Begin;
        PipeTwo.Pixel = PipeTwo.Begin;
        digitalWrite(Output_2,LOW);
      }
    }
  } 
}

void Shower(){
  if(CurrentTime >= PipesFrame.LastTriggered + PipesFrame.Duration){
    if(digitalRead(Input_3) == HIGH && PipeThree.run == false){
      digitalWrite(Output_3,HIGH);

      if(PipeThree.start == false){
        ShowerWait.LastTriggered = CurrentTime;
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
        PipeThree.run = false;
        PipeThree.start = false;
        PipeThree.Stream = PipeThree.Begin;
        PipeThree.Pixel = PipeThree.Begin;
        digitalWrite(Output_3,LOW);
      }
    }
  }
}

void Sink(){
  if(CurrentTime >= PipesFrame.LastTriggered + PipesFrame.Duration){
    if(analogRead(Input_4) >= 530 && PipeFour.run == false){
      digitalWrite(Output_4,HIGH);

      if(PipeFour.start == false){
        SinkWait.LastTriggered = CurrentTime;
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
        PipeFour.run = false;
        PipeFour.start = false;
        PipeFour.Stream = PipeFour.Begin;
        PipeFour.Pixel = PipeFour.Begin;
        digitalWrite(Output_4,LOW);
      }
    }
  }
}

void Rain(){
  if(digitalRead(Rain_Input)== HIGH && RainPush == false){
    RainWait.LastTriggered = CurrentTime;
    if(ProjectedStormLevel<3){ProjectedStormLevel++;}
    RainPush = true;
  }
  else if(digitalRead(Rain_Input)==LOW){RainPush = false;}

  

  if(ProjectedStormLevel == 1 || (ProjectedStormLevel < StormLevel && StormLevel == 1)){
    digitalWrite(Rain_Output1, HIGH);
    digitalWrite(Rain_Output2, LOW);
    digitalWrite(Rain_Output3, LOW);
    ProjectedSpeed.Duration = 11;
    // RainFrame.Duration = 10;
    // SewersFrame.Duration = 10;
  }
  else if(ProjectedStormLevel == 2 || (ProjectedStormLevel < StormLevel && StormLevel == 2)){
    digitalWrite(Rain_Output1, HIGH);
    digitalWrite(Rain_Output2, HIGH);
    digitalWrite(Rain_Output3, LOW);
    ProjectedSpeed.Duration = 9;
    // RainFrame.Duration = 9;
    // SewersFrame.Duration = 9;

  }
  else if (ProjectedStormLevel== 3 || (ProjectedStormLevel < StormLevel && StormLevel == 3)){
    digitalWrite(Rain_Output1, HIGH);
    digitalWrite(Rain_Output2, HIGH);
    digitalWrite(Rain_Output3, HIGH);
    ProjectedSpeed.Duration = 7;
    // RainFrame.Duration = 7;
    // SewersFrame.Duration = 7;

  }
  else {
    digitalWrite(Rain_Output1, LOW);
    digitalWrite(Rain_Output2, LOW);
    digitalWrite(Rain_Output3, LOW);
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
  }


  if(StormLevel < ProjectedStormLevel){
    if(CurrentTime >= RainWait.LastTriggered + RainWait.Duration){
      StormLevel++;
      RainWait.LastTriggered = CurrentTime;
    }
  }
  else if(StormLevel > ProjectedStormLevel){
    if(CurrentTime >= RainWait.LastTriggered + RainWait.Duration && CurrentTime >= RainHold.LastTriggered + RainHold.Duration){
      StormLevel--;
      RainWait.LastTriggered = CurrentTime;
    }
  }
  else if(StormLevel == ProjectedStormLevel){
    RainHold.LastTriggered = CurrentTime;
    ProjectedStormLevel = 0;
  }

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
  if(CurrentTime >= SewersFrame.LastTriggered + SewersFrame.Duration){
    for(int x = 0; x < LengthOfUpperSewer; x++){
      strip.setPixelColor(x+UpperSewerStart,0,(StormLevel+1)*(SewerWaveHue[x-(Length*(x/Length))]/3)+15,20+(StormLevel*40));
      strip.setPixelColor(UpperSewerEnd-x,0,(StormLevel+1)*(SewerWaveHue[x-(Length*(x/Length))]/3)+15,20+(StormLevel*40));
  }

  }
}

void LowerSewers(){
 if(CurrentTime  >= SewersFrame.Duration + SewersFrame.LastTriggered){
   if(StormLevel < 3){
      for(int x = 0; x < LowerSewersLength; x++){
        strip.setPixelColor((LowerSewersStart+LowerSewersLength)-x,0,(StormLevel+1)*(SewerWaveHue[x-(Length*(x/Length))]/3)+15,20+(StormLevel*40));
        if(x <(Runoff-Length)+(DrainageRegulator) || x < DrainageRegulator){strip.setPixelColor((LowerSewersEnd-LowerSewersLength)+x,0,(StormLevel+1)*(SewerWaveHue[x-(Length*(x/Length))]/3)+15,20+(StormLevel*40));}
        else{strip.setPixelColor((LowerSewersEnd-LowerSewersLength)+x,0,0,0);}
        
        if(Runoff + Length > 0){
          if(CurrentTime >= RunoffWait.LastTriggered + RunoffWait.Duration){
            for(int x = 0; x < Length; x++){
              if(Runoff - x < 0){break;}
              else if(Runoff - x <= LowerSewersLength - DrainageRegulator){strip.setPixelColor(LowerSewersEnd-(Runoff - x),0,RunoffHue[x],RunoffHue[x]);}
            }
            Runoff--;
            RunoffWait.LastTriggered = CurrentTime;
          }
        }
      }
    }
    else if(StormLevel == 3){
      for(int x = 0; x < LowerSewersLength; x++){
        strip.setPixelColor((LowerSewersStart+LowerSewersLength)-x,0,(StormLevel+1)*(SewerWaveHue[x-(Length*(x/Length))]/3)+15,20+(StormLevel*40));
        if(x < (Runoff-Length)+(DrainageRegulator))strip.setPixelColor((LowerSewersEnd-LowerSewersLength)+x,0,(StormLevel+1)*(SewerWaveHue[x-(Length*(x/Length))]/3)+15,20+(StormLevel*40));
        
        if(Runoff <= (LowerSewersLength-DrainageRegulator) + Length){
          if(CurrentTime >= RunoffWait.LastTriggered + RunoffWait.Duration){
            for(int x = 0; x < Length; x++){
              if(Runoff - x < 0){break;}
              else if (Runoff - x <= LowerSewersLength-DrainageRegulator){strip.setPixelColor(((LowerSewersEnd-LowerSewersLength)+DrainageRegulator + (Runoff - x)), 0, RunoffHue[x], RunoffHue[x]);}
            }
            Runoff++;
            RunoffWait.LastTriggered = CurrentTime;
          }
        }
      }
    }

  }
}

void Poo(){
  if(CurrentTime >= PooOneSpeed.LastTriggered + PooOneSpeed.Duration){
    if(PipeOne.Stream >= PipeOne.End){
      if(PooOne.active != true){
        PooOne.Start[0] = UpperSewerEnd - PipeOneSewer;
        PooOne.Start[1] = (LowerSewersStart + LowerSewersLength);
        PooOne.Start[2] = (LowerSewersStart+(LowerSewersLength - DrainageRegulator));
        PooOne.End[0] = UpperSewerEnd - LengthOfUpperSewer;
        PooOne.End[1] = (LowerSewersStart+(LowerSewersLength - DrainageRegulator));
        PooOne.End[2] = LowerSewersStart;
        
          if(StormLevel ==3){
            PooOne.Start[2] = (LowerSewersEnd -(LowerSewersLength - DrainageRegulator));
            PooOne.End[2] = LowerSewersEnd;
          }


        PooOne.Poo[0]=PooOne.Start[0];
        PooOne.active = true;
      }
    }

    PooOneTravel();
    PooOneSpeed.LastTriggered=CurrentTime;
    PooOneSpeed.Duration = SewersFrame.Duration;

  }

}

void PooOneTravel() {
  if(CurrentTime >= PooOneSpeed.LastTriggered + PooOneSpeed.Duration){
    for(int x = 0; x < StreakLength; x++){
      if(PooOne.Poo[x] != 0){strip.setPixelColor(PooOne.Poo[x],StreakHue[x],0,0);}
    }

    for(int x = StreakLength; x > 0; x--){PooOne.Poo[x] = PooOne.Poo[x-1];}

    if(PooOne.Poo[0] < PooOne.End[PooOne.CurrentPath]){PooOne.Poo[0]++;}
    else if(PooOne.Poo[0] > PooOne.End[PooOne.CurrentPath]){PooOne.Poo[0]--;}
    if(PooOne.Poo[0] == PooOne.End[PooOne.CurrentPath]){
      PooOne.CurrentPath++;
      PooOne.Poo[0] =PooOne.Start[PooOne.CurrentPath];
    }
    
    if(PooOne.End[PooOne.CurrentPath] == 0 || PooOne.CurrentPath == 4){
      for(int x = 0; x < StreakLength; x++){PooOne.End[x] = 0;}
      for(int x = 0; x < StreakLength; x++){PooOne.Start[x] = 0;}
      for(int x = 0; x < StreakLength; x++){PooOne.Poo[x] = 0;}
      PooOne.CurrentPath = 0;
      PooOne.active = false;
    }
  }
}

void loop() {
  CurrentTime = millis();
  Rain();
  Toilet();
  BathTub();
  Shower();
  Sink();
  UpperSewers();
  LowerSewers();
  Poo();

  if(CurrentTime  >= RainFrame.Duration + RainFrame.LastTriggered){
    RainGutters();
    RainFrame.LastTriggered = CurrentTime;
  }

  if(CurrentTime  >= SewersFrame.Duration + SewersFrame.LastTriggered){
    SewersLastPixel = SewerWaveHue[Length-1];
    for(int x = Length; x >= 0; x--){
      if(x!=0){SewerWaveHue[x]=SewerWaveHue[x-1];} 
      else {SewerWaveHue[x]=SewersLastPixel;}
    }

    SewersFrame.LastTriggered = CurrentTime;
  }

  if(CurrentTime  >= PipesFrame.Duration + PipesFrame.LastTriggered){
    LastPixel = WaveHue[Length-1];
    for(int x = Length; x >= 0; x--){
      if(x!=0){WaveHue[x]=WaveHue[x-1];} 
      else {WaveHue[x]=LastPixel;}
    }
    PipesFrame.LastTriggered = CurrentTime;
  }

  strip.show();
}
