#include "messages.h"
#include <FlexCAN_T4.h>
static FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> MsgCan;


void setupCan() {
  MsgCan.begin();
  MsgCan.setBaudRate(1000000);
  MsgCan.setMaxMB(16);
  MsgCan.enableFIFO();
  MsgCan.enableFIFOInterrupt();
  MsgCan.onReceive(processMessage);
}


void sendMessage(uint8_t type, uint8_t nodeId, void *data, size_t sz) {
  CAN_message_t msg;
  msg.id = (type << 4) | nodeId;
  memcpy(msg.buf, data, sz);
  msg.len = sz;
  MsgCan.write(msg);
}


void processMessage(const CAN_message_t &msg) {
  uint8_t type = msg.id >> 4;
  uint8_t src = msg.id & 0xf;
  switch(type) {
  case INPUT_LEVELS_MSG:
    InputLevels levels;
    memcpy(&levels, msg.buf, sizeof(InputLevels));
    processInputLevels(src, levels);
    break;

  default:
    // do nothing
    Serial.print("WARNING: unknown message type: "); Serial.println(type);
    break;
  }
}



void sendInputLevels(uint8_t src, InputLevels levels) {
  sendMessage(INPUT_LEVELS_MSG, src, &levels, sizeof(InputLevels));
}
