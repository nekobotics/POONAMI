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

const int GutterLength = 3;

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

struct PixelControl {
  bool run;
  bool start;
  int Stream;
  int Pixel;
  const int End;
  const int Begin;
  int Delay;
};

PixelControl PipeOne = {false, false,0,0,9,0,0};
PixelControl PipeTwo = {false, false,10,10,27,10,0};
PixelControl PipeThree = {false, false,28,28,40,28,0};
PixelControl PipeFour = {false, false,41,41,69,41,0};

struct GutterPixel{
  int Start;
  int Pixel;
  int Last;
};

GutterPixel GutterOne = {0,0,0};
GutterPixel GutterTwo = {1,0,0};
GutterPixel GutterThree = {2,0,0};
GutterPixel GutterFour = {3,0,0};

const int Length = 20;

int WaveHue[Length];
int GutterHue[Length];
int ColorHue[Length];
int LastPixel;
int LastGutterPixel;

unsigned long CurrentTime;
bool RainOn = false;

Adafruit_NeoPixel strip(Num_Pixels, Pixel_Pin, NEO_GRBW + NEO_KHZ800);

void WaveUpdate(){
  for(int x=0; x < Length; x++){
    ColorHue[x]= (150/2)+((150/2) * cos(x * (3.14/Length)));
  }

  for(int x=0; x < Length; x++){
    WaveHue[x]= (150/2)-((150/2) * sin(x * (3.14/Length)));
  }

  for(int x=0; x < Length; x++){
    GutterHue[x]= (150/2)-((150/2)*sin(x*(3.14/Length)+1.54));
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

void BathTub(){
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

void Shower(){
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

void Sink(){
  if(digitalRead(Input_4) == HIGH && PipeFour.run == false){
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
      digitalWrite(Output_2,LOW);
    }
  }
}

void Rain(){
  
}

void Gutters(){
  for(int i=0; i < 12; i++){strip.setPixelColor(Num_Pixels - i,0,100, 100);}

  // LastGutterPixel = GutterHue[Length-1];
  // for(int x = Length; x >= 0; x--){
  //   if(x!=0){GutterHue[x]=GutterHue[x-1];} 
  //   else {GutterHue[x]=LastPixel;}
  // }
}

void loop() {
  // put your main code here, to run repeatedly:
  CurrentTime = millis();
  
  // if(CurrentTime  >= PipesFrame.Duration + PipesFrame.LastTriggered){
  //   if(digitalRead(Input_1) == HIGH || PipeOne.start == true){Toilet();}
  //   if(digitalRead(Input_2) == HIGH || PipeTwo.start == true){BathTub();}
  //   if(digitalRead(Input_3) == HIGH || PipeThree.start == true){Shower();}
  //   if(digitalRead(Input_4) == HIGH || PipeFour.start == true){Sink();}
  //   LastPixel = WaveHue[Length-1];
  //   for(int x = Length; x >= 0; x--){
  //     if(x!=0){WaveHue[x]=WaveHue[x-1];} 
  //     else {WaveHue[x]=LastPixel;}
  //   }
  //   PipesFrame.LastTriggered = CurrentTime;
  // }
  
  Gutters();
  // BathTub();
    
  // LastPixel = WaveHue[Length-1];
  // for(int x = Length; x >= 0; x--){
  //   if(x!=0){WaveHue[x]=WaveHue[x-1];} 
  //   else {WaveHue[x]=LastPixel;}
  // }


  strip.show();
}
