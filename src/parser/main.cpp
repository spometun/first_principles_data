#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <memory>



#include "SimpleLog.h"

#include "StringUtils.h"
#include "HtmlParser.h"
#include "XmlSink.h"
#include "Json.h"
#include "HtmlFP.h"

using namespace std;




void printUsageAndExit()
{
    cout << endl << "Usage: fp_template <filename.html>" << endl;
    cout << "as an output you will get filename.xml (if no errors)" << endl;
    exit(-1);
}


shared_ptr<HtmlParser> html;
shared_ptr<XmlSink> sink;


void test1(const string& fileName)
{
    HtmlParser html(fileName);
    html.write(fileName + "C");
    return;
    string text;
    while(html.getNextTextOrFail(text))
    {
        cout<<endl<<text;
    }
}

void putHeader(pt::ptree& tree)
{
    tree.put("metadata.language_name_in_english", "Russian");
    tree.put("metadata.language_name_local", "Русский");
    tree.put("metadata.translator1_name", "Yauheni Sobaleu");
    tree.put("metadata.translator1_email", "sblfff@gmail.com");
    tree.put("metadata.translator1_date", "2015-12-20");
    tree.put("audio_link", "???");
}

void addHtmlId(string* str, const string& id)
{
    *str = "<span id = \"" + id + "\">" + *str + "</span>";
}

const char * Code = R"code(
$(document).ready(function() {
$.getJSON("lang/ru/text/%s", function(json)
{
    console.log("God give me strength!");
    document.title = json.lesson_headline_translated;
    document.getElementById('lesson_headline').innerHTML = json.lesson_headline_translated;
    for(i = 1; i <= %d; i++)
    {
        eval("document.getElementById('phrases.p" + i + "').innerHTML = json.phrases.p" + i + "_translated;");
    }
});
});
)code";


void addJavaScriptCode(string* str, const string& jsonFileName, int n)
{
    char code[10000];
    sprintf(code, Code, jsonFileName.c_str(), n);
    *str = *str + "\n" + code + "\n";
}

void convertXmlToJson(const string& xmlFileName)
{
    ASSERT(xmlFileName.substr(xmlFileName.size() - 4, xmlFileName.size()) == ".xml");
    string jsonFileName = xmlFileName;
    jsonFileName.replace(xmlFileName.size() - 4, xmlFileName.size(), ".json");
    pt::ptree xml;
    pt::read_xml(xmlFileName, xml);
    pt::ptree json;
    putHeader(json);
    string val = xml.get<string>("document.page_title.english");
    json.put("lesson_title_english", val);
    val = xml.get<string>("document.page_title.translated");
    json.put("lesson_title_translated", val);
    val = xml.get<string>("document.page_headline.english");
    json.put("lesson_headline_english", val);
    val = xml.get<string>("document.page_headline.translated");
    json.put("lesson_headline_translated", val);
    int n = 1;
    while(1)
    {
        char key[100];
        sprintf(key, "document.phrase%d.english", n);
        val = xml.get<string>(key, "");
        if(val.empty())
        {
            break;
        }
        sprintf(key, "phrases.p%d_english", n);
        json.put(key, val);

        sprintf(key, "document.phrase%d.translated", n);
        val = xml.get<string>(key);
        sprintf(key, "phrases.p%d_translated", n);
        json.put(key, val);
        n++;
    }
    pt::write_json(jsonFileName, json);
    printf("\n%s: %d phrases copied\n", jsonFileName.c_str(), n - 1);
}

int main(int argc, const char* argv[])
{
    cout << "Glory to God!";
    //cout<< "This is the First Principles Translation Template creator" << endl;
    if(argc != 2)
    {
        printUsageAndExit();
    }
   // convertXmlToJson(argv[1]);
//    return 0;


    string srcFileName = argv[1];
    if(srcFileName.substr(srcFileName.size() - 5, srcFileName.size()) != ".html")
    {
        printUsageAndExit();
    }
    string dstJsonFileName = srcFileName;
    dstJsonFileName.replace(srcFileName.size() - 5, srcFileName.size(), ".json");
    string dstHtmlFileName = srcFileName;
    dstHtmlFileName.replace(srcFileName.size() - 5, srcFileName.size(), "_lang.html");
    cout<<" "<<dstHtmlFileName<<": "<<flush;

    html = make_shared<HtmlParser>(srcFileName);
    HtmlFP fpSource(html);
    Node node;
    pt::ptree tree;
    putHeader(tree);
    while(fpSource.getNextOrFail(node))
    {
        string text = *node.text;
        purify(text);
        tree.put(node.id + "_english", text);
        tree.put(node.id + "_translated" , translate(text));
        addHtmlId(node.text, node.id);
       // if(node.isLink) cout<<endl<<"link: "<<node.linkText;
    }
//    fflush(stdout);
    pt::write_json(dstJsonFileName, tree);

    bool ok = html->getNextOrFail(node);
    if(ok)
    {
        addJavaScriptCode(node.text, dstJsonFileName, fpSource.getPhrasesAmount());
    }else
    {
        LOGW("Couldn't find java script cluase near the end - do not embedd javascript code");
    }
    html->write(dstHtmlFileName);
    cout<<fpSource.getPhrasesAmount()<<" phrases written"<<endl;

    return 0;
}


















