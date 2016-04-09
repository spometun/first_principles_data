#include <iostream>
#include <curses.h>

#include "tools/SimpleLog.h"
#include "tools/StringUtils.h"
#include "tools/Json.h"


using namespace std;


static void processAudio(const string& htmlFilePath)
{
    const string audioBegin = R"(<div class="usd21audio">)";
    const string controlsBegin = R"(<audio controls="controls")";
    const string typeOgg = R"(type="audio/ogg")";
    const string typeAac = R"(type="audio/aac")";
    const string typeMpeg = R"(type="audio/mpeg")";
    string lessonName = getFileName(htmlFilePath, false);
    string html = loadFromFile(htmlFilePath);

    size_t pos0 = html.find(audioBegin);
    if(pos0 == string::npos)
    {
        LOG("Couldn't find audio");
        return;
    }
    pos0 += audioBegin.size();
    size_t pos1 = html.find(controlsBegin, pos0);
    ASSERT(pos1 != string::npos);
    ASSERT(pos1 - pos0 < 1000);

    html.insert(pos1 + controlsBegin.size(), " id = \"audio\"");


//    html.erase(pos0, pos1 - pos0);
//    html.insert(pos0, "\n\t\t");
//    pos1 = html.find(typeOgg, pos0);
//    ASSERT(pos1 - pos0 < 1000);
//    html.insert(pos1 + typeOgg.size(), string(" id = \"") + lessonName + "_ogg\"");

//    pos1 = html.find(typeAac, pos0);
//    ASSERT(pos1 - pos0 < 1000);
//    html.insert(pos1 + typeAac.size(), string(" id = \"") + lessonName + "_m4a\"");

//    pos1 = html.find(typeMpeg, pos0);
//    ASSERT(pos1 - pos0 < 1000);
//    html.insert(pos1 + typeMpeg.size(), string(" id = \"") + lessonName + "_mp3\"");
    saveToFile(htmlFilePath, html);
    cout<<lessonName<<" processed";
}

static void updateAudioJson(const string& jsonPath, const string& lessonName)
{
    pt::ptree audio = loadPtree(jsonPath);
    audio.put(lessonName + "_mp3", "");
    audio.put(lessonName + "_m4a", "");
    audio.put(lessonName + "_ogg", "");
    savePtree(jsonPath, audio);
}

const string g_Script = R"(

                          <script type="text/javascript">

                          function translate()
                          {
                              var url = window.location.toString();
                              var langIndex = url.indexOf('lang=');
                              if(langIndex == -1)
                              {
                                  console.log("Couldn't find required language. Do not change anything - show original version");
                                  return;
                              }
                              $(document.body).hide();
                              var lang = url.substring(langIndex+5);
                              var fileName = url.substring(url.lastIndexOf('/') + 1, url.lastIndexOf('.html'));
                              console.log(fileName);
                              var NJsonsProcessed = 0;


                              $.getJSON(
                              "lang/" + lang + "/text/" + fileName + ".json",
                              function(json)
                              {
                                  console.log("God give me strength!");
                                  document.title = json.lesson_title_translated;
                                  document.getElementById('lesson_headline').innerHTML = json.lesson_headline_translated;
                                  for(var phrase in json.phrases)
                                  {
                                      i = phrase.indexOf('_');
                                      if(phrase[i + 1] == 'e') // begining of english
                                      {
                                          continue;
                                      }
                                      phraseNum = phrase.substr(0, i);
                                      document.getElementById("phrases." + phraseNum).innerHTML = json.phrases[phrase];
                                  }
                              }).always(function(json) {
                                  NJsonsProcessed += 1;
                                  if(NJsonsProcessed == 2)
                                  {
                                      console.log("show from text");
                                      $(document.body).show();
                                  }
                              });

                              $.getJSON(
                              "lang/" + lang + "/links/scriptures.json",
                              function(json)
                              {
                                  for(var passage in json.scripture)
                                  {
                                      var link = document.getElementById("link." + passage);
                                      if(link != null)
                                      {
                                          link.href = json.scripture[passage];
                                      }
                                  }
                              }).always(function(json) {
                                  NJsonsProcessed += 1;
                                  if(NJsonsProcessed == 2)
                                  {
                                      console.log("show from scriptures");
                                      $(document.body).show();
                                  }
                              });

                          $.getJSON(
                              "lang/" + lang + "/links/audio.json",
                              function(json)
                              {
                                  var audio = document.getElementById("audio");

                                  var mp3 = document.getElementById(fileName + "_mp3");
                                  var mp3Src = json[fileName + "_mp3"];
                                  if(mp3Src)	mp3.src = mp3Src;

                                  var m4a = document.getElementById(fileName + "_m4a");
                                  var m4aSrc = json[fileName + "_m4a"];
                                  if(m4aSrc) m4a.src = m4aSrc;

                                  var ogg = document.getElementById(fileName + "_ogg");
                                  var oggSrc = json[fileName + "_ogg"];
                                  if(oggSrc) ogg.src = oggSrc;

                                  audio.load();
                              });

                          }

                          translate();


                          </script>

                          )";

static void processScript(const string& htmlFilePath)
{
    const string beginScript = "<script";
    const string endScript = "</script>";
    string html = loadFromFile(htmlFilePath);
    size_t pos0 = html.rfind(beginScript);
    size_t pos1 = html.find("function translate()");
    size_t pos2 = html.rfind(endScript);
    if(! (pos0 < pos1 && pos1 < pos2))
    {
        LOG("Coudn't recognize content - skip processing");
        return;
    }
    html.erase(pos0, pos2 + endScript.size() - pos0);
    html.insert(pos0, g_Script);
    saveToFile(htmlFilePath, html);
    cout<<getFileName(htmlFilePath)<<" processed";
}

int main(int argc, const char* argv[])
{
    ASSERT(argc == 2);
    cout<<endl;
    char c = ' ';
    while(c != 'y' && c!= 'n')
    {
        cout<<"Process file " + getFileName(argv[1]) + " [y/n]";
        cin >> c;
    }
    if(c == 'y')
    {
        //processAudio(argv[1]);
        //updateAudioJson(argv[2], getFileName(argv[1], false));
        processScript(argv[1]);
    }
    return 0;
}
