#include "buttons.hpp"

#include "constants.hpp"
#include "logger.hpp"

namespace {
constexpr bool buttonPinIsActiveLow = (buttonPinType == levelType::activeLow);
}

Buttons::Buttons()
: CommandSource()
#ifndef MPR121TOUCH
//            pin             dbTime        puEnable              invert
, buttonPause(buttonPausePin, buttonDbTime, buttonPinIsActiveLow, buttonPinIsActiveLow)
, buttonUp   (buttonUpPin   , buttonDbTime, buttonPinIsActiveLow, buttonPinIsActiveLow)
, buttonDown (buttonDownPin , buttonDbTime, buttonPinIsActiveLow, buttonPinIsActiveLow)
#ifdef FIVEBUTTONS
, buttonFour (buttonFourPin , buttonDbTime, buttonPinIsActiveLow, buttonPinIsActiveLow)
, buttonFive (buttonFivePin , buttonDbTime, buttonPinIsActiveLow, buttonPinIsActiveLow)
#endif
#endif
{
#ifndef MPR121TOUCH
  buttonPause.begin();
  buttonUp   .begin();
  buttonDown .begin();
#ifdef FIVEBUTTONS
  buttonFour .begin();
  buttonFive .begin();
#endif
#endif
}

commandRaw Buttons::getCommandRaw() {
  commandRaw ret = commandRaw::none;
  readButtons();

  if ((ignoreRelease || ignoreAll) && isNoButton()) {
    ignoreAll     = false;
    ignoreRelease = false;
    longPressFactor = 0;
    return commandRaw::none;
  }

  if (ignoreAll) {
    return commandRaw::none;
  }

#ifndef MPR121TOUCH
  if ((  buttonPause.pressedFor(buttonLongPress)
      || buttonUp   .pressedFor(buttonLongPress)
      || buttonDown .pressedFor(buttonLongPress)
      )
     && buttonPause.isPressed()
     && buttonUp   .isPressed()
     && buttonDown .isPressed()) {
#else
  if ((  touchedFor(buttonLongPress, buttonPausePin)
      || touchedFor(buttonLongPress, buttonUpPin)
      || touchedFor(buttonLongPress, buttonDownPin)
      )
     && isTouched(buttonPausePin)
     && isTouched(buttonUpPin)
     && isTouched(buttonDownPin)) {
#endif
    ret = commandRaw::allLong;
    ignoreAll = true;
  }

#ifdef FIVEBUTTONS
  else if ((  buttonPause.pressedFor(buttonLongPress)
           || buttonFour .pressedFor(buttonLongPress)
           || buttonFive .pressedFor(buttonLongPress)
      )
     && buttonPause.isPressed()
     && buttonFour .isPressed()
     && buttonFive .isPressed()) {
    ret = commandRaw::allLong;
    ignoreAll = true;
  }
#endif

#ifndef MPR121TOUCH
  else if ((  buttonUp   .pressedFor(buttonLongPress)
           || buttonDown .pressedFor(buttonLongPress)
      )
     && buttonUp   .isPressed()
     && buttonDown .isPressed()) {
#else
  else if ((  touchedFor(buttonLongPress, buttonUpPin)
           || touchedFor(buttonLongPress, buttonDownPin)
      )
     && isTouched(buttonUpPin)
     && isTouched(buttonDownPin)) {
#endif
    ret = commandRaw::updownLong;
    ignoreAll = true;
  }

#ifndef MPR121TOUCH
  else if (buttonPause.wasReleased() && not ignoreRelease) {
#else
  else if (wasReleased(buttonPausePin) && not ignoreRelease) {
#endif
    ret = commandRaw::pause;
  }

#ifndef MPR121TOUCH
  else if (buttonPause.pressedFor(buttonLongPress + longPressFactor * buttonLongPressRepeat)) {
#else
  else if (touchedFor(buttonLongPress + longPressFactor * buttonLongPressRepeat, buttonPausePin)) {
#endif
    if (longPressFactor == 0)
      ret = commandRaw::pauseLong;
  }

#ifndef MPR121TOUCH
  else if (buttonUp.wasReleased() && not ignoreRelease) {
#else
  else if (wasReleased(buttonUpPin) && not ignoreRelease) {
#endif
    ret = commandRaw::up;
  }

#ifndef MPR121TOUCH
  else if (buttonUp.pressedFor(buttonLongPress + longPressFactor * buttonLongPressRepeat)) {
#else
  else if (touchedFor(buttonLongPress + longPressFactor * buttonLongPressRepeat, buttonUpPin)) {
#endif
    if (longPressFactor == 0)
      ret = commandRaw::upLong;
    else
      ret = commandRaw::upLongRepeat;
  }

#ifndef MPR121TOUCH
  else if (buttonDown.wasReleased() && not ignoreRelease) {
#else
  else if (wasReleased(buttonDownPin) && not ignoreRelease) {
#endif
    ret = commandRaw::down;
  }

#ifndef MPR121TOUCH
  else if (buttonDown.pressedFor(buttonLongPress + longPressFactor * buttonLongPressRepeat)) {
#else
  else if (touchedFor(buttonLongPress + longPressFactor * buttonLongPressRepeat, buttonDownPin)) {
#endif
    if (longPressFactor == 0)
      ret = commandRaw::downLong;
    else
      ret = commandRaw::downLongRepeat;
  }

#ifdef FIVEBUTTONS
  else if (buttonFour.wasReleased() && not ignoreRelease) {
    ret = commandRaw::four;
  }

  else if (buttonFour.pressedFor(buttonLongPress + longPressFactor * buttonLongPressRepeat)) {
    if (longPressFactor == 0)
      ret = commandRaw::fourLong;
    else
      ret = commandRaw::fourLongRepeat;
  }

  else if (buttonFive.wasReleased() && not ignoreRelease) {
    ret = commandRaw::five;
  }

  else if (buttonFive.pressedFor(buttonLongPress + longPressFactor * buttonLongPressRepeat)) {
    if (longPressFactor == 0)
      ret = commandRaw::fiveLong;
    else
      ret = commandRaw::fiveLongRepeat;
  }
#endif

  switch (ret) {
  case commandRaw::pauseLong     :
  case commandRaw::upLong        :
  case commandRaw::upLongRepeat  :
  case commandRaw::downLong      :
  case commandRaw::downLongRepeat:
#ifdef FIVEBUTTONS
  case commandRaw::fourLong      :
  case commandRaw::fourLongRepeat:
  case commandRaw::fiveLong      :
  case commandRaw::fiveLongRepeat:
#endif
                                   ++longPressFactor;
                                   ignoreRelease = true;
                                   break;
  default                        : break;
  }

  if (ret != commandRaw::none) {
    LOG(button_log, s_debug, F("Button raw: "), static_cast<uint8_t>(ret));
  }
  return ret;
}

bool Buttons::isNoButton() {
#ifndef MPR121TOUCH
  return not buttonPause.isPressed()
      && not buttonUp   .isPressed()
      && not buttonDown .isPressed()
#ifdef FIVEBUTTONS
      && not buttonFour .isPressed()
      && not buttonFive .isPressed()
#endif
#else
  return not isTouched(buttonPausePin)
      && not isTouched(buttonUpPin)
      && not isTouched(buttonDownPin)
#endif
      ;
}

bool Buttons::isReset() {
#ifndef MPR121TOUCH
  constexpr int buttonActiveLevel = getLevel(buttonPinType, level::active);
  return (digitalRead(buttonPausePin) == buttonActiveLevel &&
          digitalRead(buttonUpPin   ) == buttonActiveLevel &&
          digitalRead(buttonDownPin ) == buttonActiveLevel );
#else
  readButtons();
  return (isTouched(buttonPausePin) && isTouched(buttonUpPin) && isTouched(buttonDownPin));
#endif
}

void Buttons::readButtons() {
#ifndef MPR121TOUCH
  buttonPause.read();
  buttonUp   .read();
  buttonDown .read();
#ifdef FIVEBUTTONS
  buttonFour .read();
  buttonFive .read();
#endif
#else
  // save last state
  lasttouched = currtouched;
  // Get the currently touched pads
//  currtouched = cap.touched();
  read(TOUCH_STATUS_REGISTER_ADDRESS,currtouched);
  touchButton(buttonPausePin);
  touchButton(buttonUpPin);
  touchButton(buttonDownPin);
#endif
}

#ifdef MPR121TOUCH
void Buttons::init()
{
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  Wire.begin();
  // soft reset
  write(SRST_REGISTER_ADDRESS,0x63); // soft reset - SRST
  write(ECR_REGISTER_ADDRESS,0x0); // stop measurement - ECR
  write(MHDR_REGISTER_ADDRESS,mhd_nhd);
  write(NHDR_REGISTER_ADDRESS,mhd_nhd);
  write(NCLR_REGISTER_ADDRESS,ncl);
  write(FDLR_REGISTER_ADDRESS,fdl);
  write(MHDF_REGISTER_ADDRESS,mhd_nhd);
  write(NHDF_REGISTER_ADDRESS,mhd_nhd);
  write(NCLF_REGISTER_ADDRESS,ncl);
  write(FDLF_REGISTER_ADDRESS,fdl);
  write(NHDT_REGISTER_ADDRESS,mhd_nhd);
  write(NCLT_REGISTER_ADDRESS,ncl);
  write(FDLT_REGISTER_ADDRESS,0xff);
  write(DEBOUNCE_REGISTER_ADDRESS,0x33);
  write(AFE1_REGISTER_ADDRESS,0xD2); // 34 samples first filter iter., 18uA charge current
  write(CDC0_REGISTER_ADDRESS+2, 0x10); // 16uA charge current
  write(AFE2_REGISTER_ADDRESS, 0x54); // 1uS encoding, 10 samples second filter iter., 16ms period
//  for (uint8_t device_channel=0; device_channel<3; ++device_channel) {
    write(TOUCH_THRESHOLD0_REGISTER_ADDRESS,15);
    write(RELEASE_THRESHOLD0_REGISTER_ADDRESS,7);
    write(TOUCH_THRESHOLD0_REGISTER_ADDRESS+2,16);
    write(RELEASE_THRESHOLD0_REGISTER_ADDRESS+2,8);
    write(TOUCH_THRESHOLD0_REGISTER_ADDRESS+4,32);
    write(RELEASE_THRESHOLD0_REGISTER_ADDRESS+4,13);
//  }
  write(ECR_REGISTER_ADDRESS,B11000011);
  // auto config
  //write(device_address,ACCR0_REGISTER_ADDRESS,0x00);
  //write(device_address,ACCR1_REGISTER_ADDRESS,0x00);
  //write(device_address,USL_REGISTER_ADDRESS,200);
  //write(device_address,LSL_REGISTER_ADDRESS,130);
  //write(device_address,TL_REGISTER_ADDRESS,165);

/*    cap.writeRegister(MPR121_ECR, 0);
    cap.writeRegister(MPR121_CONFIG1, 0x11); // default, 17uA charge current
    cap.writeRegister(MPR121_CHARGECURR_0+2, 0xf); // 15uA charge current
    cap.writeRegister(MPR121_CONFIG2, 0x4c); // 4uS encoding, 16ms period
    cap.writeRegister(MPR121_DEBOUNCE, 0x33); // 3 samples for touch and release detection
    cap.writeRegister(MPR121_FDLR, 0x7f); // filter delay count rising 127
    cap.writeRegister(MPR121_FDLF, 0x7f); // filter delay count falling 127
    cap.writeRegister(MPR121_FDLT, 0x7f); // filter delay count touched 127
    //cap.writeRegister(MPR121_AUTOCONFIG0, 0x69); // enable autoconfig
    //cap.writeRegister(MPR121_UPLIMIT , 200);
    //cap.writeRegister(MPR121_LOWLIMIT , 130);
    //cap.writeRegister(MPR121_TARGETLIMIT , 165);
    cap.setThresholds(14, 7); // set new threshold values
  */
/*#ifndef FIVEBUTTONS
    cap.writeRegister(MPR121_ECR, B10000000 + 3);
#else
    cap.writeRegister(MPR121_ECR, B10000000 + 5);
#endif*/
/*
#ifdef DEBUG_MPR121
  Serial.print(cap.readRegister8(MPR121_CONFIG1), HEX);Serial.print("\t");
  Serial.print(cap.readRegister8(MPR121_CONFIG2), HEX);Serial.print("\t");
  Serial.print(cap.readRegister8(0x2));Serial.print("\t");  // out of range
  Serial.print(cap.readRegister8(MPR121_CHARGETIME_1), HEX);Serial.print("\t");
  Serial.print(cap.readRegister8(MPR121_CHARGETIME_1+1), HEX);Serial.print("\t");
  Serial.print(cap.readRegister8(MPR121_CHARGECURR_0), HEX);Serial.print("\t");
  Serial.print(cap.readRegister8(MPR121_CHARGECURR_0+1), HEX);Serial.print("\t");
  Serial.print(cap.readRegister8(MPR121_CHARGECURR_0+2), HEX);Serial.println();
#endif
*/
}
void Buttons::write(uint8_t register_address, uint8_t data)
{
  Wire.beginTransmission(device_address);
  Wire.write(register_address);
  Wire.write(data);
  Wire.endTransmission();
}

void Buttons::read(uint8_t register_address, uint8_t & data)
{
  Wire.requestFrom(device_address,(uint8_t)1);
  data = Wire.read();
}

void Buttons::touchButton(uint8_t touchNo)
{
  uint8_t index;
  uint32_t ms = millis();
  
  switch(touchNo) {
    case buttonPausePin:
      index = 0;
      break;
    case buttonUpPin:
      index = 1;
      break;
    case buttonDownPin:
      index = 2;
      break;
#ifdef FIVEBUTTONS
    case buttonFourPin:
      index = 3;
      break;
    case buttonFivePin:
      index = 4;
      break;
#endif
  }
  changedtouched[index] = ((currtouched & touchNo) != (lasttouched & touchNo));
  if(changedtouched[index]) {
    m_lastChange[index] = ms;
#ifdef DEBUG_MPR121
    Serial.print(index);
    if((currtouched & touchNo)) {
      Serial.println(F(" button pressed"));
    } else {
      Serial.println(F(" button released"));
    }
#endif
  }
  m_time[index] = ms;
}

/*----------------------------------------------------------------------*
 * isTouched(touchNo) check the touch state when it was last            *
 * read, and return false (0) or true (!=0) accordingly.                *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
bool Buttons::isTouched(uint8_t touchNo)
{
  //return touchedFor(buttonDbTime, touchNo);
  return (currtouched & touchNo);
}
/*----------------------------------------------------------------------*
 * wasReleased(touchNo) check the touch state to see if it              *
 * changed between the last two reads and return false (0) or           *
 * true (!=0) accordingly.                                              *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
bool Buttons::wasReleased(uint8_t touchNo)
{
  uint8_t index;
  switch(touchNo) {
    case buttonPausePin:
      index = 0;
      break;
    case buttonUpPin:
      index = 1;
      break;
    case buttonDownPin:
      index = 2;
      break;
#ifdef FIVEBUTTONS
    case buttonFourPin:
      index = 3;
      break;
    case buttonFivePin:
      index = 4;
      break;
#endif
  }
  return !(currtouched & touchNo) && changedtouched[index];
}
/*----------------------------------------------------------------------*
 * touchedFor(ms, touchNo) check to see if the button(touchNo) is     *
 * touched, and has been in that state for the specified  *
 * time in milliseconds. Returns false (0) or true (!=0) accordingly.   *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
bool Buttons::touchedFor(uint32_t ms, uint8_t touchNo)
{
  uint8_t index;
  switch(touchNo) {
    case buttonPausePin:
      index = 0;
      break;
    case buttonUpPin:
      index = 1;
      break;
    case buttonDownPin:
      index = 2;
      break;
#ifdef FIVEBUTTONS
    case buttonFourPin:
      index = 3;
      break;
    case buttonFivePin:
      index = 4;
      break;
#endif
  }
  return ((currtouched & touchNo) && ((m_time[index] - m_lastChange[index]) >= ms));
}
#endif