#include "myJson.hpp"
#include "cqfun.hpp"
#include "mycurl.hpp"

#include <cqcppsdk/cqcppsdk.hpp>
#include <iostream>
#include <fstream>
#include <typeinfo>

using namespace std;

//初始化于init.cpp
extern string appDir;

MyJson::MyJson()
    {
        long_limit = 150;
        short_limit = 10;
    }

//json写入文件
bool MyJson::write_json(string path, json jroot) 
    {
        ofstream file;
        file.open(cq::utils::ansi(path));
        if (!file.good()) {
            return false;
        } else {
            //文件可读性考虑
            file << this->conf_json.dump(4);
            file.close();
            return true;
        }
    }

//文件读取json
bool MyJson::read_json(string path, json jroot) 
    {
        ifstream file;
        file.open(cq::utils::ansi(path));
        if (!file.good()) {
            //如果未成功打开文件，新建文件
            fstream newfile;
            newfile.open(cq::utils::ansi(path), ios::out);
            json basejson = R"(
            {
                "API-key": "",
                "Adminer": [],
                "EnableGroup": [],
                "Filter": 33.33,
                "LowSimRetuen": "识图是有极限,所以,我不识图了,JOJO!",
                "num":0
            })"_json;
            newfile << basejson.dump(4);
            newfile.close();
            this->conf_json = basejson;
        } else {
            this->conf_json << file;
            file.close();
            return true;
        }
    }

//解析json数组 调用之前务必确保 vector<类型> 与 json_array 内的值保持一致
template <typename T >
bool MyJson::dis_jarray ( vector<T> &Varray, json Jarray) 
    {

        if( !Jarray.is_array() || Jarray.is_null() ) {
            return false;
        }

        for( auto jit : Jarray ){
            Varray.emplace_back(static_cast<T> (jit));
        }

    }



//从conf_json读取许可群列表到变量
void MyJson::read_EnbGrp() 
    {
        cq::logging::debug("redE","do");
        //得到conf_json中的关键数组
        auto jarray = this -> conf_json.at("/EnableGroup"_json_pointer);

        //清除滞留数据
        this -> Enable_Group.clear();

        //解析关键数组到 EnbGrp
        try {
            dis_jarray(this->Enable_Group,jarray);
        } catch (exception& e) {
            cq::logging::error("read_EnbGrp", e.what());
        }
        
    }

//从conf_json读取管理员列表到变量
void MyJson::read_AdLis() 
    {
        //得到conf_json中的关键数组
        auto jarray = this -> conf_json.at("/Adminer"_json_pointer);

        //清除滞留数据
        this -> Adminer_List.clear();

        //解析关键数组到 AdLis
        try {
            dis_jarray(this->Adminer_List,jarray);
        } catch (exception& e) {
            cq::logging::error("read_AdLis", e.what());
        }
    }

//从conf_json读取 API_KEY
void MyJson::read_APIkey() 
    {
        API_KEY = conf_json.value("API-key","null");
    }

void MyJson::read_Filter() 
    {
        Filter = conf_json.value("Filter",80);
    }

void MyJson::read_LowSim() 
    {
        LowSim = conf_json.value("LowSimRetuen","识图是有极限的,所以，我不识图了！");
    }

void MyJson::read_num() 
    {
        num = conf_json.value("num",0);
    }


//从变量保存到conf_json
void MyJson::write_EnbGrp() 
    {
        json j_temp(Enable_Group);
        conf_json.at("/EnableGroup"_json_pointer) = j_temp;
    }

// 从变量保存到conf_json
void MyJson::write_AdLis() 
    {
        json j_temp(Adminer_List);
        conf_json.at("/Adminer"_json_pointer) = j_temp;
    }

void MyJson::write_APIkey() 
    {
        conf_json["API-key"] = API_KEY;
    }

void MyJson::write_Filter() 
    {
        conf_json["Filter"] = Filter;
    }

void MyJson::write_LowSim() 
    {
        conf_json["LowSimRetuen"] = LowSim;
    }

void MyJson::write_num() 
    {
        conf_json["num"] = num;
    }




//将conf_json写到文件里
void MyJson::json2file() 
    {
        try {
            write_json((appDir + "conf.json"), conf_json);
        } catch (exception& e) {
            cq::logging::error("read_json", e.what());
        }
        cq::logging::debug("json2file","done");
    }

//从文件中读取conf_json
void MyJson::file2json() 
    {
        try {
            read_json((appDir + "conf.json"), conf_json);
        } catch (exception& e) {
            cq::logging::error("read_json", e.what());
        }
    }


void MyJson::all2read() 
    {
        read_EnbGrp();
        read_AdLis();
        read_APIkey();
        read_Filter();
        read_LowSim();
        read_num();
    }

void MyJson::all2write() 
    {
        write_EnbGrp();
        write_AdLis();
        write_APIkey();
        write_Filter();
        write_LowSim();
        write_num();
    }



void MyJson::get_serch(string str) 
    {
        num++;
        write_num();
        imgurl = cqfun::get_img_url(str);
        stringstream buf,hbuf;
        if( mycurl::get_search_result(hbuf,buf,imgurl) ) {
            search_json << buf;
            cq::logging::debug("result_head" , hbuf.str());
            del_rjson();
        } else {
            res = "看来哪里出现问题了呢，查看日志报告给开发者吧";
            cq::logging::debug("result_head" , hbuf.str());
        }
    }



std::string MyJson::get_time_to(long long time){
    int sec = int(time);
    int h = sec / 3600;
    int m = sec % 3600 / 60;
    int s = sec % 60;
    
    std::string hs,ms,ss;
    if(h<10){ hs="0"+to_string(h); } else { hs = to_string(h); }
    if(m<10){ ms="0"+to_string(m); } else { ms = to_string(m); }
    if(s<10){ ss="0"+to_string(s); } else { ss = to_string(s); }
    
    return hs+":"+ms+":"+ss;
}


std::string MyJson::get_similarity(json j){
    float sim = j.at("/similarity"_json_pointer);
    sim=sim*100;
    std::string sim_t = to_string(sim);
    std::string similarity;
    similarity.assign(sim_t,0,5);
    return similarity + "%";
}









bool MyJson::del_rjson () 
    {
        json j = search_json;
        if(!j.contains("RawDocsCount")){
            res = j.dump(4);
            return false;
        }
        
        short_limit = j.value("limit",0);
        long_limit = j.value("quota",0);


        json jdocs = j.at("/docs"_json_pointer);
        if(jdocs.empty()){
            res = "docs is empty , try later";
            return false;
        }

        json jresult = jdocs.at("/0"_json_pointer);
        float sim = jresult.at("/similarity"_json_pointer);
        if( static_cast<double>(sim) < this->sim) {
            res = LowSim;
            return false;
        }
        auto similarity = get_similarity(jresult);
        auto title_native = jresult.value("title_native","null");
        auto title_chinese = jresult.value("title_chinese","null");
        auto title_english = jresult.value("title_english","null");
        std::string episode;
        if(jresult.at("/episode"_json_pointer).is_number()){
            int ep = jresult.at("/episode"_json_pointer);
            episode = to_string(ep);
        } else {
            episode = jresult.at("/episode"_json_pointer);
        }
        std::string is_adult = to_string(jresult.value("is_adult",false));
        auto season = jresult.value("season","null");
        auto at = get_time_to(jresult.value("at",0ll));
        auto anilist_id = to_string(jresult.value("anilist_id",0));
        auto anilist_url = "https://anilist.co/anime/"+anilist_id;

        res = "相似度:" + similarity + "\r\n日文名:" + title_native + "\r\n中文名:" + title_chinese + "\r\n英文名:" + title_english +"\r\n季度:" + season +"\r\n位置: #" + episode + " " + at +"\r\nis_hentai:" + is_adult + "\r\n作品信息:" + anilist_url;
        
        return true;
    }