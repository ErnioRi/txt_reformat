#include <map>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <vector>
using namespace std;
//-d -s -a

const string config_content_split = "config_content_split";
const string config_kv_split = "config_kv_split";
const string config_value_split = "config_value_split";
const string changable_str = "changable_str";
const string temple_kv_split = "temple_kv_split";
const string temple_value_split = "temple_value_split";
const string config_flag = "config_flag";
const string comment_flag = "comment_flag";
const string write_flag = "write_flag";

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
    infoFormat(string in_kv_split,string in_value_split)
        :kv_split(in_kv_split)
        ,value_split(in_value_split)
    {}
    ~infoFormat(){}
};

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



list<string> fill_list (map<string, list<string>> in_config_map, list<list<string>> in_temple_list){
    list<string> res;
    map<string, wtrlist> config_map;
    list<wtrlist> temple_list;
    map<string, int> config_ptr;
    map<string, bool> config_valid;
    string last_of_config;
    string first_of_config;


    for(auto it_config : in_config_map){
        string key = it_config.first;
        config_ptr[key] = 0;
        config_valid[key] = false;
        config_map[key].data = new string[it_config.second.size()];
        config_map[key].length = 0;
        for(auto it_config_value : it_config.second){
            config_map[key].data[config_map[key].length++] = it_config_value;
        }
    }
    for(auto it_temple : in_temple_list){
        wtrlist tr_list;
        tr_list.data = new string[it_temple.size()];
        tr_list.length = 0;
        for(auto it_temple_value : it_temple){
            tr_list.data[tr_list.length++] = it_temple_value;
        }
        temple_list.push_back(tr_list);
    }

    for(auto it_temple : temple_list){
        // 初始化模板
        string bas_str = it_temple.data[0];
        first_of_config = "";
        last_of_config = "";

        // 将指针都初始化为0
        for(auto it_config : config_ptr){
            config_ptr[it_config.first] = 0;
            config_valid[it_config.first] = false;
        }

        // 标记需要用到的config项
        for(int i = 1;i < it_temple.length;i ++){
            string key = it_temple.data[i];
            if(config_valid.find(key) != config_valid.end()){
               config_valid[key] = true;
            }
        }

        for(auto it_config : config_valid){
            if(config_valid[it_config.first] == true){
                if(first_of_config == ""){
                    first_of_config = it_config.first;
                }
                last_of_config = it_config.first;
            }
        }
        while(true){
            string tr_res = bas_str;
            int up_flag = 0;

            // 根据指针拼接一次temple
            for(int i = 1;i < it_temple.length;i ++){
                string key = it_temple.data[i];
                if(config_map.find(key) != config_map.end()){
                    wtrsprintf(tr_res, config_map[key].data[config_ptr[key]]);
                }else{
                    wtrsprintf(tr_res, key);
                }
            }

            cout << "result line: " << tr_res << endl;
            res.push_back(tr_res);

            // 最前面的增加，使得下一次temple组合不同
            cout << "first of config: " << first_of_config << endl;
            if(first_of_config != ""){
                config_ptr[first_of_config]++;
            }else{
                printf("note: a temple without any key!\n");
                break;
            }

            // 检查是否遍历所有组合，调整指针
            for(auto it_config : config_ptr){
                string key = it_config.first;
                if(config_valid[key] == false){
                    continue;
                }
                if(up_flag == 1){
                    config_ptr[it_config.first]++;
                    up_flag = 0;
                }
                if(config_ptr[it_config.first] == config_map[key].length){
                    config_ptr[it_config.first] = 0;
                    up_flag = 1;
                }
            }
            /*for(auto it_config : config_ptr){
                cout << "current ptr " << it_config.first << ":" << it_config.second << endl;
            }*/
            if(up_flag == 1){
                printf("all got\n");
                break;
            }
        }
        printf("end of while\n");
    }

    // 释放
    for(auto it_config : config_map){
        delete [] config_map[it_config.first].data;
    }
    for(auto it_temple : temple_list){
        delete [] it_temple.data;
    }
    return res;
}

void write_file(string filepath, list<string> cmd_list){
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

int main(int opt, char** content){
    // attention
    string optcontent;
    if(content[1]){
        optcontent = content[1];
    }else{
        optcontent = "";
    }
    if(optcontent != "y"){
        while(true){
            char buffer_clear;
            printf("put the path of config files you need in ./config/files.txt, terminal by '\\n'\n"
                   "are you sure to continue? (y/n):");
            char input_char;
            scanf("%c", &input_char);
            while((buffer_clear = getchar()) != '\n'){
            }
            if(input_char == 'n'){
                printf("end by user");
                return 0;
            }else if(input_char == 'y'){
                printf("start run!\n");
                break;
            }else{
                printf("error input!\n");
            }
        }
    }
    fstream op;
    char buffer[256];
    string per_line;
    map<string, list<string> > config_map;
    list<list<string>> temple_list;
    int line = 0;
    vector<string> file_path;

    // config key
    map<string,string> config_key;
    config_key[config_content_split] = "";
    config_key[config_kv_split] = "";
    config_key[config_value_split] = "";
    config_key[changable_str] = "";
    config_key[temple_kv_split] = "";
    config_key[temple_value_split] = "";
    config_key[config_flag] = "";
    config_key[comment_flag] = "";
    config_key[write_flag] = "";

    op.open("./config/base.txt",ios_base::in);
    if(!op.is_open ()){
        cout << "Open base file failure" << endl;
    }
    while(!op.eof()){
        op.getline(buffer,256,'\n');
        per_line = buffer;
        while(true){
            auto position = per_line.find("\t");
            if(position != string::npos){
                per_line = per_line.substr(0,position) + per_line.substr(position + 1);
            }else{
                break;
            }
        }
        // 检查关键字
        int find_flag = 0;
        for(auto it_key : config_key){
            printf("start config! ");
            if(per_line.find(it_key.first) != string::npos){
                cout << it_key.first << endl;
                config_key[it_key.first] = per_line.substr(it_key.first.length()+1);
                find_flag = 1;
                break;
            }
        }
        if(find_flag != 1){
            printf("config file error!\n");
            return 1;
        }

    }
    op.close();
    printf("end of key config\n");
    op.open("./config/config_files.txt",ios_base::in);
    if(!op.is_open ()){
        cout << "Open config file failure" << endl;
    }
    while(!op.eof()){
        op.getline(buffer,256,'\n');
        per_line = buffer;
        while(true){
            auto position = per_line.find("\t");
            if(position != string::npos){
                per_line = per_line.substr(0,position) + per_line.substr(position + 1);
            }else{
                break;
            }
        }
        // 检查关键字
        file_path.push_back(per_line);
    }
    op.close();
    printf("end of file config\n");
    infoFormat config_info_format(config_key[config_kv_split], config_key[config_value_split]);
    infoFormat temple_info_format(config_key[temple_kv_split], config_key[temple_value_split]);
    for( auto it : file_path){
        op.open(it,ios_base::in);
        if(!op.is_open ()){
            cout << "Open user file failure : " << it << endl;
        }
        cout << "file path: " << it << endl;
        while(!op.eof()){
            line++;
            printf("in line: %d\n", line);

            op.getline(buffer,256,'\n'); // 以\n分隔
            per_line = buffer;

            // 清理\t
            while(true){
                auto position = per_line.find("\t");
                if(position != string::npos){
                    per_line = per_line.substr(0,position) + per_line.substr(position + 1);
                }else{
                    break;
                }
            }
            // 检查关键字
            if(per_line.find(config_key[comment_flag]) == 0){
                continue;
            }else if(per_line.find(config_key[config_flag]) == 0){
                per_line = per_line.substr(config_key[config_flag].length());
                list<string> tr_config_list = wtrsplit_string(per_line, config_key[config_content_split]);
                for(auto it_config : tr_config_list){
                    wtrget_kv_info(config_map, it_config, config_info_format);
                }
            }else if(per_line.find(config_key[write_flag]) == 0){
                string write_file_path = per_line.substr(config_key[write_flag].length());
                printf("\n start fill\n\n");
                list<string> txt_list = fill_list(config_map, temple_list);
                printf("\n have filt!\n\n");
                write_file(write_file_path, txt_list);
                printf("\n have written!\n\n");
                config_map.clear();
                temple_list.remove_if([](auto it){return true;});
            }else{
                auto pos = string::npos;
                list<string> value_tr;
                list<string> value;
                if((pos = per_line.find(temple_info_format.kv_split)) != string::npos){
                    value_tr.push_back(per_line.substr(0, pos));
                    per_line = per_line.substr(pos + temple_info_format.kv_split.length());
                    value = wtrsplit_string(per_line, temple_info_format.value_split);
                    for(auto it : value){
                        value_tr.push_back(it);
                    }
                    temple_list.push_back(value_tr);
                }
            }
        }
        op.close();
    }
    printf("finish work, press any key to continue:");
    char end_char = getchar();
    return 0;
}
// 密码格式：10位，前五位数字，后五位字母
// @pos1:1 2 3 4 5,pos2:2 6 5 1 4,pos3:2 1 0,pos4:6,pos5:9 8 5
//pos6:q w e r t y u i o p a s d f g h j k l z x c v b n m,pos7:^pos6,pos8:^pos6,pos9:^pos6,pos10:^pos6
// ##########&pos1,pos2,pos3,pos4,pos5,pos6,pos7,pos8,pos9,pos10
