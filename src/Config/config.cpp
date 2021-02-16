// Config Management and
// Command Line Interface for the dRehmFlight Flight Control Software
//
// Guy Turcotte
// (c) February 2021 - GPL 3.0

#include <cinttypes>
#include <cstring>

#include <EEPROM.h>
#include <CRC32.h>

#define __CONFIG__ 1
#include "config.h"

// ---- Modifyable variables from the dRehmFlight global data ----

extern unsigned long USB_output; // = 0;

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
  
  unsigned long USB_output; // = 0;

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

const char     ESC     =  27;
const char     BS      =   8;
const char     LF      =  10;
const char     CR      =  13;
const char     DEL     = 127;

const uint32_t VERSION =   1;

static SelectEntry output_select[] = {
  "None",
  "Target State",
  "Gyro Data",
  "Accelerometer Data",
  "Magnetometer Data",
  "Roll, Pitch, Yaw from Madgwick",
  "Computed PID stabilization",
  "Motors' Commands",
  "Servos' Commands",
  "Loop Duration",
   nullptr
};

static MenuEntry debug_menu[] = {
  { "USB Data Output", "USB_output", ValueType::SELECT, &USB_output, &config_data.USB_output, output_select, { uval: 0UL } },
  { "",                 nullptr,     ValueType::END,     nullptr,     nullptr,                nullptr,               0UL   }
};

static MenuEntry roll_menu [] = {
  { "Max Angle",         "maxRoll",       ValueType::FLOAT, &maxRoll,       &config_data.maxRoll,       nullptr, { fval: (float)  30.0    } },
  { "P-gain Angle Mode", "Kp_roll_angle", ValueType::FLOAT, &Kp_roll_angle, &config_data.Kp_roll_angle, nullptr, { fval: (float)   0.2    } },
  { "I-gain Angle Mode", "Ki_roll_angle", ValueType::FLOAT, &Ki_roll_angle, &config_data.Ki_roll_angle, nullptr, { fval: (float)   0.3    } },
  { "D-gain Angle Mode", "Kd_roll_angle", ValueType::FLOAT, &Kd_roll_angle, &config_data.Kd_roll_angle, nullptr, { fval: (float)   0.05   } },
  { "P-gain Rate Mode",  "Kp_roll_rate",  ValueType::FLOAT, &Kp_roll_rate,  &config_data.Kp_roll_rate,  nullptr, { fval: (float)   0.15   } },
  { "I-gain Rate Mode",  "Ki_roll_rate",  ValueType::FLOAT, &Ki_roll_rate,  &config_data.Ki_roll_rate,  nullptr, { fval: (float)   0.2    } },
  { "D-gain Rate Mode",  "Kd_roll_rate",  ValueType::FLOAT, &Kd_roll_rate,  &config_data.Kd_roll_rate,  nullptr, { fval: (float)   0.0002 } },
  { "Loop Damping",      "B_loop_roll",   ValueType::FLOAT, &B_loop_roll,   &config_data.B_loop_roll,   nullptr, { fval: (float)   0.9    } },
  { "",                   nullptr,        ValueType::END,   nullptr,         nullptr,                   nullptr,                   0UL      }
};

static MenuEntry pitch_menu [] = {
  { "Max Angle",         "maxPitch",       ValueType::FLOAT, &maxPitch,       &config_data.maxPitch,       nullptr, { fval: (float)  30.0    } },
  { "P-gain Angle Mode", "Kp_pitch_angle", ValueType::FLOAT, &Kp_pitch_angle, &config_data.Kp_pitch_angle, nullptr, { fval: (float)   0.2    } },
  { "I-gain Angle Mode", "Ki_pitch_angle", ValueType::FLOAT, &Ki_pitch_angle, &config_data.Ki_pitch_angle, nullptr, { fval: (float)   0.3    } },
  { "D-gain Angle Mode", "Kd_pitch_angle", ValueType::FLOAT, &Kd_pitch_angle, &config_data.Kd_pitch_angle, nullptr, { fval: (float)   0.05   } },
  { "P-gain Rate Mode",  "Kp_pitch_rate",  ValueType::FLOAT, &Kp_pitch_rate,  &config_data.Kp_pitch_rate,  nullptr, { fval: (float)   0.15   } },
  { "I-gain Rate Mode",  "Ki_pitch_rate",  ValueType::FLOAT, &Ki_pitch_rate,  &config_data.Ki_pitch_rate,  nullptr, { fval: (float)   0.2    } },
  { "D-gain Rate Mode",  "Kd_pitch_rate",  ValueType::FLOAT, &Kd_pitch_rate,  &config_data.Kd_pitch_rate,  nullptr, { fval: (float)   0.0002 } },
  { "Loop Damping",      "B_loop_pitch",   ValueType::FLOAT, &B_loop_pitch,   &config_data.B_loop_pitch,   nullptr, { fval: (float)   0.9    } },
  { "",                   nullptr,         ValueType::END,   nullptr,          nullptr,                    nullptr,                   0UL      }
};

static MenuEntry yaw_menu [] = {
  { "Max Rate",          "maxYaw", ValueType::FLOAT, &maxYaw, &config_data.maxYaw, nullptr, { fval: (float) 160.0     } },
  { "P-gain",            "Kp_yaw", ValueType::FLOAT, &Kp_yaw, &config_data.Kp_yaw, nullptr, { fval: (float)   0.3     } },
  { "I-gain",            "Ki_yaw", ValueType::FLOAT, &Ki_yaw, &config_data.Ki_yaw, nullptr, { fval: (float)   0.05    } },
  { "D-gain",            "Kd_yaw", ValueType::FLOAT, &Kd_yaw, &config_data.Kd_yaw, nullptr, { fval: (float)   0.00015 } },
  { "",                   nullptr, ValueType::END,   nullptr,  nullptr,            nullptr,                   0UL       }
};

static MenuEntry ctrl_menu[] =
{
  { "Integrator Saturation Level", "i_limit", ValueType::FLOAT,  &i_limit, &config_data.i_limit, nullptr, { fval: (float) 25.0 } },
  { "Roll",                         nullptr,  ValueType::MENU,  roll_menu,  nullptr,             nullptr,                 0UL    },
  { "Pitch",                        nullptr,  ValueType::MENU, pitch_menu,  nullptr,             nullptr,                 0UL    },
  { "Yaw",                          nullptr,  ValueType::MENU,   yaw_menu,  nullptr,             nullptr,                 0UL    },
  { "",                             nullptr,  ValueType::END,     nullptr,  nullptr,             nullptr,                 0UL    }
};

static MenuEntry fail_safe_menu[] =
{
  { "Channel 1", "channel_1_fs", ValueType::ULONG, &channel_1_fs, &config_data.channel_1_fs, nullptr, { uval: 1000UL } },
  { "Channel 2", "channel_2_fs", ValueType::ULONG, &channel_2_fs, &config_data.channel_2_fs, nullptr, { uval: 1500UL } },
  { "Channel 3", "channel_3_fs", ValueType::ULONG, &channel_3_fs, &config_data.channel_3_fs, nullptr, { uval: 1500UL } },
  { "Channel 4", "channel_4_fs", ValueType::ULONG, &channel_4_fs, &config_data.channel_4_fs, nullptr, { uval: 1500UL } },
  { "Channel 5", "channel_5_fs", ValueType::ULONG, &channel_5_fs, &config_data.channel_5_fs, nullptr, { uval: 2000UL } },
  { "Channel 6", "channel_6_fs", ValueType::ULONG, &channel_6_fs, &config_data.channel_6_fs, nullptr, { uval: 2000UL } },
  { "",           nullptr,       ValueType::END,    nullptr,       nullptr,                  nullptr,         0UL      }
};

static MenuEntry filter_menu[] =
{
  { "Madgwick",               "B_madgwick", ValueType::FLOAT, &B_madgwick, &config_data.B_madgwick, nullptr, { fval: (float) 0.04 } },
  { "Accelerometer Low Pass", "B_accel",    ValueType::FLOAT, &B_accel,    &config_data.B_accel,    nullptr, { fval: (float) 0.14 } },
  { "Gyro Low Pass",          "B_gyro",     ValueType::FLOAT, &B_gyro,     &config_data.B_gyro,     nullptr, { fval: (float) 0.1  } },
  { "Magnetometer Low Pass",  "B_mag",      ValueType::FLOAT, &B_mag,      &config_data.B_mag,      nullptr, { fval: (float) 1.0  } },
  { "",                        nullptr,     ValueType::END,    nullptr,     nullptr,                nullptr,                 0UL    }
};

static MenuEntry mag_menu[] =
{
  { "Error X", "MagErrorX", ValueType::FLOAT, &MagErrorX, &config_data.MagErrorX, nullptr, { fval: (float) 0.0 } },
  { "Error Y", "MagErrorY", ValueType::FLOAT, &MagErrorY, &config_data.MagErrorY, nullptr, { fval: (float) 0.0 } },
  { "Error Z", "MagErrorZ", ValueType::FLOAT, &MagErrorZ, &config_data.MagErrorZ, nullptr, { fval: (float) 0.0 } },
  { "Scale X", "MagScaleX", ValueType::FLOAT, &MagScaleX, &config_data.MagScaleX, nullptr, { fval: (float) 1.0 } },
  { "Scale Y", "MagScaleY", ValueType::FLOAT, &MagScaleY, &config_data.MagScaleY, nullptr, { fval: (float) 1.0 } },
  { "Scale Z", "MagScaleZ", ValueType::FLOAT, &MagScaleZ, &config_data.MagScaleZ, nullptr, { fval: (float) 1.0 } },
  { "",         nullptr,    ValueType::END,    nullptr,    nullptr,               nullptr,                 0UL   }
};

static MenuEntry main_menu[] = 
{
  { "Controller Params",              nullptr, ValueType::MENU,  ctrl_menu,      nullptr, nullptr, { uval: 0UL } },
  { "Fail Safe Params",               nullptr, ValueType::MENU,  fail_safe_menu, nullptr, nullptr, { uval: 0UL } },
  { "Filter Params",                  nullptr, ValueType::MENU,  filter_menu,    nullptr, nullptr, { uval: 0UL } },
  { "Magnetometer Params",            nullptr, ValueType::MENU,  mag_menu,       nullptr, nullptr, { uval: 0UL } },
  { "Debug Params",                   nullptr, ValueType::MENU,  debug_menu,     nullptr, nullptr, { uval: 0UL } },
  { "Save params to EEPROM",          nullptr, ValueType::SAVE,  nullptr,        nullptr, nullptr, { uval: 0UL } },
  { "Reset params to default values", nullptr, ValueType::RESET, nullptr,        nullptr, nullptr, { uval: 0UL } },
  { "List all params",                nullptr, ValueType::LIST,  nullptr,        nullptr, nullptr, { uval: 0UL } },
  { "Exit",                           nullptr, ValueType::EXIT,  nullptr,        nullptr, nullptr, { uval: 0UL } },
  { "",                               nullptr, ValueType::END,   nullptr,        nullptr, nullptr,         0UL   }
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
    char ch = Serial.read();
    found = (ch == CR) || (ch == LF);

    if (!found) {
      Serial.printf("\r%d... ", i); 
      Serial.flush();
      delay(1000);
    }
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

void 
Config::save_config_to_eeprom()
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
    else if (menu->value_type == ValueType::SELECT) {
      *(unsigned long *) menu->ptr_running = *(unsigned long *) menu->ptr_config;
    }
    else if (menu->value_type == ValueType::MENU) {
      copy_config_to_running((MenuEntry *) menu->ptr_running, level + 1);
    }

    menu++;
  } 
}

void 
Config::list_params(MenuEntry * menu, const char * caption, int level)
{
  bool first = true;
  while (menu->value_type != ValueType::END) {
    if (menu->value_type == ValueType::FLOAT) {
      if (first) {
        first = false;
        Serial.printf("\n// %s\n\n", caption);
      }
      Serial.printf("%-15s = %10.5f  // %s\n", 
                    menu->name, 
                    *(float *) menu->ptr_running, 
                    menu->caption);
    }
    else if (menu->value_type == ValueType::ULONG) {
      if (first) {
        first = false;
        Serial.printf("\n// %s\n\n", caption);
      }
      Serial.printf("%-15s = %10lu  // %s\n", 
                    menu->name, 
                    *(unsigned long *) menu->ptr_running, 
                    menu->caption);
    }
    else if (menu->value_type == ValueType::SELECT) {
      if (first) {
        first = false;
        Serial.printf("\n// %s\n\n", caption);
      }
      Serial.printf("%-15s = %10lu  // %s -> %s\n", 
                    menu->name, 
                    *(unsigned long *) menu->ptr_running, 
                    menu->caption, 
                    menu->select_entries[*(unsigned long *) menu->ptr_running].caption);
    }
    else if (menu->value_type == ValueType::MENU) {
      list_params((MenuEntry *) menu->ptr_running, menu->caption, level + 1);
    }

    menu++;
  } 
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
    else if (menu->value_type == ValueType::SELECT) {
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
Config::ask(const char * question, bool default_value)
{
  int pos = 0;
  char ans;
  char ch;

  Serial.printf("%s (%c/%c): ", question, default_value ? 'Y' : 'y', default_value ? 'n' : 'N');

  bool done = false;
  while (!done) {
    if ((ch = Serial.read()) != -1) {
      if ((pos == 1) && ((ch == BS) || (ch == DEL))) {
        pos = 0;
        Serial.print(BS);
        Serial.print(' ');
        Serial.print(BS);
      }
      else if ((pos == 0) && ((ch == 'Y') || (ch == 'y') || (ch == 'N') || (ch == 'n'))) {
        Serial.print(ch);
        ans = ch;
        pos = 1;
      }
      else if ((ch == '\r') || (ch == '\n') || (ch == ESC)) {
        done = true;
      }
    }
  }

  Serial.println();

  if (pos == 0) return default_value;
  else return ((ans == 'Y') || (ans == 'y'));
}

void 
Config::get_str(char * buff, int size, ValueType type)
{
  int  pos  = 0;
  bool done = false;
  bool dot  = false;
  char ch;

  size--;

  while (!done) {

    ch = Serial.read();

    if (ch != -1) {
      //Serial.printf("[%d]", ch);
      if ((ch == CR) || (ch == LF) || (ch == ESC)) {
        done = true;
      }
      else if ((type == ValueType::FLOAT) && (ch == '-') && (pos == 0)) {
        buff[pos++] = '-';
        Serial.print('-');
      }
      else if ((type == ValueType::FLOAT) && (ch == '.') && !dot) {
        dot = true;
        buff[pos++] = '.';
        Serial.print('.');
      }
      else if ((ch >= '0') && (ch <= '9')) {
        buff[pos++] = ch;
        Serial.print(ch);
      }
      else if ((ch == BS) || (ch == DEL)) {
        if (pos > 0) {
          Serial.print(BS);
          Serial.print(' ');
          Serial.print(BS);
          pos--;
        }
      }
    }

    done = done || (pos >= size);
  }

  Serial.println();
  buff[pos] = 0;
}

bool 
Config::get_ulong(unsigned long & val)
{
  char buff[20];

  get_str(buff, 30, ValueType::ULONG);
  if (buff[0] == 0) return false;
  val = atol(buff);

  return true;
}

bool 
Config::get_float(float & val)
{
  char buff[20];

  get_str(buff, 30, ValueType::FLOAT);
  if (buff[0] == 0) return false;
  val = atof(buff);

  return true;
}

uint32_t 
Config::show_select_entries(SelectEntry * select_entries, const char * caption)
{
  uint32_t max_idx = 0;
  
  Serial.printf("\n%s\nPlease select one of the following:\n\n", caption);

  while (select_entries->caption != nullptr) {
    Serial.printf("%3d - %s\n", max_idx, select_entries->caption);
    select_entries++;
    max_idx++;
  }
  Serial.println("---");

  return max_idx;
}

uint32_t 
Config::display_menu(MenuEntry * menu, const char * caption) 
{
  int  len  = strlen(caption);

  Serial.println();
  Serial.println(caption);

  for (int i = 0; i < len; i++) Serial.print('-');
  Serial.println();

  uint32_t max_idx = 0;

  while (menu->value_type != ValueType::END) {

    max_idx++;

    if (menu->value_type == ValueType::FLOAT) {
      Serial.printf("%d - (Parm) %s [%s](%.5f)\n", max_idx, menu->caption, menu->name, *(float *) menu->ptr_running);
    }
    else if (menu->value_type == ValueType::ULONG) {
      Serial.printf("%d - (Parm) %s [%s](%lu)\n", max_idx, menu->caption, menu->name, *(unsigned long *) menu->ptr_running);
    }
    else if (menu->value_type == ValueType::SELECT) {
      Serial.printf("%d - (Parm) %s [%s](%d: %s)\n", 
                    max_idx, 
                    menu->caption, 
                    menu->name, 
                    *(unsigned long *) menu->ptr_running,
                    menu->select_entries[*(unsigned long *) menu->ptr_running].caption);
    }
    else if (menu->value_type == ValueType::MENU) {
      Serial.printf("%d - (Menu) %s\n", max_idx, menu->caption);
    }
    else if (menu->value_type != ValueType::EXIT) {
      Serial.printf("%d - (Task) %s\n", max_idx, menu->caption);
    }
    else {
      Serial.printf("%d - %s\n", max_idx, menu->caption);
    }

    menu++;
  }

  return max_idx;
}

void 
Config::show_menu(MenuEntry * menu, const char * caption, int level) 
{
  bool done = false;

  while (!done) {
    uint32_t max_idx = display_menu(menu, caption);

    Serial.print("> ");

    unsigned long idx;
    if (get_ulong(idx) && ((idx > 0) && (idx <= max_idx))) {
      if (menu[idx - 1].value_type == ValueType::EXIT) {
        done = true;
        if (some_parameter_changed) {
          if (ask("Some parameter changed. Save to EEPROM?", false)) {
            save_config_to_eeprom();
            some_parameter_changed = false;
            Serial.println("Configuration has been saved to EEPROM.");
          }
          else {
            Serial.println("Configuration has NOT been saved.");
          }
        }
      }
      else if (menu[idx - 1].value_type == ValueType::MENU) {
        show_menu((MenuEntry *) menu[idx - 1].ptr_running, menu[idx - 1].caption, level + 1);
      }
      else if (menu[idx - 1].value_type == ValueType::RESET) {
        if (ask("Resetting configuration to default values. Are you sure?", false)) {
          reset_config_to_defaults(main_menu, 0);
          Serial.println("Configuration reset to default values.");
          some_parameter_changed = true;
        }
        else Serial.println("Reset not done.");
      }
      else if (menu[idx - 1].value_type == ValueType::SAVE) {
        if (ask("Saving configuration to EEPROM. Are you sure?", false)) {
          save_config_to_eeprom();
          Serial.println("Configuration saved to EEPROM.");
          some_parameter_changed = false;
        }
        else Serial.println("Configuration not saved.");
      }
      else if (menu[idx - 1].value_type == ValueType::LIST) {
        list_params(main_menu, "Main Menu", 0);
      }
      else if (menu[idx - 1].value_type == ValueType::FLOAT) {
        float val;
        Serial.printf("%s [%s](%.5f): ", menu[idx - 1].caption, menu[idx - 1].name, *(float *) menu[idx - 1].ptr_running);
        if (get_float(val)) {
          *(float *) menu[idx - 1].ptr_running = val;
          *(float *) menu[idx - 1].ptr_config  = val;
          some_parameter_changed = true;
        }
      }
      else if (menu[idx - 1].value_type == ValueType::ULONG) {
        unsigned long val;
        Serial.printf("%s [%s](%lu): ", menu[idx - 1].caption, menu[idx - 1].name, *(unsigned long *) menu[idx - 1].ptr_running);
        if (get_ulong(val)) {
          *(unsigned long *) menu[idx - 1].ptr_running = val;
          *(unsigned long *) menu[idx - 1].ptr_config  = val;
          some_parameter_changed = true;
        }
      }
      else if (menu[idx - 1].value_type == ValueType::SELECT) {
        unsigned long val;
        max_idx = show_select_entries(menu[idx - 1].select_entries, menu[idx - 1].caption);
        Serial.printf("%s [%s](%lu: %s): ", 
                      menu[idx - 1].caption, 
                      menu[idx - 1].name,
                      *(unsigned long *) menu[idx - 1].ptr_running, 
                      menu[idx - 1].select_entries[*(unsigned long *) menu[idx - 1].ptr_running].caption);
        if (get_ulong(val) && (val < max_idx)) {
          *(unsigned long *) menu[idx - 1].ptr_running = val;
          *(unsigned long *) menu[idx - 1].ptr_config  = val;
          some_parameter_changed = true;
        }
      }
    }
    else if (level > 0) done = true;
  }
}

void 
Config::show_main_menu() 
{
  show_menu(main_menu, "Main Menu", 0);
}
