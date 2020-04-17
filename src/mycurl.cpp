#include "mycurl.hpp"
#include "myJson.hpp"

extern MyJson conf;

const std::string trace_API = "https://trace.moe/api/search?";
const std::string hitokoto_API = "https://v1.hitokoto.cn/?c=a&c=c&encode=text";

bool mycurl::get_search_result(std::stringstream &http_head, std::stringstream &http_result, std::string imgurl) 
    {
        CURL *curl;
        CURLcode result_code;
        curl = curl_easy_init();
        if (curl) {
            std::string url;
            if(!conf.API_KEY.empty()) {
                url  = trace_API + "token=" + conf.API_KEY + "&" + imgurl;
            } else {
                url = trace_API  + imgurl;
            }
            
            curl_easy_setopt(curl, CURLOPT_URL, url.data());
            curl_easy_setopt(curl, CURLOPT_POST, 1);

            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);


            auto receive = [](char *buf, size_t size, size_t count, void *data) {
                (*static_cast<std::stringstream *>(data)) << std::string(buf, count);
                return size * count;
            };
            typedef size_t (*WriteFunction)(char *, size_t, size_t, void *);


            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, static_cast<WriteFunction>(receive));
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &http_head);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, static_cast<WriteFunction>(receive));
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &http_result);
            
            result_code = curl_easy_perform(curl);
            if (result_code != CURLE_OK) {
                cq::logging::error("curl_easy_perform",  curl_easy_strerror(result_code) );
                curl_easy_cleanup(curl);
                return false;
            }
        } else {
            cq::logging::error("curl_easy_init",  "failed" );
            return false;
        }
        return true;
    }


std::string mycurl:: get_hitokoto() 
    {
        std::string body;

        auto curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, hitokoto_API.data());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            auto receive = [](char *buf, size_t size, size_t count, void *data) {
                (*static_cast<std::string *>(data)) += std::string(buf, count);
                return size * count;
            };
            typedef size_t (*WriteFunction)(char *, size_t, size_t, void *);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, static_cast<WriteFunction>(receive));
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);

            curl_easy_perform(curl);

            curl_easy_cleanup(curl);
        }

        return body;
    }

string mycurl::get_me_limit()
    {
        std::string body;

        auto curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, hitokoto_API.data());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            auto receive = [](char *buf, size_t size, size_t count, void *data) {
                (*static_cast<std::string *>(data)) += std::string(buf, count);
                return size * count;
            };
            typedef size_t (*WriteFunction)(char *, size_t, size_t, void *);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, static_cast<WriteFunction>(receive));
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);

            curl_easy_perform(curl);

            curl_easy_cleanup(curl);
        }

        json j = j.parse(body);

        string res;

        conf.long_limit = j.value("quota",0);
        conf.short_limit = j.value("limit",0);

        res = "60秒余量: " + to_string(conf.short_limit) + "\r\n24小时余量: " + to_string(conf.long_limit) + "\r\n累计搜番: " + to_string(conf.num);

        return res;
    }