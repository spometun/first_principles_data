#pragma once

#include <string>

std::string loadFromFile(const std::string& fileName);
void saveToFile(const std::string& fileName, const std::string& text);
bool isDummy(const std::string& s);
void rtrim(std::string& s);
void ltrim(std::string& s);
void trim(std::string& s);
void replace_all(std::string &s, const char * s1, const char * s2);
void purify(std::string& s);

std::string translate(const std::string& s);
