#include <Adafruit_DotStar.h>
#include <SPI.h>

#define DATAPIN 8
#define CLOCKPIN 9
#define NUMPIXELS  296

Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);

const int streakLength = 20;
const int waveLength = 60;
unsigned long CurrentTime;

int PathStarts[4][3] = {{156,155,0},{192,79,0},{293,244,6},{293,6,0}};
int PathsEnd[4][3] = {{189,0,0},{223,0,0},{294,225,0},{225,0,0}};

bool PooStage[8][4] = {
  {true,false,false,false},
  {false,true,false,false},
  {false,false,true,false},
  {false,false,false,true},
  {true,true,false,false},
  {false,true,true,false},
  {false,false,true,true},
  {true,true,true,true}
};

int currentStage;
bool newStage = true;

struct Time{
  unsigned long LastTriggered;
  int Delay;
};
Time PipesFrame = {8,16};
Time PooFrame = {0,16};

struct WaveColor{
  int Red[waveLength];
  int Green[waveLength];
  int Blue[waveLength];
  int LowRed;
  int LowGreen;
  int LowBlue;
  int LastRed;
  int LastGreen;
  int LastBlue;

  void setup(int MaxRed, int MinRed, int MaxGreen, int MinGreen, int MaxBlue, int MinBlue){
    for(int x =0; x < waveLength; x++){
      Red[x] = ((MaxRed-MinRed)/2) - (((MaxRed-MinRed)/2)*sin(x * (3.14/waveLength)));
      Green[x] = ((MaxGreen-MinGreen)/2) - (((MaxGreen-MinGreen)/2)*sin(x * (3.14/waveLength)));
      Blue[x] = ((MaxBlue-MinBlue)/2) - (((MaxBlue-MinBlue)/2)*sin(x * (3.14/waveLength)));
    }
    LowRed = MinRed;
    LowGreen = MinGreen;
    LowBlue = MinBlue;
  }

  void update(){
    LastRed = Red[waveLength - 1];
    LastGreen = Green[waveLength - 1];
    LastBlue = Blue[waveLength - 1];

    for(int x = waveLength -1; x >= 0;x--){
      if(x==0){
        Red[x]=LastRed;
        Blue[x]=LastBlue;
        Green[x]=LastGreen;
      }
      else{
        Red[x]=Red[x-1];
        Blue[x]=Blue[x-1];
        Green[x]=Green[x-1];
      }
    }
  }
};
WaveColor PipeFlow;


struct StreakControl{
  int Red[streakLength];
  int Green[streakLength];
  int Blue[streakLength];

  int PathStart[3];
  int PathEnd[3];
  int streak[streakLength];
  int CurrentPath;
  int Pin;
  bool active;

  void Setup(int MaxRed, int MaxGreen, int MaxBlue, int PinNum, int PathNum){
    for(int x =0; x < streakLength; x++){
      Red[x] = ((MaxRed)/2) + (((MaxRed)/2)*cos(x * (3.14/streakLength)));
      Green[x] = ((MaxGreen)/2) + (((MaxGreen)/2)*cos(x * (3.14/streakLength)));
      Blue[x] = ((MaxBlue)/2) + (((MaxBlue)/2)*cos(x * (3.14/streakLength)));
    }
    Red[streakLength - 1] = 0;
    Green[streakLength - 1] = 0;
    Blue[streakLength - 1] = 0;

    pinMode(PinNum,OUTPUT);
    Pin = PinNum;

    for(int x=0; x < 3; x++){
      PathStart[x] = PathStarts[PathNum][x];
      PathEnd[x] = PathsEnd[PathNum][x];
    }
    streak[0] = PathStart[0];
  }

  void run(){
    digitalWrite(Pin,HIGH);

    for(int x = streakLength-1; x >= 0  ; x--){
      if(streak[x] > 0){strip.setPixelColor(streak[x],Red[x],Green[x],Blue[x]);}
      if(x!=0){streak[x] = streak[x-1];}
    }
    
    if(streak[0] > PathEnd[CurrentPath]){streak[0]--;}
    else if (streak[0] < PathEnd[CurrentPath]){streak[0]++;}
    else if(streak[0] == PathEnd[CurrentPath]){
      if((PathEnd[CurrentPath+1] == 0 || PathEnd[CurrentPath] == 0) && streak[streakLength-1] == PathEnd[CurrentPath]){
        digitalWrite(Pin, LOW);
        CurrentPath = 0;
        for(int x=1; x < streakLength; x++){streak[x]=0;}
        streak[0] = PathStart[0];
        //Serial.println("end");
        active = false;
      }
      else if(PathEnd[CurrentPath+1] != 0 || CurrentPath != 2){
        CurrentPath++;
        streak[0] = PathStart[CurrentPath];
      }
    }

  }
};

StreakControl Poo[4];
int running = 0;

// 0 = Toilet
// 1 = Washing Machine
// 2 = Dish Wahser
// 3 = Shower

void setup() {
  Poo[0].Setup(20,200,20, 2, 0);
  Poo[1].Setup(225, 225,20, 3, 1);
  Poo[2].Setup(200, 20, 20, 22, 2);
  Poo[3].Setup(200,20,242, 23, 3);

  Serial.begin(9600);

  PipeFlow.setup(0, 0, 255, 55, 255, 55);
  strip.begin();
  strip.setBrightness(255);
  strip.show();
}

void runPipes(){
  for(int x=0; x < 155; x++){strip.setPixelColor(155-x,PipeFlow.Red[x - (waveLength * (x/waveLength))] + PipeFlow.LowRed, PipeFlow.Green[x - (waveLength * (x/waveLength))]+PipeFlow.LowGreen, PipeFlow.Blue[x - (waveLength * (x/waveLength))]+PipeFlow.LowBlue);}
  for(int x= 156; x < 224; x++){strip.setPixelColor(x,PipeFlow.Red[x - (waveLength * (x/waveLength))] + PipeFlow.LowRed, PipeFlow.Green[x - (waveLength * (x/waveLength))]+PipeFlow.LowGreen, PipeFlow.Blue[x - (waveLength * (x/waveLength))]+PipeFlow.LowBlue);}
  for(int x = 0; x < NUMPIXELS-225; x++){strip.setPixelColor(NUMPIXELS-x,PipeFlow.Red[x - (waveLength * (x/waveLength))] + PipeFlow.LowRed, PipeFlow.Green[x - (waveLength * (x/waveLength))]+PipeFlow.LowGreen, PipeFlow.Blue[x - (waveLength * (x/waveLength))]+PipeFlow.LowBlue);}
  PipeFlow.update();
}

void runPoo(){
  if(newStage == true){
    if(PooStage[currentStage][0] == true){Poo[0].active = true;}
    if(PooStage[currentStage][1] == true){Poo[1].active = true;}
    if(PooStage[currentStage][2] == true){Poo[2].active = true;}
    if(PooStage[currentStage][3] == true){Poo[3].active = true;}
    newStage = false;
  }

  for(int x=0; x < 4; x++){
    if(Poo[x].active == true){
      Poo[x].run();
    }
  }
  if(Poo[0].active == false && Poo[1].active == false && Poo[2].active == false && Poo[3].active == false){
    newStage = true;
    if(currentStage < 7){currentStage++;}
    else{currentStage = 0;}
  }
}

void loop() {
  CurrentTime = millis();

  if(CurrentTime >= PipesFrame.LastTriggered + PipesFrame.Delay){
    runPipes();
    PipesFrame.LastTriggered = CurrentTime;
  }
  
  if(CurrentTime >= PooFrame.LastTriggered + PooFrame.Delay){
    runPoo();
    PooFrame.LastTriggered = CurrentTime;
  }

  strip.show();
}
