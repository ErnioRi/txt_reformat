
#include "head_file.h"
#include "new_struct.h"
#include "new_method.h"

using namespace std;
//-d -s -a


string delete_tab(string src){
    auto position = src.find('\t');
    while(true){
        if(position == string::npos){
            break;
        }
        src = src.substr(0,position) + src.substr(position + 1);
        position = src.find("\t", position);
    }
    return src;
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
    list<list<string>> txt_template_list;
    vector<string> file_path;

    // config key
    map<string,string> config_key;
    config_key[config_content_split] = "!!!";
    config_key[config_kv_split] = "!!!";
    config_key[config_value_split] = "!!!";
    config_key[changable_str] = "!!!";
    config_key[txt_template_kv_split] = "!!!";
    config_key[txt_template_value_split] = "!!!";
    config_key[config_flag] = "!!!";
    config_key[comment_flag] = "!!!";
    config_key[write_flag] = "!!!";
    config_key[file_model_start] = "!!!";
    config_key[file_model_end] = "!!!";
    config_key[clear_flag] = "!!!";
    config_key[config_sub_flag_start] = "!!!";
    config_key[config_sub_flag_end] = "!!!";
    config_key[config_sub_split] = "!!!";

    op.open("./config/base.txt",ios_base::in);
    if(!op.is_open ()){
        cout << "Open base file failure" << endl;
    }
    while(!op.eof()){
        op.getline(buffer,256,'\n');
        per_line = buffer;
        delete_tab(per_line);
        // 检查关键字
        int find_flag = 0;
        for(auto it_key : config_key){
            if(per_line.find(it_key.first) != string::npos){
                config_key[it_key.first] = per_line.substr(it_key.first.length());
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
    op.open("./config/config_files.txt",ios_base::in);
    if(!op.is_open ()){
        cout << "Open config file failure" << endl;
    }
    while(!op.eof()){
        op.getline(buffer,256,'\n');
        per_line = buffer;
        delete_tab(per_line);
        file_path.push_back(per_line);
    }
    op.close();
    infoFormat config_info_format(config_key[config_kv_split],
                                  config_key[config_value_split],
                                  config_key[config_sub_flag_start],
                                  config_key[config_sub_flag_end],
                                  config_key[config_sub_split]);

    infoFormat txt_template_info_format(config_key[txt_template_kv_split],
                                        config_key[txt_template_value_split]);

    for( auto it : file_path){
        op.open(it,ios_base::in);
        if(!op.is_open ()){
            cout << "Open user file failure : " << it << endl;
        }
        cout << "file path: " << it << endl;
        int line = 0;
        while(!op.eof()){
            line++;
            printf("in line: %d\n", line);

            op.getline(buffer,256,'\n'); // 以\n分隔
            per_line = buffer;

            // 清理\t
            delete_tab(per_line);

            // 检查关键字
            cout << config_key[config_flag] << endl;
            printf("config :%d\n", per_line.find(config_key[config_flag]));
            if(per_line.find(config_key[comment_flag]) == 0){// 注释
                continue;

            }else if(per_line.find(config_key[config_flag]) == 0){// 配置行
                per_line = per_line.substr(config_key[config_flag].length());
                list<string> tr_config_list = wtrsplit_string(per_line, config_key[config_content_split]);
                for(auto it_config : tr_config_list){
                    wtrget_kv_info(config_map, it_config, config_info_format);
                }
            }else if(per_line.find(config_key[clear_flag]) == 0){// 清除缓存
                config_map.clear();
                txt_template_list.clear();

            }else if(per_line.find(config_key[write_flag]) == 0){// 写文件
                per_line = per_line.substr(config_key[write_flag].length());

                // 文件模式检查
                string file_model = get_value_in_pattern(config_key[file_model_start], config_key[file_model_end], per_line);
                if(file_model == "*none*"){
                    file_model = "flush";
                }
                cout<< "file_model: " << file_model << endl;

                int add_flag = 0;
                if(file_model[0] == 'a'){
                    file_model = file_model.substr(1);
                    add_flag = 1;
                }
                if(file_model == "flush"){
                    write_list_to_file(per_line, move(flush_fill_list(config_map, txt_template_list, config_key)), add_flag);

                }else if(file_model == "cluster"){
                    write_list_to_file(per_line, move(cluster_fill_list(config_map, txt_template_list, config_key)), add_flag);

                }else{
                    printf("unknown file model\n");

                }
            }else{// 视为template
                auto pos = string::npos;
                list<string> value_tr;
                list<string> value;
                if((pos = per_line.find(txt_template_info_format.kv_split)) != string::npos){
                    printf("in else\n");
                    value_tr.push_back(per_line.substr(0, pos));
                    per_line = per_line.substr(pos + txt_template_info_format.kv_split.length());
                    printf("pos: %d\n", pos);
                    value = wtrsplit_string(per_line, txt_template_info_format.value_split);
                    int sss = 0;
                    for(auto it : value){
                        printf("%d\n", sss);
                        value_tr.push_back(it);
                    }
                    txt_template_list.push_back(value_tr);
                }
            }
        }
        op.close();
        config_map.clear();
        txt_template_list.clear();
    }
    printf("finish work, press enter to continue:");
    char end_char = getchar();
    return 0;
}
// 密码格式：10位，前五位数字，后五位字母
// @pos1:1 2 3 4 5,pos2:2 6 5 1 4,pos3:2 1 0,pos4:6,pos5:9 8 5
//pos6:q w e r t y u i o p a s d f g h j k l z x c v b n m,pos7:^pos6,pos8:^pos6,pos9:^pos6,pos10:^pos6
// ##########&pos1,pos2,pos3,pos4,pos5,pos6,pos7,pos8,pos9,pos10
