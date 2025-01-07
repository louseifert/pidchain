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
 * */
/**
 * ! mainpage Main CPP and examples
 *
 * <a href="./coverage.html">Test Coverage</a>
 * If the options exist both in the ini and the command line the ini will
 * override the command line This is so default options can be specified by an
 * administrator or other access and not overridden
 */


#include <exception>
#include <string>
#ifndef _PARSEREXCEPTIONS_HPP_INCLUDED //NOLINT
#define _PARSEREXCEPTIONS_HPP_INCLUDED //NOLINT

/**
 * @class file_access_exception
 * @brief Thrown if it cannot find the config.
 */
class file_access_exception : public std::exception {
 public:
/**
 * @brief standard constructor
 * @param message
 */
  explicit file_access_exception(const std::string &message)
      : _message(message) {}
  /**
   * @brief what message for exception
   * returns string
   */
  virtual const char *what() const noexcept {
    return _message.c_str();
  }

 private:
  std::string _message;
};

/**
 * @class security_exception
 * @brief thrown when ever there is a problem with secuirty like file permissions, or a
 * setting not in the excepted format
 */
class security_exception : public std::exception {
 public:
  /**
   * @brief thrown when there is an attempt to performa protected function
   */
  explicit security_exception(const std::string &message) : _message(message) {}
  /**
   * @brief what message for exception
   * returns string
   */
  virtual const char *what() const noexcept {
    return _message.c_str();
  }

 private:
  std::string _message;
};

/**
 * @class key_value_exception
 * @brief exception when there is a problem with a key value pair
 */
class key_value_exception : public std::exception {
 public:
  /**
   * @brief standard constructor
   * @param message
   */
  explicit key_value_exception(const std::string &message)
      : _message(message) {}
  /**
   * @brief what message for exception
   * returns string
   */
  virtual const char *what() const noexcept {
    return _message.c_str();
  }

 private:
  std::string _message;
};

#endif  // _PARSEREXCEPTIONS_HPP_INCLUDED //NOLINT
