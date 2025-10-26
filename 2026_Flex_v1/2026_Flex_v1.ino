#include <FlexCAN_T4.h>
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can1;

// Circular buffer
const int BUFFER_SIZE = 128;
CAN_message_t canBuffer[BUFFER_SIZE];
volatile int head = 0;
volatile int tail = 0;

// Sensor variables
float rpm = 0, coolInTemp = 0, coolOutTemp = 0, batteryVoltage = 0, fuelUsed = 0;
int gear = 0;

void setup() {
  Serial.begin(115200); // optional for debugging
  Serial1.begin(250000);  // Nextion

  Can1.begin();
  Can1.setBaudRate(250000); // match MoTeC M150
  Can1.enableFIFO();
  Can1.enableFIFOInterrupt();
  Can1.onReceive(canISR);

  Serial.println("CAN + Nextion ready");
}

// ISR: store messages into buffer
void canISR(const CAN_message_t &msg) {
  int nextHead = (head + 1) % BUFFER_SIZE;
  if (nextHead != tail) {  // buffer not full
    canBuffer[head] = msg;
    head = nextHead;
  }
}

// Main loop: process all buffered messages
void loop() {
  while (tail != head) {
    CAN_message_t msg = canBuffer[tail];
    tail = (tail + 1) % BUFFER_SIZE;

    handleCANMessage(msg);
  }
}

// Parse message and send updated value to Nextion
void handleCANMessage(const CAN_message_t &msg) {
  switch (msg.id) {
    case 0x102: // RPM + gear
      rpm = extractFloat(msg.buf) / 6;
      gear = msg.buf[7];
      sendToNextion("rpmP1", rpm);
      sendToNextion("rpmP2", rpm);
      sendToNextion("gearP1", gear);
      sendToNextion("gearP2", gear);
      break;

    case 0x103: // coolant temps
      coolInTemp = extractFloat(msg.buf);
      coolOutTemp = extractFloat(msg.buf + 4);
      sendToNextion("b3", coolInTemp);
      sendToNextion("b4", coolOutTemp);
      break;

    case 0x104: // battery + fuel
      batteryVoltage = extractFloat(msg.buf);
      fuelUsed = extractFloat(msg.buf + 4);
      sendToNextion("batteryVoltage", batteryVoltage);
      sendToNextion("c1", fuelUsed);
      break;
  }
}

// Convert 4 bytes to float (big endian)
float extractFloat(const uint8_t *buf) {
  union {
    uint32_t bits;
    float num;
  } data;
  data.bits = ((uint32_t)buf[0] << 24) |
              ((uint32_t)buf[1] << 16) |
              ((uint32_t)buf[2] << 8) |
              ((uint32_t)buf[3]);
  return data.num;
}

// Send value to Nextion (numeric)
void sendToNextion(const char *objectName, float value) {
  Serial1.print(objectName);
  Serial1.print(".val=");
  Serial1.print(value);
  Serial1.write(0xFF);
  Serial1.write(0xFF);
  Serial1.write(0xFF);
}

void sendToNextion(const char *objectName, int value) {
  Serial1.print(objectName);
  Serial1.print(".val=");
  Serial1.print(value);
  Serial1.write(0xFF);
  Serial1.write(0xFF);
  Serial1.write(0xFF);
}
