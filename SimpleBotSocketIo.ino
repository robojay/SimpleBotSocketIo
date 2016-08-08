// SimpleBotSocketIo.ino ~ Succesfully built w/ Arduino IDE 1.6.10
#include "SimpleBot.h"
#include "personalWifi.h"

const char HexLookup[17] = "0123456789ABCDEF";

SocketIOClient socket;
JS_Timer timer = JS_Timer();

void setupNetwork() {
  #ifdef SOFTAP_MODE
    WiFi.disconnect();
    byte mac[6];
    WiFi.macAddress(mac);
    char ssid[14] = "Minion-000000";
    ssid[7] = HexLookup[(mac[3] & 0xf0) >> 4];
    ssid[8] = HexLookup[(mac[3] & 0x0f)];
    ssid[9] = HexLookup[(mac[4] & 0xf0) >> 4];
    ssid[10] = HexLookup[(mac[4] & 0x0f)];
    ssid[11] = HexLookup[(mac[5] & 0xf0) >> 4];
    ssid[12] = HexLookup[(mac[5] & 0x0f)];
    ssid[13] = 0;
    WiFi.softAP(ssid, password);
  #else
    WiFi.begin(ssid, password);
    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
    if(i == 21){
      while(1) delay(500);
    }
  #endif
}

// stop the motors
void stop() {
  left(0);
  right(0);
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
String ID = "{\"id\":";                    // first line item of JSON
String StatusKey = ", \"status\":\"";      // middle line item
String BotType = "\", \"type\":\"base\"}"; // last line item defines head/body

void botFind(String from) {
  socket.emit("here", ID + "\"" + from + "\"" + StatusKey + status + BotType);
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
  // inialize the output pins
  stop();

  Serial.begin(115200);

  setupNetwork();

  socket.connect(SocketHost, SocketPort);
  socket.on("botFind", botFind);
  socket.on("own", own);
  socket.on("relinquish", relinquish);
  socket.on("remote", remote);
}

//
// This code runs over and over again
//
void loop() {
  socket.monitor();
}

