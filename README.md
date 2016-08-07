# SimpleBotSocketIo
Simple ESP8266 based robot teleoperated using socket.io


To connect to your network create a header file called personalWifi, comment in SOFTAP_MODE def to create a personal network for ESP

    // personalWifi.h
    // #define SOFTAP_MODE

    #ifdef SOFTAP_MODE
    const char* password = "myMinion";
    #else
    const char* ssid = "SkyNet";
    const char* password = "myMaster";
    #endif

    String SocketHost = "192.168.4.2";
    int SocketPort = 3000;


A collaborative project between Paul Beaudet (https://github.com/PaulBeaudet) and Jay Francis (https://github.com/robojay).
This is an experimental robot side for Paul's telezumo (https://github.com/PaulBeaudet/telezumo) project.
