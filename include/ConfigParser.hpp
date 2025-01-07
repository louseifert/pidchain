/* *
 * This software has a Copyright 2024 to Louis C Seifert III all rights reserved 
 * it is freely distributable under the MIT license. If you use this api: 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
 * documentation files (the “Software”), to deal in the Software without restriction, including without 
 * limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT 
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH 
 * THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _CONFIG_PARSER_HPP  //NOLINT
#define _CONFIG_PARSER_HPP  //NOLINT
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include <regex> //NOLINT
#include <cctype>
#include <set>
#include <string>
#include <ParserExceptions.hpp>
#define NOKEY "The key does not exist."
#define STROPVARS "^[A-Z=a-z0-9\"\\-_.~]*$"
#define STROPFLAGS "^[A-Za-z0-9-_.]*"
#define NAN "NaN: Not a number"
namespace std {

/**
 * @class string_ops
 * @brief string operations
 * String Helper and processing class, string_ops
 * contains functions that are in plenty of other libraries specifically
 * written for use with this library.
 */
class string_ops {
 private:
  regex re;
  regex flag;

 public:
  /**
   * @brief sets the default regex for the class 
   * this will supercede regex it is created with.
   * TODO(lou):remove this?
   */
  void set_default_regex() {
    regex re(STROPVARS);
    regex flag(STROPFLAGS);
    this->re = re;
  }

 public:
  /**
   * @brief Default constructor that sets the default pattern to match against
   */
  string_ops() : re(STROPVARS), flag(STROPFLAGS) {}

  /**
   * @brief Constructor that accepts a string pattern to match against
   * @param option_pattern regex string for options
   * @param flag_pattern regex string for flags 
   */
  string_ops(string option_pattern, string flag_pattern)
      : re(option_pattern), flag(flag_pattern) {}

  /**
   * @brief Regex match function accepts a string pointer and a regex pattern
   * and returns true if the pattern matches standard alphanumeric
   * or what ever is specified when the string_ops object is created.
   * @param *s pointer to a string 
   * @return bool
   */
  bool strip_check(string *s) {
    string_ops::trim(s);
    string_ops::strip_qoutes(s);
    return default_match(*s);
  }

  /**
   * @brief Regex match function accepts a string and a regex pattern
   * and returns true if the pattern matches the string.
   * @param s string 
   * @return bool
   */
  bool default_match(const string &s) { return regex_match(s, re); }

  /**
   * @brief Right Trim Function accepts a pointer to a string;
   * @param *s pointer to a string
   */
  static void rtrim(string *s) {
    while (isspace(s->at(s->size() - 1))) {
      s->pop_back();
    }
  }

  /**
   *
   * @brief Right trim function accepts a string and returns a trimmed string
   * consider using the pointer version for performance reasons.
   * @param s string 
   * @return string
   */
  static string rtrim(string s) {
    rtrim(&s);
    return s;
  }

  /**
   * @brief Left trim function accepts a string pointer operated on the string in place
   * @param *s pointer to a string
   */
  static void ltrim(string *s) {
    uint32_t pos = 0;
    while (isspace(s->at(pos)) && pos < s->size()) {
      pos++;
    }
    s->erase(0, pos);
  }

  /**
   * @brief Left trim function accepts a string and returns a trimmed string
   * consider using the pointer version of this function for performance
   * reasons.
   * @param s string 
   * @return string
   */
  static string ltrime(string s) {
    ltrim(&s);
    return s;
  }

  /**
   * @brief Trim function accepts a string pointer and trims the string in place
   * @param *s pointer to a string
   */
  static void trim(string *s) {
    rtrim(s);
    ltrim(s);
  }

  /**
   * @brief Trim function accepts a string and returns a trimmed string
   * consider using the pointer version of this function for performance
   * reasons.
   * @param s string 
   * @returns string
   */
  static string trim(string s) {
    trim(&s);
    return s;
  }

  /**
   * @brief Strips one set of qoutes on both ends of the variable
   * @param *s pointer to a string
   */
  static void strip_qoutes(string *s) {
    if ((s->at(0) == '"' && s->at(s->length() - 1) == '"') ||
        (s->at(0) == '\'' && s->at(s->length() - 1) == '\'')) {
      s->erase(0, 1);
      s->pop_back();
    }
  }

  /**
   * @brief Strips one set of qoutes on both ends of the string
   * @param s string
   * @return string
   */
  static string strip_qoutes(string s) {
    strip_qoutes(&s);
    return s;
  }
};



/*******************************************************************
 * @class ConfigParser 
 * @brief class is built for parsing a standard INI,
 * planned support for other file formats
 */
class ConfigParser {
 private:
  /** @brief map of options internal */
  map<std::string, std::string> options;
  /** @brief flags set*/
  set<string> flags;
  /** @brief default config file */
  string configFile = "";
  /** @brief map of restricted internal */
  map<std::string, std::string> restricted;
  /** @brief allows override of restricted lock */
  bool allow_override = false;
  /** @brief allows the parser to read 
   * writeable files comes with security risks */
  bool allow_writeable = false;
  /** @brief allows access to restricted list */
  bool allow_restricted = false;
  /** @brief prevents more than one ini from being loaded per object */
  bool ini_loaded = false;
  /** @brief internal string ops */
  string_ops stringops;
  /** @brief keep track of params loaded by clp */
  int clp_parm_count = 0;
  /** @brief keep track of parm count vs restricted */
  int ini_parm_count = 0;
  /** @brief track restricted count */
  int restricted_count = 0;

 public:
  enum FILETYPE { INI, JSON, XML, TEST };
  /**
   * @brief plain constructor
   */
  ConfigParser() {}

  /**
   * @brief parses standard c/c++ arguements in the constructor
   * constructors that allow override of default behavior
   * takes argc **argv
   * @param c (int argc,
   * @param *v[]  **argv) 
   * */
  ConfigParser(int c, char *v[]) { parse(c, v); }

  /**
   * @brief By default the regex allows a limited superset of alpha num
   * Anything that has characters that fall outside of standard path and
   * alphanum is placed in restricted and not accessible except by specifying
   * that it is allowed in the constructor, this is so
   * if the object is passed to other parts in the code it can't
   * access them except through the interface.
   * @param _allow_restricted bool
   */
  explicit ConfigParser(bool _allow_restricted) {
    this->allow_restricted = _allow_restricted;
  }

  /**
   * @brief this allows processing of the command line and allowing access to
   * the restricted/filtered ini or clp settings. takes argc and **argv
   * @param c int argc from main
   * @param *v[] **argv from main
   * @param _allow_restricted
   */
  ConfigParser(int c, char *v[], bool _allow_restricted) {
    this->allow_restricted = _allow_restricted;
    parse(c, v);
  }
  /**
   * @brief this allows processing of the command line and allowing access to
   * the restricted/filtered ini or clp settings.
   * @param c int argc from main clp
   * @param *v[] **argv from main clp
   * @param _allow_restricted allows access to restricted vars
   * @param string_ops_values the regex to determine valid string
   * @param string_ops_flags regex to determin valid flags TODO
   */
  ConfigParser(int c, char *v[], bool _allow_restricted,
               string string_ops_values, string string_ops_flags)
      : stringops(string_ops_values, string_ops_flags) {
    parse(c, v);
    this->allow_restricted = _allow_restricted;
  }
  /**
   * @brief parses both settings and clp options have precidence over ini on identical
   * identifiers takes filename type argc argv
   * TODO(lou): needs to be completed or removed.
   * @param filename that will be loaded
   * @param type of file for switching parsers
   * @param c int argc
   * @param *v[] **argv
   * */
  ConfigParser(string filename, FILETYPE type, int c, char *v[]) {
    parse(c, v);
    // TODO(lou): FINISH IMPLEMENTATION OF TYPES
    if (type == ConfigParser::FILETYPE::INI) {
      load_ini(filename);
    } else if (type == ConfigParser::FILETYPE::TEST) {
      cout << "Parser FILETYPE is TEST TYPE that prints this line" << endl;
    }
  }

  /**
   * @brief Parses a config file with basic INI structure lines starting with ; and #
   * are comments, category or sections are bracketed [section]
   * when accessing them by the get_string or get int function
   * variables under section heads [title] are accessed by title.variablename
   * See test cases for basic usage
   * This function will throw an exception if the file doesn't exist,
   * or the file is writeable by the executor
   * This enforced /etc/ security best practice.
   * @param filename of file to be loaded for parameters
   */
  void load_ini(string filename) {
    if (ini_loaded) {
      throw security_exception("you can only load an ini file once");
    }
    /**
     * Don't try and open the file, and throw an exception,
     * if the file does not exist or is not a regular file.
     */
    if (!std::filesystem::exists(filename) ||
        !std::filesystem::is_regular_file(filename)) {
      throw file_access_exception(
          (const string) "file either does not exist or is not a regular file");
    }

    /**
     * We don't want the user executing the executable to be able to write to
     * the ini by default
     */
    std::ofstream ofile(filename, std::ios::out);
    if (!allow_writeable && ofile.is_open()) {
      ofile.close();
      throw security_exception((
          const string) "Config files should not be writeable by the executor");
    }
    std::string line;

    /**
     * if the file is not open abort
     * */
    std::ifstream file(filename, std::ios::in);
    /*not sure this condition could happen? 
     * if (!file.is_open()) {
      throw file_access_exception(
          "File System Error: cannot open, potentially permission");
    }*/

    string section_head = "";
    while (std::getline(file, line)) {
      if (line.empty()) {
        continue;
      }
      while (line[0] == ' ' || line[0] == '\t') {
        line.erase(0, 1);
      }
      if (line.size() == 0 || line[0] == '#') {
        continue;
      }
      if (line[0] == ';' ||
          (line.length() > 2 && line[0] == '/' && line[1] == '/')) {
        continue;
      }
      /*TODO add regex check here*/
      if (line[0] == '[') {
        section_head = line.substr(1, line.find_first_of(']') - 1) + ".";
        continue;
      }

      const size_t s = line.find_first_of("=", 0);
      if (s == line.npos) {
        flags.insert(line);
        continue;
      }

      string key = line.substr(0, s);
      string value = line.substr(s + 1, line.length());
      string_ops::strip_qoutes(&key);
      string_ops::strip_qoutes(&value);
      key = section_head + key;

      if (!stringops.strip_check(&key) || !stringops.strip_check(&value)) {
        restricted.emplace(key, value);
        restricted_count++;
      } else {
        if (options.contains(key)) {
          options[key] = value;
        } else {
          options.emplace(key, value);
          ini_parm_count++;
        }
      }
    }
    ini_loaded = true;
  }

  /**
   * @brief returns the number of flags that have been parsed from the command line
   * @returns uint32_t 
   */
  uint32_t get_flags_count() { return flags.size(); }

  /**
   * @brief returns the number of keys that have been parsed from the ini file and clp
   * arguments
   * @returns uint32_t count
   */
  uint32_t get_parm_count() { return ini_parm_count + clp_parm_count; }

  /**
   * @brief returns a vector of keys, if there is alot of variables parsed this might
   * In most cases you aren't going to be dynamically finding keys.
   * @returns vector<string> list of keys
   */
  vector<string> get_keys() {
    vector<string> r;
    for (const auto &[key, value] : options) {
      r.push_back(key);
    }
    return r;
  }

  /**
   * @brief This parses main(c,arg) command line argument arrays. it will grab -f=as
   * and put it in the options as f -f without an equals are treated as flags,
   * same as -flag, these are easy to check for and have an O(log(n)) lookup.
   * Where as getting specific keys will have the same lookup but are stored in
   * a map Typically called explicitly but can be called after construction
   * @param c int argc 
   * @param *v[] **argv
   */
  void parse(int c, char *v[]) {
    string prev = "";
    for (int i = 0; i < c; i++) {
      string str(v[i]);
      string key;
      string value;

      size_t flag = str.find_first_of('-', 0);
      size_t assigner = str.find_first_of('=', 0);
      // if (flag == assigner) {
      //   continue;
      // }

      if (flag == 0 && assigner >= str.npos) {
        while (str[0] == '-') {
          str.erase(0, 1);
        }
        prev = str;
        flags.insert(str);
        continue;
      } else if (flag == 0 && assigner != string::npos) {
        // Case where input is a -something=something
        key = str.substr(1, assigner - 1);
        value = str.substr(assigner + 1, str.length() - assigner - 1);
        if (!options.contains(key)) {
          options.emplace(key, value);
        }
        continue;
      }
    }
  }

  /**
   * @brief Checks to see if the key exists before accessing it.
   * @param key to check
   * @returns bool
   * */
  bool has_key(string key) { return options.contains(key); }
  /**
   * @brief returns true if the flag exists
   * @param flag to check
   * @returns bool
   */
  bool has_flag(string flag) { return flags.find(flag) != flags.end(); }
  /**
   * @brief Returns the value of the key pair in string form
   * @param key to return the value of
   * @returns string value of the key pair
   */
  string get_string(string key) {
    if (options.contains(key)) {
      return this->options[key];
    } else {
      throw key_value_exception((const string)NOKEY);
    }
  }
  /**
   * @brief returns the keys of the restricted list
   * this can be done with allow_restrictions
   * but accessing them requires the flag
   * @returns vector<string> list of restricted keys
   */
  vector<string> get_restricted_keys() {
    vector<string> r;
    for (const auto &[key, value] : restricted) {
      r.push_back(key);
    }
    return r;
  }

  /**
   * @brief Restricted options are set when during parsing the variable or variable
   * name doesn't meet the regex this returns a restricted variable only if
   * allow restricted is enabled. considering allowing for some authorization
   * check
   * @param key (string)
   * @return string of the restricted value
   */
  string get_restricted_string(string key) {
    if (allow_restricted) {
      if (restricted.contains(key)) {
        return this->restricted[key];
      }
    } else {
      throw security_exception("Allow_restricted is set to false by default, "
                               "enable it to use restricted");
    }
    throw key_value_exception(NOKEY);
  }

  /**
   * @brief Gets an option or variable by key but attempts to return it as an int type.
   * throws key_value_exception if the key does not exist or invalid argument
   * NaN
   * @param key key of the int
   * @returns int64_t longlong
   */
  int64_t get_longlong(string key) {
    if (options.contains(key)) {
      try {
        int64_t r = stoll(options[key]);
        return r;
      } catch (exception &e) {
        throw key_value_exception(NAN);
      }
    } else {
      throw key_value_exception((const string)NOKEY);
    }
  }

  /**
   * @brief gets long option if NaN or key does not exist throws and exception
   * @param key of the int
   * @return int32 resulting number
   */
  int32_t get_long(string key) {
    if (options.contains(key)) {
      try {
        int32_t r = stol(options[key]);
        return r;
      } catch (exception &e) {
        throw key_value_exception(NAN);
      }
    } else {
      throw key_value_exception((const string)NOKEY);
    }
  }
  /**
   * @brief gets an int 
   * @param key string
   * return int32_t return value
   */
  int32_t get_int(string key) {
    if (options.contains(key)) {
      try {
        int r = stoi(options[key]);

        return r;
      } catch (exception &e) {
        throw key_value_exception(NAN);
      }
    } else {
      throw key_value_exception((const string)NOKEY);
    }
  }

  /**
   * @brief gets double option if NaN or key does not exist throws and exception
   * @param key string 
   * @returns double value
   */
  double get_double(string key) {
    if (options.contains(key)) {
      try {
        double r = stod(options[key]);
        return r;
      } catch (exception &e) {
        throw key_value_exception(NAN);
      }
    } else {
      throw key_value_exception((const string)NOKEY);
    }
  }

  /**
   * @brief gets float option if NaN or key does not exist throws and exception
   * @param key string 
   * @return float value
   */
  float get_float(string key) {
    if (options.contains(key)) {
      try {
        float r = stof(options[key]);
        return r;
      } catch (exception &e) {
        throw key_value_exception(NAN);
      }
    } else {
      throw key_value_exception((const string)NOKEY);
    }
  }
};
}  //end standard namespace //NOLINT
#pragma GCC diagnostic pop
#endif  //_CONFIG_PARSER_HPP //NOLINT
