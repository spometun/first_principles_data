#include "StringUtils.h"

#include <fstream>
#include <iostream>

#include "SimpleLog.h"

using namespace std;

string loadFromFile(const string& fileName)
{
    std::ifstream fin(fileName, std::ios::in | std::ios::binary);
    if (!fin)
    {
        std::cout << "File " << fileName << " not found!\n";
        exit(EXIT_FAILURE);
    }
    std::string res;
    fin.seekg(0, std::ios::end);
    res.resize(fin.tellg());
    fin.seekg(0, std::ios::beg);
    fin.read(&res[0], res.size());
    fin.close();
    return res;
}

void saveToFile(const string& fileName, const string& text)
{
    FILE* fout = fopen(fileName.c_str(), "wb");
    ASSERT(fout, "Couldn't open file %s for writing", fileName.c_str());
    fwrite(text.c_str(), text.length(), 1, fout);
    fclose(fout);
}

bool isDummy(const string& s)
{
    string tmp = s;
    purify(tmp);
    auto pos = tmp.find_first_not_of("| \n\r\t");
    return pos == string::npos;
}

void rtrim(string& s)
{
    s.erase(s.find_last_not_of(" \n\r\t")+1);
}


void ltrim(string& s)
{
    s.erase(0,s.find_first_not_of(" \n\r\t"));
}

void trim(string& s)
{
    rtrim(s);
    ltrim(s);
}

void replace_all(string& s, const char* s1, const char* s2)
{
    string t1(s1);
    size_t len = t1.length();
    size_t pos = s.find(t1);
    while (pos != string::npos)
    {
        s.replace(pos, len, s2);
        pos = s.find(t1, pos + len);
    }
}

void purify(string& s)
{
    replace_all(s, "&nbsp;", "");
    replace_all(s, "&copy;", "©");
    replace_all(s, "&mdash;", "—");
    rtrim(s);
    ltrim(s);
}

std::string translate(const std::string& s)
{
    ASSERT(! isDummy(s), "Internal error (dummy: %s)", s.c_str());
    return "???";
}
