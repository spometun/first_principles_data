#pragma once

#include <string>

#include <htmlcxx/html/ParserDom.h>
#include <htmlcxx/html/utils.h>

#include "Types.h"

class HtmlParser : public Source
{
public:
    HtmlParser(const std::string& fileName);
    std::string getNextText();
    std::string getNextLink();
    bool getNextTextOrFail(std::string& text);
    bool getNextLinkOrFail(std::string& link);
    bool getNextOrFail(Node& node);
    void write(const std::string& fileName) const;
private:
    std::string _data;
    tree<htmlcxx::HTML::Node> _dom;
    tree<htmlcxx::HTML::Node>::iterator _cur;
};
