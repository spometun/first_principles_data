#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <memory>


#include "SimpleLog.h"

#include "StringUtils.h"
#include "HtmlSource.h"
#include "XmlSink.h"

using namespace std;


const string ValHome = "Home";
const string ValAudioText = "Download audio:";
const string ValMP3 = "MP3";
const string ValM4A = "M4A";
const string ValOGG = "OGG";

const string ValContactUs = "Contact Us";
const string ValFooter = R"fp(© 2013 International Christian Churches (SoldOut Discipling Movement))fp";


void printUsageAndExit()
{
    cout << endl << "Usage: fp_template <filename.html>" << endl;
    cout << "as an output you will get filename.xml (if no errors)" << endl;
    exit(-1);
}


unique_ptr<HtmlSource> source;
unique_ptr<XmlSink> sink;

void writeAudio()
{
    Node node;
    node.text = ValAudioText;
    sink->writeSectionBegin("audio");
    sink->writeField("text_intro_english", node.text);
    sink->writeField("text_intro_translated", translate(node.text));

    node = source->getNext();
    ASSERT(node.isLink, "Parse error (expected link but there was not)");
    ASSERT(node.text == ValMP3, "Parsing error. Expected \"%s\" but found \"%s\"", ValMP3.c_str(), node.text.c_str());
    sink->writeField("text_mp3_english", node.text);
    sink->writeField("text_mp3_translated", translate(node.text));
    sink->writeField("link_mp3", node.linkText);

    node = source->getNext();
    ASSERT(node.isLink, "Parse error (expected link but there was not)");
    ASSERT(node.text == ValM4A, "Parsing error. Expected \"%s\" but found \"%s\"", ValM4A.c_str(), node.text.c_str());
    sink->writeField("text_m4a_english", node.text);
    sink->writeField("text_m4a_translated", translate(node.text));
    sink->writeField("link_m4a", node.linkText);

    node = source->getNext();
    ASSERT(node.isLink, "Parse error (expected link but there was not)");
    ASSERT(node.text == ValOGG, "Parsing error. Expected \"%s\" but found \"%s\"", ValOGG.c_str(), node.text.c_str());
    sink->writeField("text_ogg_english", node.text);
    sink->writeField("text_ogg_translated", translate(node.text));
    sink->writeField("link_ogg", node.linkText);

    sink->writeSectionEnd();

}

int main(int argc, const char* argv[])
{
    cout << "Glory to God!" << endl;
    cout<< "This is First Principles Translation Template creator" << endl;
    if(argc != 2)
    {
        printUsageAndExit();
    }
    string srcFileName = argv[1];
    if(srcFileName.substr(srcFileName.size() - 5, srcFileName.size()) != ".html")
    {
        printUsageAndExit();
    }
    string dstFileName = srcFileName;
    dstFileName.replace(srcFileName.size() - 5, srcFileName.size(), ".xml");

    source = unique_ptr<HtmlSource>(new HtmlSource(srcFileName));
    sink = unique_ptr<XmlSink>(new XmlSink(dstFileName));

    sink->writeHeader();

    Node node;
    do
    {
        node = source->getNext();
    }while(node.isLink);

    sink->writeTitle(node.text);

    Node node1 = source->getNext();
    ASSERT(node1.isLink == false && node1.text == node.text, "Parse error (title and headline are different: %s and %s)", node.text.c_str(), node1.text.c_str());
    sink->writeHeadline(node1.text);

    // skip "Home"
    node = source->getNext();
    ASSERT(node.isLink, "Parse error (expected link but there was not)");
    ASSERT(node.text == ValHome, "Parsing error. Expected \"%s\" but found \"%s\"", ValHome.c_str(), node.text.c_str());


    int N = 0;
    do
    {
        node = source->getNext();
        if(node.text == ValAudioText)
        {
            ASSERT(! node.isLink, "Parse error (unexpected link found)");
            writeAudio();
        }else
        {
            sink->writePhrase(node, ++N);
        }
    }while(node.text != ValContactUs);

    do
    {
        node = source->getNext();
    }while(node.isLink);

    ASSERT(node.text == ValFooter, "Parsing error. Expected \"%s\" but found \"%s\"", ValFooter.c_str(), node.text.c_str());
    sink->writePhrase(node, ++N);

    sink->writeFooter();
    cout<<endl<<N<<" phrases written"<<endl;

    return 0;
}

