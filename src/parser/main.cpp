#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <memory>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>


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

namespace boost { namespace property_tree { namespace json_parser
{
    // Create necessary escape sequences from illegal characters
    template<>
    std::basic_string<char> create_escapes(const std::basic_string<char> &s)
    {
        std::basic_string<char> result;
        std::basic_string<char>::const_iterator b = s.begin();
        std::basic_string<char>::const_iterator e = s.end();
        while (b != e)
        {
            // This assumes an ASCII superset. But so does everything in PTree.
            // We escape everything outside ASCII, because this code can't
            // handle high unicode characters.
            if (*b == 0x20 || *b == 0x21 || (*b >= 0x23 && *b <= 0x2E) ||
                (*b >= 0x30 && *b <= 0x5B) || (*b >= 0x5D && *b <= 0xFF)  //it fails here because char are signed
                || (*b >= -0x80 && *b < 0 ) ) // this will pass UTF-8 signed chars
                result += *b;
            else if (*b == char('\b')) result += char('\\'), result += char('b');
            else if (*b == char('\f')) result += char('\\'), result += char('f');
            else if (*b == char('\n')) result += char('\\'), result += char('n');
            else if (*b == char('\r')) result += char('\\'), result += char('r');
            else if (*b == char('/')) result += char('\\'), result += char('/');
            else if (*b == char('"'))  result += char('\\'), result += char('"');
            else if (*b == char('\\')) result += char('\\'), result += char('\\');
            else
            {
                const char *hexdigits = "0123456789ABCDEF";
                typedef make_unsigned<char>::type UCh;
                unsigned long u = (std::min)(static_cast<unsigned long>(
                                                 static_cast<UCh>(*b)),
                                             0xFFFFul);
                int d1 = u / 4096; u -= d1 * 4096;
                int d2 = u / 256; u -= d2 * 256;
                int d3 = u / 16; u -= d3 * 16;
                int d4 = u;
                result += char('\\'); result += char('u');
                result += char(hexdigits[d1]); result += char(hexdigits[d2]);
                result += char(hexdigits[d3]); result += char(hexdigits[d4]);
            }
            ++b;
        }
        return result;
    }
} } }





namespace pt = boost::property_tree;


void printPtree(pt::ptree& tree, int level)
{
    for(pt::ptree::iterator it = tree.begin(); it != tree.end(); ++it)
    {
        for(int i = 0; i < level; i++) cout<<"    ";
        cout << "{" << endl;
        for(int i = 0; i < level + 1; i++) cout<< "    ";
        string s = it->second.get_value<string>();

        cout<<it->first;
        if(! it->second.empty())
        {
            it->second.put_value<string>("");
        }
        cout<< " : " <<it->second.get_value<string>();
        cout<<endl;
        printPtree(it->second, level + 1);
        for(int i = 0; i < level; i++) cout<<"    ";
        cout << "}" << endl;
    }
}
// << " "<< it->second.get_value<string>() << endl;

static void test(const string& fileName)
{
    pt::ptree tree;
    pt::read_xml(fileName, tree);
    printPtree(tree, 0);
    pt::write_xml(fileName + "zu", tree);
    pt::write_json(fileName + "zuj", tree);

}

int main(int argc, const char* argv[])
{
    cout << "Glory to God!" << endl;
    test(argv[1]);
    return 0;
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

