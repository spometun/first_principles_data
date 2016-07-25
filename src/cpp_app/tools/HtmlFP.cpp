#include "HtmlFP.h"

#include <string>

#include "SimpleLog.h"
#include "StringUtils.h"


using namespace std;

const string ValHome = "Home";
const string ValAudioText = "Download audio:";
const string ValMP3 = "MP3";
const string ValM4A = "M4A";
const string ValOGG = "OGG";

const string ValContactUs = "Contact Us";
const string ValFooter = R"fp(© 2013 International Christian Churches (SoldOut Discipling Movement))fp";



HtmlFP::HtmlFP(std::shared_ptr<HtmlParser> parser) : _src(parser)
{
    _state = State::header;
}

static bool isAudio(const string& str)
{
    string text = str;
    purify(text);
    return     text == ValAudioText
            || text == ValMP3
            || text == ValM4A
            || text == ValOGG;
}

static bool isFooter(const string& str)
{
    string text = str;
    purify(text);
    return text == ValFooter;
}

string getPhraseId(int N)
{
        char s[100];
        sprintf(s, "phrases.p%d", N);
        return s;
}

bool HtmlFP::getNextOrFail(Node& node)
{
begin:
    switch (_state)
    {
    case State::header:
        if(N == 0)
        {
            node = _src->getNext();
            ASSERT(node.tag == "title");
            string title = *node.text;
            do
            {
                node = _src->getNext();
            }while(node.tag != "h1");
            if(title != *node.text)
            {
                LOGW("title and headline differs: %s %s", title.c_str(), node.text->c_str());
            }
            node.id = "lesson_headline";
            N++;
            return true;
        }
        node = _src->getNext();
        ASSERT(node.tag != "h1" || *node.text == ValHome);
        _state = State::ok;
        N = 0;
        goto begin;
    case State::ok:
        do
        {
            node = _src->getNext();
        }while(isAudio(*node.text));
        if(*node.text == ValContactUs || *node.text == ValHome)
        {
            _state = State::footer;
        }
        node.id = getPhraseId(++N);
        return true;
    case State::footer:
        do
        {
            node = _src->getNext();
        }while(node.isLink && isDummy(*node.text));
        node.id = getPhraseId(++N);
        if(isFooter(*node.text))
        {
            ASSERT(! node.isLink, "Footer is a link???");
            _state = State::end;
        }
        return true;
    case State::end:
        return false;
    }
}

int HtmlFP::getPhrasesAmount() const
{
    return N;
}











