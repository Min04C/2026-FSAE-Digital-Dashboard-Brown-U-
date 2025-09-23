//Current Running Version

/*

Changes:
 
  - Removed interval based updates
    - replaced with "when new data ready" updates
  - Set ISR as flag & moved reading packet to inside the loop
    - Why? There were risks with having SPI communincation inside an ISR
    - 



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

#define BUFFER_SIZE 10  // Circular buffer size
CANMessage canBuffer[BUFFER_SIZE];
volatile int bufferHead = 0;
volatile int bufferTail = 0;
volatile bool canMessageReady = false;

MCP_CAN CAN(CS_Pin);

unsigned int rpm, rpm1, rpm2, rpm3dig, gear, coolInTemp, coolOutTemp, batteryVoltage, fuelUsed;
const unsigned int UNINT = -1;
unsigned int prevGear = UNINT;
unsigned int prevInTemp = UNINT;
unsigned int prevOutTemp = UNINT;
unsigned int prevBattery = UNINT;
unsigned int prevFuel = UNINT;


void setup() {
  pinMode(CS_Pin, OUTPUT);
  pinMode(INTRPT_Pin, INPUT);
  
  Serial1.begin(9600); // default baud rate on dash
  delay(500);
  Serial1.print("baud=921600"); // Tell dash to change to/accept new baud rate (w/o falshing new .HMI)
  Serial1.write(0xFF); Serial1.write(0xFF); Serial1.write(0xFF);
  delay(100);
  Serial1.end(); 
  Serial1.begin(921600);  // update baud rate on Teensy

  if (CAN.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ) == CAN_OK) {
    attachInterrupt(digitalPinToInterrupt(INTRPT_Pin), canISR, FALLING);
  } else {
    while (1);
  }

  CAN.setMode(MCP_NORMAL);
  delay(1000);
}

void loop() {

  if (canMessageReady) {
    canMessageReady = false; //reset

    while (CAN.checkReceive() == CAN_MSGAVAIL) {
      CANMessage msg;
      CAN.readMsgBuf(&msg.id, &msg.len, msg.buf);
      int nextHead = (bufferHead + 1) % BUFFER_SIZE;
      if (nextHead != bufferTail) {
        canBuffer[bufferHead] = msg;
        bufferHead = nextHead;
      }
    }
  }

  processCANMessages();
  //unsigned long currentMillis = millis();
   
    sendRPM();
    if (gear != prevGear) {
      sendGear();
      prevGear = gear;
    }
    if ((coolInTemp != prevInTemp) || (coolOutTemp != prevOutTemp)){
      sendCoolantTemp();
      prevInTemp = coolInTemp;
      prevOutTemp = coolOutTemp;
    }
    if (batteryVoltage != prevBattery) {
      sendBattery();
      prevBattery = batteryVoltage;
    }
    if (fuelUsed != prevFuel) {
      sendFuel();
      prevFuel = fuelUsed;
    }
    
}

void canISR() {
  canMessageReady = true;
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
  switch (msg.id) {
    case 0x102:
      rpm = extractFloatFromBuffer(msg.buf) / 6;
      gear = msg.buf[7];
      break;
    case 0x103:
      coolInTemp = extractFloatFromBuffer(msg.buf);
      coolOutTemp = extractFloatFromBuffer(msg.buf + 4);
      break;
    case 0x104:
      batteryVoltage = extractFloatFromBuffer(msg.buf);
      fuelUsed = extractFloatFromBuffer(msg.buf + 4);
      break;
  }
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
  sendToNextion("rpmP1", String(rpm), false); delay(3);
  sendToNextion("rpmP2", String(rpm), false); delay(3);
  sendToNextion("rpm1", String(rpm1), true); delay(3);
  sendToNextion("rpm2", String(rpm2), true); delay(3); 
}
void sendCoolantTemp() {
  sendToNextion("b3", coolInTemp, false); delay(3);
  sendToNextion("b4", coolOutTemp, false); delay(3);
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
  sendToNextion("gearP1", String(gear), true); delay(3);
  sendToNextion("gearP2", String(gear), true);
}

void sendToNextion(const String& objectName, const String& value, bool isNumeric) {

  Serial1.print(objectName + (isNumeric ? ".val=" : ".txt=\"") + value + (isNumeric ? "" : "\""));
  Serial1.write(0xFF); Serial1.write(0xFF); Serial1.write(0xFF);
}