#include <OctoWS2811.h>

const int numPins = 6;
byte pinList[numPins] = {2,3,23,22,14,8};

const int ledsPerStrip = 60;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config, numPins, pinList);


unsigned long CurrentTime;

struct Time{
  unsigned long LastTriggered;
  int Delay;
};

Time raintime = {0,8};
Time rainselecttime[numPins]; 

const int NumRaindrops = 3;
const int rainSize = 5;
int Color[rainSize];

struct rain{
  int position;
  int start;
  bool active;

  void run(){
    if(position == 64){
      position = 0;
      active = false;
    }
    else{
      for(int x=0; x < rainSize;x++){if(position - x >=0 && position - x <= 59){leds.setPixel(position - x + start,Color[x]/10,Color[x]/10,Color[x]);}}
      position++;
    }
  }
};

rain rainDrops[numPins][NumRaindrops];


void setup() {
  // put your setup code here, to run once:
  leds.begin();
  leds.show();

  for(int x=0; x < numPins; x++){
    for(int y = 0; y< NumRaindrops; y++){
      rainDrops[x][y].start = 60 * x;
    }
  }

  for(int x = 0; x < rainSize; x++){
    Color[x] = (200/2) + ((200/2) * cos(x* 3.14/4)); 
  }
  Color[rainSize - 1] = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  CurrentTime = millis();

  for(int x= 0; x < numPins; x++){
    if(CurrentTime >= rainselecttime[x].LastTriggered + rainselecttime[x].Delay){
      rainselecttime[x].Delay = (random(5 ,50) * 10);
      for(int y=0; y < NumRaindrops; y++){
        if(rainDrops[x][y].active == false){
          rainDrops[x][y].active = true;
          break;
        }
      }
    rainselecttime[x].LastTriggered = CurrentTime;
    }
  }

  if(CurrentTime >= raintime.LastTriggered + raintime.Delay){
    for(int x = 0; x < numPins; x++){
      for(int y=0; y< NumRaindrops; y++){
        if(rainDrops[x][y].active == true){rainDrops[x][y].run();}
      }
    }

    raintime.LastTriggered = CurrentTime;
  }

  leds.show();
}
