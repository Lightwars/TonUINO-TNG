#ifndef SRC_BUTTONS_HPP_
#define SRC_BUTTONS_HPP_

#include <Arduino.h>

#include "commands.hpp"
#include "constants.hpp"

#ifndef MPR121TOUCH
#include <JC_Button.h>
#else
#include <Wire.h>
#endif

class Buttons: public CommandSource {
public:
  Buttons();

  commandRaw getCommandRaw() override;
  bool isNoButton();
  bool isReset();
#ifdef MPR121TOUCH
  void init();
#endif

private:

  void readButtons();
#ifdef MPR121TOUCH
  void touchButton(uint8_t touchNo);
  bool isTouched(uint8_t touchNo);
  bool wasReleased(uint8_t touchNo);
  bool touchedFor(uint32_t ms, uint8_t touchNo);
  void write(uint8_t register_address, uint8_t data);
  void read(uint8_t register_address, uint8_t & data);
#endif

#ifndef MPR121TOUCH
  Button buttonPause;
  Button buttonUp   ;
  Button buttonDown ;
#ifdef FIVEBUTTONS
  Button  buttonFour;
  Button  buttonFive;
#endif
#else
  // Keeps track of the last pins touched
  // so we know when buttons are 'released'
  uint8_t lasttouched = 0;
  uint8_t currtouched = 0;
  bool changedtouched[3] = {false, false, false};
  uint32_t m_time[3];        // time of current state (ms from millis)
  uint32_t m_lastChange[3];  // time of last state change (ms)
  const static uint8_t device_address = 0x5A; // mpr121 address
  const static uint8_t mhd_nhd = 0x01; //
  const static uint8_t ncl = 0x10; //
  const static uint8_t fdl = 0x20; //

  // register addresses
  const static uint8_t TOUCH_STATUS_REGISTER_ADDRESS = 0x00;
  
  // general electrode touch sense baseline filters
  // rising filter
  const static uint8_t MHDR_REGISTER_ADDRESS = 0x2B;
  const static uint8_t NHDR_REGISTER_ADDRESS = 0x2C;
  const static uint8_t NCLR_REGISTER_ADDRESS = 0x2D;
  const static uint8_t FDLR_REGISTER_ADDRESS = 0x2E;

  // falling filter
  const static uint8_t MHDF_REGISTER_ADDRESS = 0x2F;
  const static uint8_t NHDF_REGISTER_ADDRESS = 0x30;
  const static uint8_t NCLF_REGISTER_ADDRESS = 0x31;
  const static uint8_t FDLF_REGISTER_ADDRESS = 0x32;

  // touched filter
  const static uint8_t NHDT_REGISTER_ADDRESS = 0x33;
  const static uint8_t NCLT_REGISTER_ADDRESS = 0x34;
  const static uint8_t FDLT_REGISTER_ADDRESS = 0x35;

  // // electrode touch and release thresholds
  const static uint8_t TOUCH_THRESHOLD0_REGISTER_ADDRESS = 0x41;
  const static uint8_t RELEASE_THRESHOLD0_REGISTER_ADDRESS = 0x42;

  // debounce settings
  const static uint8_t DEBOUNCE_REGISTER_ADDRESS = 0x5B;
  // // configuration registers
  const static uint8_t AFE1_REGISTER_ADDRESS = 0x5C;
  const static uint8_t AFE2_REGISTER_ADDRESS = 0x5D;
  const static uint8_t ECR_REGISTER_ADDRESS = 0x5E;
    // electrode currents
  const static uint8_t CDC0_REGISTER_ADDRESS = 0x5F;
    // electrode charge times
  const static uint8_t CDT0_REGISTER_ADDRESS = 0x6C;

    // auto-config
 /* const static uint8_t ACCR0_REGISTER_ADDRESS = 0x7B;
  const static uint8_t ACCR1_REGISTER_ADDRESS = 0x7C;
  const static uint8_t USL_REGISTER_ADDRESS = 0x7D;
  const static uint8_t LSL_REGISTER_ADDRESS = 0x7E;
  const static uint8_t TL_REGISTER_ADDRESS = 0x7F;*/

  // soft reset
  const static uint8_t SRST_REGISTER_ADDRESS = 0x80;
#endif
  bool ignoreRelease     = false;
  bool ignoreAll         = false;

  uint8_t longPressFactor = 0;
};

#endif /* SRC_BUTTONS_HPP_ */
