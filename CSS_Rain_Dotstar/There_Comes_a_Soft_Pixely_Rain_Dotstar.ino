#include <Adafruit_DotStar.h>
#include <SPI.h>
#define NUMPIXELS  288
#define DATAPIN    8
#define CLOCKPIN   9

Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);

unsigned long CurrentTime;

struct Time{
  unsigned long LastTriggered;
  int Delay;
};

Time raintime = {0,8};
Time rainselecttime[3]; 

const int NumRaindrops = 3;
int Color[3];

struct rain{
  int position;
  bool active;
  int Start;
  int End;

  void run(){
    if(position == End+4){
      position = Start;
      active = false;
    }
    else{
      for(int x=0; x < 3;x++)
      if(position - x >=0 && position - x <= End){strip.setPixelColor(position - x,0,Color[x],Color[x]);}
      
      position++;
    }
  }
};

rain rainDrops[3][NumRaindrops];


void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.show();
  strip.setBrightness(255);

  for(int x =0; x < NumRaindrops ;x++){
    rainDrops[0][x].Start = 0;
    rainDrops[0][x].End = 84;
    rainDrops[1][x].Start = 85;
    rainDrops[1][x].End = 169;
    rainDrops[2][x].Start = 170;
    rainDrops[2][x].End = NUMPIXELS;
  }

  for(int x = 0; x < 3; x++){
    Color[x] = (255/2) + ((255/2) * cos(x* 3.14/2)); 
  }
  Color[2] = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  CurrentTime = millis();

  for(int x=0; x < 3;x++){
    if(CurrentTime >= rainselecttime[x].LastTriggered + rainselecttime[x].Delay){    
      rainselecttime[x].Delay = (random(20 ,50) * 10);
      for(int i=0; i < NumRaindrops; i++){
        if(rainDrops[x][i].active == false){
          rainDrops[x][i].active = true;
          break;
        }
      }
    rainselecttime[x].LastTriggered = CurrentTime;
    }
  }

  if(CurrentTime >= raintime.LastTriggered + raintime.Delay){
    for(int x = 0; x < 3; x++){
      for(int i=0; i< NumRaindrops; i++){
        if(rainDrops[x][i].active == true){rainDrops[x][i].run();}
      }
    }
    raintime.LastTriggered = CurrentTime;
  }

  strip.show();
}
