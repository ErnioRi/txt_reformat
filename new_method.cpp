#include "new_method.h"

int wtrsprintf(string& temple, string param){
    auto position = temple.find("#");
    if(position != string::npos){
        temple = temple.substr(0,position) + param + temple.substr(position + 1);
    }
    return 0;
}

list<string> wtrsplit_string(string src, string key){
    list<string> res;
    auto pos = string::npos;
    while((pos = src.find(key)) != string::npos){
        res.push_back(src.substr(0,pos));
        src = src.substr(pos + key.length());
    }
    res.push_back(src);
    return res;
}

void wtrget_kv_info(map<string, list<string>>& dst, string src, infoFormat info_format){
    auto pos = string::npos;
    string key;
    list<string> value;
    if((pos = src.find(info_format.kv_split)) != string::npos){
        key = src.substr(0, pos);
        string tr_value = src.substr(pos + info_format.kv_split.length());
        value = wtrsplit_string(tr_value, info_format.value_split);
        dst[key] = value;
    }
}

void write_list_to_file(string filepath, list<string> cmd_list){
    fstream outfile;
    outfile.open(filepath.c_str(), ios::out);   //每次写都定位的文件结尾，不会丢失原来的内容，用out则会丢失原来的内容
    if(!outfile.is_open ()){
        cout << "Open file failure" << endl;
    }else{
        for(auto it:cmd_list){
            outfile << it << endl;  //在result.txt中写入结果
        }
    }
    outfile.close();
}
