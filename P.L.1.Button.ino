#include <bluefruit.h>
BLEUart bleuart;
BLEClientDis clientDis;
BLEClientUart clientUart;
const int PIN_RED = PIN_LED1;
const int PIN_BLUE = PIN_LED2;
const int PIN_INPUT = 11;
const int DELAY = 3;
const char BLUETOOTH_NAME[] = "BLE_Button_Proto_1.0";
const char MESSAGE_OFF[] = "0";
const char MESSAGE_ON[] = "1";
const int MESSAGE_LOOP_DELAY = 100/DELAY;//Every 0.1s I'll send a message
int messageLoopDelayCount = 0;
uint16_t loopCount = 0;
const uint16_t LOOP_LOCK = (10*1000)/DELAY;//10sec to milli
bool buttonIsPressed = false;
int locked = 0;
void setup()
{
  pinMode(PIN_RED, OUTPUT);
  digitalWrite(PIN_RED, HIGH);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_INPUT, INPUT);
  Bluefruit.begin(1, 1);
  clientDis.begin();
  clientUart.begin();
  Bluefruit.setName(BLUETOOTH_NAME);
  bleuart.begin();
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Central.setConnectCallback(connect_callback);
  Bluefruit.Central.setDisconnectCallback(disconnect_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80);
  Bluefruit.Scanner.filterUuid(bleuart.uuid);
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0);
  Bluefruit._stopConnLed();
  digitalWrite(PIN_BLUE, LOW);
  digitalWrite(PIN_RED, LOW);
  delay(DELAY);
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
  Bluefruit._stopConnLed();
  digitalWrite(PIN_BLUE, LOW);
}
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;
  Bluefruit._stopConnLed();
  digitalWrite(PIN_BLUE, LOW);
}
void loop()
{
  digitalWrite(PIN_RED, HIGH);
  if(locked>0)
  {
    if(locked==1&&Bluefruit.Central.connected()&&clientUart.discovered())
    {
      locked = 2;
      clientUart.println(MESSAGE_OFF);
    }
    digitalWrite(PIN_RED, LOW);
    delay(DELAY);
    return;
  }
  if(Bluefruit.Central.connected()&&clientUart.discovered())
  {
    digitalWrite(PIN_BLUE, HIGH);
    if(digitalRead(PIN_INPUT))
    {
      loopCount++;
      if(!buttonIsPressed)
      {
        buttonIsPressed = true;
        clientUart.println(MESSAGE_ON);
      }
    }
    else
    {
      loopCount = 0;
      if(buttonIsPressed)
      {
        buttonIsPressed = false;
        clientUart.println(MESSAGE_OFF);
      }
    }
    messageLoopDelayCount++;
    if(messageLoopDelayCount>=MESSAGE_LOOP_DELAY)
    {
      messageLoopDelayCount = 0;
      if(buttonIsPressed)
        clientUart.println(MESSAGE_ON);
      else
        clientUart.println(MESSAGE_OFF);
    }
    digitalWrite(PIN_BLUE, LOW);
  }
  else
  {
    if(digitalRead(PIN_INPUT))
    {
      loopCount++;
      if(!buttonIsPressed)
        buttonIsPressed = true;
    }
    else
    {
      loopCount = 0;
      if(buttonIsPressed)
        buttonIsPressed = false;
    }
  }
  if(loopCount>=LOOP_LOCK)
    locked = 1;
  if(!buttonIsPressed)
    digitalWrite(PIN_RED, LOW);
  delay(DELAY);
}
