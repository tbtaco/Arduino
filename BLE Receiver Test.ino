#include <bluefruit.h>
BLEUart bleuart;
int pinRed = 17;
int loopDelayValue = 1;
char bluetoothName[] = "BLE_Test_00247_R";
void setup()
{
  digitalWrite(pinRed, LOW);
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
void loop()
{
  String input = "";
  if (Bluefruit.connected())
    input = getInput();
  if (input.equalsIgnoreCase("On") || input.equals("1"))
    digitalWrite(pinRed, HIGH);
  else if (input.equalsIgnoreCase("Off") || input.equals("0"))
    digitalWrite(pinRed, LOW);
  delay(loopDelayValue);
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
  return input;
}
