const int PIN_1_ADD = 11;
const int PIN_1_SUB = 8;
const int PIN_10_ADD = 12;
const int PIN_10_SUB = 9;
const int PIN_RESET = 10;
const int PIN_NIXIE_1[] = {5, 7, 6, 4};//{4, 6, 7, 5};
const int PIN_NIXIE_10[] = {1, 3, 2, 0};//{0, 2, 3, 1};
const int PIN_LIGHT = 13;
const int DELAY = 5;
bool buttonState[] = {false, false, false, false, false};//In order above
int count = 0;
void setup()
{
  pinMode(PIN_LIGHT, OUTPUT);
  lightOn();
  pinMode(PIN_1_ADD, INPUT);
  pinMode(PIN_1_SUB, INPUT);
  pinMode(PIN_10_ADD, INPUT);
  pinMode(PIN_10_SUB, INPUT);
  pinMode(PIN_RESET, INPUT);
  for(int i = 0; i < 4; i++)
  {
    pinMode(PIN_NIXIE_1[i], OUTPUT);
    digitalWrite(PIN_NIXIE_1[i], LOW);
    pinMode(PIN_NIXIE_10[i], OUTPUT);
    digitalWrite(PIN_NIXIE_10[i], LOW);
  }
  lightOff();
}
void loop()
{
  bool newState[] = {digitalRead(PIN_1_ADD), 
                     digitalRead(PIN_1_SUB), 
                     digitalRead(PIN_10_ADD), 
                     digitalRead(PIN_10_SUB), 
                     digitalRead(PIN_RESET)};
  if((buttonState[0]!=newState[0])||
     (buttonState[1]!=newState[1])||
     (buttonState[2]!=newState[2])||
     (buttonState[3]!=newState[3])||
     (buttonState[4]!=newState[4]))//May seem redundant, but this is the order I wanted stuff to work
  {
    lightOn();
    for(int i = 0; i <= 4; i++)
    {
      if(buttonState[i]!=newState[i])
      {
        if(buttonState[i]&&!newState[i])
          buttonState[i]=newState[i];
        else
        {
          buttonState[i]=newState[i];
          handleAction(i);
        }
      }
    }
    lightOff();
  }
  delay(DELAY);
}
void lightOn()
{
  digitalWrite(PIN_LIGHT, HIGH);
}
void lightOff()
{
  digitalWrite(PIN_LIGHT, LOW);
}
void handleAction(int i)
{
  int previous = count;
  if(i==0&&count==99)
    count=0;
  else if(i==0)
    count++;
  else if(i==1&&count==0)
    count=99;
  else if(i==1)
    count--;
  else if(i==2&&count>=90)
    count-=90;
  else if(i==2)
    count+=10;
  else if(i==3&&count<10)
    count+=90;
  else if(i==3)
    count-=10;
  else if(i==4)
    count=0;
  else
    count=66;//If I see a random 66, I'll know there was an error
  if(count<0||count>=100)
    count=66;
  updateNixie(previous%10!=count%10, count%10, previous/10!=count/10, count/10);
}
void updateNixie(bool update1, int digit1, bool update10, int digit10)
{
  if(update1)
  {
    int index = 0;
    for(int i=8; i>0; i/=2)
    {
      digitalWrite(PIN_NIXIE_1[index], (digit1%(i*2))/i>0);
      index++;
    }
  }
  if(update10)
  {
    int index = 0;
    for(int i=8; i>0; i/=2)
    {
      digitalWrite(PIN_NIXIE_10[index], (digit10%(i*2))/i>0);
      index++;
    }
  }
}
