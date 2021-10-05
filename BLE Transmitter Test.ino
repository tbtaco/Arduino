#include <bluefruit.h>
BLEUart bleuart;
BLEClientDis clientDis;
BLEClientUart clientUart;
int pinRed = 17;
int pinInput = A5;
int loopDelayValue = 1;
char bluetoothName[] = "BLE_Test_00247_T";
bool lightOn = false;
void setup()
{
  Bluefruit.begin(1, 1);
  clientDis.begin();
  clientUart.begin();
  Bluefruit.setName(bluetoothName);
  bleuart.begin();
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Central.setConnectCallback(connect_callback);
  Bluefruit.Central.setDisconnectCallback(disconnect_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80);
  Bluefruit.Scanner.filterUuid(bleuart.uuid);
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0);
}
void scan_callback(ble_gap_evt_adv_report_t* report)
{
  if(Bluefruit.Scanner.checkReportForService(report, clientUart))
    Bluefruit.Central.connect(report);
}
void connect_callback(uint16_t conn_handle)
{
  if(clientUart.discover(conn_handle))
    clientUart.enableTXD();
  else
    Bluefruit.Central.disconnect(conn_handle);
}
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;
}
void loop()
{
  digitalWrite(pinRed, analogRead(pinInput) > 128);
  bool connected = Bluefruit.Central.connected() && clientUart.discovered();
  bool currentState = analogRead(pinInput) > 128;
  if(connected && currentState != lightOn)
  {
    digitalWrite(pinRed, currentState);
    clientUart.print(currentState);
    lightOn = currentState;
  }
  else if(!connected)
  {
    digitalWrite(pinRed, LOW);
    lightOn = false; //Introduces a bug, but not important for testing purposes...
  }
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
