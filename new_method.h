#ifndef NEW_METHOD_H_INCLUDED
#define NEW_METHOD_H_INCLUDED

#include "head_file.h"
#include "new_struct.h"

using namespace std;

int wtrsprintf(string& temple, string param);

list<string> wtrsplit_string(string src, string key);

void wtrget_kv_info(map<string, list<string>>& dst, string src, infoFormat info_format);

void write_list_to_file(string filepath, list<string> cmd_list);

#endif // NEW_METHOD_H_INCLUDED
