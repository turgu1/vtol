#pragma once

// Config Management and
// Command Line Interface for the dRehmFlight Flight Control Software
//
// Guy Turcotte
// (c) February 2021 - GPL 3.0

#include <cinttypes>

enum class ValueType : int8_t { END, ULONG, FLOAT, SELECT, MENU, RESET, SAVE, LIST, EXIT };  

struct SelectEntry {
  const char * caption;
};

struct MenuEntry {
  const char    * caption;
  const char    * name;
  ValueType       value_type;
  void          * ptr_running;
  void          * ptr_config;
  SelectEntry   * select_entries;
  union {
    unsigned long uval;
    float         fval;
  } value;
};

class Config
{
  private:
    bool some_parameter_changed;

    uint32_t         display_menu(MenuEntry * menu, const char * caption);
    uint32_t  show_select_entries(SelectEntry * select_entries, const char * caption);
    void                show_menu(MenuEntry * menu, const char * caption, int level);
    void                  get_str(char * buff, int size, ValueType type);
    bool                get_ulong(unsigned long & val);
    bool                get_float(float & val);
    bool                      ask(const char * question, bool default_value);
    bool  load_config_from_eeprom();
    void    save_config_to_eeprom();
    void reset_config_to_defaults(MenuEntry * menu, int level);
    void   copy_config_to_running(MenuEntry * menu, int level);
    void              list_params(MenuEntry * menu, const char * caption, int level);

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
