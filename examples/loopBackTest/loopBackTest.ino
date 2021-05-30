#include <CanBusMCP2515_asukiaaa.h>
#define PIN_CS 10
#define PIN_INT 7

static const auto QUARTZ_FREQUENCY =
    CanBusMCP2515_asukiaaa::QuartzFrequency::MHz16;
static const auto BITRATE = CanBusMCP2515_asukiaaa::BitRate::Kbps125;
static const uint16_t SENDER_ID = 55;

CanBusMCP2515_asukiaaa::Driver can(PIN_CS, PIN_INT);

void setup() {
  CanBusMCP2515_asukiaaa::Settings settings(QUARTZ_FREQUENCY, BITRATE);
  settings.mOperationMode = CanBusMCP2515_asukiaaa::OperationMode::LoopBack;
  Serial.println("settings:");
  Serial.println(settings.toString());
  const uint16_t errorCode = can.begin(settings, [] { can.isr(); });
  if (errorCode == 0) {
    Serial.print("Succeeced in beginning");
  } else {
    Serial.print("Configuration error 0x");
    Serial.println(errorCode, HEX);
  }
}

void loop() {
  static unsigned long trySendAt = 0;
  static const unsigned long intervalMs = 1000UL;
  if (trySendAt == 0 || millis() - trySendAt > intervalMs) {
    trySendAt = millis();
    CanBusData_asukiaaa::Frame frame;
    frame.id = SENDER_ID;
    frame.data64 = millis();
    frame.idx = frame.data64 % 3;
    const bool ok = can.tryToSend(frame);
    Serial.println("Send");
    Serial.println(frame.toString());
    Serial.print(ok ? "succeeded" : "failed");
    Serial.print(" at ");
    Serial.println(millis());
  }
  if (can.available()) {
    CanBusData_asukiaaa::Frame frame;
    can.receive(&frame);
    Serial.println("Received");
    Serial.println(frame.toString());
    Serial.print("at ");
    Serial.println(millis());
  }
  delay(10);
}
