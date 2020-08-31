
#include "head_file.h"
#include "new_struct.h"
#include "new_method.h"

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
const string file_model_start = "file_model_start";
const string file_model_end = "file_model_end";


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
    config_key[config_content_split] = "!!!";
    config_key[config_kv_split] = "!!!";
    config_key[config_value_split] = "!!!";
    config_key[changable_str] = "!!!";
    config_key[temple_kv_split] = "!!!";
    config_key[temple_value_split] = "!!!";
    config_key[config_flag] = "!!!";
    config_key[comment_flag] = "!!!";
    config_key[write_flag] = "!!!";
    config_key[file_model_start] = "!!!";
    config_key[file_model_end] = "!!!";

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
                config_map.clear();
                temple_list.clear();
                per_line = per_line.substr(config_key[config_flag].length());
                list<string> tr_config_list = wtrsplit_string(per_line, config_key[config_content_split]);
                for(auto it_config : tr_config_list){
                    wtrget_kv_info(config_map, it_config, config_info_format);
                }

            }else if(per_line.find(config_key[write_flag]) == 0){
                per_line = per_line.substr(config_key[write_flag].length());
                string file_model = "flush";
                auto pos_start = string::npos;
                auto pos_end = string::npos;
                if((pos_start = per_line.find(config_key[file_model_start])) != string::npos){
                    if((pos_end = per_line.find(config_key[file_model_end])) != string::npos){
                        auto tr_start = pos_start + config_key[file_model_start].length();
                        file_model = per_line.substr(tr_start, pos_end-tr_start);
                        cout<< "file_model: " << file_model << endl;
                        per_line = per_line.substr(0,pos_start) + per_line.substr(pos_end+config_key[file_model_end].length());
                    }
                }

                cout<< "per_line1: " << per_line.substr(0,pos_start) << endl;
                string write_file_path = per_line;
                printf("\n start fill\n\n");
                list<string> txt_list;
                if(file_model == "flush"){
                    txt_list = flush_fill_list(config_map, temple_list);

                }else if(file_model == "cluster"){
                    txt_list = cluster_fill_list(config_map, temple_list);

                }
                printf("\n have filt!\n\n");
                write_list_to_file(write_file_path, txt_list);
                printf("\n have written!\n\n");

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
        config_map.clear();
        temple_list.clear();
    }
    printf("finish work, press enter to continue:");
    char end_char = getchar();
    return 0;
}
// 密码格式：10位，前五位数字，后五位字母
// @pos1:1 2 3 4 5,pos2:2 6 5 1 4,pos3:2 1 0,pos4:6,pos5:9 8 5
//pos6:q w e r t y u i o p a s d f g h j k l z x c v b n m,pos7:^pos6,pos8:^pos6,pos9:^pos6,pos10:^pos6
// ##########&pos1,pos2,pos3,pos4,pos5,pos6,pos7,pos8,pos9,pos10
