// SimpleBotSocketIo.ino ~ Succesfully built w/ Arduino IDE 1.6.10
#include "SimpleBot.h"
#include "personalWifi.h"

const char HexLookup[17] = "0123456789ABCDEF"; // look up mac hex
char uniqueID[14] = "Minion-000000";           // unique id to populate w/ mac address

SocketIOClient socket;
JS_Timer timer = JS_Timer();

void setupNetwork() {
  #ifdef SOFTAP_MODE
    WiFi.disconnect();
    byte mac[6];
    WiFi.macAddress(mac);
    char ssid[14] = "Minion-000000";
    ssid[7]  = HexLookup[(mac[3] & 0xf0) >> 4];
    ssid[8]  = HexLookup[(mac[3] & 0x0f)];
    ssid[9]  = HexLookup[(mac[4] & 0xf0) >> 4];
    ssid[10] = HexLookup[(mac[4] & 0x0f)];
    ssid[11] = HexLookup[(mac[5] & 0xf0) >> 4];
    ssid[12] = HexLookup[(mac[5] & 0x0f)];
    ssid[13] = 0;
    WiFi.softAP(ssid, password);
  #else
    WiFi.begin(ssid[0], password[0]);
    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED && i++ < 20) { delay(500); }
    if(i == 21){
      while(1) { delay(500); }
    }
  #endif
}

void findWifi(){              // recursively checks for a connection every x millis
  static byte network = 0;    // which network to try
  static byte attempt = 0;    // how many attempts have been made on any given network

  if (WiFi.status() == WL_CONNECTED){
    socket.connect(SocketHost, SocketPort);
    attempt = 0;              // start from 0 attempts after connected
    timer.setTimeout(checkWifi, 1000);
  } else {
    if(attempt){
      if ( attempt > 20 ){    // after 2 seconds worth of attempts
        if(network > 2 ){ network = 0;}
        else{network++;}
        attempt = 0;
      } else {
        attempt++;
      }
    } else { // given no attempts have been made
      socket.disconnect();
      WiFi.begin(ssid[network], password[network]);
      attempt++;
    }
    timer.setTimeout(findWifi, 500);
  }

}

void checkWifi(){ // checks status of wifi to determine whether it needs to be found again
  if (WiFi.status() == WL_CONNECTED){
    timer.setTimeout(checkWifi, 1000);
  } else {
    timer.setTimeout(findWifi, 500);
  }
}

// stop the motors
void stop() {
  left(0);
  right(0);
}

void straight(float percent) {
  left(percent);
  right(percent);
}

// pivot in place
// counter-clockwise is positive
void pivot(float percent) {
  left(-percent);
  right(percent);
}

// drive left motors
// percent should be from -1 to 1
// positive is forward (A terminal positive)
void left(float percent) {
  static float lastPwm = 0.0;
  static bool lastForward = true;

  percent = constrain(percent, -1.0, 1.0);
  bool forward = (percent >= 0) ? true : false;
  uint16_t pwm = PWMRANGE - int(abs(percent) * PWMRANGE);
  // only change pwm and direction if it is different
  if ( (pwm != lastPwm) || (forward != lastForward) ) {
    lastPwm = pwm;
    lastForward = forward;
    if (forward) {
      analogWrite(LeftIn1Pin, PWMRANGE);
      analogWrite(LeftIn2Pin, pwm);
    }
    else {
      analogWrite(LeftIn1Pin, pwm);
      analogWrite(LeftIn2Pin, PWMRANGE);
    }
  }
}


// drive right motors
// percent should be from -1 to 1
// positive is forward (A terminal positive)
void right(float percent) {
  static float lastPwm = 0.0;
  static bool lastForward = true;

  percent = constrain(percent, -1.0, 1.0);
  bool forward = (percent >= 0) ? true : false;
  uint16_t pwm = PWMRANGE - int(abs(percent) * PWMRANGE);
  // only change pwm and direction if it is different
  if ( (pwm != lastPwm) || (forward != lastForward) ) {
    lastPwm = pwm;
    lastForward = forward;
    if (forward) {
      analogWrite(RightIn1Pin, PWMRANGE);
      analogWrite(RightIn2Pin, pwm);
    }
    else {
      analogWrite(RightIn1Pin, pwm);
      analogWrite(RightIn2Pin, PWMRANGE);
    }
  }
}


//
// socket events
//
void remote(String data) {
  static float speedSetting = 1.0;
  float l, r;

  if (data.length() != 2){ return; }

  if (data.startsWith(CmdSpeed)) {
    bool valid = true;
    data.remove(0, 1);
    switch (data.toInt()) {
      case CmdSpeed1:
        speedSetting = 0.25;
        break;
      case CmdSpeed2:
        speedSetting = 0.50;
        break;
      case CmdSpeed3:
        speedSetting = 0.75;
        break;
      case CmdSpeed4:
        speedSetting = 1.0;
        break;
      default:
        valid = false;
        break;
    }
  }
  else if (data.startsWith(CmdMotion)) {
    bool valid = true;
    data.remove(0, 1);
    switch(data.toInt()) {
      case CmdBackLeft:
        l = -speedSetting/2;
        r = -speedSetting;
        break;
      case CmdBack:
        l = -speedSetting;
        r = -speedSetting;
        break;
      case CmdBackRight:
        l = -speedSetting;
        r = -speedSetting/2;
        break;
      case CmdSpinLeft:
        l = -speedSetting;
        r = speedSetting;
        break;
      case CmdStop:
        l = 0;
        r = 0;
        break;
      case CmdSpinRight:
        l = speedSetting;
        r = -speedSetting;
        break;
      case CmdForwardLeft:
        l = speedSetting/2;
        r = speedSetting;
        break;
      case CmdForward:
        l = speedSetting;
        r = speedSetting;
        break;
      case CmdForwardRight:
        l = speedSetting;
        r = speedSetting/2;
        break;
      default:
        valid = false;
        break;
    }
    if (valid) {
      left(l);
      right(r);
    }
  }
  else if (data.startsWith(CmdControl)) {
    bool valid = true;
    data.remove(0, 1);
    switch(data.toInt()) {
      case CmdHorn:
        break;
      default:
        valid = false;
        break;
    }
    if (valid) {
      //
    }
  }
}

String status = "open";
String master = "";
String ID = "{\"id\":";               // first line item of JSON
String StatusKey = ", \"status\":\""; // middle line item
String BotType = "\", \"type\":\"";   // last line item defines head/body

void getUniqueID(){
  WiFi.disconnect();
  byte mac[6];
  WiFi.macAddress(mac);
  uniqueID[7]  = HexLookup[(mac[3] & 0xf0) >> 4];
  uniqueID[8]  = HexLookup[(mac[3] & 0x0f)];
  uniqueID[9]  = HexLookup[(mac[4] & 0xf0) >> 4];
  uniqueID[10] = HexLookup[(mac[4] & 0x0f)];
  uniqueID[11] = HexLookup[(mac[5] & 0xf0) >> 4];
  uniqueID[12] = HexLookup[(mac[5] & 0x0f)];
  uniqueID[13] = 0;
  String UID = String(uniqueID);
  BotType = BotType + UID + "\"}";    // Complete contstruction of Json messages
}

void botFind(String from) {
  socket.emit("here", ID + "\"" + from + "\"" + StatusKey + status + BotType);
}

void botConnect(){
  status = "open";
  socket.emit("here", ID + "false" + StatusKey + status + BotType);
}

void own(String from) {
  master = from;
  status = "taken";
  socket.emit("here", ID + "false" + StatusKey + status + BotType);
}

void relinquish(String from) {
  if (master == from) {
    master = "";
    if (status == "taken") {
      status = "open";
      socket.emit("here", ID + "false" + StatusKey + status + BotType);
    }
  }
}

//
// This code runs only once
//
void setup() {
  // set up our output pins
  pinMode(RightIn1Pin, OUTPUT);
  pinMode(RightIn2Pin, OUTPUT);
  pinMode(LeftIn1Pin, OUTPUT);
  pinMode(LeftIn2Pin, OUTPUT);
  stop(); // inialize the output pins

  Serial.begin(115200);

  // setupNetwork();
  getUniqueID();
  findWifi();

  socket.on("botFind", botFind);
  socket.on("own", own);
  socket.on("relinquish", relinquish);
  socket.on("remote", remote);
  socket.emit("here", ID + "false" + StatusKey + status + BotType);
}

//
// This code runs over and over again
//
void loop() {
  timer.todoChecker();
  if (WiFi.status() == WL_CONNECTED){
    if(socket.monitor()){ // returns true on connect
        botConnect();      // signal open on connect
    }
  }
}

