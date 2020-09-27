#include "new_method.h"

int wtrsprintf(string& in_template, string param, int last_pos){
    auto position = in_template.find("#", last_pos);
    if(position != string::npos){
        in_template = in_template.substr(0,position) + param + in_template.substr(position + 1);
    }
    last_pos = position+1;
    return last_pos;
}

list<string> wtrsplit_string(string src, string key){
    list<string> res;
    auto pos = src.find(key);
    while(pos != string::npos){
        res.push_back(src.substr(0,pos));
        src = src.substr(pos + key.length());
        pos = src.find(key);
    }
    res.push_back(src);
    return res;
}

string get_value_in_pattern(string start_pattern, string end_pattern, string& src){

    auto pos_start = src.find(start_pattern);
    auto pos_end = src.find(end_pattern);
    if(pos_start != string::npos){
        if(pos_end != string::npos){
            auto tr_start = pos_start + start_pattern.length();
            string res = src.substr(tr_start, pos_end-tr_start);
            src = src.substr(0,pos_start) + src.substr(pos_end+end_pattern.length());
            return move(res);
        }
    }
    return "*none*";
}

void wtrget_kv_info(map<string, list<string>>& dst, string src, infoFormat info_format){
    auto pos = src.find(info_format.kv_split);
    if(pos != string::npos){
        string tr_value = src.substr(pos + info_format.kv_split.length());
        string tr_key = src.substr(0, pos);
        string key_flag = get_value_in_pattern(info_format.sub_start, info_format.sub_end, tr_key);
        list<string> value_list = wtrsplit_string(tr_value, info_format.value_split);
        if(key_flag == "s"){
            for(auto it_value = value_list.begin();it_value != value_list.end();it_value ++){
                string value_sub = get_value_in_pattern(info_format.sub_start, info_format.sub_end, *it_value);
                if(value_sub != "*none*"){
                    dst[*it_value] = wtrsplit_string(value_sub,info_format.sub_split);
                }
            }
        }
        dst[tr_key] = value_list;
    }
}

void write_list_to_file(string filepath, list<string> cmd_list, int add_flag){
    fstream outfile;
    if(add_flag == 1){
        outfile.open(filepath.c_str(), ios::app);   //每次写都定位的文件结尾，不会丢失原来的内容，用out则会丢失原来的内容
    }
    else{
        outfile.open(filepath.c_str(), ios::out);   //每次写都定位的文件结尾，不会丢失原来的内容，用out则会丢失原来的内容
    }
    if(!outfile.is_open ()){
        cout << "Open file failure : " << filepath << endl;
    }else{
        for(auto it:cmd_list){
            outfile << it << endl;  //在result.txt中写入结果
        }
    }
    outfile.close();
}

list<string> flush_fill_list (map<string, list<string>> in_config_map, list<list<string>> in_template_list, map<string,string> config_key){
    list<string> res;
    map<string, wtrlist> config_map;
    list<wtrlist> template_list;
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
    for(auto it_template : in_template_list){
        wtrlist tr_list;
        tr_list.data = new string[it_template.size()];
        tr_list.length = 0;
        for(auto it_template_value : it_template){
            tr_list.data[tr_list.length++] = it_template_value;
        }
        template_list.push_back(tr_list);
    }

    for(auto it_template : template_list){
        // 初始化模板
        string bas_str = it_template.data[0];
        first_of_config = "";
        last_of_config = "";

        // 将指针都初始化为0
        for(auto it_config : config_ptr){
            config_ptr[it_config.first] = 0;
            config_valid[it_config.first] = false;
        }

        // 标记需要用到的config项
        int it_template_length = it_template.length;
        for(int i = 1;i < it_template_length;i ++){
            string key = it_template.data[i];
            if(config_valid.find(key) != config_valid.end()){
                config_valid[key] = true;
            }else{
                string key_model = get_value_in_pattern(config_key[config_sub_flag_start], config_key[config_sub_flag_end], key);
                if(key_model == "s"){
                    string current_key = config_map[key].data[0];
                    config_valid[current_key] = true;
                }
            }
        }

        for(auto it_config : config_valid){
            if(it_config.second == true){
                if(first_of_config == ""){
                    first_of_config = it_config.first;
                }
                last_of_config = it_config.first;
            }
        }
        while(true){
            string tr_res = bas_str;
            int up_flag = 0;

            // 根据指针拼接一次template
            int last_pos = 0;
            for(int i = 1;i < it_template.length;i ++){
                string key = it_template.data[i];
                if(config_map.find(key) != config_map.end()){
                    last_pos = wtrsprintf(tr_res, config_map[key].data[config_ptr[key]], last_pos);
                }else{
                    string key_model = get_value_in_pattern(config_key[config_sub_flag_start], config_key[config_sub_flag_end], key);
                    if(key_model == "s"){
                        string current_key = config_map[key].data[config_ptr[key]];
                        string last_key;
                        if(config_ptr[key] > 0){
                           last_key  = config_map[key].data[config_ptr[key]-1];
                        }
                        if(!config_valid[current_key]){
                            config_valid[last_key] = false;
                            config_valid[current_key] = true;
                            if(first_of_config == last_key){
                                first_of_config = current_key;
                            }
                            if(last_of_config == last_key){
                                last_of_config = current_key;
                            }
                        }
                        last_pos = wtrsprintf(tr_res, config_map[current_key].data[config_ptr[current_key]], last_pos);
                    }else{
                        last_pos = wtrsprintf(tr_res, key, last_pos);
                    }
                }
            }

            cout << "result line: " << tr_res << endl;
            res.push_back(tr_res);

            // 最前面的增加，使得下一次template组合不同
            cout << "first of config: " << first_of_config << endl;
            if(first_of_config != ""){
                config_ptr[first_of_config]++;
            }else{
                printf("note: a template without any key!\n");
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
    for(auto it_template : template_list){
        delete [] it_template.data;
    }
    return res;
}

list<string> cluster_fill_list (map<string, list<string>> in_config_map, list<list<string>> in_template_list, map<string,string> config_key){
    list<string> res;
    map<string, wtrlist> config_map;
    list<wtrlist> template_list;
    map<string, int> config_ptr;
    string last_of_config;
    string first_of_config;


    for(auto it_config : in_config_map){
        string key = it_config.first;
        config_ptr[key] = 0;
        config_map[key].data = new string[it_config.second.size()];
        config_map[key].length = 0;
        for(auto it_config_value : it_config.second){
            config_map[key].data[config_map[key].length++] = it_config_value;
        }
    }

    for(auto it_template : in_template_list){
        wtrlist tr_list;
        tr_list.data = new string[it_template.size()];
        tr_list.length = 0;
        for(auto it_template_value : it_template){
            tr_list.data[tr_list.length++] = it_template_value;
        }
        template_list.push_back(tr_list);
    }

    // 将指针都初始化为0
    for(auto it_config : config_ptr){
        config_ptr[it_config.first] = 0;
    }

    // 获取首位与末端
    for(auto it_config : config_ptr){
        if(first_of_config == ""){
            first_of_config = it_config.first;
        }
        last_of_config = it_config.first;
    }

    {


        while(true){
            for(auto it_template : template_list){
                // 初始化模板
                string tr_res = it_template.data[0];

                // 根据指针拼接一次template
                int last_pos = 0;
                for(int i = 1;i < it_template.length;i ++){
                    string key = it_template.data[i];
                    if(config_map.find(key) != config_map.end()){
                        last_pos = wtrsprintf(tr_res, config_map[key].data[config_ptr[key]], last_pos);
                    }else{
                        last_pos = wtrsprintf(tr_res, key,last_pos);
                    }
                }
                cout << "result line: " << tr_res << endl;
                res.push_back(tr_res);
            }


            // 最前面的增加，使得下一次template组合不同
            cout << "first of config: " << first_of_config << endl;
            if(first_of_config != ""){
                config_ptr[first_of_config]++;
            }else{
                printf("note: a template without any key!\n");
                break;
            }

            int up_flag = 0;

            // 检查是否遍历所有组合，调整指针
            for(auto it_config : config_ptr){
                string key = it_config.first;
                if(up_flag == 1){
                    config_ptr[key]++;
                    up_flag = 0;
                }
                if(config_ptr[key] == config_map[key].length){
                    config_ptr[key] = 0;
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
    for(auto it_template : template_list){
        delete [] it_template.data;
    }
    return res;
}
