#pragma once

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#define WHITESPACE_CHARS " \t\n\r\f\v"

void ToUpperCase(std::string& str, uint32_t startPos = 0);
void ToLowerCase(std::string& str, uint32_t startPos = 0);

std::string ToUpperCase(const std::string& str, uint32_t startPos, uint32_t endPos);
std::string ToLowerCase(const std::string& str, uint32_t startPos, uint32_t endPos);

std::string Format(const std::string& str, ...);
std::string Format(const char* format, ...);

std::string Trim(std::string s, const char* t = WHITESPACE_CHARS);
std::string LTrim(std::string s, const char* t = WHITESPACE_CHARS);
std::string RTrim(std::string s, const char* t = WHITESPACE_CHARS);

std::vector<std::string> Split(const std::string& str, char c);
std::vector<std::string> Keys(const std::map<std::string, uint32_t>& v);
std::map<std::string, std::string> Split(const std::string& str, char c1, char c2);

std::string VectorToString(const std::vector<uint32_t>& values);
