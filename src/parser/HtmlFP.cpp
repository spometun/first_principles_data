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

bool HtmlFP::getNextOrFail(Node& node)
{
    bool ok;
begin:
    switch (_state)
    {
    case State::header:
        do
        {
            ok = _src->getNextOrFail(node);
            if(ok && *node.text == ValHome)
            {
                _state = State::ok;
                goto begin;
            }
        }while(ok && isAudio(*node.text) );
        break;
    case State::ok:
        do
        {
            ok = _src->getNextOrFail(node);
        }while(ok && isAudio(*node.text));
        if(*node.text == ValContactUs)
        {
            _state = State::footer;
        }
        break;
    case State::footer:
        do
        {
            ok = _src->getNextOrFail(node);
        }while(ok && node.isLink);
        ASSERT(isFooter(*node.text), "Parsing error. Expected \"%s\" but found \"%s\"", ValFooter.c_str(), node.text->c_str());
        _state = State::end;
        break;
    case State::end:
        return false;
    }
    if(!ok)
    {
        _state = State::end;
        return false;
    }
    N++;
    if(_state == State::header)
    {
        node.id = (N == 1) ? "lesson_title" : "lesson_headline";
    }else
    {
        char s[100];
        sprintf(s, "phrases.p%d", N - 2);
        node.id = s;
    }
    return true;
}

int HtmlFP::getPhrasesAmount() const
{
    return N - 2;
}

//    Node node;
//    do
//    {
//        node = source->getNext();
//    }while(node.isLink);

//    sink->writeTitle(node.text);

//    Node node1 = source->getNext();
//    ASSERT(node1.isLink == false && node1.text == node.text, "Parse error (title and headline are different: %s and %s)", node.text.c_str(), node1.text.c_str());
//    sink->writeHeadline(node1.text);

//    // skip "Home"
//    node = source->getNext();
//    ASSERT(node.isLink, "Parse error (expected link but there was not)");
//    ASSERT(node.text == ValHome, "Parsing error. Expected \"%s\" but found \"%s\"", ValHome.c_str(), node.text.c_str());


//    int N = 0;
//    do
//    {
//        node = source->getNext();
//        if(node.text == ValAudioText)
//        {
//            ASSERT(! node.isLink, "Parse error (unexpected link found)");
//            writeAudio();
//        }else
//        {
//            sink->writePhrase(node, ++N);
//        }
//    }while(node.text != ValContactUs);

//    do
//    {
//        node = source->getNext();
//    }while(node.isLink);

//    ASSERT(node.text == ValFooter, "Parsing error. Expected \"%s\" but found \"%s\"", ValFooter.c_str(), node.text.c_str());
//    sink->writePhrase(node, ++N);

//void writeAudio()
//{
//    Node node;
//    node.text = ValAudioText;
//    sink->writeSectionBegin("audio");
//    sink->writeField("text_intro_english", node.text);
//    sink->writeField("text_intro_translated", translate(node.text));

//    node = source->getNext();
//    ASSERT(node.isLink, "Parse error (expected link but there was not)");
//    ASSERT(node.text == ValMP3, "Parsing error. Expected \"%s\" but found \"%s\"", ValMP3.c_str(), node.text.c_str());
//    sink->writeField("text_mp3_english", node.text);
//    sink->writeField("text_mp3_translated", translate(node.text));
//    sink->writeField("link_mp3", node.linkText);

//    node = source->getNext();
//    ASSERT(node.isLink, "Parse error (expected link but there was not)");
//    ASSERT(node.text == ValM4A, "Parsing error. Expected \"%s\" but found \"%s\"", ValM4A.c_str(), node.text.c_str());
//    sink->writeField("text_m4a_english", node.text);
//    sink->writeField("text_m4a_translated", translate(node.text));
//    sink->writeField("link_m4a", node.linkText);

//    node = source->getNext();
//    ASSERT(node.isLink, "Parse error (expected link but there was not)");
//    ASSERT(node.text == ValOGG, "Parsing error. Expected \"%s\" but found \"%s\"", ValOGG.c_str(), node.text.c_str());
//    sink->writeField("text_ogg_english", node.text);
//    sink->writeField("text_ogg_translated", translate(node.text));
//    sink->writeField("link_ogg", node.linkText);

//    sink->writeSectionEnd();

//}
