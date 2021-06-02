#include <esp_now.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <Update.h>
//#include <WebOTA.h>
//#include <WiFiManager.h>
//#include <DNSServer.h>
//#include <WebServer.h>
#include <esp_attr.h>
#include <Preferences.h>
Preferences prefs;

#define SERIAL_ON
//WebServer server(80);
//WiFiManager local intialization
//WiFiManager wifiManager;
//ERASE_WIFI_SETTINGS
//Uncomment and run this program once to erase the WiFi configuration(s). Don't forget to comment out again
//wifiManager.resetSettings();

/****************** iDefinitions shared with wearable, make sure to keep in sync *****************/
// TODO - mode to a shared header file 

#define MAX_MESSAGE 250
unsigned char in_message[MAX_MESSAGE];

enum message_type_e {
    SCORE,
    SCREEN_CHANGE,
    STEPS,
    HEART_RATE,
    /* Add new message types at the end */
};

typedef struct score_data_s {
  unsigned char message_type;
  unsigned char player_index;
  unsigned int score; 
} score_data_t;

typedef struct screen_change_data_s {
  unsigned char message_type;
  unsigned char screen; // TBD
} screen_change_data_t;
/****************** iDefinitions shared with wearable, make sure to keep in sync *****************/
void IRAM_ATTR OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  //static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  //memcpy(&buttonData, incomingData, sizeof(buttonData));
  //vTaskNotifyGiveFromISR(wearMonTask_handle, &xHigherPriorityTaskWoken); 
  //portYIELD_FROM_ISR();
  /* 
   *  First byte in incoming data from wearable is always message type 
   *  From that we can derive the lengthof the message
   *  Or perhaps use TLV format instead ? TBD
   */
  unsigned char message_type = (unsigned char)*incomingData;
  Serial.printf("Receied message type %d\n", message_type);
  switch (message_type) {
    case SCORE:
      score_data_t score_msg;
      memcpy(in_message, incomingData, sizeof(score_data_t));
      score_msg.player_index = in_message[1];
      score_msg.score = ntohl(*(unsigned int*)&(in_message[2]));
      Serial.printf("Score %d player %d\n", score_msg.score, score_msg.player_index);
      break;
    case SCREEN_CHANGE:
      screen_change_data_t screen_change_msg;
      memcpy(in_message, incomingData, sizeof(screen_change_data_t));
      screen_change_msg.screen = in_message[1];
      Serial.printf("Screen %d\n", screen_change_msg.screen);
      break;
    default:
      Serial.printf("message type %d not supported\n", message_type);
      break;
  }
}

void setup() {
  // put your setup code here, to run once:
#ifdef SERIAL_ON
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // wait for serial port to connect. Needed for native USB port only
  }
#endif
  //Set the WiFi to station Mode
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());
  //Initialize ESP-NOW
  if(esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
    //FUTURE_UPDATE: here we can have special screen on touchscreen to say WiFi error
  }
#ifdef SERIAL_ON
  Serial.println("ESP-NOW initalized");
#endif
  esp_now_register_recv_cb(OnDataRecv);
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
