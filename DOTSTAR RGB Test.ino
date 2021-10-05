#include <Adafruit_DotStar.h>
const int PIN_RED_LED = 13;
const int PIN_DOTSTAR_DATA = 7;
const int PIN_DOTSTAR_CLOCK = 8;
const int PIXEL_COUNT = 1;
Adafruit_DotStar strip(PIXEL_COUNT, PIN_DOTSTAR_DATA, PIN_DOTSTAR_CLOCK, DOTSTAR_BGR);
int r = 0;
int g = 0;
int b = 0;
int lightState = 0;
int tempState = 0;
void setup()
{
  pinMode(PIN_RED_LED, OUTPUT);
  digitalWrite(PIN_RED_LED, HIGH);
  strip.begin();
  strip.setPixelColor(0, r, g, b);
  strip.show();
  digitalWrite(PIN_RED_LED, LOW);
}
void loop()
{
  digitalWrite(PIN_RED_LED, HIGH);
  /*
  Status 0, 1, and 2 will be:
    Red Up, Red Down, Green Up, Green Down, Blue Up, Blue Down, Increment Status
  Status 3 gets ready for the next pattern:
    Red Up, Increment Status
  Status 4, 5, and 6 will be:
    Green Up, Red Down, Blue Up, Green Down, Red Up, Blue Down, Increment Status
  Status 7 gets ready to repeat at 0:
    Red Down, Status = 0
  */
  int oldLightState = lightState;
  if(lightState>=0&&lightState<=2)
  {
    if(tempState==0)
      r++;
    else if(tempState==1)
      r--;
    else if(tempState==2)
      g++;
    else if(tempState==3)
      g--;
    else if(tempState==4)
      b++;
    else if(tempState==5)
      b--;
    else
      lightState=100;
    if((r==0||r==255)&&(g==0||g==255)&&(b==0||b==255))
      tempState++;
    if(tempState==6&&r==0&&g==0&&b==0)
    {
      lightState++;
      tempState=0;
    }
  }
  else if(lightState==3)
  {
    r++;
    if(r==255)
      lightState++;
  }
  else if(lightState>=4&&lightState<=6)
  {
    if(tempState==0)
      g++;
    else if(tempState==1)
      r--;
    else if(tempState==2)
      b++;
    else if(tempState==3)
      g--;
    else if(tempState==4)
      r++;
    else if(tempState==5)
      b--;
    else
      lightState=100;
    if((r==0||r==255)&&(g==0||g==255)&&(b==0||b==255))
      tempState++;
    if(tempState==6&&r==255&&g==0&&b==0)
    {
      lightState++;
      tempState=0;
    }
  }
  else if(lightState==7)
  {
    r--;
    if(r==0)
      lightState=0;
  }
  else
  {
    r=0;
    g=0;
    b=0;
    lightState=0;
  }
  if(lightState!=oldLightState)
  {
    if(lightState==0||lightState==3||lightState==7)
      delay(3000);
  }
  strip.setPixelColor(0, r, g, b);
  strip.show();
  delay(5);
  digitalWrite(PIN_RED_LED, LOW);
}
