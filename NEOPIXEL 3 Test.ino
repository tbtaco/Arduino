#include <bluefruit.h>
#include <Adafruit_NeoPixel.h>
BLEUart bleuart;
const char bluetoothName[] = "Bluetooth Name Here";
const int serialOut = 16;
const int neoPixelCount = 3;
int loopDelay = 1;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(neoPixelCount, serialOut, NEO_RGB + NEO_KHZ800);
int rgb[neoPixelCount][3];
int currentEdit = 0;
String previousMessage = "-Reset-";
void setup()
{
  for(int i = 0; i < neoPixelCount; i++)
    for(int j = 0; j < 3; j++)
      rgb[i][j] = 0;
  pinMode(serialOut, OUTPUT);
  strip.begin();
  updateNeoPixels();
  Bluefruit.begin(1, 1);
  Bluefruit.setName(bluetoothName);
  bleuart.begin();
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80);
  Bluefruit.Scanner.filterUuid(bleuart.uuid);
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0);
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addService(bleuart);
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);
  Bluefruit.Advertising.setFastTimeout(30);
  Bluefruit.Advertising.start(0);
  Serial.begin(9600);
}
void loop()
{
  String input = "";
  if(Bluefruit.connected())
    input = getInput();
  else
  {
    delay(loopDelay);
    return;
  }
  /*if(input.length()>0&&previousMessage.equals(input))
  {
    currentEdit++;
    previousMessage = "-Reset-";
  }*/
  if(currentEdit >= neoPixelCount)
    currentEdit = 0;
  
  char inputC[input.length()+1];
  strcpy(inputC, input.c_str());
  char *partsC = strtok(inputC, " ");

  //if(partsC!=NULL)
  //blink(input.length());
  //if(input.length()>0)
    //Serial.println(input);


  
  String command = partsC;
  int args[7];
  int count = 0;
  if(command.length()==6)
  {
    for(int i = 0; i < 3; i++)
    {
      char c[2];
      c[0] = command[2*i];
      c[1] = command[2*i+1];
      sscanf(c, "%x", &args[i]);
    }
    count = 7;
  }
  partsC = strtok(NULL, " ");
  while(partsC != NULL && count < 7)
  {
    args[count] = atoi(partsC);
    count++;
    partsC = strtok(NULL, " ");
  }
  

  //if(input.length()>0)
    //blink(args[1]);
    //blink(sizeof(split)/sizeof(split[0]));
  //delay(2000);


  if(command.equalsIgnoreCase("Next"))
    currentEdit++;
  else if(command.equalsIgnoreCase("C"))
    for(int i = 0; i < 3; i++)
      rgb[currentEdit][i] = args[i];
  else if(command.equalsIgnoreCase("Delay"))
    loopDelay = args[0];

/*
  if(input.length()>0)
  {
    Serial.write("Current Edit: ");
    Serial.print(currentEdit, DEC);
    Serial.write("\n");
    for(int i = 0; i < 3; i++)
    {
      Serial.write("  ");
      Serial.print(i+1, DEC);
      Serial.write(": ");
      Serial.print(rgb[i][0], DEC);
      Serial.write(", ");
      Serial.print(rgb[i][1], DEC);
      Serial.write(", ");
      Serial.print(rgb[i][2], DEC);
      Serial.write("\n");
    }
    Serial.write("\n");
  }*/
  
  

  
  updateNeoPixels();
  previousMessage = input;
  delay(loopDelay);
}
String getInput()
{
  String input = "";
  int count = 0;
  while(bleuart.available())
  {
    char c = bleuart.read();
    if(c != '\n' && !(c == '!' && count == 0) && !(c == 'C' && count == 1))
      input = input + c;
    count++;
  }
  return input;
}
void updateNeoPixels()
{
  for(int i = 0; i < neoPixelCount; i++)
    strip.setPixelColor(i, strip.Color(rgb[i][0], rgb[i][1], rgb[i][2]));
  strip.show();
}
void blink(int i)
{
  for(int j = 0; j < i; j++)
  {
    delay(500);
    rgb[0][0] = 40;
    rgb[0][1] = 255;
    rgb[0][2] = 40;
    updateNeoPixels();
    delay(500);
    rgb[0][0] = 40;
    rgb[0][1] = 0;
    rgb[0][2] = 40;
    updateNeoPixels();
    
  }
}
