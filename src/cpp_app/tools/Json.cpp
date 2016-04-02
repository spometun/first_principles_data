#include "Json.h"

#include <iostream>


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

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
           // else if (*b == char('/')) result += char('\\'), result += char('/');
            else if (*b == char('/')) result += char('/');
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




void testPrintPtree(pt::ptree& tree, int level)
{
    cout<<endl;
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
        testPrintPtree(it->second, level + 1);
        for(int i = 0; i < level; i++) cout<<"    ";
        cout << "}" << endl;
    }
}
// << " "<< it->second.get_value<string>() << endl;


pt::ptree loadPtree(const string& fileName)
{
    pt::ptree res;
    pt::read_json(fileName, res);
    return res;
}

void savePtree(const string& fileName, const pt::ptree& ptree)
{
    pt::write_json(fileName, ptree);
}

void test(const string& fileName)
{
    pt::ptree tree;
    pt::read_xml(fileName, tree);
    testPrintPtree(tree, 0);
    pt::write_xml(fileName + "zu", tree);
    pt::write_json(fileName + "zuj", tree);

}
