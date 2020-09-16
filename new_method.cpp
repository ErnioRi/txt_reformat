#include "new_method.h"

int wtrsprintf(string& temple, string param, int last_pos){
    auto position = temple.find("#", last_pos);
    if(position != string::npos){
        temple = temple.substr(0,position) + param + temple.substr(position + 1);
    }
    last_pos = position+1;
    return last_pos;
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

list<string> flush_fill_list (map<string, list<string>> in_config_map, list<list<string>> in_temple_list){
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
            int last_pos = 0;
            for(int i = 1;i < it_temple.length;i ++){
                string key = it_temple.data[i];
                if(config_map.find(key) != config_map.end()){
                    last_pos = wtrsprintf(tr_res, config_map[key].data[config_ptr[key]], last_pos);
                }else{
                    last_pos = wtrsprintf(tr_res, key, last_pos);
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

list<string> cluster_fill_list (map<string, list<string>> in_config_map, list<list<string>> in_temple_list){
    list<string> res;
    map<string, wtrlist> config_map;
    list<wtrlist> temple_list;
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

    for(auto it_temple : in_temple_list){
        wtrlist tr_list;
        tr_list.data = new string[it_temple.size()];
        tr_list.length = 0;
        for(auto it_temple_value : it_temple){
            tr_list.data[tr_list.length++] = it_temple_value;
        }
        temple_list.push_back(tr_list);
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
            for(auto it_temple : temple_list){
                // 初始化模板
                string tr_res = it_temple.data[0];

                // 根据指针拼接一次temple
                int last_pos = 0;
                for(int i = 1;i < it_temple.length;i ++){
                    string key = it_temple.data[i];
                    if(config_map.find(key) != config_map.end()){
                        last_pos = wtrsprintf(tr_res, config_map[key].data[config_ptr[key]], last_pos);
                    }else{
                        last_pos = wtrsprintf(tr_res, key,last_pos);
                    }
                }
                cout << "result line: " << tr_res << endl;
                res.push_back(tr_res);
            }


            // 最前面的增加，使得下一次temple组合不同
            cout << "first of config: " << first_of_config << endl;
            if(first_of_config != ""){
                config_ptr[first_of_config]++;
            }else{
                printf("note: a temple without any key!\n");
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
    for(auto it_temple : temple_list){
        delete [] it_temple.data;
    }
    return res;
}
