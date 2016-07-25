#pragma once

#include <string>

struct ScripturePassageAddress
{
    std::string bookName;
    int beginChapter;
    int beginVerse;
    int endChapter;
    int endVerse;
    std::string toString() const;
    bool setFromStringOrFail(const std::string& passageName);
    bool isWholeChapter() const;
};


struct Node
{
    std::string* text;
    bool isLink;
    std::string linkText;
    std::string id;
    std::string tag;
};

class Source
{
public:
    virtual bool getNextOrFail(Node& node) = 0;
    Node getNext();
};

