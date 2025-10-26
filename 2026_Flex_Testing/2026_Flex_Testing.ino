#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1> Can1;  // Teensy CAN1

volatile uint32_t messageCount = 0;  // total messages received

void canISR(const CAN_message_t &msg) {
  // Drain FIFO in ISR
  CAN_message_t tmp;
  tmp = msg;  // include first message
  messageCount++;  

  // read remaining messages in FIFO immediately
  while (Can1.read(tmp)) {
    messageCount++;
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 4000);
  Serial.println("=== Full CAN Capture Counter ===");

  Can1.begin();
  Can1.setBaudRate(250000);  // match ECU
  Can1.enableFIFO();
  Can1.enableFIFOInterrupt();
  Can1.onReceive(canISR);    // attach ISR

  Serial.println("Listening for CAN messages...");
}

void loop() {
  static uint32_t lastPrint = 0;
  uint32_t now = millis();

  if (now - lastPrint >= 1000) {
    noInterrupts();
    uint32_t count = messageCount;
    messageCount = 0; // reset
    interrupts();

    Serial.print("Messages received this second: ");
    Serial.println(count);
    lastPrint = now;
  }
}





//--------------------------------------------------------------------------------


/*
//caught 0, then 108, then 75 
// was ran with the car already on
#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1> Can1;  // Teensy CAN1

volatile uint32_t rawMsgCount = 0;  // incremented by ISR
uint32_t lastPrint = 0;

void canISR(const CAN_message_t &msg) {
  rawMsgCount++;  // minimal work in ISR
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 4000); // wait for Serial Monitor

  Serial.println("=== High-Throughput CAN Counter ===");

  Can1.begin();
  Can1.setBaudRate(250000);     // match your ECU
  Can1.enableFIFO();
  Can1.enableFIFOInterrupt();
  //Can1.setFIFOInterruptLevel(16); // trigger ISR at max FIFO depth
  Can1.onReceive(canISR);          // attach ISR

  Serial.println("Listening for CAN messages...");
}

void loop() {
  static uint32_t lastRawCount = 0;
  uint32_t now = millis();

  // Drain FIFO as fast as possible
  CAN_message_t msg;
  while (Can1.read(msg)) {
    // optionally do something with the message
  }

  // Print messages/sec every second
  if (now - lastPrint >= 1000) {
    noInterrupts();
    uint32_t count = rawMsgCount;
    rawMsgCount = 0;
    interrupts();

    Serial.print("Messages received this second: ");
    Serial.println(count);

    lastPrint = now;
  }
} */






//--------------------------------------------------------------------------------


//also gets 75

/* #include <FlexCAN_T4.h>

FlexCAN_T4<CAN1> Can1;  // Teensy CAN1
volatile uint32_t messageCount = 0;

void canISR(const CAN_message_t &msg) {
  messageCount++;

  // read additional messages from FIFO to prevent overflow
  CAN_message_t m;
  while (Can1.read(m)) {
    messageCount++;
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 4000);
  Serial.println("=== Optimized CAN Message Counter ===");

  Can1.begin();
  Can1.setBaudRate(250000);
  Can1.enableFIFO();
  Can1.enableFIFOInterrupt();
  // no filter needed — default accepts all IDs
  Can1.onReceive(canISR);

  Serial.println("Listening for CAN messages...");
}

void loop() {
  static uint32_t lastPrint = 0;
  uint32_t now = millis();

  if (now - lastPrint >= 1000) { // every 1 second
    lastPrint = now;

    noInterrupts();
    uint32_t count = messageCount;
    messageCount = 0;
    interrupts();

    Serial.print("Messages received this second: ");
    Serial.println(count);
  }
}
*/









//--------------------------------------------------------------------------------


/*
//works (catches 75 packets per second) but need TX --> TX and RX --> RX


#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1> Can1;  // Teensy CAN1

volatile uint32_t messageCount = 0;

// ISR callback increments counter
void canISR(const CAN_message_t &msg) {
  messageCount++;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 4000); // wait for serial monitor
  Serial.println("=== Simple CAN Message Counter ===");

  Can1.begin();
  Can1.setBaudRate(250000);    // match your ECU baud
  Can1.enableFIFO();
  Can1.enableFIFOInterrupt();
  Can1.onReceive(canISR);

  Serial.println("Listening for CAN messages...");
}

void loop() {
  static uint32_t lastPrint = 0;
  uint32_t now = millis();

  if (now - lastPrint >= 1000) { // every 1 second
    lastPrint = now;

    noInterrupts();
    uint32_t count = messageCount;
    messageCount = 0; // reset counter
    interrupts();

    Serial.print("Messages received this second: ");
    Serial.println(count);
  }
}
*/



//--------------------------------------------------------------------------------


// this code works, it prints "=== Simple CAN1 Self-Test ===
//                             Sending 1 byte every second...
//                             TX 0x123 -> sent ..."
// meaning that 1. the teensy's CAN controller successfully initialized and sent a CAN frame, 2) the transreciever is powered
// and driving th ebus (no errors or bus-off) 3) no "received ID 0x123 ..." meaning the Teensy did not read back its own transmitted frame.
// but the 3rd is normal when no other CAN nodes are present.
// this is a healthy CAN transmitter

/*
#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1> Can1;   // Use CAN1: TX=22, RX=23 on Teensy 4.1

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 4000);   // Wait for Serial Monitor
  Serial.println("=== Simple CAN1 Self-Test ===");

  // --- Initialize CAN1 ---
  Can1.begin();
  Can1.setBaudRate(250000);             // Try 250000 or 500000 depending on setup
  Can1.enableFIFO();
  Can1.enableFIFOInterrupt();

  // --- Receive callback ---
  Can1.onReceive([](const CAN_message_t &msg) {
    Serial.print("Received ID 0x");
    Serial.print(msg.id, HEX);
    Serial.print("  Data: ");
    for (uint8_t i = 0; i < msg.len; i++) {
      if (msg.buf[i] < 0x10) Serial.print('0');
      Serial.print(msg.buf[i], HEX);
      Serial.print(' ');
    }
    Serial.println();
  });

  Serial.println("Sending 1 byte every second...");
}

void loop() {
  static uint32_t t = millis();
  if (millis() - t > 1000) {
    t = millis();

    CAN_message_t msg;
    msg.id = 0x123;
    msg.len = 1;
    msg.buf[0] = 0xAB;

    bool ok = Can1.write(msg);
    Serial.print("TX 0x123 -> ");
    Serial.println(ok ? "sent" : "failed");
  }
}


*/

