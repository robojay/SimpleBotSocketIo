#include <JS_Timer.h>
#include <SocketIOClient.h>

//
// Define the motor driver pins
// Right will go to the right motor (Output A on the DRV8835)
// Left will go to the left motor (Output B on the DRV8835)
//
const uint8_t RightIn1Pin = 15;
const uint8_t RightIn2Pin = 14;
const uint8_t LeftIn1Pin = 12;
const uint8_t LeftIn2Pin = 13;

const String CmdMotion         = "M";
const uint8_t CmdBackLeft      = 1;
const uint8_t CmdBack          = 2;
const uint8_t CmdBackRight     = 3;
const uint8_t CmdSpinLeft      = 4;
const uint8_t CmdStop          = 5;
const uint8_t CmdSpinRight     = 6;
const uint8_t CmdForwardLeft   = 7;
const uint8_t CmdForward       = 8;
const uint8_t CmdForwardRight  = 9;

const String CmdSpeed          = "S";
const uint8_t CmdSpeed1        = 1;
const uint8_t CmdSpeed2        = 2;
const uint8_t CmdSpeed3        = 3;
const uint8_t CmdSpeed4        = 4;

const String CmdControl        = "C";
const uint8_t CmdHorn          = 1;

const uint32_t StatusTime      = 1000;
