#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <memory>



#include "tools/SimpleLog.h"

#include "tools/StringUtils.h"
#include "tools/HtmlParser.h"
#include "tools/XmlSink.h"
#include "tools/Json.h"
#include "tools/HtmlFP.h"
#include "tools/Commands.h"

using namespace std;


void printUsageAndExit()
{
    cout << endl << "Usage: fp_template <filename.html>" << endl;
    cout << "as an output you will get filename.xml (if no errors)" << endl;
    exit(-1);
}

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

int main(int argc, const char* argv[])
{
    cout << "Glory to God!";
    //cout<< "This is the First Principles Translation Template creator" << endl;
    if(argc != 2)
    {
        printUsageAndExit();
    }
    string htmlFileName = argv[1];
    if(htmlFileName.substr(htmlFileName.size() - 5, htmlFileName.size()) != ".html")
    {
        printUsageAndExit();
    }
    int nPhrases = writeHtmlWithIdJavaScriptAndWriteJsonTemplate(htmlFileName);
   // convertXmlToJson(argv[1]);
//    return 0;


    cout<<nPhrases<<" phrases written"<<endl;

    return 0;
}


















