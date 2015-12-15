#include "XmlSink.h"

#include <iostream>

#include "StringUtils.h"
#include "SimpleLog.h"

using namespace std;

XmlSink::XmlSink(const std::string& fileName)
{
    _fout = fopen(fileName.c_str(), "w+");
    ASSERT(_fout, "Couldn't open file %s for writing", fileName.c_str());
}

XmlSink::~XmlSink()
{
    fclose(_fout);
    ASSERT(_sections.size() == 0, "Internal error: not all section(s) closed (%ld remained)", _sections.size())
}

void XmlSink::writeHeader()
{
    const char * Header = R"fp(<?xml version="1.0" encoding="utf-8"?>
<document>
  <translation>
    <language_name_in_english>Russian</language_name_in_english>
    <language_name_local>Русский</language_name_local>
    <translator1_name>Yauheni Sobaleu</translator1_name>
    <translator1_email>sblfff@gmail.com</translator1_email>
    <translator1_date>2015-12-01</translator1_date>
  </translation>)fp";
    fprintf(_fout, "%s", Header);
}

void XmlSink::writeFooter()
{
    fprintf(_fout, "\n</document>\n");
}

void XmlSink::writeTitle(const string& title)
{
  fprintf(_fout, "\n  <page_title>\n    <english>%s</english>\n    <translated>\?\?\?</translated>\n  </page_title>", title.c_str());
}

void XmlSink::writeHeadline(const string& headline)
{
  fprintf(_fout,  "\n  <page_headline>\n    <english>%s</english>\n    <translated>\?\?\?</translated>\n  </page_headline>", headline.c_str());
}

void XmlSink::writeField(const std::string& key, const std::string& value)
{
    fprintf(_fout, "\n    <%s>%s</%s>", key.c_str(), value.c_str(), key.c_str());
}

void XmlSink::writeSectionBegin(const std::string& name)
{
    fprintf(_fout, "\n  <%s>", name.c_str());
    _sections.push_back(name);
}

void XmlSink::writeSectionEnd()
{
    ASSERT(_sections.size() > 0, "Internal error: try to write close tag but all tags were already closed");
    fprintf(_fout, "\n  </%s>", _sections.front().c_str());
    _sections.pop_front();
}

void XmlSink::writePhrase(const Node& phrase, int number)
{
//    cout<<endl<<phrase.text;
//    char sectionName[100];
//    sprintf(sectionName, "phrase%d", number);
//    writeSectionBegin(sectionName);
//    writeField("english", phrase.text);
//    writeField("translated", translate(phrase.text));
//    if(phrase.isLink)
//    {
//        writeField("link", translate(phrase.linkText));
//    }
//    writeSectionEnd();
}















