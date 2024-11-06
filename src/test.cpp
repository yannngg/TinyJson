#include <gtest/gtest.h>
#include "..\include\TinyJson.h"

using namespace TinyJson;

TEST(SimpleJsonConstructor, Strings)
{
    json s1("json string object");
    EXPECT_EQ(s1.get_string(), "json string object");

    json s2("");
    EXPECT_EQ(s2.get_string(), "");

    json s3("  ");
    EXPECT_EQ(s3.get_string(), "  ");
}

TEST(SimpleJsonConstructor, Numbers)
{
    json i1(3256);
    EXPECT_EQ(i1.get_integer(), 3256);

    json i2(-245);
    EXPECT_EQ(i2.get_integer(), -245);

    json i3(0.2356);
    EXPECT_DOUBLE_EQ(i3.get_double(), 0.2356);

    json i4(-0.255356);
    EXPECT_DOUBLE_EQ(i4.get_double(), -0.255356);

    json i5(000);
    EXPECT_EQ(i5.get_integer(), 0);

    json i6(-000);
    EXPECT_EQ(i6.get_integer(), 0);
}

TEST(SimpleJsonConstructor, Bools)
{
    json b1(true);
    EXPECT_EQ(b1.get_bool(), true);

    json b2(false);
    EXPECT_EQ(b2.get_bool(), false);
}

TEST(SimpleJsonConstructor, Null)
{
    json n1;
    EXPECT_EQ(n1.get_null(), nullptr);
}

TEST(SimpleJsonConstructor, Arrays)
{
    std::vector<json> elms{2134, 781450, 0.187601, -984578, 0, 1, 24.059};
    json a1(elms);
    EXPECT_EQ(a1[0].get_integer(), 2134);
    EXPECT_EQ(a1[1].get_integer(), 781450);
    EXPECT_DOUBLE_EQ(a1[2].get_double(), 0.187601);
    EXPECT_EQ(a1[3].get_integer(), -984578);
    EXPECT_EQ(a1[4].get_integer(), 0);
    EXPECT_EQ(a1[5].get_integer(), 1);
    EXPECT_DOUBLE_EQ(a1[6].get_double(), 24.059);
}

TEST(SimpleJsonConstructor, Objects)
{
    json o1(json_object{});
    o1.add_member("p1", "helloworld");
    o1.add_member("p2", 3.1415926);
    o1.add_member("p3", 9876543210);
    o1.add_member("p4", json(json_object{}));

    o1["p4"].add_member("p4_1", true);
    o1["p4"].add_member("p4_2", -0.1356456);

    EXPECT_EQ(o1["p1"].get_string(), "helloworld");
    EXPECT_DOUBLE_EQ(o1["p2"].get_double(), 3.1415926);
    EXPECT_EQ(o1["p3"].get_integer(), 9876543210);

    EXPECT_EQ(o1["p4"]["p4_1"].get_bool(), true);
    EXPECT_DOUBLE_EQ(o1["p4"]["p4_2"].get_double(), -0.1356456);
}

TEST(TinyJsonStringParsing, Basic)
{
    std::u32string u32(U"\x0022"
                       U"2021"
                       U"\x4E16\x754C"
                       U"World "
                       U"\x4F60\x597D"
                       U"012 Hello!\x0022");
    u32_sstream u32strm(u32);

    auto s = parser::parse_string(u32strm);
    EXPECT_EQ(s.get_string(), "2021世界World 你好012 Hello!");

    u32_sstream ss1(U"\"hello \\\\ world \"");
    auto s1 = parser::parse_string(ss1);
    EXPECT_EQ(s1.get_string(), "hello \\ world ");

    u32_sstream ss2(U"\"hello \\\" world \"");
    auto s2 = parser::parse_string(ss2);
    EXPECT_EQ(s2.get_string(), "hello \" world ");

    u32_sstream ss3(U"\" \\thello \\\"\\r world \\n \\/ \\b \\f \"");
    auto s3 = parser::parse_string(ss3);
    EXPECT_EQ(s3.get_string(), " \thello \"\r world \n / \b \f ");

    u32_sstream ss4(U"\"\"");
    auto s4 = parser::parse_string(ss4);
    EXPECT_EQ(s4.get_string(), "");
}

TEST(TinyJsonHexParsing, Basic)
{
    std::u32string u32hex(U"\x0022"
                          U"2021"
                          U"\\u4E16\\u754C"
                          U"World "
                          U"\\u4F60\\u597D"
                          U"012 Hello! \\u0026\x0022");
    u32_sstream u32strm(u32hex);

    auto s1 = parser::parse_string(u32strm);
    EXPECT_EQ(s1.get_string(), "2021世界World 你好012 Hello! &");
}

TEST(TinyJsonEscapeParsing, Basic)
{
    std::u32string u32(U"\x0022\\\"a\\b\\/c\\bd\\fe  \\fg\\n\\r;\\t\\u4E16\\u754C\x0022");
    u32_sstream u32strm(u32);

    auto s1 = parser::parse_string(u32strm);
    std::string ss1 = s1.get_string();
    EXPECT_EQ(s1.get_string(), "\"a\b/c\bd\fe  \fg\n\r;\t世界");
}

TEST(TinyJsonBoolParsing, Basic)
{
    u32_sstream bs1(U"true");
    auto b1 = parser::parse_bool(bs1);
    EXPECT_EQ(true, b1.get_bool());

    u32_sstream bs2(U"tRue ");
    auto b2 = parser::parse_bool(bs2);
    EXPECT_EQ(true, b2.get_bool());

    u32_sstream bs3(U" FALSE");
    auto b3 = parser::parse_bool(bs3);
    EXPECT_EQ(false, b3.get_bool());

    u32_sstream bs4(U" falsE ");
    auto b4 = parser::parse_bool(bs4);
    EXPECT_EQ(false, b4.get_bool());
}

TEST(TinyJsonNULLParsing, Basic)
{
    u32_sstream ns1(U"null");
    auto n1 = parser::parse_null(ns1);
    EXPECT_EQ(nullptr, n1.get_null());

    u32_sstream ns2(U" NULL ");
    auto n2 = parser::parse_null(ns2);
    EXPECT_EQ(nullptr, n2.get_null());

    u32_sstream ns3(U" nUlL");
    auto n3 = parser::parse_null(ns3);
    EXPECT_EQ(nullptr, n3.get_null());
}

TEST(TinyJsonNumberParsing, Basic)
{
    u32_sstream ns1(U"0.124    \t");
    auto n1 = parser::parse_number(ns1);
    EXPECT_DOUBLE_EQ(0.124, n1.get_double());

    u32_sstream ns2(U"-0.10921234567824 ");
    auto n2 = parser::parse_number(ns2);
    EXPECT_DOUBLE_EQ(-0.10921234567824, n2.get_double());

    u32_sstream ns3(U"921.234567824");
    auto n3 = parser::parse_number(ns3);
    EXPECT_DOUBLE_EQ(921.234567824, n3.get_double());

    u32_sstream ns4(U"  .987123654");
    auto n4 = parser::parse_number(ns4);
    EXPECT_DOUBLE_EQ(.987123654, n4.get_double());

    u32_sstream ns5(U".23545E-34  ");
    auto n5 = parser::parse_number(ns5);
    EXPECT_DOUBLE_EQ(.23545E-34, n5.get_double());

    u32_sstream ns6(U"8.7894e+34");
    auto n6 = parser::parse_number(ns6);
    EXPECT_DOUBLE_EQ(8.7894e+34, n6.get_double());

    u32_sstream ns7(U"9874563121555444");
    auto n7 = parser::parse_number(ns7);
    EXPECT_EQ(9874563121555444, n7.get_integer());

    u32_sstream ns8(U"7895484569216311245.006");
    auto n8 = parser::parse_number(ns8);
    EXPECT_DOUBLE_EQ(7895484569216311245.006, n8.get_double());

    u32_sstream ns9(U"00000");
    auto n9 = parser::parse_number(ns9);
    EXPECT_EQ(0, n9.get_integer());

    u32_sstream ns10(U"-426981544458458");
    auto n10 = parser::parse_number(ns10);
    EXPECT_EQ(-426981544458458, n10.get_integer());
}

TEST(TinyJsonHexCharParsing, Basic)
{
    u32_sstream hs1(U"0024");
    auto h1 = parser::parse_hex(hs1);
    EXPECT_EQ(U'$', h1);

    u32_sstream hs2(U"0047");
    auto h2 = parser::parse_hex(hs2);
    EXPECT_EQ(U'G', h2);

    u32_sstream hs3(U"754C");
    auto h3 = parser::parse_hex(hs3);
    EXPECT_EQ(U'\x754C', h3);
}

TEST(TinyJsonArrayParsing, Basic)
{
    u32_sstream as1(U" [124, -2.534, " hello world ", null, false ]");
    auto a1 = parser::parse_array(as1);

    EXPECT_EQ(5, a1.size());
    EXPECT_EQ(json_t::array, a1.type());
    EXPECT_EQ(124, a1[0].get_integer());
    EXPECT_DOUBLE_EQ(-2.534, a1[1].get_double());
    EXPECT_EQ("hello world  ", a1[2].get_string());
    EXPECT_EQ(nullptr, a1[3].get_null());
    EXPECT_EQ(false, a1[4].get_bool());

    u32_sstream as2(U" [  ] ");
    auto a2 = parser::parse_array(as2);

    EXPECT_EQ(0, a2.size());
    EXPECT_EQ(json_t::array, a2.type());

    u32_sstream as3(U" [ \" elem1\" ] ");
    auto a3 = parser::parse_array(as3);

    EXPECT_EQ(1, a3.size());
    EXPECT_EQ(json_t::array, a3.type());
    EXPECT_EQ(" elem1", a3[0].get_string());

    u32_sstream as4(U" [ \" elem1\", {}, {\"p1\" : 498723123} ] ");
    auto a4 = parser::parse_array(as4);

    EXPECT_EQ(3, a4.size());
    EXPECT_EQ(json_t::array, a4.type());
    EXPECT_EQ(" elem1", a4[0].get_string());
    EXPECT_EQ(0, a4[1].size());
    EXPECT_EQ(1, a4[2].size());
    EXPECT_EQ(498723123, a4[2]["p1"].get_integer());

    u32_sstream as5(U" [[]  ] ");
    auto a5 = parser::parse_array(as5);

    EXPECT_EQ(json_t::array, a5.type());
    EXPECT_EQ(1, a5.size());
    EXPECT_EQ(json_t::array, a5[0].type());
    EXPECT_EQ(0, a5[0].size());

    u32_sstream as6(U" [[1984.2020], 2020.1984  ] ");
    auto a6 = parser::parse_array(as6);

    EXPECT_EQ(json_t::array, a6.type());
    EXPECT_EQ(2, a6.size());
    EXPECT_DOUBLE_EQ(2020.1984, a6[1].get_double());
    EXPECT_EQ(json_t::array, a6[0].type());
    EXPECT_EQ(1, a6[0].size());
    EXPECT_DOUBLE_EQ(1984.2020, a6[0][0].get_double());

    u32_sstream as7(U" [[1984, \"1984\"], 2020, 1984 ] ");
    auto a7 = parser::parse_array(as7);
    EXPECT_EQ(json_t::array, a7.type());
    EXPECT_EQ(3, a7.size());
    EXPECT_EQ(json_t::array, a7[0].type());
    EXPECT_EQ(1984, a7[0][0].get_integer());
    EXPECT_EQ("1984", a7[0][1].get_string());
    EXPECT_EQ(2020, a7[1].get_integer());
    EXPECT_EQ(1984, a7[2].get_integer());
}

TEST(TinyJsonObjectParsing, Basic)
{
    u32_sstream os1(U" {}");
    auto o1 = parser::parse_object(os1);
    EXPECT_EQ(json_t::object, o1.type());
    EXPECT_EQ(0, o1.size());

    u32_sstream os2(U" {\"p1\" : 987135}");
    auto o2 = parser::parse_object(os2);
    EXPECT_EQ(json_t::object, o2.type());
    EXPECT_EQ(1, o2.size());
    EXPECT_EQ(987135, o2["p1"].get_integer());

    u32_sstream os3(U" {\"p1\" : 987135, \"P_1\" : true}");
    auto o3 = parser::parse_object(os3);
    EXPECT_EQ(json_t::object, o3.type());
    EXPECT_EQ(2, o3.size());
    EXPECT_EQ(987135, o3["p1"].get_integer());
    EXPECT_EQ(true, o3["P_1"].get_bool());

    u32_sstream os4(U" {\"p1\" : -0.26897415, \"P_1\" : { \"_53245\": -235235 } }");
    auto o4 = parser::parse_object(os4);
    EXPECT_EQ(json_t::object, o4.type());
    EXPECT_EQ(2, o4.size());
    EXPECT_DOUBLE_EQ(-0.26897415, o4["p1"].get_double());
    EXPECT_EQ(json_t::object, o4["P_1"].type());
    EXPECT_EQ(-235235, o4["P_1"]["_53245"].get_integer());
}

TEST(TinyJsonMemberParsing, Basic)
{
    u32_sstream ss1(U""\x4F60\x202F2 hello "");
    auto member_name = parser::parse_member(ss1);
    EXPECT_EQ("你𠋲 hello", member_name);

    u32_sstream ss2(U"\"_hello world!\"");
    member_name = parser::parse_member(ss2);
    EXPECT_EQ("_hello world!", member_name);

    u32_sstream ss3(U"\"__\\u0069_\\u005E\"");
    member_name = parser::parse_member(ss3);
    EXPECT_EQ("__i_^", member_name);

    u32_sstream ss4(U"\"hello \\\"world\\\"\"");
    member_name = parser::parse_member(ss4);
    EXPECT_EQ("hello \"world\"", member_name);

    u32_sstream ss5(U"\"hello { world }\"");
    member_name = parser::parse_member(ss5);
    EXPECT_EQ("hello { world }", member_name);

    u32_sstream ss6(U"\"hello ,{ world }\"");
    member_name = parser::parse_member(ss6);
    EXPECT_EQ("hello ,{ world }", member_name);

    u32_sstream ss7(U"\"[hello ,{ world }]\"");
    member_name = parser::parse_member(ss7);
    EXPECT_EQ("[hello ,{ world }]", member_name);

    u32_sstream ss8(U"\"[\\thello ,{ world }]\"");
    member_name = parser::parse_member(ss8);
    EXPECT_EQ("[\thello ,{ world }]", member_name);
}

TEST(SimpleJsonSerialization, Basic)
{
    json a = parser::parse("{}");

    a.add_member("p1", 13525.4235);
    a.add_member("p2", "abcdefg hijklmnop rsq  uvwxyz ");
    a.add_member("_p3_", false);
    a.add_member("p4_____________", -0.9876540321);
    a.add_member("_______p5", std::vector<json>{"abc", 0.31424, false});
    a.add_member("示例一", false);
    a.add_member("你好 hello", "world\\u0039");
    a.add_member("世界__\\u0069_\\u005E", "你好");

    std::string a1 = a.to_string();
    json b = parser::parse(a1.c_str());

    EXPECT_EQ(8, b.size());
    EXPECT_DOUBLE_EQ(13525.4235, b["p1"].get_double());
    EXPECT_EQ("abcdefg hijklmnop rsq  uvwxyz ", b["p2"].get_string());
    EXPECT_EQ(false, b["_p3_"].get_bool());
    EXPECT_DOUBLE_EQ(-0.9876540321, b["p4_____________"].get_double());

    EXPECT_EQ(json_t::array, b["_______p5"].type());
    EXPECT_EQ("abc", b["_______p5"][0].get_string());
    EXPECT_DOUBLE_EQ(0.31424, b["_______p5"][1].get_double());
    EXPECT_EQ(false, b["_______p5"][2].get_bool());

    EXPECT_EQ(false, b["示例一"].get_bool());
    EXPECT_EQ("world9", b["你好 hello"].get_string());
    EXPECT_EQ("你好", b["世界__i_^"].get_string());

    json c(json_object{});
    EXPECT_EQ("{}", c.to_string());

    json d(json_array{});
    EXPECT_EQ("[]", d.to_string());
}

TEST(SimpleJsonJsonClassTest, Basic)
{
    json a, b;
    EXPECT_EQ(a, b);

    json a1(1984), b1(1984);
    EXPECT_EQ(a1, b1);

    json a2(-0.326345), b2(-0.326345);
    EXPECT_EQ(a2, b2);

    a2 = json(0.326345);
    EXPECT_NE(a2, b2);

    json a3(true), b3(true);
    EXPECT_EQ(a3, b3);

    a3 = json(false);
    EXPECT_NE(a3, b3);

    json a4("hello world"), b4("hello world");
    EXPECT_EQ(a4, b4);

    a4 = json("hello world!");
    EXPECT_NE(a4, b4);

    json a5(json_object{}), b5(json_object{});
    EXPECT_EQ(a5, b5);

    a5.add_member("p1", "hello1984");
    EXPECT_NE(a5, b5);

    b5.add_member("p1", "hello1984");
    EXPECT_EQ(a5, b5);

    json a6(json_array{}), b6(json_array{});
    EXPECT_EQ(a6, b6);

    a6.add_element(1984);
    EXPECT_NE(a6, b6);

    EXPECT_NE(a1, a2);
    EXPECT_NE(a1, a3);
    EXPECT_NE(a1, a4);
    EXPECT_NE(a1, a5);
    EXPECT_NE(a1, a6);

    EXPECT_NE(a2, a3);
    EXPECT_NE(a2, a4);
    EXPECT_NE(a2, a5);
    EXPECT_NE(a2, a6);

    EXPECT_NE(a3, a4);
    EXPECT_NE(a3, a5);
    EXPECT_NE(a3, a6);

    EXPECT_NE(a4, a5);
    EXPECT_NE(a4, a6);

    EXPECT_NE(a5, a6);

    json c(1984);
    json d(c);
    EXPECT_EQ(c, d);
    EXPECT_EQ(1984, d.get_integer());

    json e(json("1984"));
    EXPECT_EQ("1984", e.get_string());
}

TEST(SimpleJsonNumberParsingFailure, Basic)
{
    u32_sstream ns1(U"0.124abc");
    EXPECT_THROW(parser::parse_number(ns1), std::exception);

    u32_sstream ns2(U"124abc");
    EXPECT_THROW(parser::parse_number(ns2), std::exception);

    u32_sstream ns3(U"124 000");
    EXPECT_THROW(parser::parse_number(ns3), std::exception);

    u32_sstream ns4(U".124 000");
    EXPECT_THROW(parser::parse_number(ns4), std::exception);

    u32_sstream ns5(U"not a number 00.23");
    EXPECT_THROW(parser::parse_number(ns5), std::exception);
}

TEST(SimpleJsonBoolParsingFailure, Basic)
{
    u32_sstream bs1(U"falt");
    EXPECT_THROW(parser::parse_bool(bs1), std::exception);

    u32_sstream bs2(U"Falsa");
    EXPECT_THROW(parser::parse_bool(bs2), std::exception);
}

TEST(SimpleJsonStringParsingFailure, Basic)
{
    u32_sstream ss1(U"" hello world \"");
    EXPECT_THROW(parser::parse_string(ss1), std::exception);

    u32_sstream ss2(U"\"hello world \\u00A");
    EXPECT_THROW(parser::parse_string(ss2), std::exception);

    u32_sstream ss3(U"\"hello \\a world ");
    EXPECT_THROW(parser::parse_string(ss3), std::exception);

    u32_sstream ss4(U"\"hello \\uworld\"");
    EXPECT_THROW(parser::parse_string(ss4), std::exception);
}

TEST(SimpleJsonArrayParsingFailure, Basic)
{
    u32_sstream as1(U"[a]");
    EXPECT_THROW(parser::parse_array(as1), std::exception);

    u32_sstream as2(U"[,]");
    EXPECT_THROW(parser::parse_array(as2), std::exception);

    u32_sstream as3(U"[1.5,]");
    EXPECT_THROW(parser::parse_array(as3), std::exception);

    u32_sstream as4(U"[\"abc\":]");
    EXPECT_THROW(parser::parse_array(as4), std::exception);

    u32_sstream as5(U"[3.14 , \"\", a]");
    EXPECT_THROW(parser::parse_array(as5), std::exception);

    u32_sstream as6(U"[[3.14 , \"\", a]");
    EXPECT_THROW(parser::parse_array(as6), std::exception);
}

TEST(SimpleJsonObjectParsingFailure, Basic)
{
    u32_sstream os1(U"{a}");
    EXPECT_THROW(parser::parse_object(os1), std::exception);

    u32_sstream os2(U"{");
    EXPECT_THROW(parser::parse_object(os2), std::exception);

    u32_sstream os3(U"{1}");
    EXPECT_THROW(parser::parse_object(os3), std::exception);

    u32_sstream os4(U"{\"hello\"}");
    EXPECT_THROW(parser::parse_object(os4), std::exception);

    u32_sstream os5(U"{\"hello\":}");
    EXPECT_THROW(parser::parse_object(os5), std::exception);

    u32_sstream os6(U"{\"hello: 124 }");
    EXPECT_THROW(parser::parse_object(os6), std::exception);
}