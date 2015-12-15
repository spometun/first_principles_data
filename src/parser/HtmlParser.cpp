#include "HtmlParser.h"

#include <stdio.h>
#include <iostream>
#include <map>

#include "SimpleLog.h"
#include "StringUtils.h"


using namespace std;
using namespace htmlcxx;

using uint = unsigned int;

HtmlParser::HtmlParser(const std::string& fileName)
{
    _data = loadFromFile(fileName);
    HTML::ParserDom parser;
    _dom = parser.parseTree(_data);
    //    cout << _dom << endl;
    _cur = _dom.begin();
}

string HtmlParser::getNextText()
{
    string res;
    bool ok = getNextTextOrFail(res);
    ASSERT(ok, "Internal error (unexpected end of file?)");
    return res;
}

string HtmlParser::getNextLink()
{
    string res;
    bool ok = getNextLinkOrFail(res);
    ASSERT(ok, "Internal error (unexpected end of file?)");
    return res;
}

void HtmlParser::write(const string& fileName) const
{
    FILE* fout = fopen(fileName.c_str(), "wb");
    ASSERT(fout);
    map<uint, string> closing;
    for(tree<htmlcxx::HTML::Node>::iterator it = _dom.begin(); it != _dom.end(); ++it)
    {
        string s;
        uint offset = it->offset();
        for(auto it = closing.begin(); it != closing.end() && it->first <= offset;)
        {
            s = it->second;
            fwrite(s.c_str(), s.length(), 1, fout);
            auto cur = it++;
            closing.erase(cur);
        }
        s = it->text();
        fwrite(s.c_str(), s.length(), 1, fout);
        s = it->closingText();
        if(! s.empty())
        {
            closing[offset + it->length()] = s;
        }
    }
    fclose(fout);
}

bool HtmlParser::getNextTextOrFail(std::string& text)
{
    for(; _cur != _dom.end(); ++_cur)
    {
        if(_cur->isTag() || _cur->isComment())
        {
            continue;
        }
        string s = _cur->text();
        if(isDummy(s))
        {
            continue;
        }
        purify(s);
        text = s;
        ++_cur;
        return true;
    }
    return false;
}

bool HtmlParser::getNextOrFail(Node& node)
{
    for(; _cur != _dom.end(); ++_cur)
    {
        if(_cur->isTag() && _cur->tagName() == "a")
        {
            node.isLink = true;
            _cur->parseAttributes();
            std::pair<bool, std::string> href = _cur->attribute("href");
            ASSERT(href.first == true, "Internal error");
            node.linkText = href.second;
            while(++_cur != _dom.end() && (_cur->isTag() || _cur->isComment()));
            ASSERT(_cur != _dom.end(), "Parsing internal error: couldn't get link text (unexpected end of file?)");
            node.text = &const_cast<string&>(_cur->text());
            _cur++;
            return true;
        }
        if(! _cur->isTag() && ! _cur->isComment())
        {
            string s = _cur->text();
            purify(s);
            if(isDummy(s))
            {
                continue;
            }
            node.isLink = false;
            node.text = &const_cast<string&>(_cur->text());
            node.linkText = "";
            ++_cur;
            return true;
        }
    }
    return false;
}


bool HtmlParser::getNextLinkOrFail(std::string& link)
{
    for(; _cur != _dom.end(); ++_cur)
    {
        if(_cur->tagName() == "a")
        {
            link = _cur->text();
            ++_cur;
            return true;
            _cur->parseAttributes();
            std::pair<bool, std::string> attribute = _cur->attribute("href");
            if(attribute.first == true)
            {
                link = attribute.second;
                ++_cur;
                return true;
            }
        }
    }
    return false;
}



