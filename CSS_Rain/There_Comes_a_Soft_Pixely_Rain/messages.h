#pragma once
#include <FlexCAN_T4.h>


#define INPUT_LEVELS_MSG 0
typedef struct {
  uint8_t rainFlow;
  uint8_t toiletFlow;
  uint8_t washerFlow;
  uint8_t dishWasherFlow;
  uint8_t showerFlow;
} InputLevels;
void processInputLevels(uint8_t src, InputLevels levels);
void sendInputLevels(uint8_t src, InputLevels levels);


void setupCan();
void sendMessage(uint8_t type, uint8_t nodeId, void *data);
void processMessage(const CAN_message_t &msg);
