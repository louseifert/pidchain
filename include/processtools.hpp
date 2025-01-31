/* *
 * This software has a Copyright 2024 to Louis C Seifert III all rights reserved
 * it is freely distributable under the MIT license. If you use this api:
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions: THE SOFTWARE IS
 * PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * */
#include <exception>                //NOLINT
#include <iostream>                 //NOLINT
#include <memory>                   //NOLINT
#include <sstream>                  //NOLINT
#include <unistd.h>                 //NOLINT
				    
#ifndef PROCESSTOOLS_HPP	    //NOLINT
#define PROCESSTOOLS_HPP            //NOLINT
bool verbose = false;
using namespace std;                //NOLINT
/*TODO: there might be a better way to do this
 * research later though this isn't going to be a huge performance issu
 * for this use
 */
string inttostr(pid_t pid) {
  stringstream ss;
  ss << pid;
  return ss.str();
}
/*TODO: move this to a header file so it can be included in an api*/
void r_find_origin(string pid, vector<std::string> *proclist) {
  string lpid = "";
  pid_t npid = -1;
  bool have_str = false;
  bool have_pid = false;
  string filename = "/proc/" + pid + "/status";
  bool found_end = false;
  if (!std::filesystem::exists(filename) ||
      !std::filesystem::is_regular_file(filename)) {
    found_end = true;
  }
  std::ifstream file(filename, std::ios::in);
  string line = "";
  while (!found_end && std::getline(file, line)) {
    if (have_pid && have_str) {
      break;
    }
    if (!line.empty() && line.size() > 6) {
      if (line.find("Name") < line.size()) {
        int16_t s = line.find(":") + 1;
        std::string name = line.substr(s, line.size());
        string_ops::trim(&name);
        proclist->push_back(name);
        have_str = true;
      }
      if (line.find("PPid:") < line.size()) {
        int16_t s = line.find(":") + 1;
        string temp = line.substr(s, line.size());
        string_ops::trim(&temp);
        try {
          npid = stoi(temp);
          lpid = temp;
          have_pid = true;
        } catch (exception &e) {
          if (verbose) {
            cerr << "stoi failure" << temp << endl;
          }
          found_end = true;
          break;
        }
      }
    }
  }
  file.close();
  if (lpid.size() > 0 && npid > -1) {
    r_find_origin(lpid, proclist);
  }
}
void r_find_origin(string pid, vector<pid_t> *pidslist) {
  string lpid = "";
  pid_t npid = -1;
  string filename = "/proc/" + pid + "/status";
  bool found_end = false;
  if (!std::filesystem::exists(filename) ||
      !std::filesystem::is_regular_file(filename)) {
    found_end = true;
  }
  std::ifstream file(filename, std::ios::in);
  string line = "";
  while (!found_end && std::getline(file, line)) {
    if (!line.empty() && line.size() > 6) {
      if (line.find("PPid:") < line.size()) {
        int16_t s = line.find(":") + 1;
        string temp = line.substr(s, line.size());
        string_ops::trim(&temp);
        try {
          npid = stoi(temp);
          lpid = temp;
          break;
        } catch (exception &e) {
          if (verbose) {
            cerr << "stoi failure" << temp << endl;
          }
          found_end = true;
          break;
        }
      }
    }
  }
  file.close();
  if (lpid.size() > 0 && npid > -1) {
    pidslist->push_back(npid);
    r_find_origin(lpid, pidslist);
  }
}
void r_find_origin(pid_t pid, vector<pid_t> *pidslist) {
  r_find_origin(inttostr(pid), pidslist);
}
/*
std::unique_ptr<vector<long>> get_pidchain(string pid){
        vector<long> *v;
        r_find_origin(pid,v);
        return std::make_unique<vector<long>>(v);
}
std::unique_ptr<vector<long>> get_pidchain(int pid){
        return get_pidchain(inttostr(pid));
}
*/
#endif // PROCESSTOOLS_HPP //NOLINE
