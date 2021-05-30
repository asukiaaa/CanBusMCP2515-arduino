#include <CanBusMCP2515_asukiaaa.h>
#define PIN_CS 10
#define PIN_INT 7

static const uint32_t QUARTZ_FREQUENCY = 16UL * 1000UL * 1000UL;  // 16 MHz

CanBusMCP2515_asukiaaa::Driver can(PIN_CS, PIN_INT);

void setup() {
  CanBusMCP2515_asukiaaa::Settings settings(
      QUARTZ_FREQUENCY,
      125UL * 1000UL);  // CAN bit rate 125 kb/s
  settings.mRequestedMode =
      CanBusMCP2515_asukiaaa::Settings::LoopBackMode;  // Select loopback mode
  const uint16_t errorCode = can.begin(settings, [] { can.isr(); });
  if (errorCode == 0) {
    Serial.print("Bit Rate prescaler: ");
    Serial.println(settings.mBitRatePrescaler);
    Serial.print("Propagation Segment: ");
    Serial.println(settings.mPropagationSegment);
    Serial.print("Phase segment 1: ");
    Serial.println(settings.mPhaseSegment1);
    Serial.print("Phase segment 2: ");
    Serial.println(settings.mPhaseSegment2);
    Serial.print("SJW: ");
    Serial.println(settings.mSJW);
    Serial.print("Triple Sampling: ");
    Serial.println(settings.mTripleSampling ? "yes" : "no");
    Serial.print("Actual bit rate: ");
    Serial.print(settings.actualBitRate());
    Serial.println(" bit/s");
    Serial.print("Exact bit rate ? ");
    Serial.println(settings.exactBitRate() ? "yes" : "no");
    Serial.print("Sample point: ");
    Serial.print(settings.samplePointFromBitStart());
    Serial.println("%");
  } else {
    Serial.print("Configuration error 0x");
    Serial.println(errorCode, HEX);
  }
}

void loop() {
  CanBusData_asukiaaa::Frame frame;
  static unsigned long trySendAt = 0;
  static const unsigned long intervalMs = 1000UL;
  if (trySendAt == 0 || millis() - trySendAt > intervalMs) {
    trySendAt = millis();
    const bool ok = can.tryToSend(frame);
    if (ok) {
      Serial.print("SentAt: ");
      Serial.println(millis());
    } else {
      Serial.print("Send failure at ");
      Serial.println(millis());
    }
  }
  if (can.available()) {
    CanBusData_asukiaaa::Frame receivedFrame;
    can.receive(&receivedFrame);
    Serial.print("ReceivedAt: ");
    Serial.println(millis());
  }
}
