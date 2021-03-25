#pragma once

// Config Management and
// Command Line Interface for the dRehmFlight Flight Control Software
//
// Guy Turcotte
// (c) February 2021 - GPL 3.0

#include <cinttypes>

#define RIGHT_ELEVATOR_CENTER  0.48
#define LEFT_ELEVATOR_CENTER   0.46

#define RIGHT_AILERON_CENTER   0.56
#define RIGHT_AILERON_BOTTOM   0.95
#define RIGHT_AILERON_45       0.87

#define LEFT_AILERON_CENTER    0.55
#define LEFT_AILERON_BOTTOM    0.05
#define LEFT_AILERON_45        0.22

#define FRONT_MOTOR_CENTER     0.57

#if DEBUGGING
  #define DEBUG(str) Serial.println(str); Serial.flush()
  const int WAITING_SECONDS = 50;
#else
  #define DEBUG(str)
  const int WAITING_SECONDS = 10;
#endif

enum class ValueType : int8_t { 
  END, ULONG, FLOAT, SELECT, MENU, RESET, SAVE, LIST, SERVO, MOTOR, CALIB, EXIT
};  

struct SelectEntry {
  const __FlashStringHelper * caption;
};

struct MenuEntry {
  const __FlashStringHelper * caption;
  const __FlashStringHelper * name;
  ValueType                   value_type;
  void                      * ptr_running;
  void                      * ptr_config;
  SelectEntry               * select_entries;
  union {
    unsigned long uval;
    float         fval;
  } value;
};

class Config
{
  private:
    bool some_parameter_changed;

    uint32_t  show_select_entries(SelectEntry * select_entries, const __FlashStringHelper * caption);
    uint32_t         display_menu(MenuEntry   * menu,           const __FlashStringHelper * caption);
    void                show_menu(MenuEntry   * menu,           const __FlashStringHelper * caption, int level);
    void                  get_str(char * buff, int size, ValueType type);
    bool                get_ulong(unsigned long & val);
    bool                get_float(float & val);
    bool                      ask(const __FlashStringHelper * question, bool default_value);
    bool  load_config_from_eeprom();
    void    save_config_to_eeprom();
    void reset_config_to_defaults(MenuEntry * menu, int level);
    void   copy_config_to_running(MenuEntry * menu, int level);
    void              list_params(MenuEntry * menu, const __FlashStringHelper * caption, int level);

  public:
    Config() : some_parameter_changed(false) { }
   ~Config() { }

    void setup();
    void show_main_menu();
};

#if __CONFIG__
  Config config;
#else
  extern Config config;
#endif
