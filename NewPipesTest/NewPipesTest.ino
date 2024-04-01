#include <Adafruit_NeoPixel.h>

#define Output_1 7 //
#define Output_2 6 //
#define Output_3 5 //
#define Output_4 4 //

#define Input_1 13 //
#define Input_2 12 //
#define Input_3 11 //
#define Input_4 10 //

#define Pixel_Pin 22 //
#define Num_Pixels 261
#define BRIGHTNESS 255

Adafruit_NeoPixel strip(Num_Pixels, Pixel_Pin, NEO_GRBW + NEO_KHZ800);

bool PullOne;
bool PullTwo;
bool PullThree;
bool PullFour;

const int Length = 20;
const int StreakLength = 5;

unsigned long CurrentTime;
struct Time {
  unsigned long LastTriggered;
  long Duration;
};

Time PipesFrame = {0,5};


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

struct StaticColor {
  int RedHue[Length];
  int GreenHue[Length];
  int BlueHue[Length];
};

StaticColor PipeLeadHue;

int StreakHue[StreakLength];

struct Pipe {
  const int Start;
  const int Length;
  bool active;
};

Pipe Toilet = {0,9,false};
Pipe Washer = {10,17,false};
Pipe Shower = {28,12, false};
Pipe Sink = {41,28,false};

struct PipeFlow {
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
  }
  
  void Flow(){
    if(sound == false){
      if(PipeStart == Toilet.Start){Serial.println(1);}
      else if(PipeStart == Washer.Start){Serial.println(2);}
      else if(PipeStart == Shower.Start){Serial.println(3);}
      else if(PipeStart == Sink.Start){Serial.println(4);}

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
      DrainPixel = 0;
      StreamPixel = 0;
      PooPixel = 0;
      PipeStart = 0;
      PipeLength = 0;
      Trigger = 0;
      digitalWrite(Apartment, LOW);
    }

    if(DrainPixel > Length){ // Turn off Active Pipe
      if(PipeStart == Toilet.Start){Toilet.active = false;}
      else if(PipeStart == Washer.Start){Washer.active = false;}
      else if(PipeStart == Shower.Start){Shower.active = false;}
      else if(PipeStart == Sink.Start){Sink.active = false;}

      if(sound == true){
        if(PipeStart == Shower.Start){Serial.println(5);}
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
    else{SewersBegin = true;}
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

void WaveUpdate(){
  for(int x=0; x < Length; x++){
    PipeHue.BlueHue[x] = (200/2)-((200/2) * sin(x * (3.14/Length)));
    PipeHue.GreenHue[x] = (200/2)-((200/2) * sin(x * (3.14/Length)));

    PipeLeadHue.GreenHue[x] = (200/2)+((200/2) * cos(x * (3.14/Length)));
    PipeLeadHue.BlueHue[x] = (200/2)+((200/2) * cos(x * (3.14/Length)));
    //Serial.println(PipeLeadHue.GreenHue[x]);
  }

  for(int x=0; x < StreakLength; x++){
    StreakHue[x] = (200/2)+((200/2)*cos(x*(3.14/StreakLength)));
  }
  PipeLeadHue.GreenHue[Length-1] = 0;
  PipeLeadHue.BlueHue[Length-1] = 0; 
}

void setup() {
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
  pinMode(Input_4,INPUT);

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

      PullFour = true;
    }
    else if (digitalRead(Input_4) == LOW && PullFour == true){PullFour = false;}

    if(PipeOne.active == true){PipeOne.run();}
    if(PipeTwo.active == true){PipeTwo.run();}
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  CurrentTime = millis();
  PipeControl();

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
