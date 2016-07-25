#pragma once

#include <stdio.h>

#include <string>
#include <list>

#include "Types.h"

class XmlSink
{
public:
    XmlSink(const std::string& fileName);
    void writeHeader();
    void writeFooter();
    void writeTitle(const std::string& title);
    void writeHeadline(const std::string& headline);
    void writeField(const std::string& key, const std::string& value);
    void writeSectionBegin(const std::string& name);
    void writeSectionEnd();
    void writePhrase(const Node& phrase, int number);
    ~XmlSink();
private:
    FILE* _fout;
    std::list<std::string> _sections;

};
