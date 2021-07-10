#include <CanBusMCP2515_asukiaaa.h>
#ifndef PIN_CS
#define PIN_CS 10
#endif
#ifndef PIN_INT
#define PIN_INT 7
#endif

static const auto QUARTZ_FREQUENCY =
    CanBusMCP2515_asukiaaa::QuartzFrequency::MHz16;
static const auto BITRATE = CanBusMCP2515_asukiaaa::BitRate::Kbps125;
static const uint16_t SENDER_ID = 55;

CanBusMCP2515_asukiaaa::Driver can(PIN_CS);

void setup() {
  CanBusMCP2515_asukiaaa::Settings settings(QUARTZ_FREQUENCY, BITRATE);
  settings.mOperationMode = CanBusMCP2515_asukiaaa::OperationMode::LoopBack;
  Serial.begin(9600);
  Serial.println("settings:");
  Serial.println(settings.toString());
  // while(!Serial) { delay(10); }
  while (true) {
    // uint16_t errorCode = can.begin(settings);
    uint16_t errorCode = can.begin(settings, PIN_INT);
    // uint16_t errorCode = can.begin(settings, PIN_INT, [] { can.isr(); });
    if (errorCode == 0) break;
    Serial.print("Configuration error: ");
    Serial.println(CanBusMCP2515_asukiaaa::Error::toString(errorCode));
    delay(1000);
  }
  Serial.print("Succeeced in beginning");
}

void loop() {
  static unsigned long trySendAt = 0;
  static const unsigned long intervalMs = 1000UL;
  if (trySendAt == 0 || millis() - trySendAt > intervalMs) {
    trySendAt = millis();
    CanBusData_asukiaaa::Frame frame;
    frame.id = SENDER_ID;
    frame.data64 = millis();
    // frame.idx = frame.data64 % 3;
    const bool ok = can.tryToSend(frame);
    Serial.println("Sent");
    Serial.println(frame.toString());
    Serial.print(ok ? "Succeeded" : "Failed");
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
