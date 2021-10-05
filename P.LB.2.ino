#include <bluefruit.h>
#include <Wire.h>
#include "Adafruit_FRAM_I2C.h"
BLEUart bleuart;//Limit Of 23 Characters Sent At A Time
BLEDis bledis;
Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();
const uint16_t TOTAL_MEMORY = 0x7FFF;
const char BLUETOOTH_NAME[] = "LB Prototype B2";
const uint8_t normalDelay = 5;
bool locked = false;
void setup()
{
  lightOn();
  Serial.begin(9600);
  while(!Serial)
    delayMicroseconds(50);
  Serial.print("Setting Up Bluetooth: ");
  Bluefruit.begin(1, 1);
  Bluefruit.setName(BLUETOOTH_NAME);
  bledis.setManufacturer("Tyler Richey");
  bledis.setModel("Prototype Board #2");
  bledis.begin();
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
  Serial.println("Success");
  Serial.print("Initializing FRAM Module: ");
  if(fram.begin(0x50))
    Serial.println("Success");
  else
    Serial.println("Connection Error");
  Serial.println("All Input/Output Will Be Handled Through Bluetooth From Here On");
  lightOff();
}
void loop()
{
  if(locked)
  {
    Bluefruit._stopConnLed();
    digitalWrite(PIN_LED2, LOW);
  }
  String input = "";
  if(Bluefruit.connected())
  {
    lightOn();
    input = getInput();
  }
  if(input.length()==0)
  {
    lightOff() ;
    delay(normalDelay);
    return;
  }
  if(locked)
  {
    checkBattery();
    bleuart.println("Device Is Locked");
    bleuart.print("To Unlock, Turn P");
    bleuart.println("ower Off And Back On");
    lightOff();
    delay(normalDelay);
    return;
  }
  String command = "";
  String data = "";
  char input_c[input.length()+1];
  strcpy(input_c, input.c_str());
  char *part = strtok(input_c, " ");
  command = part;
  part = strtok(NULL, " ");
  while(part != NULL)
  {
    data = data + part;
    part = strtok(NULL, " ");
    if(part != NULL)
      data = data + " ";
  }
/****PASSWORD*SECTION****
//Lots of ideas for this section but for now I'll do something fairly simple.
//Pretty much you must know the length of the saved String, and to put it in this format.
  checkConnection();
  if(command.equalsIgnoreCase("Read")||command.equalsIgnoreCase("R")||
     command.equalsIgnoreCase("Write")||command.equalsIgnoreCase("W"))//Debug Is Already Password Protected
  {
    String data2 = "";
    String userPass = "";
    char data_c[data.length()+1];
    strcpy(data_c, data.c_str());
    char *part = strtok(data_c, " ");
    userPass = part;
    part = strtok(NULL, " ");
    while(part != NULL)
    {
      data2 = data2 + part;
      part = strtok(NULL, " ");
      if(part != NULL)
        data2 = data2 + " ";
    }
    String password;
    password = password + fram.read8(0x0000) + "." + fram.read8(0x0001); //For Example 0.0 Or 65.216
    if(userPass!=NULL&&userPass.length()>0&&userPass.equalsIgnoreCase(password))
      data = data2;
    else
    {
      bleuart.println("Password Was Incorrect");
      delay(normalDelay);
      return;
    }
  }
****/
  if(command.equalsIgnoreCase("Read")||command.equalsIgnoreCase("R"))
  {
    checkConnection();
    int l = 256*fram.read8(0x0000)+fram.read8(0x0001);
    bleuart.print("Reading... (");
    bleuart.print(l);
    bleuart.println(" Bytes)");
    for(int i = 0; i < l; i++)
    {
      uint16_t address = 0x0002 + i;
      char c = fram.read8(address);
      bleuart.print(c);
    }
    bleuart.println("\nDone");
  }
  else if(command.equalsIgnoreCase("Write")||command.equalsIgnoreCase("W"))
  {
    checkConnection();
    if(isWriteProtected())
    {
      bleuart.print("Error: Disk Is ");
      bleuart.print("Currently In Write ");
      bleuart.println("Protect Mode");
      delay(normalDelay);
      lightOff();
      return;
    }
    int l = data.length();
    bleuart.print("Writing... (");
    bleuart.print(l);
    bleuart.println(" Bytes)");
    uint8_t l1 = l/256;
    uint8_t l2 = l%256;
    int totalLength = 256*fram.read8(0x0000)+fram.read8(0x0001);
    if(totalLength<l)
      totalLength = l;
    if(totalLength>TOTAL_MEMORY-2)
    {
      bleuart.print("Error: Not Enough ");
      bleuart.println("Memory");
      delay(normalDelay);
      lightOff();
      return;
    }
    for(uint16_t i = 0; i < totalLength; i++)
    {
      uint16_t address = 0x0002 + i;
      if(i<=l)
        fram.write8(address, data[i]);
      else
        fram.write8(address, 0x00);
    }
    fram.write8(0x0000, l1);
    fram.write8(0x0001, l2);
    bleuart.println("Done");
  }
  else if(command.equalsIgnoreCase("Debug")||command.equalsIgnoreCase("D"))
  {
    checkConnection();
    String command2 = "";
    String data2 = "";
    char data_c[data.length()+1];
    strcpy(data_c, data.c_str());
    char *part = strtok(data_c, " ");
    command2 = part;
    part = strtok(NULL, " ");
    while(part != NULL)
    {
      data2 = data2 + part;
      part = strtok(NULL, " ");
      if(part != NULL)
        data2 = data2 + " ";
    }
    String password;
    password = password + fram.read8(0x0000) + "." + fram.read8(0x0001); //For Example 0.0 Or 65.216
    if(data2!=NULL&&data2.length()>0&&data2.equalsIgnoreCase(password))
      if(command2.equalsIgnoreCase("Clear")||command2.equalsIgnoreCase("C"))
        clearMemory();
      else if(command2.equalsIgnoreCase("Print")||command2.equalsIgnoreCase("P"))
        printMemory();
      else
        bleuart.println("Invalid Debug Command");
    else
      bleuart.println("Password Was Incorrect");
  }
  else if(command.equalsIgnoreCase("Battery")||command.equalsIgnoreCase("Bat")||command.equalsIgnoreCase("B"))
    checkBattery();
  else if(command.equalsIgnoreCase("Lock")||command.equalsIgnoreCase("L"))
  {
    locked = true;
    bleuart.println("Device Is Now Locked");
    bleuart.print("To Unlock, Turn P");
    bleuart.println("ower Off And Back On");
    lightOff();
    delay(normalDelay);
    return;
  }
  else
  {
    bleuart.print("Commands: Read, R, Wri");
    bleuart.print("te, W, Debug*, D*, ");
    bleuart.print("Battery, Bat, B, ");
    bleuart.println("Lock, L");
    bleuart.println("  *Requires Password");
    //bleuart.print("Commands: Read, R, Wri");
    //bleuart.print("te, W, Debug, D, ");
    //bleuart.println("Battery, Bat, B");
    //bleuart.print("  *All Require ");
    //bleuart.println("Passwords");
  }
  delay(normalDelay);
  lightOff();
}
void connect_callback(uint16_t conn_handle)
{
  char centralName[32];
  Bluefruit.Gap.getPeerName(conn_handle, centralName, sizeof(centralName));
}
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;
}
void clearMemory()
{
  checkConnection();
  if(isWriteProtected())
  {
    bleuart.print("Error: Disk Is ");
    bleuart.print("Currently In Write ");
    bleuart.println("Protect Mode");
	delay(normalDelay);
    return;
  }
  bleuart.print("Starting A Full ");
  bleuart.println("Wipe Of Memory");
  for(uint16_t address = 0; address < TOTAL_MEMORY; address++)
    fram.write8(address, 0x00);
  bleuart.println("Memory Wipe Complete");
}
String getInput()
{
  String input = "";
  while (bleuart.available())
  {
    char c = bleuart.read();
    if (c != '\n')
      input = input + c;
  }
  if(input.length()>0)//138 And 158 Both Seen As Limits...
  {
    delay(400); //6ms latency, 3ms minimum for data, 125 kbit/s, calculated 0.265s max delay, using 0.3s then. 119ms for 32KB of data?  0.4s then?
    if(bleuart.available())
      input = input + getInput();
  }
  return input;
}
void printMemory()
{
  bleuart.println("Printing All Memory");
  bleuart.print("Anything Not Printed ");
  bleuart.println("Is 0x00/NULL");
  bleuart.print("Grouping Memory Every ");
  bleuart.println("16 Bytes");
  uint8_t grouping = 16;
  uint8_t nullVal = 0x00;
  for(uint16_t address = 0; address < TOTAL_MEMORY; address++)
    if(fram.read8(address)!=nullVal)
    {
      uint8_t remainder = address%grouping;
      print16(address-remainder);
      address+=(grouping-remainder-1);
    }
  bleuart.print("Memory Printing ");
  bleuart.println("Complete");
}
void print16(uint16_t address)
{
  bleuart.print("0x");
  if(address<0x1000)
    bleuart.print("0");
  if(address<0x0100)
    bleuart.print("0");
  if(address<0x0010)
    bleuart.print("0");
  bleuart.print(address, HEX);
  bleuart.print(":");
  for(uint16_t addr = address; addr < address + 16; addr++)
  {
    bleuart.print(" 0x");
    uint8_t data = fram.read8(addr);
    if(data<0x10)
      bleuart.print("0");
    bleuart.print(data, HEX);
  }
  bleuart.println();
}
bool isWriteProtected()
{
  uint8_t i1 = fram.read8(TOTAL_MEMORY-1);
  uint8_t i2 = i1 + 1;
  fram.write8(TOTAL_MEMORY-1, i2);
  if(fram.read8(TOTAL_MEMORY-1)==i1)
    return true;
  fram.write8(TOTAL_MEMORY-1, i1);
  return false;
}
void checkConnection()
{
  if(fram.read8(0x0000)==0xFF&&fram.read8(0x0001)==0xFF)//A disconnected/faulty drive will read 0xFF for everything, and both these addresses together can't go this high using my layout.
  {
    bleuart.print("Error: FRAM Module ");
	bleuart.print("Connection Was ");
	bleuart.println("Interrupted");
	delay(1000);
    bleuart.print("Initializing FRAM ");
    bleuart.print("Module Again: ");
    if(fram.begin(0x50))
      bleuart.println("Success");
    else
      bleuart.println("Connection Error");
  }
}
void lightOn()
{
  digitalWrite(PIN_LED1, HIGH);
}
void lightOff()
{
  digitalWrite(PIN_LED1, LOW);
}
void printDouble(double val, uint16_t precision)
{
  bleuart.print(int(val));
  bleuart.print(".");
  int frac;
  if(val >= 0)
    frac = (val-int(val))*precision;
  else
    frac = (int(val)-val)*precision;
  int frac1 = frac;
  while(frac1 /= 10)
    precision /= 10;
  precision /= 10;
  while(precision /= 10)
    bleuart.print("0");
  bleuart.print(frac, DEC) ;
}
void checkBattery()
{
  int batteryVal = analogRead(A7);//Analog 7 / Pin 31 / VBAT ADC
  //From 0 to 1023, referring to 0v and 5v.
  //My battery is 3.7v (757), although it may never reach that.
  //Everything starts to mess up around 2.45v (502) or so.
  const int maxVal = 836;//760 for 3.7v, 836 for 4.09v
  const int minVal = 500;
  double voltage = 5.0/1023*batteryVal;
  double percent = ((double)batteryVal-minVal)/((double)maxVal-minVal)*100;
  if(percent > 100.0)
    percent = 100.0;
  else if(percent < 0.0)
    percent = 0.0;
  if(percent==100.0&&voltage>=4.09)
  {
    bleuart.print("Battery Is ");
    bleuart.println("Completely Charged");
    bleuart.print("(Or The Battery ");
    bleuart.println("Switch Is Off)");
  }
  else
  {
    bleuart.print("Voltage: ");
    printDouble(voltage, 100);
    bleuart.print("v, Percent: ");
    printDouble(percent, 100);
    bleuart.println("%");
    bleuart.print("(Max: About 4.1v, ");
    bleuart.println("Min: 2.45v)");
  }
}
