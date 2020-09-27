#ifndef NEW_STRUCT_H_INCLUDED
#define NEW_STRUCT_H_INCLUDED

#include "head_file.h"

using namespace std;
struct wtrlist{
    int length;
    string* data;
};
struct configInfo{
    list<string> config_content;
    int cur_idx;
};


class infoFormat{
public:
    string kv_split;
    string value_split;
    string sub_start;
    string sub_end;
    string sub_split;
    infoFormat(string in_kv_split,
               string in_value_split,
               string in_sub_start = "!!!",
               string in_sub_end = "!!!",
               string in_sub_split = "!!!")
        :kv_split(in_kv_split)
        ,value_split(in_value_split)
        ,sub_start(in_sub_start)
        ,sub_end(in_sub_end)
        ,sub_split(in_sub_split)
    {}
    ~infoFormat(){}
};

#endif // NEW_STRUCT_H_INCLUDED
