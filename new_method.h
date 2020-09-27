#ifndef NEW_METHOD_H_INCLUDED
#define NEW_METHOD_H_INCLUDED

#include "head_file.h"
#include "new_struct.h"

using namespace std;

int wtrsprintf(string& in_template, string param);

list<string> wtrsplit_string(string src, string key);

string get_value_in_pattern(string start_pattern, string end_pattern, string& line);

void wtrget_kv_info(map<string, list<string>>& dst, string src, infoFormat info_format);

void write_list_to_file(string filepath, list<string> cmd_list, int add_flag);

list<string> flush_fill_list (map<string, list<string>> in_config_map, list<list<string>> in_template_list, map<string,string> config_key);

list<string> cluster_fill_list (map<string, list<string>> in_config_map, list<list<string>> in_template_list, map<string,string> config_key);

#endif // NEW_METHOD_H_INCLUDED
