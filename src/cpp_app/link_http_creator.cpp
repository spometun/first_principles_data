#include "tools/SimpleLog.h"
#include "tools/Types.h"
#include "tools/Json.h"

using namespace std;

static void printUsageAndExit()
{
    printf("Adds scripture links id's to html and corresponding links to json");
    printf("\nUsage: link_http_creator <some_study.html> <scriptures.json>\n");
    exit(-1);
}

static string generateBibleGateWayLink(const ScripturePassageAddress& address)
{
    char res[1000];
    string bookNameTransfomed = address.bookName;
    ASSERT(bookNameTransfomed.size() >= 2);
    if(bookNameTransfomed.substr(1, 1) == "-")
    {
        bookNameTransfomed.erase(1, 1);
    }
    char passage[100];
    if(address.isWholeChapter())
    {
        sprintf(passage, "%s%d", bookNameTransfomed.c_str(), address.beginChapter);
    }else
    {
        sprintf(passage, "%s%d:%d-%d:%d", bookNameTransfomed.c_str(), address.beginChapter, address.beginVerse, address.endChapter, address.endVerse);
    }
    sprintf(res, "https://www.biblegateway.com/passage/?search=%s&version=NRT&interface=print", passage);
    return res;
}


int main(int argc, const char* argv[])
{
    printf("Glory to God!\n");
    if(argc != 2)
    {
        printUsageAndExit();
    }
    pt::ptree json = loadPtree(argv[1]);
    pt::ptree links = json.get_child("scripture");
    int N = 0;
    for(auto& link : links)
    {
        string key = link.first;
        string value = link.second.get_value<string>();
        ScripturePassageAddress address;
        bool ok = address.setFromStringOrFail(key);
        if(! ok)
        {
            LOGW("Failed to parse %s  - skip it", key.c_str());
            continue;
        }
        string httpLink = generateBibleGateWayLink(address);
        json.put("scripture." + key, httpLink);
        LOG("%s -> %s", key.c_str(), httpLink.c_str());
        N++;
    }
    savePtree(argv[1], json);
    LOG("%d links processed", N);
}



















