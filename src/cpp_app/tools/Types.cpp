#include "Types.h"

#include <string>

#include "SimpleLog.h"

using namespace std;

Node Source::getNext()
{
    Node res;
    bool ok = getNextOrFail(res);
    ASSERT(ok, "Internal error (unexpected end of file?)");
    return res;
}


string ScripturePassageAddress::toString() const
{
    char res[100];
    int pos = sprintf(res, "%s_%d", bookName.c_str(), beginChapter);
    if(isWholeChapter())
    {
        return res;
    }
    pos += sprintf(res + pos, ":%d", beginVerse);
    if(endChapter == beginChapter)
    {
        if(endVerse == beginVerse)
        {
            return res;
        }
        sprintf(res + pos, "-%d", endVerse);
        return res;
    }
    sprintf(res + pos, "-%d:%d", endChapter, endVerse);
    return res;
}

bool ScripturePassageAddress::setFromStringOrFail(const string& passageName)
{
    if(passageName.size() < 2)
    {
        return false;
    }
    string passageNameExt = passageName + "dummy42";
    string allowedBookLetters = "qwertyuiopasdfghjklzxcvbnm123-";
    size_t bookEnd = passageNameExt.find_first_of('_');
    if(bookEnd == string::npos)
    {
        return false;
    }
    ScripturePassageAddress res;
    res.bookName = passageNameExt.substr(0, bookEnd);
    if(res.bookName.find_first_not_of(allowedBookLetters) != string::npos)
    {
        return false;
    }
    int dummy;
    int nRead = sscanf(passageNameExt.c_str() + bookEnd + 1, "%d:%d-%d:%ddummy%d", &res.beginChapter, &res.beginVerse, &res.endChapter, &res.endVerse, &dummy);
    if(nRead == 5)
    {
        *this = res;
        return true;
    }
    nRead = sscanf(passageNameExt.c_str() + bookEnd + 1, "%d:%d-%ddummy%d", &res.beginChapter, &res.beginVerse, &res.endVerse, &dummy);
    if(nRead == 4)
    {
        res.endChapter = res.beginChapter;
        *this = res;
        return true;
    }
    nRead = sscanf(passageNameExt.c_str() + bookEnd + 1, "%d:%ddummy%d", &res.beginChapter, &res.beginVerse, &dummy);
    if(nRead == 3)
    {
        res.endChapter = res.beginChapter;
        res.endVerse = res.beginVerse;
        *this = res;
        return true;
    }
    nRead = sscanf(passageNameExt.c_str() + bookEnd + 1, "%ddummy%d", &res.beginChapter, &dummy);
    if(nRead == 2)
    {
        res.endChapter = res.beginChapter;
        res.beginVerse = 0;
        res.endVerse = 0;
        *this = res;
        return true;
    }
    return false;
}


bool ScripturePassageAddress::isWholeChapter() const
{
    return  beginVerse == 0 && endVerse == 0 && beginChapter == endChapter;
}


















