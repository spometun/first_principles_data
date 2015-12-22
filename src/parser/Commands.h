#pragma once

#include <string>

#include "Json.h"

void putHeader(pt::ptree& tree);
void addHtmlId(std::string* str, const std::string& id);
void addJavaScriptCode(std::string* str, const std::string& jsonFileName, int nPhrases);
void convertXmlToJson(const std::string& xmlFileName);
int writeHtmlWithIdJavaScriptAndWriteJsonTemplate(const std::string& srcHtmlFileName);
