#include "IniFile.h"

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <cctype>

std::string del(const std::string& str){
    std::string result;
    for (char c : str){
        if (c != ' '){
            result += c;
        }
    }
    return result;
}

IniFile::IniFile(const std::string &path) : filePath(path){
    std::ifstream file(path);
    if (!file.is_open()){
        throw "Error";
    }
    std::string line;
    std::string sec;
    std::string key;
    std::string value;

    while (getline(file, line)){
      line = del(line);
      int rav = 0;
      int countr = 0;
      int countl = 0;

      // удаление коментария
      if (line.find(";") != std::string::npos){
        line.erase(line.find(";"), line.length());
      }
      
      // обработка исключений
      if (line.find(']') < line.find('[')) {
        line.erase(0, line.length());
      }
      if ((line[0] == '=') || (line[line.length() - 1] == '=')) {
        line.erase(0, line.length());
      }
      for (int i = 0; i < line.length(); ++i){
        if (line[i] == '='){
          rav++;
        }
        if (line[i] == '['){
          countr++;
        }
        if (line[i] == ']'){
          countl++;
        }
      }
      if (rav > 1 || countr > 1 || countl > 1){
        line.erase(0, line.length());
      }
      
      if (line.empty()){
        continue;
      }
      //
      
      // обработка ключа и значения
      if (line.find("=") != std::string::npos){
        key = line.substr(0, line.find("="));
        value = line.substr(line.find("=") + 1);
        if (!key.empty() && !value.empty()){
          data[sec][key] = value;
        }
      }

      // обработка секций
      if (line.find("[") != std::string::npos && line.find("]") != std::string::npos){
          sec = line.substr(line.find("[") + 1, line.find("]") - line.find("[") - 1);
          if (!sec.empty()){
            if (data.find(sec) != data.end()){
              for (const auto& entry : data[sec]){
                data[sec][entry.first] = entry.second;
              }
            }
            else{
              data[sec] = KeysMap();
            }
          }
      }
  }
}


IniFile::~IniFile(){
  std::ofstream file(filePath);
  for (auto &sec : data){
    std::ofstream file(filePath, std::ios::app);
    file << "[" << sec.first << "]" << std::endl;
    for (auto &key : sec.second){
      file << key.first << "=" << key.second << std::endl;
    }
    file.close();
  }
}

void IniFile::save() {
  std::ofstream file(filePath);
  if (file.is_open()){
  // цикл по секциям
  for (auto &sec : data){
    std::ofstream file(filePath, std::ios::app);
    //записывается секция в конец файла
    file << "[" << sec.first << "]" << std::endl;
    //цикл по ключам в секции
    for (auto &key: sec.second){
      //хаписывается ключ и значение в конец файла
      file << key.first << "=" << key.second << std::endl;
    }
    file.close();
  }
  }
  else
    throw "Error";
}

int IniFile::readInt(const std::string& section, const std::string& key, int def) {
  // если секция не найдена вернем значение по умолчанию
  if (data.find(section) == data.end()){
    return def;
  }
  // если ключ не найден в секции вернем значение по умолчанию
  if (data[section].find(key) == data[section].end()){
    return def;
  }
  // если секция и ключ найдены вернем значение ключа
  return std::stoi(data[section][key]);
}

double IniFile::readDouble(const std::string& section, const std::string& key, double def){
  if (data.find(section) == data.end()){
    return def;
  }
  if (data[section].find(key) == data[section].end()){
    return def;
  }
  return std::stod(data[section][key]);
}

std::string IniFile::readString(const std::string& section, const std::string &key, const std::string& def){
  if (data.find(section) == data.end()){
    return def;
  }
  if (data[section].find(key) == data[section].end()){
    return def;
  }
  return data[section][key];
}

bool IniFile::readBool(const std::string& section, const std::string& key, bool def){
  if (data.find(section) == data.end()){
    return def;
  }
  if (data[section].find(key) == data[section].end()){
    return def;
  }
  // переводим значение ключа в нижний регистр
  std::string value = data[section][key];
  for (char &c : value) {
     c = std::tolower(c);
  }
  bool isTrueValue = false;
    // проходмся по всем значениям возможного true
    for (const std::string& trueVal : trueValues) {
        // если значение совпадает с одним из true значений
        if (value == trueVal) {
            isTrueValue = true;
            break;
        }
    }
    return isTrueValue;
}

void IniFile::writeInt(const std::string& section, const std::string& key, int value){
  //Проверка существет ли секция, если нет делаем новую
  if (data.find(section) == data.end()){
    data[section] = KeysMap();
  }

  // записываем строковое значение в ключ
  data[section][key] = std::to_string(value);
}

void IniFile::writeDouble(const std::string& section, const std::string& key, double value){
  if (data.find(section) == data.end()){
    data[section] = KeysMap();
  }

  data[section][key] = std::to_string(value);
}

void IniFile::writeString(const std::string&section, const std::string& key, const std::string& value){
  if (data.find(section) == data.end()){
    data[section] = KeysMap();
  }

  data[section][key] = value;
}

void IniFile::writeBool(const std::string&section, const std::string& key, bool value){
  if (data.find(section) == data.end()){
    data[section] = KeysMap();
  }
  if (value){
    // устанавливается значение в true для данной секции и ключа
    data[section][key] = "true";
  }
  else{
    data[section][key] = "false";
  }
}

bool IniFile::isSectionExist(const std::string& section) {
    return data.find(section) != data.end();
}

bool IniFile::isKeysExist(const std::string& section, const std::string& key) {
  if (data.find(section) != data.end()) {
    return data[section].find(key) != data[section].end();
  }
  return false;
}

size_t IniFile::getSectionsCount() {
    return data.size();
}

size_t IniFile::getKeysCount(const std::string& section){
  return data[section].size();
}

SectionsMap IniFile::getSections() const{
  return data;
}

bool IniFile::deleteSection(const std::string& section){
  if (data.find(section) != data.end()){
    data.erase(data.find(section));
    return true;
  }
  return false;
}

bool IniFile::deleteKey(const std::string& section, const std::string& key){
  if (data.find(section) != data.end()){
    auto& KeysMap = data.find(section)->second;
    if (KeysMap.find(key) != KeysMap.end()){
      KeysMap.erase(key);
      return true;
    }
  }
  return false;
}

bool IniFile::addNewSection(const std::string& section){
  if (data.find(section) == data.end()){
    data[section] = KeysMap();
    return true;
  }
  return false;
} 
