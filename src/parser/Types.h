#pragma once

#include <string>


struct Node
{
    std::string* text;
    bool isLink;
    std::string linkText;
    std::string id;
};

class Source
{
public:
    virtual bool getNextOrFail(Node& node) = 0;
    Node getNext();
};

