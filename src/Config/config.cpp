// Command Line Interface for the dRehmFlight Flight Control Software
//
// Guy Turcotte
// (c) 2021 - GPL 3.0

#include <cinttypes>
#include <cstring>

#include <EEPROM.h>
#include <CRC32.h>

#define __CONFIG__ 1
#include "config.h"

// ---- Modifyable variables from the dRehmFlight global data ----

//Radio failsafe values for every channel in the event that bad reciever data is detected. Recommended defaults:
extern unsigned long channel_1_fs; // = 1000; //thro
extern unsigned long channel_2_fs; // = 1500; //ail
extern unsigned long channel_3_fs; // = 1500; //elev
extern unsigned long channel_4_fs; // = 1500; //rudd
extern unsigned long channel_5_fs; // = 2000; //gear, greater than 1500 = throttle cut
extern unsigned long channel_6_fs; // = 2000; //aux1

//Filter parameters - Defaults tuned for 2kHz loop rate; Do not touch unless you know what you are doing:
extern float B_madgwick;     // = 0.04;  //Madgwick filter parameter
extern float B_accel;        // = 0.14;  //Accelerometer LP filter paramter, (MPU6050 default: 0.14. MPU9250 default: 0.2)
extern float B_gyro;         // = 0.1;   //Gyro LP filter paramter, (MPU6050 default: 0.1. MPU9250 default: 0.17)
extern float B_mag;          // = 1.0;   //Magnetometer LP filter parameter

//Magnetometer calibration parameters - if using MPU9250, uncomment calibrateMagnetometer() in void setup() to get these values, else just ignore these
extern float MagErrorX;      // = 0.0;
extern float MagErrorY;      // = 0.0; 
extern float MagErrorZ;      // = 0.0;
extern float MagScaleX;      // = 1.0;
extern float MagScaleY;      // = 1.0;
extern float MagScaleZ;      // = 1.0;

//Controller parameters (take note of defaults before modifying!): 
extern float i_limit;        // = 25.0;    //Integrator saturation level, mostly for safety (default 25.0)
extern float maxRoll;        // = 30.0;    //Max roll angle in degrees for angle mode (maximum 60 degrees), deg/sec for rate mode 
extern float maxPitch;       // = 30.0;    //Max pitch angle in degrees for angle mode (maximum 60 degrees), deg/sec for rate mode
extern float maxYaw;         // = 160.0;   //Max yaw rate in deg/sec

extern float Kp_roll_angle;  // = 0.2;     //Roll P-gain - angle mode 
extern float Ki_roll_angle;  // = 0.3;     //Roll I-gain - angle mode
extern float Kd_roll_angle;  // = 0.05;    //Roll D-gain - angle mode (if using controlANGLE2(), has no effect. Use B_loop_roll)
extern float B_loop_roll;    // = 0.9;     //Roll damping term for controlANGLE2(), lower is more damping (must be between 0 to 1)
extern float Kp_pitch_angle; // = 0.2;     //Pitch P-gain - angle mode
extern float Ki_pitch_angle; // = 0.3;     //Pitch I-gain - angle mode
extern float Kd_pitch_angle; // = 0.05;    //Pitch D-gain - angle mode (if using controlANGLE2(), has no effect. Use B_loop_pitch)
extern float B_loop_pitch;   // = 0.9;     //Pitch damping term for controlANGLE2(), lower is more damping (must be between 0 to 1)

extern float Kp_roll_rate;   // = 0.15;    //Roll P-gain - rate mode
extern float Ki_roll_rate;   // = 0.2;     //Roll I-gain - rate mode
extern float Kd_roll_rate;   // = 0.0002;  //Roll D-gain - rate mode (be careful when increasing too high, motors will begin to overheat!)
extern float Kp_pitch_rate;  // = 0.15;    //Pitch P-gain - rate mode
extern float Ki_pitch_rate;  // = 0.2;     //Pitch I-gain - rate mode
extern float Kd_pitch_rate;  // = 0.0002;  //Pitch D-gain - rate mode (be careful when increasing too high, motors will begin to overheat!)

extern float Kp_yaw;         // = 0.3;     //Yaw P-gain
extern float Ki_yaw;         // = 0.05;    //Yaw I-gain
extern float Kd_yaw;         // = 0.00015; //Yaw D-gain (be careful when increasing too high, motors will begin to overheat!)

// This is the data configuration saved in EEPROM. This reflects the parameters defined above.
// A Version number and a CRC checksum are used to validate the content.

static struct ConfigData {
  
  //Radio failsafe values for every channel in the event that bad reciever data is detected. Recommended defaults:
  unsigned long channel_1_fs; // = 1000; //thro
  unsigned long channel_2_fs; // = 1500; //ail
  unsigned long channel_3_fs; // = 1500; //elev
  unsigned long channel_4_fs; // = 1500; //rudd
  unsigned long channel_5_fs; // = 2000; //gear, greater than 1500 = throttle cut
  unsigned long channel_6_fs; // = 2000; //aux1

  //Filter parameters - Defaults tuned for 2kHz loop rate; Do not touch unless you know what you are doing:
  float B_madgwick;     // = 0.04;  //Madgwick filter parameter
  float B_accel;        // = 0.14;  //Accelerometer LP filter paramter, (MPU6050 default: 0.14. MPU9250 default: 0.2)
  float B_gyro;         // = 0.1;   //Gyro LP filter paramter, (MPU6050 default: 0.1. MPU9250 default: 0.17)
  float B_mag;          // = 1.0;   //Magnetometer LP filter parameter

  //Magnetometer calibration parameters - if using MPU9250, uncomment calibrateMagnetometer() in void setup() to get these values, else just ignore these
  float MagErrorX;      // = 0.0;
  float MagErrorY;      // = 0.0; 
  float MagErrorZ;      // = 0.0;
  float MagScaleX;      // = 1.0;
  float MagScaleY;      // = 1.0;
  float MagScaleZ;      // = 1.0;

  //Controller parameters (take note of defaults before modifying!): 
  float i_limit;        // = 25.0;    //Integrator saturation level, mostly for safety (default 25.0)
  float maxRoll;        // = 30.0;    //Max roll angle in degrees for angle mode (maximum 60 degrees), deg/sec for rate mode 
  float maxPitch;       // = 30.0;    //Max pitch angle in degrees for angle mode (maximum 60 degrees), deg/sec for rate mode
  float maxYaw;         // = 160.0;   //Max yaw rate in deg/sec

  float Kp_roll_angle;  // = 0.2;     //Roll P-gain - angle mode 
  float Ki_roll_angle;  // = 0.3;     //Roll I-gain - angle mode
  float Kd_roll_angle;  // = 0.05;    //Roll D-gain - angle mode (if using controlANGLE2(), has no effect. Use B_loop_roll)
  float B_loop_roll;    // = 0.9;     //Roll damping term for controlANGLE2(), lower is more damping (must be between 0 to 1)
  float Kp_pitch_angle; // = 0.2;     //Pitch P-gain - angle mode
  float Ki_pitch_angle; // = 0.3;     //Pitch I-gain - angle mode
  float Kd_pitch_angle; // = 0.05;    //Pitch D-gain - angle mode (if using controlANGLE2(), has no effect. Use B_loop_pitch)
  float B_loop_pitch;   // = 0.9;     //Pitch damping term for controlANGLE2(), lower is more damping (must be between 0 to 1)

  float Kp_roll_rate;   // = 0.15;    //Roll P-gain - rate mode
  float Ki_roll_rate;   // = 0.2;     //Roll I-gain - rate mode
  float Kd_roll_rate;   // = 0.0002;  //Roll D-gain - rate mode (be careful when increasing too high, motors will begin to overheat!)
  float Kp_pitch_rate;  // = 0.15;    //Pitch P-gain - rate mode
  float Ki_pitch_rate;  // = 0.2;     //Pitch I-gain - rate mode
  float Kd_pitch_rate;  // = 0.0002;  //Pitch D-gain - rate mode (be careful when increasing too high, motors will begin to overheat!)

  float Kp_yaw;         // = 0.3;     //Yaw P-gain
  float Ki_yaw;         // = 0.05;    //Yaw I-gain
  float Kd_yaw;         // = 0.00015; //Yaw D-gain (be careful when increasing too high, motors will begin to overheat!)

  uint32_t version;
  uint32_t crc;

} config_data;

const char     ESC     = 27;
const uint32_t VERSION =  1;

static MenuEntry roll_menu [] = {
  { "Max Angle",         ValueType::FLOAT, &maxRoll,       &config_data.maxRoll,       { fval: (float)  30.0    } },
  { "P-gain Angle Mode", ValueType::FLOAT, &Kp_roll_angle, &config_data.Kp_roll_angle, { fval: (float)   0.2    } },
  { "I-gain Angle Mode", ValueType::FLOAT, &Ki_roll_angle, &config_data.Ki_roll_angle, { fval: (float)   0.3    } },
  { "D-gain Angle Mode", ValueType::FLOAT, &Kd_roll_angle, &config_data.Kd_roll_angle, { fval: (float)   0.05   } },
  { "P-gain Rate Mode",  ValueType::FLOAT, &Kp_roll_rate,  &config_data.Kp_roll_rate,  { fval: (float)   0.15   } },
  { "I-gain Rate Mode",  ValueType::FLOAT, &Ki_roll_rate,  &config_data.Ki_roll_rate,  { fval: (float)   0.2    } },
  { "D-gain Rate Mode",  ValueType::FLOAT, &Kd_roll_rate,  &config_data.Kd_roll_rate,  { fval: (float)   0.0002 } },
  { "",                  ValueType::END,   nullptr,         nullptr,                                     0UL      }
};

static MenuEntry pitch_menu [] = {
  { "Max Angle",         ValueType::FLOAT, &maxPitch,       &config_data.maxPitch,       { fval: (float)  30.0    } },
  { "P-gain Angle Mode", ValueType::FLOAT, &Kp_pitch_angle, &config_data.Kp_pitch_angle, { fval: (float)   0.2    } },
  { "I-gain Angle Mode", ValueType::FLOAT, &Ki_pitch_angle, &config_data.Ki_pitch_angle, { fval: (float)   0.3    } },
  { "D-gain Angle Mode", ValueType::FLOAT, &Kd_pitch_angle, &config_data.Kd_pitch_angle, { fval: (float)   0.05   } },
  { "P-gain Rate Mode",  ValueType::FLOAT, &Kp_pitch_rate,  &config_data.Kp_pitch_rate,  { fval: (float)   0.15   } },
  { "I-gain Rate Mode",  ValueType::FLOAT, &Ki_pitch_rate,  &config_data.Ki_pitch_rate,  { fval: (float)   0.2    } },
  { "D-gain Rate Mode",  ValueType::FLOAT, &Kd_pitch_rate,  &config_data.Kd_pitch_rate,  { fval: (float)   0.0002 } },
  { "",                  ValueType::END,   nullptr,          nullptr,                                      0UL      }
};

static MenuEntry yaw_menu [] = {
  { "Max Rate",          ValueType::FLOAT, &maxYaw,         &config_data.maxYaw,         { fval: (float) 160.0     } },
  { "P-gain ",           ValueType::FLOAT, &Kp_yaw,         &config_data.Kp_yaw,         { fval: (float)   0.3     } },
  { "I-gain ",           ValueType::FLOAT, &Ki_yaw,         &config_data.Ki_yaw,         { fval: (float)   0.05    } },
  { "D-gain ",           ValueType::FLOAT, &Kd_yaw,         &config_data.Kd_yaw,         { fval: (float)   0.00015 } },
  { "",                  ValueType::END,   nullptr,          nullptr,                                      0UL       }
};

static MenuEntry ctrl_menu[] =
{
  { "Integrator Saturation Level", ValueType::FLOAT,  &i_limit, &config_data.i_limit, { fval: (float) 25.0 } },
  { "Roll",                        ValueType::MENU,  roll_menu,  nullptr,                             0UL    },
  { "Pitch",                       ValueType::MENU, pitch_menu,  nullptr,                             0UL    },
  { "Yaw",                         ValueType::MENU,   yaw_menu,  nullptr,                             0UL    },
  { "",                            ValueType::END,     nullptr,  nullptr,                             0UL    }
};

static MenuEntry fail_safe_menu[] =
{
  { "Channel 1", ValueType::ULONG, &channel_1_fs, &config_data.channel_1_fs, { uval: 1000UL } },
  { "Channel 2", ValueType::ULONG, &channel_2_fs, &config_data.channel_2_fs, { uval: 1500UL } },
  { "Channel 3", ValueType::ULONG, &channel_3_fs, &config_data.channel_3_fs, { uval: 1500UL } },
  { "Channel 4", ValueType::ULONG, &channel_4_fs, &config_data.channel_4_fs, { uval: 1500UL } },
  { "Channel 5", ValueType::ULONG, &channel_5_fs, &config_data.channel_5_fs, { uval: 2000UL } },
  { "Channel 6", ValueType::ULONG, &channel_6_fs, &config_data.channel_6_fs, { uval: 2000UL } },
  { "",          ValueType::END,    nullptr,       nullptr,                          0UL      }
};

static MenuEntry filter_menu[] =
{
  { "Madgwick",               ValueType::FLOAT, &B_madgwick, &config_data.B_madgwick, { fval: (float) 0.04 } },
  { "Accelerometer Low Pass", ValueType::FLOAT, &B_accel,    &config_data.B_accel,    { fval: (float) 0.14 } },
  { "Gyro Low Pass",          ValueType::FLOAT, &B_gyro,     &config_data.B_gyro,     { fval: (float) 0.1  } },
  { "Magnetometer Low Pass",  ValueType::FLOAT, &B_mag,      &config_data.B_mag,      { fval: (float) 1.0  } },
  { "",                       ValueType::END,    nullptr,     nullptr,                                0UL    }
};

static MenuEntry mag_menu[] =
{
  { "Error X", ValueType::FLOAT, &MagErrorX, &config_data.MagErrorX, { fval: (float) 0.0 } },
  { "Error Y", ValueType::FLOAT, &MagErrorY, &config_data.MagErrorY, { fval: (float) 0.0 } },
  { "Error Z", ValueType::FLOAT, &MagErrorZ, &config_data.MagErrorZ, { fval: (float) 0.0 } },
  { "Scale X", ValueType::FLOAT, &MagScaleX, &config_data.MagScaleX, { fval: (float) 1.0 } },
  { "Scale Y", ValueType::FLOAT, &MagScaleY, &config_data.MagScaleY, { fval: (float) 1.0 } },
  { "Scale Z", ValueType::FLOAT, &MagScaleZ, &config_data.MagScaleZ, { fval: (float) 1.0 } },
  { "",        ValueType::END,    nullptr,    nullptr,                               0UL   }
};

static MenuEntry main_menu[] = 
{
  { "Save params to EEPROM",          ValueType::SAVE,  nullptr,        nullptr, { uval: 0UL } },
  { "Reset params to default values", ValueType::RESET, nullptr,        nullptr, { uval: 0UL } },
  { "Controller Params",              ValueType::MENU,  ctrl_menu,      nullptr, { uval: 0UL } },
  { "Fail Safe Params",               ValueType::MENU,  fail_safe_menu, nullptr, { uval: 0UL } },
  { "Filter Params",                  ValueType::MENU,  filter_menu,    nullptr, { uval: 0UL } },
  { "Magnetometer Params",            ValueType::MENU,  mag_menu,       nullptr, { uval: 0UL } },
  { "Exit",                           ValueType::EXIT,  nullptr,        nullptr, { uval: 0UL } },
  { "",                               ValueType::END,   nullptr,        nullptr, { uval: 0UL } }
};

static CRC32 crc;

void 
Config::setup()
{
  if (!load_config_from_eeprom()) {
    reset_config_to_defaults(main_menu, 0);
    save_config_to_eeprom();
  }
  copy_config_to_running(main_menu, 0);

  bool found = false;

  for (int i = 10; (i > 0) && !found; i--) {
    Serial.printf("\r%d...", i);
    found = Serial.read() == '\r';
    delay(1000);
  }

  Serial.flush();
  if (found) show_main_menu();
}

bool 
Config::load_config_from_eeprom()
{
  byte * ptr = (byte *) &config_data;

  for (uint32_t i = 0; i < sizeof(config_data); i++) {
    *ptr++ = EEPROM.read(i);
  }

  uint32_t crc_sum;

  crc.reset();
  crc_sum = crc.calculate<ConfigData>(&config_data, sizeof(config_data) - sizeof(uint32_t));

  if (crc_sum != config_data.crc    ) return false;
  if (VERSION != config_data.version) return false;

  return true;
}

void Config::save_config_to_eeprom()
{
  config_data.version = VERSION;
  crc.reset();
  config_data.crc = crc.calculate<ConfigData>(&config_data, sizeof(config_data) - sizeof(uint32_t));

  byte * ptr = (byte *) &config_data;

  for (uint32_t i = 0; i < sizeof(config_data); i++) {
    EEPROM.write(i, *ptr++);
  }
}

void 
Config::copy_config_to_running(MenuEntry * menu, int level)
{
  while (menu->value_type != ValueType::END) {
    if (menu->value_type == ValueType::FLOAT) {
      *(float *) menu->ptr_running = *(float *) menu->ptr_config;
    }
    else if (menu->value_type == ValueType::ULONG) {
      *(unsigned long *) menu->ptr_running = *(unsigned long *) menu->ptr_config;
    }
    else if (menu->value_type == ValueType::MENU) {
      copy_config_to_running((MenuEntry *) menu->ptr_running, level + 1);
    }
  } 
  menu++;
}

void 
Config::reset_config_to_defaults(MenuEntry * menu, int level)
{
  while (menu->value_type != ValueType::END) {
    if (menu->value_type == ValueType::FLOAT) {
      *(float *) menu->ptr_config  = menu->value.fval;
      *(float *) menu->ptr_running = *(float *) menu->ptr_config;
    }
    else if (menu->value_type == ValueType::ULONG) {
      *(unsigned long *) menu->ptr_config  = menu->value.uval;
      *(unsigned long *) menu->ptr_running = *(unsigned long *) menu->ptr_config;
    }
    else if (menu->value_type == ValueType::MENU) {
      reset_config_to_defaults((MenuEntry *) menu->ptr_running, level + 1);
    }
    menu++;
  } 
}

bool 
Config::get_ulong(unsigned long & val)
{
  unsigned long value = 0;
  bool          ok    = false;
  char          ch;

  while (true) {
    ch = Serial.read();
    if ((ch >= '0') && (ch <= '9')) {
      value = value * 10 + (ch - '0');
      Serial.print(ch);
      ok = true;
    }
    else if ((ch == '\r') || (ch == ESC)) break;
  }

  if (ch == ESC) ok = false;
  else val = value;

  Serial.println();

  return ok;
}

bool 
Config::get_float(float & val)
{
  float value = 0;
  bool  ok    = false;
  bool  first = true;
  bool  neg   = false;
  char  ch;

  while (true) {
    ch = Serial.read();
    if (first && (ch == '-')) {
      neg = true;
    }
    first = false;
    if ((ch >= '0') && (ch <= '9')) {
      value = value * 10.0 + (ch - '0');
      Serial.print(ch);
      ok = true;
    }
    else if ((ch == ESC) || (ch == '\r') || (ch == '.')) break;
  }

  if (ch == '.') {
    float dec = 0.1;
    Serial.print('.');
    while (true) {
      ch = Serial.read();
      if ((ch >= '0') && (ch <= '9')) {
        value = value + (dec * (ch - '0'));
        dec = dec * 0.1;
        Serial.print(ch);
        ok = true;
      }
      else if ((ch == ESC) || (ch == '\r')) break;
    }
  }

  if (neg) value = -value;

  if (ch == ESC) ok = false;
  else val = value;

  Serial.println();

  return ok;
}

void 
Config::show_menu(MenuEntry * menu, const char * caption, int level) 
{
  int  len  = strlen(caption);
  bool done = false;

  while (!done) {
    Serial.println();
    Serial.println(caption);

    for (int i = 0; i < len; i++) Serial.print('-');
    Serial.println();

    uint32_t max_idx = 0;
    MenuEntry *m = menu;

    while (m->value_type != ValueType::END) {
      Serial.print(max_idx + 1);
      if (m->value_type == ValueType::FLOAT) {
        Serial.printf(" - %s (%9.5f)\n", m->caption, *(float *) m->ptr_running);
      }
      else if (m->value_type == ValueType::ULONG) {
        Serial.printf(" - %s (%lu)\n", m->caption, *(unsigned long *) m->ptr_running);
      }
      else {
        Serial.printf(" - %s\n", m->caption);
      }
      max_idx++;
      m++;
    }

    Serial.print("> ");

    unsigned long idx;
    if (get_ulong(idx) && ((idx > 0) && (idx <= max_idx))) {
      if (menu[idx - 1].value_type == ValueType::EXIT) {
        done = true;
      }
      else if (menu[idx - 1].value_type == ValueType::MENU) {
        show_menu((MenuEntry *) m->ptr_running, m->caption, level + 1);
      }
      else if (menu[idx - 1].value_type == ValueType::RESET) {
        reset_config_to_defaults(main_menu, 0);
        Serial.println("Configuration reset to default values.");
      }
      else if (menu[idx - 1].value_type == ValueType::SAVE) {
        save_config_to_eeprom();
        Serial.println("Configuration saved to EEPROM.");
      }
      else if (menu[idx - 1].value_type == ValueType::FLOAT) {
        float val;
        Serial.printf("%s (%9.5f): ", menu[idx - 1].caption, *(float *) menu[idx - 1].ptr_running);
        if (get_float(val)) {
          *(float *) menu[idx - 1].ptr_running = val;
          *(float *) menu[idx - 1].ptr_config  = val;
        }
      }
      else if (menu[idx - 1].value_type == ValueType::ULONG) {
        unsigned long val;
        Serial.printf("%s (%lu): ", menu[idx - 1].caption, *(unsigned long *) menu[idx - 1].ptr_running);
        if (get_ulong(val)) {
          *(unsigned long *) menu[idx - 1].ptr_running = val;
          *(unsigned long *) menu[idx - 1].ptr_config  = val;
        }
      }
    }
    else if (level > 0) done = true;
  }
}

void 
Config::show_main_menu() {
  show_menu(main_menu, "Main Menu", 0);
}
