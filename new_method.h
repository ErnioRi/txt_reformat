#ifndef NEW_METHOD_H_INCLUDED
#define NEW_METHOD_H_INCLUDED

#include "head_file.h"
#include "new_struct.h"

using namespace std;

int wtrsprintf(string& temple, string param);

list<string> wtrsplit_string(string src, string key);

void wtrget_kv_info(map<string, list<string>>& dst, string src, infoFormat info_format);

void write_list_to_file(string filepath, list<string> cmd_list);

list<string> flush_fill_list (map<string, list<string>> in_config_map, list<list<string>> in_temple_list);

list<string> cluster_fill_list (map<string, list<string>> in_config_map, list<list<string>> in_temple_list);

#endif // NEW_METHOD_H_INCLUDED
