#include <bluefruit.h>

BLEUart bleuart;
int pinOutput = 16; //Pin 16.  With the P-Channel MOSFET, HIGH will turn the device Off and LOW will turn it On.  Should mention, the MOSFET is meant to have the device Off most of the time.  If left On for a long time (maybe 30+ seconds), it will heat up quite a bit.  Tests have shown it gets up to 120F and may rise higher in the device casing.  The MOSFET will operate normally anywhere below 350F so I don't expect any problems, just worth noting it should be kept away from other important parts of the circuit.  Another note, Off puts about 20mW across the MOSFET where On puts 800mW across it.
int pinRed = 17; //Red LED

int loopDelayValue = 1;
char bluetoothName[] = "Device Prototype 1.0";

void setup() //Runs this once on startup
{
  pinMode(pinOutput, OUTPUT);
  
  deviceOff();

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
}

void loop() //Loops through this after setup() runs once
{
  String input = "";
  if (Bluefruit.connected())
    input = getInput();

  if (input.equalsIgnoreCase("On") || input.equals("1"))
    deviceOn();
  else if (input.equalsIgnoreCase("Off") || input.equals("0"))
    deviceOff();

  delay(loopDelayValue);
}

String getInput() //Gets a message sent through BLE UArt
{
  String input = "";
  while (bleuart.available())
  {
    char c = bleuart.read();
    if (c != '\n') //Ignores the newline character
      input = input + c;
  }
  return input;
}

void deviceOn() //Turns device On
{
  digitalWrite(pinOutput, LOW);
  digitalWrite(pinRed, HIGH);
}

void deviceOff() //Turns device Off
{
  digitalWrite(pinOutput, HIGH);
  digitalWrite(pinRed, LOW);
}
