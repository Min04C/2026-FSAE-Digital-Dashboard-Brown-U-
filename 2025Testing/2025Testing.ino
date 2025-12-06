//Current Testing Suite

/*

Current ifs:

- need to fix the cooling code, calculations, etc
- need to add the rest of the sensors for page 2

*/
#include <mcp_can.h>  // necessary for CAN Protocol communication commands
#include <SPI.h>

#define CS_Pin 10
#define INTRPT_Pin 9  // Interrupt pin

struct CANMessage {
  unsigned long id;
  unsigned char len;
  unsigned char buf[8];
};

#define BUFFER_SIZE 50  // Circular buffer size
CANMessage canBuffer[BUFFER_SIZE];
volatile int bufferHead = 0;
volatile int bufferTail = 0;

MCP_CAN CAN(CS_Pin);

unsigned long last500Update = 0;  // 0.5 seconds
unsigned long last1000Update = 0;  // 1.0 seconds
unsigned long last5000Update = 0;  // 5.0 seconds
const unsigned long interval500 = 500;
const unsigned long interval1000 = 1000;
const unsigned long interval5000 = 5000;

volatile unsigned long count = 0;
volatile unsigned long counttt = 0;

unsigned int rpm, rpm1, rpm2, rpm3dig, gear, coolInTemp, coolOutTemp, batteryVoltage, fuelUsed;

unsigned long lastPrintTime = 0;

volatile unsigned long count_102 = 0;
volatile unsigned long count_103 = 0;
volatile unsigned long count_104 = 0;
volatile unsigned long count_105 = 0;
volatile unsigned long count_106 = 0;
volatile unsigned long count_648 = 0;
volatile unsigned long count_700 = 0;
volatile unsigned long count_701 = 0;

volatile unsigned long droppedMessages = 0;
unsigned long dMessages = 0;



void setup() {
  pinMode(CS_Pin, OUTPUT);
  pinMode(INTRPT_Pin, INPUT);
  Serial1.begin(9600);
  Serial.begin(250000);

  if (CAN.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ) == CAN_OK) {
    attachInterrupt(digitalPinToInterrupt(INTRPT_Pin), canISR, FALLING);
  } else {
    while (1);
  }

  CAN.setMode(MCP_NORMAL);
  delay(1000);
}

void loop() {

  processCANMessages();
   
  
  //unsigned long currentMillis = millis();
  /*if (currentMillis - last500Update >= interval500) { // 500 - RPM, gear
    sendRPM();
    sendGear();
    last500Update = currentMillis;
  }
  if (currentMillis - last1000Update >= interval1000) { // 1000 - coolant
    sendCoolantTemp();
    last1000Update = currentMillis;
  }
  if (currentMillis - last5000Update >= interval5000) { // 5000 - battery, fuel
    sendBattery();
    sendFuel();
    last5000Update = currentMillis;
  }*/

  unsigned long now = millis();
  if (now - lastPrintTime >= 1000) {
    noInterrupts(); // prevent change while reading
    //unsigned long packetsPerSecond = count;
    //count = 0;
    unsigned long packsPerSecond = counttt;
    counttt = 0;

    dMessages = droppedMessages;
    droppedMessages = 0;


    interrupts();

    Serial.println("-");
    Serial.print("CAN Packets receieved per second: ");
    Serial.println(packsPerSecond);
    //Serial.print("CAN Packets buffered per second: ");
    //Serial.println(packetsPerSecond);
    Serial.print("0x102 packets/sec: ");
    Serial.println(count_102);
    Serial.print("0x103 packets/sec: ");
    Serial.println(count_103);
    Serial.print("0x104 packets/sec: ");
    Serial.println(count_104);
    Serial.print("0x105 packets/sec: ");
    Serial.println(count_105);
    Serial.print("0x106 packets/sec: ");
    Serial.println(count_106);
    Serial.print("0x648 packets/sec: ");
    Serial.println(count_648);
    Serial.print("0x700 packets/sec: ");
    Serial.println(count_700);
    Serial.print("0x701 packets/sec: ");
    Serial.println(count_701);
    //Serial.print("Dropped messages per second: ");
    //Serial.println(dMessages);

    Serial.println("-");

    count_102 = 0;
    count_103 = 0;
    count_104 = 0;
    count_105 = 0;
    count_106 = 0;
    count_648 = 0;
    count_700 = 0;
    count_701 = 0;

    lastPrintTime = now;
  }

}

void canISR() {
    while (CAN.checkReceive() == CAN_MSGAVAIL) {
      CANMessage msg;
      CAN.readMsgBuf(&msg.id, &msg.len, msg.buf);
      //Serial.print("--ID: 0x");
      //Serial.print(msg.id, HEX);
      int nextHead = (bufferHead + 1) % BUFFER_SIZE;
      if (nextHead != bufferTail) {
        canBuffer[bufferHead] = msg;
        bufferHead = nextHead;
      } else {
        //droppedMessages++;
      }
      //count++;

      switch (msg.id) {
    case 0x102:
      count_102++;

      break;
    case 0x103:
      count_103++;

      break;
    case 0x104:
    
      count_104++;

      break;
    case 0x105:
      count_105++;
      break;
    case 0x106:
      count_106++;
      break;
    case 0x648:
      count_648++;
      break;  
    case 0x700:
      count_700++;
      break;
    case 0x701:
      count_701++;
      //Serial.print("0x701 data: ");
      //for (int i = 0; i < msg.len; i++) {
      //  Serial.print(msg.buf[i], HEX);
      ///  Serial.print(" ");
      //}
      //Serial.println();
      break; }
    }
    counttt++;
    
    
     //test this to see calls to ISR without time-intesive code

}

//-------------------SET VARIABLES FROM CAN PACKET-------------------------------------------
void processCANMessages() {
  while (bufferTail != bufferHead) {
    CANMessage msg = canBuffer[bufferTail];
    bufferTail = (bufferTail + 1) % BUFFER_SIZE;
    handleCANMessage(msg);
  }
}

void handleCANMessage(CANMessage msg) {
  //Serial.print("--ID: 0x");
  //Serial.print(msg.id, HEX);
  //if (msg.id == 0x700 || msg.id == 0x701) {
  //return; // ignore diagnostic chatter
  //}
  //counttt++;
  /*switch (msg.id) {
    case 0x102:
      rpm = extractFloatFromBuffer(msg.buf) / 6;
      gear = msg.buf[7];
      count_102++;

      break;
    case 0x103:
      coolInTemp = extractFloatFromBuffer(msg.buf);
      coolOutTemp = extractFloatFromBuffer(msg.buf + 4);
      count_103++;

      break;
    case 0x104:
      batteryVoltage = extractFloatFromBuffer(msg.buf);
      fuelUsed = extractFloatFromBuffer(msg.buf + 4);
      count_104++;

      break;
    case 0x105:
      count_105++;
      break;
    case 0x106:
      count_106++;
      break;
    case 0x648:
      count_648++;
      break;  
    case 0x700:
      count_700++;
      break;
    case 0x701:
      count_701++;
      //Serial.print("0x701 data: ");
      //for (int i = 0; i < msg.len; i++) {
      //  Serial.print(msg.buf[i], HEX);
      ///  Serial.print(" ");
      //}
      //Serial.println();
      break;*/

  //}
}

float extractFloatFromBuffer(unsigned char* buf) {
  union {
    uint32_t bits;
    float number;
  } data;
  data.bits = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
  return data.number;
}

//--------------------SENDING VALUES TO NEXTION---------------------------------------------
void sendRPM() {
  rpm3dig = rpm / 100;
  if (rpm3dig <= 100) {
    rpm1 = rpm3dig;
    rpm2 = 0;
  } else if (rpm3dig <= 135) {
    rpm1 = 100;
    rpm2 = (rpm3dig % 100) * 100 / 35;
  } else {
    rpm1 = 100;
    rpm2 = 100;
  }
  sendToNextion("rpmP1", String(rpm), false);
  sendToNextion("rpmP2", String(rpm), false);
  sendToNextion("rpm1", String(rpm1), true);
  sendToNextion("rpm2", String(rpm2), true);
}
void sendCoolantTemp() {
  sendToNextion("b3", coolInTemp, false);
  sendToNextion("b4", coolOutTemp, false);
  if (coolInTemp > 70.0 && coolInTemp < 90.0) {
    sendToNextion("a2", "Heating Up", false);
    Serial1.print("warning.aph=70"); Serial1.write(0xFF); Serial1.write(0xFF); Serial1.write(0xFF);
  } else if (coolInTemp >= 90.0) {
    sendToNextion("a2", "OVERHEATING", false);
    Serial1.print("warning.aph=127"); Serial1.write(0xFF); Serial1.write(0xFF); Serial1.write(0xFF);
  } else { 
    sendToNextion("a2", "", false);
    Serial1.print("warning.aph=0"); Serial1.write(0xFF); Serial1.write(0xFF); Serial1.write(0xFF);
  }
}
void sendBattery() {
  sendToNextion("batteryVoltage", String(batteryVoltage), false);
}
void sendFuel() {
  sendToNextion("c1", String(fuelUsed), false);
}
void sendGear() {
  sendToNextion("gearP1", String(gear), true);
  sendToNextion("gearP2", String(gear), true);
}

void sendToNextion(const String& objectName, const String& value, bool isNumeric) {
  // scared of using this
  // const unsigned long timeout = 100;  // timeout in milliseconds
  // const int bytesNeeded = objectName.length() + value.length() + (isNumeric ? 6 : 10); // estimate of bytes to send

  // unsigned long start = millis();
  // while (Serial1.availableForWrite() < bytesNeeded) {
  //   if (millis() - start > timeout) return;  // give up if buffer is stuck
  // }
  
  Serial1.print(objectName + (isNumeric ? ".val=" : ".txt=\"") + value + (isNumeric ? "" : "\""));
  Serial1.write(0xFF);
  Serial1.write(0xFF);
  Serial1.write(0xFF);
}