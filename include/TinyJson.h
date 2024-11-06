 /// @file TinyJson.h
 /// @brief TinyJson 库的主要头文件，提供 JSON 的解析和序列化功能。
 /// @author [yan]


#pragma once

#include <istream>
#include <sstream>
#include <string>
#include <locale>
#include <vector>
#include <map>
#include <algorithm>
#include <codecvt>

namespace TinyJson
{

    using u32_istream = std::basic_istream<char32_t>;
    using u32_sstream = std::basic_stringstream<char32_t>;

// 检查用于检查 JSON 对象的类型是否与预期匹配，如果不匹配则抛出异常
#define CHECK_TYPE_MISMATCH(t1, t2)                                   \
    {                                                                 \
        if (t1 != t2)                                                 \
        {                                                             \
            std::ostringstream oss;                                   \
            oss << "expect type " << t1 << ", but found type " << t2; \
            throw std::runtime_error(oss.str());                      \
        }                                                             \
    }

    // 去空白字符
    inline std::string trim(const std::string &&str,
                            const std::string &whitespace = " \t\r\n")
    {
        const auto strBegin = str.find_first_not_of(whitespace);
        if (strBegin == std::string::npos)
            return "";

        const auto strEnd = str.find_last_not_of(whitespace);
        const auto strRange = strEnd - strBegin + 1;

        return str.substr(strBegin, strRange);
    }

    // UTF-32 编码的字符串转换为 UTF-8 编码的字符
    inline std::string U32ToU8(std::u32string u32)
    {
        try
        {
            // std::wstring_convert 用于在不同编码的字符串之间进行转换
            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf32conv;
            return utf32conv.to_bytes(u32);
        }
        catch (std::range_error &ex)
        {
            // 否则
            throw std::runtime_error("invalid utf32 string: " + std::string(ex.what()));
        }
    }

    // UTF-8 编码的字符串转换为 UTF-32 编码的字符
    inline std::u32string U8ToU32(std::string u8)
    {
        try
        {
            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf32conv;
            return utf32conv.from_bytes(u8);
        }
        catch (const std::exception &ex)
        {
            // 否则
            throw std::runtime_error("invalid utf32 string: " + std::string(ex.what()));
        }
    }

    // 枚举类型 json_t 表示 JSON 值的可能数据类型
    enum json_t
    {
        string = 0,     // 表示一个 JSON 字符串
        number_integer, // 表示一个 JSON 整数
        number_double,  // 表示一个 JSON 浮点数
        array,          // 表示一个 JSON 数组
        boolean,        // 表示一个 JSON 布尔值（true 或 false）
        object,         // 表示一个 JSON 对象
        null,           // 表示 JSON 的 null 值
        invalid         // 表示无效或未知的 JSON 类型
    };

    class json;

    using json_object = std::map<std::string, json>;
    using json_array = std::vector<json>;

    // json 类表示一个 JSON 值
    // 可以是字符串、数字、数组、布尔值、对象或 null
    class json
    {
    private:
        /// 指向 JSON 值实际数据的指针
        void *_value;
        /// JSON 值的类型
        json_t _type;

    public:
        json();
        json(std::string val); // 符串类型
        json(std::string &val);
        json(char val[]);
        json(double val);         // 浮点数类型
        json(int val);            // 整数类型
        json(long long val);      // 长整型类型
        json(bool val);           // 布尔类型
        json(json_array &array);  // 数组类型
        json(json_array &&array); // 数组类型（移动语义）
        json(json_object &obj);   // 对象类型
        json(json_object &&obj);  // 对象类型（移动语义）

        ~json();

        /// 获取 JSON 值的类型
        const json_t type() const;           // 返回 JSON 值的类型
        const std::string type_name() const; // 返回 JSON 值类型的名称

        json(const json &other);
        json &operator=(const json &other);

        bool operator==(const json &rhs) const;
        bool operator!=(const json &rhs) const;

        const std::string get_string() const;
        const long long get_integer() const;
        const double get_double() const;
        const bool get_bool() const;
        const json_object get_object() const;
        const json_array get_array() const;
        const void *get_null() const;

        /// 获取 JSON 值的大小（数组或对象）
        size_t size() const;

        /// 检查对象中是否存在指定的成员
        bool has_member(std::string member_name);

        /// 添加成员或元素
        void add_member(std::string member_name, json member_value);
        void add_element(json elem); // 向数组添加一个元素

        json &operator[](const char *key); // 访问对象的成员
        json &operator[](int index);       // 访问数组的元素

        operator const std::string() const; // 将 JSON 值转换为字符串
        operator const double() const;      // 将 JSON 值转换为双精度浮点数
        operator const long long() const;   // 将 JSON 值转换为长整型
        operator const bool() const;        // 将 JSON 值转换为布尔值

        /// 序列化 JSON 值
        const std::string to_string() const;

    private:
        /// 用于序列化对象和数组
        const std::string object_to_string() const; // 序列化对象类型的 JSON 值
        const std::string array_to_string() const;  // 序列化数组类型的 JSON 值
    };

    //
    // 具体实现
    //

    inline json::json() : _value(nullptr), _type(json_t::null) {}

    // 字符串类型的 JSON 对象
    inline json::json(std::string val)
        : _value(new std::string(val)), _type(json_t::string) {}

    // 字符串类型的 JSON 对象（引用）
    inline json::json(std::string &val)
        : _value(new std::string(val)), _type(json_t::string) {}

    // 字符串类型的 JSON 对象（C 风格字符串）
    inline json::json(char val[])
        : _value(new std::string(val)), _type(json_t::string) {}

    // 双精度浮点数类型的 JSON 对象
    inline json::json(double val)
        : _value(new double(val)), _type(json_t::number_double) {}

    // 长整型类型的 JSON 对象
    inline json::json(long long val)
        : _value(new long long(val)), _type(json_t::number_integer) {}

    // 整型类型的 JSON 对象
    inline json::json(int val)
        : _value(new long long(val)), _type(json_t::number_integer) {}

    // 布尔类型的 JSON 对象
    inline json::json(bool val)
        : _value(new bool(val)), _type(json_t::boolean) {}

    // 数组类型的 JSON 对象
    inline json::json(json_array &array)
        : _value(new json_array(array)), _type(json_t::array) {}

    // 数组类型的 JSON 对象（移动语义）
    inline json::json(json_array &&array)
        : _value(new json_array(array)), _type(json_t::array) {}

    // 对象类型的 JSON 对象
    inline json::json(json_object &obj)
        : _value(new json_object(obj)), _type(json_t::object) {}

    // 对象类型的 JSON 对象（移动语义）
    inline json::json(json_object &&obj)
        : _value(new json_object(obj)), _type(json_t::object) {}

    // 获取 JSON 对象的类型
    inline const json_t json::type() const { return _type; }

    // 获取数据类型
    inline const std::string json::type_name() const
    {
        switch (_type)
        {
        case (json_t::string):
            return "string";

        case (json_t::array):
            return "array";

        case (json_t::object):
            return "object";

        case (json_t::number_double):
        case (json_t::number_integer):
            return "number";

        case (json_t::boolean):
            return "boolean";

        case (json_t::null):
            return "null";

        case (json_t::invalid):
        default:
            return "invalid";
        }
    }

    // 拷贝构造函数
    inline json::json(const json &other)
    {
        _type = other._type;
        switch (_type)
        {
        case json_t::string:
            _value = new std::string(*static_cast<std::string *>(other._value));
            break;
        case json_t::object:
            _value = new json_object(*static_cast<json_object *>(other._value));
            break;
        case json_t::number_double:
            _value = new double(*static_cast<double *>(other._value));
            break;
        case json_t::number_integer:
            _value = new long long(*static_cast<long long *>(other._value));
            break;
        case json_t::array:
            _value = new json_array(*static_cast<json_array *>(other._value));
            break;
        case json_t::boolean:
            _value = new bool(*static_cast<bool *>(other._value));
            break;
        case json_t::null:
            _value = nullptr;
            break;
        default:
            throw std::runtime_error("unexpected json type: " + other.type_name());
        }
    }

    // 拷贝赋值运算符
    inline json &json::operator=(const json &other)
    {
        _type = other._type;
        switch (_type)
        {
        case json_t::string:
            _value = new std::string(*static_cast<std::string *>(other._value));
            break;
        case json_t::object:
            _value = new json_object(*static_cast<json_object *>(other._value));
            break;
        case json_t::number_double:
            _value = new double(*static_cast<double *>(other._value));
            break;
        case json_t::number_integer:
            _value = new long long(*static_cast<long long *>(other._value));
            break;
        case json_t::array:
            _value = new json_array(*static_cast<json_array *>(other._value));
            break;
        case json_t::boolean:
            _value = new bool(*static_cast<bool *>(other._value));
            break;
        case json_t::null:
            _value = nullptr;
            break;
        default:
            // 无法识别的数据类型
            throw std::runtime_error("unexpected json type" + other.type_name());
        }
        return *this;
    }

    // 比较当前 JSON 对象与另一个 JSON 对象是否相等
    // 两个 JSON 对象相等当且仅当它们类型相同且值相等
    inline bool json::operator==(const json &rhs) const
    {
        // 如果类型不同，则不相等
        if (_type != rhs._type)
        {
            return false;
        }

        // 根据 JSON 对象的类型，比较具体的值
        switch (_type)
        {
        case json_t::array:
            // 如果都是数组类型，则比较数组中的每个元素
            if (rhs._type == json_t::array)
            {
                json_array *a = static_cast<json_array *>(_value);
                json_array *b = static_cast<json_array *>(rhs._value);
                return *a == *b;
            }
            break;

        case json_t::object:
            // 如果都是对象类型，则比较对象中的每个键值对
            if (rhs._type == json_t::object)
            {
                json_object *a = static_cast<json_object *>(_value);
                json_object *b = static_cast<json_object *>(rhs._value);
                return *a == *b;
            }
            break;

        case json_t::null:
            // 如果都是 null 类型，则它们相等
            if (rhs._type == json_t::null)
            {
                return true;
            }
            break;

        case json_t::string:
            // 如果都是字符串类型，则比较字符串内容
            if (rhs._type == json_t::string)
            {
                const std::string *a = static_cast<std::string *>(_value);
                const std::string *b = static_cast<std::string *>(rhs._value);
                return *a == *b;
            }
            break;

        case json_t::boolean:
            // 如果都是布尔类型，则比较布尔值
            if (rhs._type == json_t::boolean)
            {
                const bool *a = static_cast<bool *>(_value);
                const bool *b = static_cast<bool *>(rhs._value);
                return *a == *b;
            }
            break;

        case json_t::number_integer:
            // 如果都是整数类型，则比较整数值
            if (rhs._type == json_t::number_integer)
            {
                const long long *a = static_cast<long long *>(_value);
                const long long *b = static_cast<long long *>(rhs._value);
                return *a == *b;
            }
            break;

        case json_t::number_double:
            // 如果都是浮点数类型，则比较浮点数值
            if (rhs._type == json_t::number_double)
            {
                const double *a = static_cast<double *>(_value);
                const double *b = static_cast<double *>(rhs._value);
                return *a == *b;
            }
            break;

        default:
            // 其他类型（如 invalid）不相等
            return false;
        }

        // 如果所有比较都失败，则返回 false
        return false;
    }

    inline bool json::operator!=(const json &rhs) const
    {
        return !(*this == rhs);
    }

    // 获取当前 JSON 对象的字符串值
    inline const std::string json::get_string() const
    {
        CHECK_TYPE_MISMATCH(this->_type, json_t::string);
        return *static_cast<std::string *>(_value);
    }

    // 获取当前 JSON 对象的整数值
    inline const long long json::get_integer() const
    {
        CHECK_TYPE_MISMATCH(this->_type, json_t::number_integer);
        return *static_cast<long long *>(_value);
    }

    // 获取当前 JSON 对象的双精度浮点数值
    inline const double json::get_double() const
    {
        CHECK_TYPE_MISMATCH(this->_type, json_t::number_double);
        return *static_cast<double *>(_value);
    }

    // 获取当前 JSON 对象的布尔值
    inline const bool json::get_bool() const
    {
        CHECK_TYPE_MISMATCH(this->_type, json_t::boolean);
        return *static_cast<bool *>(_value);
    }

    // 获取当前 JSON 对象的对象值
    inline const json_object json::get_object() const
    {
        CHECK_TYPE_MISMATCH(this->_type, json_t::object);
        return *static_cast<json_object *>(_value);
    }

    // 获取当前 JSON 对象的数组值
    inline const json_array json::get_array() const
    {
        CHECK_TYPE_MISMATCH(this->_type, json_t::array);
        return *static_cast<json_array *>(_value);
    }

    // 获取当前 JSON 对象的 null 值
    inline const void *json::get_null() const
    {
        CHECK_TYPE_MISMATCH(this->_type, json_t::null);
        return nullptr;
    }

    // 检查当前 JSON 对象是否包含指定名称的成员
    inline bool json::has_member(std::string member_name)
    {
        CHECK_TYPE_MISMATCH(this->_type, json_t::object);
        auto members = static_cast<json_object *>(_value);
        return (members->find(member_name) != members->end());
    }

    // 向当前 JSON 对象添加一个成员
    inline void json::add_member(std::string member_name, json member_value)
    {
        CHECK_TYPE_MISMATCH(this->_type, json_t::object);
        auto members = static_cast<json_object *>(_value);
        (*members)[member_name] = member_value;
    }

    // 向当前 JSON 数组添加一个元素
    inline void json::add_element(json elem)
    {
        CHECK_TYPE_MISMATCH(this->_type, json_t::array);
        auto elems = static_cast<json_array *>(_value);
        elems->push_back(elem);
    }

    // 获取当前 JSON 对象的大小
    // 如果当前类型是数组或对象，返回数组的长度或对象的成员数
    inline size_t json::size() const
    {
        if (_type == json_t::array)
        {
            // 如果是数组类型，返回数组的大小
            auto array = static_cast<json_array *>(_value);
            return array->size();
        }
        else if (_type == json_t::object)
        {
            // 如果是对象类型，返回对象的成员数量
            auto members = static_cast<json_object *>(_value);
            return members->size();
        }

        // 如果不是数组或对象类型，抛出异常
        throw std::runtime_error("Unexpected json type " + type_name() + ", expected array or object");
    }

    // 重载对象类型的 JSON 对象的下标运算符
    // 如果当前类型不是对象类型，则抛出异常
    inline json &json::operator[](const char *key)
    {
        CHECK_TYPE_MISMATCH(this->_type, json_t::object);
        json_object *members = static_cast<json_object *>(_value);

        // 通过键名访问对象中的成员
        if (members->find(key) == members->end())
        {
            throw std::runtime_error("key " + std::string(key) + " not found.");
        }

        return (*members)[key];
    }

    // 重载数组类型的 JSON 对象的下标运算符
    inline json &json::operator[](int index)
    {
        CHECK_TYPE_MISMATCH(this->_type, json_t::array);
        json_array *array = static_cast<json_array *>(_value);

        // 通过索引访问数组中的元素
        if (index < 0 || (size_t)index >= array->size())
        {
            throw std::runtime_error("index " + std::to_string(index) + " out of range.");
        }

        return (*array)[index];
    }

    // 析构函数
    inline json::~json()
    {
        switch (_type)
        {
        case (json_t::array):
            delete static_cast<json_array *>(_value);
            break;
        case (json_t::object):
            delete static_cast<json_object *>(_value);
            break;
        case (json_t::string):
            delete static_cast<std::string *>(_value);
            break;
        case (json_t::boolean):
            delete static_cast<bool *>(_value);
            break;
        case (json_t::number_double):
            delete static_cast<double *>(_value);
            break;
        case (json_t::number_integer):
            delete static_cast<long long *>(_value);
            break;
        default:
            break;
        }
    }

    // 类型转换运算符，将 json 对象转换为 const std::string 类型
    inline json::operator const std::string() const
    {
        switch (_type)
        {
        case json_t::string:
            return *static_cast<std::string *>(_value);
        default:
            throw std::runtime_error("cannot cast " + type_name() + " to json string");
        }
    }

    // 类型转换运算符，将 json 对象转换为 const double 类型
    inline json::operator const double() const
    {
        switch (_type)
        {
        case json_t::number_double:
            return *static_cast<double *>(_value);
        default:
            throw std::runtime_error("cannot cast " + type_name() + " to json number");
        }
    }

    // 类型转换运算符，将 json 对象转换为 const long long 类型
    inline json::operator const long long() const
    {
        switch (_type)
        {
        case json_t::number_integer:
            return (long long)(*static_cast<long long *>(_value));
        default:
            throw std::runtime_error("cannot cast " + type_name() + " to json number");
        }
    }

    // 类型转换运算符，将 json 对象转换为 const bool 类型
    inline json::operator const bool() const
    {
        switch (_type)
        {
        case json_t::boolean:
            return *static_cast<bool *>(_value);
        default:
            throw std::runtime_error("cannot cast " + type_name() + " to json boolean");
        }
    }
    // 将当前 JSON 对象转换为字符串表示
    inline const std::string json::to_string() const
    {
        switch (type())
        {
        case json_t::object:
            // 如果是对象类型，调用 object_to_string 进行序列化
            return object_to_string();

        case json_t::array:
            // 如果是数组类型，调用 array_to_string 进行序列化
            return array_to_string();

        case json_t::string:
            // 如果是字符串类型，获取字符串值并在两侧添加双引号
            return "\"" + get_string() + "\"";

        case json_t::number_integer:
            // 如果是整数类型，将整数值转换为字符串
            return std::to_string(get_integer());

        case json_t::number_double:
            // 如果是浮点数类型，将浮点数值转换为字符串
            return std::to_string(get_double());

        case json_t::boolean:
            // 如果是布尔类型，根据布尔值返回 "true" 或 "false"
            return get_bool() ? "true" : "false";

        case json_t::null:
            // 如果是 null 类型，返回 "null" 字符串
            return "null";

        default:
            // 如果遇到无效的 JSON 类型，抛出异常
            throw std::runtime_error("invalid json type");
        }
    }

    // 将当前 JSON 对象（对象类型）转换为字符串表示
    // 存在递归调用
    inline const std::string json::object_to_string() const
    {
        std::stringstream ss;
        ss << "{"; // 输出对象开始标志 {

        json_object jobj = get_object(); // 获取对象数据
        for (auto it = jobj.begin(); it != jobj.end(); it++)
        {
            ss << "\"" << it->first.c_str() << "\""; // 输出键名，并加双引号
            ss << " : ";                             // 输出键值对分隔符 :

            ss << it->second.to_string().c_str(); // 输出值的字符串表示

            if (std::next(it) != jobj.end())
            {
                ss << ","; // 如果不是最后一个成员，输出分隔符 ,
            }
        }

        ss << "}";       // 输出对象结束标志 }
        return ss.str(); // 返回序列化后的字符串
    }

    // 将当前 JSON 对象（数组类型）转换为字符串表示
    inline const std::string json::array_to_string() const
    {
        std::stringstream ss;
        ss << "["; // 输出数组开始标志 [

        json_array jarray = get_array(); // 获取数组数据
        for (auto it = jarray.begin(); it != jarray.end(); it++)
        {
            ss << it->to_string().c_str(); // 输出当前元素的字符串表示

            if (std::next(it) != jarray.end())
            {
                ss << ","; // 如果不是最后一个元素，输出分隔符 ,
            }
        }

        ss << "]";       // 输出数组结束标志 ]
        return ss.str(); // 返回序列化后的字符串
    }

    // JSON 解析器
    class parser
    {
    public:
        // 从 UTF-8 编码的字符串解析 JSON 对象
        static json parse(const char *s)
        {
            // 将输入字符串转换为 UTF-32，以便逐字符解析
            std::stringstream sstrm(s);
            std::wbuffer_convert<std::codecvt_utf8<char32_t>, char32_t> conv(sstrm.rdbuf());
            u32_istream u32strm(&conv);

            json ret_val;                                       // 用于存储解析后的 JSON 值
            char32_t first_char = peek_next_non_space(u32strm); // 查看第一个非空白字符

            // 根据第一个非空白字符判断 JSON 的类型
            if (first_char == U'{')
            {
                ret_val = parse_object(u32strm); // 解析对象
            }
            else if (first_char == U'[')
            {
                ret_val = parse_array(u32strm); // 解析数组
            }
            else
            {
                throw std::runtime_error("invalid json format"); // 格式错误
            }

            // 预期解析结束后应到达文件末尾
            if (peek_next_non_space(u32strm) != (char32_t)EOF)
            {
                throw std::runtime_error("invalid json format"); // 格式错误
            }

            return ret_val; // 返回解析后的 JSON 对象
        }

        // 解析 JSON 值
        static json parse_value(u32_istream &strm)
        {
            // 查看下一个非空白字符以确定要解析的值类型
            switch (peek_next_non_space(strm))
            {
            case U'"': // 字符串
                return parse_string(strm);

            case U'[': // 数组
                return parse_array(strm);

            // 数字，包括整数和浮点数
            case U'0':
            case U'1':
            case U'2':
            case U'3':
            case U'4':
            case U'5':
            case U'6':
            case U'7':
            case U'8':
            case U'9':
            case U'-':
            case U'.':
                return parse_number(strm);

            case U'{': // 对象
                return parse_object(strm);

            // 布尔值
            case U'T':
            case U't':
            case U'F':
            case U'f':
                return parse_bool(strm);

            // null
            case U'n':
            case U'N':
                return parse_null(strm);

            default: // 遇到意外的字符
                throw std::runtime_error("unexpected character");
            }
        }

        // 解析 JSON 对象
        static json parse_object(u32_istream &strm)
        {
            json return_val(json_object{}); // 创建一个空对象

            // 跳过开头的 '{' 字符
            skip_char(strm, U'{');

            int c = peek_next_non_space(strm);
            // 循环直到遇到 '}'
            while (c != EOF)
            {
                if (c == U'"')
                {                                            // 键名是字符串
                    std::string member = parse_member(strm); // 解析成员键名

                    skip_char(strm, U':'); // 跳过冒号

                    return_val.add_member(member, parse_value(strm)); // 解析并添加成员值
                }
                else if (c == U'}')
                {
                    break; // 遇到 '}', 结束对象解析
                }
                else if (c == U',')
                {
                    // 跳过逗号，继续解析下一个成员
                    c = get_next_non_space(strm);
                }
                else
                {
                    throw std::runtime_error("invalid object format"); // 对象格式错误
                }

                c = peek_next_non_space(strm); // 查看下一个非空白字符
            }

            skip_char(strm, U'}'); // 跳过结尾的 '}' 字符

            return return_val; // 返回解析后的对象
        }

        // 解析 JSON 成员（键）
        static std::string parse_member(u32_istream &strm)
        {
            std::u32string returnVal;

            // 跳过开头的双引号
            skip_char(strm, U'"');

            unsigned int c = strm.peek(); // 查看当前字符
            while (c != (unsigned int)EOF && c != U'"')
            { // 当不是文件结束且不是双引号时循环
                if (c == U'\\')
                {
                    // 转义字符
                    returnVal.push_back(escape_char(strm));
                }
                else
                {
                    c = strm.get(); // 获取当前字符
                    returnVal.push_back(c);
                }

                c = strm.peek(); // 再次查看当前字符
            }

            // 跳过结尾的双引号
            skip_char(strm, U'"');

            return U32ToU8(returnVal); // 将 UTF-32 字符串转换为 UTF-8 字符串
        }

        // 解析 JSON 数组
        static json parse_array(u32_istream &strm)
        {
            json_array vector_val;

            // 跳过开头的 '[' 字符
            skip_char(strm, U'[');
            int c = peek_next_non_space(strm); // 查看下一个非空白字符

            // 空数组
            if (c == U']')
            {
                skip_char(strm, U']');   // 跳过结尾的 ']' 字符
                return json(vector_val); // 返回空数组
            }

            do
            {
                vector_val.push_back(parse_value(strm)); // 解析值并添加到数组
                c = peek_next_non_space(strm);           // 查看下一个非空白字符

                if (c == U',')
                {
                    skip_char(strm, U','); // 跳过逗号
                }
                else if (c == U']')
                {
                    break; // 遇到结尾的 ']' 字符时结束循环
                }
            } while (c != EOF); // 直到文件结束

            // 跳过结尾的 ']' 字符
            skip_char(strm, U']');

            json array_val(vector_val); // 创建 JSON 数组对象
            return array_val;           // 返回 JSON 数组对象
        }

        // 解析 JSON 布尔值
        static json parse_bool(u32_istream &strm)
        {
            std::u32string val_str;

            int c = strm.peek(); // 查看当前字符
            while (c != EOF && c != U',' && c != U'}' && c != U']')
            {                                                              // 当不是文件结束且不是逗号、} 或 ] 时循环
                c = strm.get();                                            // 获取当前字符
                val_str.push_back(static_cast<char32_t>(std::tolower(c))); // 转换为小写并添加到字符串
                c = strm.peek();                                           // 再次查看当前字符
            }

            std::string bool_str = trim(U32ToU8(val_str)); // 转换为 UTF-8 字符串并去除空白字符

            bool val_bool = to_bool(bool_str); // 转换为布尔值
            json return_val(val_bool);         // 创建 JSON 布尔值对象
            return return_val;                 // 返回 JSON 布尔值对象
        }

        // 解析 JSON null 值
        static json parse_null(u32_istream &strm)
        {
            std::u32string val_str;

            int c = strm.peek(); // 查看当前字符
            while (c != EOF && c != U',' && c != U'}' && c != U']')
            {                                                              // 当不是文件结束且不是逗号、} 或 ] 时循环
                c = strm.get();                                            // 获取当前字符
                val_str.push_back(static_cast<char32_t>(std::tolower(c))); // 转换为小写并添加到字符串
                c = strm.peek();                                           // 再次查看当前字符
            }

            std::string null_str = trim(U32ToU8(val_str)); // 转换为 UTF-8 字符串并去除空白字符

            if (null_str == "null")
            {                  // 如果字符串是 "null"
                return json(); // 返回 JSON null 对象
            }
            else
            {
                throw std::runtime_error("unexpected null string"); // 抛出异常，null 字符串不符合预期
            }
        }

        // 解析 JSON 字符串值
        static json parse_string(u32_istream &strm)
        {
            std::u32string string_val;

            // 跳过字符串开头的双引号
            skip_char(strm, U'"');

            char32_t c = strm.peek(); // 查看当前字符

            // 当字符不是文件结束标志且不是双引号时循环
            while (c != EOF && c != U'"')
            {
                if (c == U'\\')
                {
                    // 处理转义字符
                    string_val.push_back(escape_char(strm));
                }
                else
                {
                    strm.get(c);             // 获取当前字符
                    string_val.push_back(c); // 添加到字符串
                }

                c = strm.peek(); // 查看下一个字符
            }

            // 跳过字符串结尾的双引号
            skip_char(strm, U'"');

            // 将 UTF-32 编码的字符串转换为 UTF-8 编码，并创建 JSON 字符串对象
            json return_val(U32ToU8(string_val));
            return return_val; // 返回 JSON 字符串对象
        }

        // 解析 JSON 数值
        static json parse_number(u32_istream &strm)
        {
            std::u32string num_str;

            int c = strm.peek(); // 查看当前字符
            while (c != EOF && c != U',' && c != U']' && c != U'}')
            {                         // 当不是文件结束且不是逗号、] 或 } 时循环
                c = strm.get();       // 获取当前字符
                num_str.push_back(c); // 添加到字符串
                c = strm.peek();      // 查看下一个字符
            }

            std::string nums = trim(U32ToU8(num_str)); // 转换为 UTF-8 字符串并去除空白字符

            // 尝试将字符串转换为整数，如果包含非数字字符，则转换为双精度浮点数
            if (nums.find_first_not_of("0123456789-") == std::string::npos)
            {
                return json(to_integer(nums)); // 转换为整数
            }
            else
            {
                return json(to_double(nums)); // 转换为双精度浮点数
            }
        }

        // 将字符串转换为布尔值
        static bool to_bool(std::string str)
        {
            std::transform(str.begin(), str.end(), str.begin(), // 转换字符串中所有字符为小写
                           [](unsigned char c)
                           {
                               return static_cast<unsigned char>(std::tolower(c));
                           });

            if (str == "true") // 如果字符串是 "true"
                return true;   // 返回 true

            if (str == "false") // 如果字符串是 "false"
                return false;   // 返回 false

            throw std::runtime_error("invalid boolean string"); // 抛出异常，布尔字符串不符合预期
        }

        // 将字符串转换为长整型数
        static long long to_integer(std::string str)
        {
            size_t pos = 0;
            long long num = std::stoll(str, &pos); // 尝试将字符串转换为长整型数

            if (pos != str.size())
            {
                // 如果转换后的字符串位置不等于字符串的长度，则格式不正确
                throw std::runtime_error("Unexpected number(integer) format.");
            }

            return num; // 返回转换后的长整型数
        }

        // 将字符串转换为双精度浮点数
        static double to_double(std::string str)
        {
            size_t pos = 0;
            double num = std::stod(str, &pos); // 尝试将字符串转换为双精度浮点数

            if (pos != str.size())
            {
                // 如果转换后的字符串位置不等于字符串的长度，则格式不正确
                throw std::runtime_error("Unexpected number(double) format.");
            }

            return num; // 返回转换后的双精度浮点数
        }

        // 解析转义字符
        static char32_t escape_char(u32_istream &strm)
        {
            skip_char(strm, U'\\'); // 跳过反斜杠

            char32_t c = strm.get(); // 获取下一个字符

            switch (c)
            {
            case U'"':
                c = U'"'; // 双引号
                break;

            case U'\\':
                c = U'\\'; // 反斜杠
                break;

            case U'/':
                c = U'/'; // 斜杠
                break;

            case U'b':
                c = U'\b'; // 退格符
                break;

            case U'f':
                c = U'\f'; // 换页符
                break;

            case U'n':
                c = U'\n'; // 换行符
                break;

            case U'r':
                c = U'\r'; // 回车符
                break;

            case U't':
                c = U'\t'; // 制表符
                break;

            case U'u':
                c = parse_hex(strm); // 解析 Unicode 转义序列
                break;

            default:
                // 如果反斜杠后不是有效的转义字符，则抛出异常
                throw std::runtime_error("backslash is followed by invalid character");
            }

            return c; // 返回转义后的字符
        }

        // 获取下一个非空白字符
        static char32_t get_next_non_space(u32_istream &strm)
        {
            skip_space(strm);  // 跳过所有空白字符
            return strm.get(); // 获取下一个字符
        }

        // 查看下一个非空白字符，但不从流中移除它
        static int peek_next_non_space(u32_istream &strm)
        {
            skip_space(strm);   // 跳过所有空白字符
            return strm.peek(); // 查看下一个字符，但不移除
        }

        // 跳过字符直到遇到预期的字符，如果遇到不同的字符则抛出异常
        static void skip_char(u32_istream &strm, char32_t expected)
        {
            skip_space(strm);                      // 跳过所有空白字符
            char32_t next = (char32_t)strm.peek(); // 查看下一个字符

            if (next != expected || next == (char32_t)EOF)
            {                                                            // 如果下一个字符不是预期的字符或文件结束
                std::basic_ostringstream<char32_t> oss;                  // 创建字符串流以构建错误信息
                oss << U"expected char '" << expected << U"' not found"; // 构建错误信息
                auto err_str = oss.str();                                // 获取错误信息字符串
                throw std::runtime_error(U32ToU8(err_str));              // 抛出异常，将 UTF-32 错误信息转换为 UTF-8
            }

            strm.get(); // 移除预期的字符
        }

        // 跳过所有空白字符
        static void skip_space(u32_istream &strm)
        {
            unsigned int next = strm.peek(); // 查看下一个字符
            while (next != (char32_t)EOF && std::isspace(next))
            {                       // 如果下一个字符是空白字符
                strm.get();         // 移除空白字符
                next = strm.peek(); // 查看下一个字符
            }
        }

        // 解析 Unicode 转义序列（例如 \uXXXX）
        static char32_t parse_hex(u32_istream &strm)
        {
            std::u32string cs; // 用于存储四位十六进制数

            // 读取四位十六进制数
            for (int i = 0; i < 4; i++)
            {
                char32_t c = strm.get(); // 获取下一个字符
                if (c != std::char_traits<char32_t>::eof() && isxdigit(c))
                {                    // 如果字符不是文件结束且是十六进制数字
                    cs.push_back(c); // 添加到字符串
                }
                else
                {
                    throw std::runtime_error("not hex number"); // 抛出异常，输入不是有效的十六进制数
                }
            }

            std::basic_stringstream<char32_t> is(cs); // 创建字符串流以解析十六进制数
            unsigned int uc;
            is >> std::hex >> uc; // 解析十六进制数

            return char32_t(uc); // 返回转换后的字符
        }
    };

} // namespace TinyJson
