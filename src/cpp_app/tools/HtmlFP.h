#pragma once

#include <memory>

#include "HtmlParser.h"
#include "Types.h"

class HtmlFP : public Source
{
public:
    HtmlFP(std::shared_ptr<HtmlParser> parser);
    virtual bool getNextOrFail(Node& node);
    int getPhrasesAmount() const;
private:
    std::shared_ptr<HtmlParser> _src;
    enum class State {header, ok, footer, end} _state;
    int N = 0;
};
