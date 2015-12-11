#include "HtmlSource.h"

#include "SimpleLog.h"
#include "StringUtils.h"

using namespace std;
using namespace htmlcxx;

HtmlSource::HtmlSource(const std::string& fileName)
{
    _data = loadFromFile(fileName);
    HTML::ParserDom parser;
    _dom = parser.parseTree(_data);
    //    cout << _dom << endl;
    _cur = _dom.begin();
}

string HtmlSource::getNextText()
{
    string res;
    bool ok = getNextTextOrFail(res);
    ASSERT(ok, "Internal error (unexpected end of file?)");
    return res;
}

string HtmlSource::getNextLink()
{
    string res;
    bool ok = getNextLinkOrFail(res);
    ASSERT(ok, "Internal error (unexpected end of file?)");
    return res;
}

bool HtmlSource::getNextTextOrFail(std::string& text)
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

bool HtmlSource::getNextOrFail(Node& node)
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
            while(++_cur != _dom.end() && (_cur->isTag() || _cur->isComment()))
            ASSERT(_cur != _dom.end(), "Parsing internal error: couldn't get link text (unexpected end of file?)");
            string s = _cur->text();
            purify(s);
            node.text = s;
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
            node.text = s;
            node.linkText = "";
            ++_cur;
            return true;
        }
    }
    return false;
}

Node HtmlSource::getNext()
{
    Node res;
    bool ok = getNextOrFail(res);
    ASSERT(ok, "Internal error (unexpected end of file?)");
    return res;
}


bool HtmlSource::getNextLinkOrFail(std::string& link)
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



