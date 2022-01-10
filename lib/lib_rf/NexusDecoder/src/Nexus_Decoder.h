/*
  Decoder for Nexus Protocol
*/

// Nexus parameters
typedef struct NEXUS_DATA {
  uint16_t Timestamp;
  uint8_t  ID;
  uint8_t  Battery;
  uint8_t  Channel;
  uint8_t  Humidity;
  float    Temperature;
} NEXUS_DATA;


void config_receiver (uint8_t pin);
NEXUS_DATA decode_nexus_data (void);
void IRAM_ATTR Nexus_Decoder (void);
extern volatile uint8_t int_enable;
