#include "Commands.h"

#include "SimpleLog.h"

#include "StringUtils.h"
#include "HtmlParser.h"
#include "XmlSink.h"
#include "HtmlFP.h"

using namespace std;

int writeHtmlWithIdJavaScriptAndWriteJsonTemplate(const string& srcHtmlFileName)
{
    string dstJsonFileName = srcHtmlFileName;
    dstJsonFileName.replace(srcHtmlFileName.size() - 5, srcHtmlFileName.size(), ".json");
    string dstHtmlFileName = srcHtmlFileName;
    dstHtmlFileName.replace(srcHtmlFileName.size() - 5, srcHtmlFileName.size(), "_lang.html");
    cout<<" "<<dstHtmlFileName<<": "<<flush;

    shared_ptr<HtmlParser> html = make_shared<HtmlParser>(srcHtmlFileName);
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
    return fpSource.getPhrasesAmount();
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


void addJavaScriptCode(string* str, const string& jsonFileName, int nPhrases)
{
    char code[10000];
    sprintf(code, Code, jsonFileName.c_str(), nPhrases);
    *str = *str + "\n" + code + "\n";
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
