#include "../include/TinyJson.h"
#include <iostream>

int main() {
    // JSON 字符串
    const char* jsonStr = R"(
        {
            "姓名": "龚",
            "年龄": 22,
            "身份": "学生",
            "婚姻状况": false
        }
    )";

    // 使用 TinyJson 库解析 JSON 字符串
    TinyJson::json j = TinyJson::parser::parse(jsonStr);

    // 访问解析后的数据
    std::string name = j["姓名"].get_string();      
    int age = j["年龄"].get_integer();               
    bool is_merried = j["婚姻状况"].get_bool();

    std::cout << "姓名: " << name << std::endl;
    std::cout << "年龄: " << age << std::endl;
    std::cout << "婚姻状况: " << (is_merried ? "已婚" : "未婚") << std::endl;

    return 0;
}