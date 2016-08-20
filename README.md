# SimpleBotSocketIo
Simple ESP8266 based robot teleoperated using socket.io


To connect to your network create a header file called personalWifi, comment in SOFTAP_MODE def to create a personal network for ESP

    // personalWifi.h
    #ifdef SOFTAP_MODE
    const char* password = "softAPpassword";
    #else
    const char* ssid[] = {
      "firstWifiName"
      "WhatIsYourName",
      "WhatISYourFavoriteColor",
      "WhatISYourQuest",
    };

    const char* password[] = {
      "firstWifiPassword"
      "Bobby",
      "Green",
      "UMMM...NOOOOOOOO!"
    };
    #endif

    // String SocketHost = "yourServer.herokuapp.com"; // relpace "yourServer" with YOUR server
    String SocketHost = "192.168.1.x"; // replace x with your locally hosted server address
    int SocketPort = 3000; // if connecting to a site remove this from connect event // change to 80 for heroku or the like

A collaborative project between Paul Beaudet (https://github.com/PaulBeaudet) and Jay Francis (https://github.com/robojay).
This is an experimental robot side for Paul's telezumo (https://github.com/PaulBeaudet/telezumo) project.
