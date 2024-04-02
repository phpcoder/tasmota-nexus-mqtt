/*
  xdrv_103_nexus_mqtt.ino - Nexus 433MHz to MQTT bridge for Tasmota
  Updated for Tasmota 13.4.0
*/


#ifdef USE_NEXUS_MQTT
/*********************************************************************************************\
 * Nexus 433MHz to MQTT bridge for Tasmota
 * Decodes Nexus/Rubiscon Temperature-Humidity Sensor Protocol
 * Sends temperature-humidity data as MQTT messages
\*********************************************************************************************/

#warning **** Nexus 433MHz to MQTT bridge Driver is included... ****

#define XDRV_103 103

#define NEXUS_RX_PIN  14    // D5 - GPIO14: pin connected to the 433MHz receiver

#include "Nexus_Decoder.h"


/*********************************************************************************************\
 * Nexus 433MHz to MQTT bridge Functions
\*********************************************************************************************/

char * payload = nullptr;
size_t payload_size = 100;
char * topic = nullptr;
size_t topic_size = 50;
bool initSuccess = false;

NEXUS_DATA nexus_data_prev;


int check_nexus_data ()
{
  NEXUS_DATA nexus_data;

  nexus_data = decode_nexus_data();

  if ((nexus_data_prev.ID == nexus_data.ID) && ((nexus_data.Timestamp - nexus_data_prev.Timestamp) < 800) ) {

    return -1; // repeated data

  } else {

    nexus_data_prev = nexus_data;

    return 0;

  }

}



/*********************************************************************************************\
 * Tasmota Functions
\*********************************************************************************************/



void NexusToMqttInit()
{


  payload = (char*)calloc(payload_size, sizeof(char));
  topic = (char*)calloc(topic_size, sizeof(char));

  if (!payload || !topic) {
    AddLog(LOG_LEVEL_DEBUG, PSTR("Nexus 433MHz to MQTT bridge: out of memory"));
    return;
  }

  // Configure gpio pin number and mode
  config_receiver(NEXUS_RX_PIN);

  initSuccess = true;

  AddLog(LOG_LEVEL_DEBUG, PSTR("Nexus 433MHz to MQTT bridge: init successful"));

}



void NexusToMqttProcessing(void)
{

  if (int_enable == 0) {


    if (check_nexus_data() == 0) {


      // Floating point is not formatted in sprintf/arduino
      char str_temp[6];
      // 3 is mininum width, 1 is precision; float value is copied onto str_temp
      dtostrf(nexus_data_prev.Temperature, 3, 1, str_temp);


      snprintf_P(topic, topic_size, PSTR("test/sensor/NEXUS_%X/state"), nexus_data_prev.ID);


      snprintf_P(payload, payload_size, 
        PSTR("{\"" D_JSON_TIME "\":\"%s\",\"proto\":\"NEXUS\",\"id\":\"%X\",\"temp\":\"%s\",\"hum\":\"%d\"}"), 
        GetDateAndTime(DT_LOCAL).c_str(), nexus_data_prev.ID, str_temp, nexus_data_prev.Humidity);

      // retain = true
      MqttPublishPayload(topic, payload, payload_size, true);


    }

    int_enable = 1;

  }

}






/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xdrv103(uint32_t function)
{


  bool result = false;

  if (FUNC_INIT == function) {
    NexusToMqttInit();
  }
  else if (initSuccess) {

    switch (function) {
  //    Select suitable interval for polling your function
      case FUNC_EVERY_SECOND:
  //    case FUNC_EVERY_250_MSECOND:
  //    case FUNC_EVERY_200_MSECOND:
  //    case FUNC_EVERY_100_MSECOND:
        NexusToMqttProcessing();
        break;
    }

  }

  return result;
}

#endif  // USE_NEXUS_MQTT
