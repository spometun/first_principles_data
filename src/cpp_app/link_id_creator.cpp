#include <map>

#include "tools/SimpleLog.h"
#include "tools/Types.h"
#include "tools/StringUtils.h"
#include "tools/Json.h"

using namespace std;

static void printUsageAndExit()
{
    printf("Adds scripture links id's to html and corresponding keys to json");
    printf("\nUsage: link_id_creator <some_study.html> <scriptures.json>\n");
    exit(-1);
}


static bool parseHtmlScriptureLink(const string& scriptureText, ScripturePassageAddress& link)
{
    if(scriptureText.size() < 2)
    {
        return false;
    }
    string scriptureTextExt = scriptureText + "42";
    string allowedBookLetters = "qwertyuiopasdfghjklzxcvbnm";
    size_t bookEnd = scriptureTextExt.find_first_of('-');
    if( bookEnd == 1 && (scriptureTextExt[0] == '1' || scriptureTextExt[0] == '2' || scriptureTextExt[0] == '3') )
    {
        bookEnd = scriptureTextExt.find_first_of('-', 2);
        allowedBookLetters += "123-";
    }
    if(bookEnd == string::npos)
    {
        return false;
    }
    link.bookName = scriptureTextExt.substr(0, bookEnd);
    if(link.bookName.find_first_not_of(allowedBookLetters) != string::npos)
    {
        return false;
    }
    int dummy;
    int nRead = sscanf(scriptureTextExt.c_str() + bookEnd + 1, "%d-%d-%d-%d.html%d", &link.beginChapter, &link.beginVerse, &link.endChapter, &link.endVerse, &dummy);
    if(nRead == 5)
    {
        return true;
    }
    nRead = sscanf(scriptureTextExt.c_str() + bookEnd + 1, "%d-%d-%d.html%d", &link.beginChapter, &link.beginVerse, &link.endVerse, &dummy);
    if(nRead == 4)
    {
        link.endChapter = link.beginChapter;
        return true;
    }
    nRead = sscanf(scriptureTextExt.c_str() + bookEnd + 1, "%d-%d.html%d", &link.beginChapter, &link.beginVerse, &dummy);
    if(nRead == 3)
    {
        link.endChapter = link.beginChapter;
        link.endVerse = link.beginVerse;
        return true;
    }
    nRead = sscanf(scriptureTextExt.c_str() + bookEnd + 1, "%d.html%d", &link.beginChapter, &dummy);
    if(nRead == 2)
    {
        link.endChapter = link.beginChapter;
        link.beginVerse = 0;
        link.endVerse = 0;
        return true;
    }
    return false;
}

int main(int argc, const char* argv[])
{
    printf("Glory to God!\n");
    if(argc != 3)
    {
        printUsageAndExit();
    }
    LOG("**** process %s", argv[1]);
    string html = loadFromFile(argv[1]);
    pt::ptree json = loadPtree(argv[2]);
    size_t linkPos = -1;
    const string scriptureTag = "scriptures/";
    int N = 0;
    map<string, int> idsCount;
    while(1)
    {
        linkPos = html.find(scriptureTag, linkPos + 1);
        if(linkPos == string::npos)
        {
            break;
        }
        size_t endPos = html.find(".html", linkPos);
        if(endPos == string::npos || endPos - linkPos > 50)
        {
            LOGW("Skip strange %s", html.substr(linkPos, 60).c_str());
            continue;
        }
        string linkFromHtml = html.substr(linkPos + scriptureTag.length(), endPos - linkPos - scriptureTag.length() + 5);
        ScripturePassageAddress passageAddress;
        bool ok = parseHtmlScriptureLink(linkFromHtml, passageAddress);
        if(! ok)
        {
            LOGW("Coudn't parse %s", linkFromHtml.c_str());
            continue;
        }
        string passageKey = passageAddress.toString();
        string id = " id = \"link." + passageKey + "\"";
        idsCount[id]++;
        if(idsCount[id] != 1)
        {
            LOG("*********************************************** passage used %d times", idsCount[id]);
            char s[10];
            sprintf(s, "%d", idsCount[id]);
            id.insert(11,s);
        }
        int insertPos = endPos + 6;
        if(html.substr(insertPos, id.length()) != id)
        {
            LOG("%s -> %s", linkFromHtml.c_str(), passageKey.c_str());
            html.insert(insertPos, id);
        }else
        {
            LOG("%s -> %s  already there - skip", linkFromHtml.c_str(), passageKey.c_str());
        }
        ASSERT(html.substr(insertPos, id.length()) == id);
        json.put("scripture." + passageKey, "https://");
        N++;
    }
    //saveToFile(argv[1], html);
    //savePtree(argv[2], json);
    LOG("%d links processed", N);
}
