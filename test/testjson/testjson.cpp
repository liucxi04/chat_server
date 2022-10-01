#include "json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

// # 基本数据类型序列化
string func1()
{
    json js;
    js["msg_type"] = 2;
    js["from"] = "zhang san";
    js["to"] = "li si";
    js["msg"] = "hello, how are you?";

    string sendBuf = js.dump();
    return sendBuf;
}

// STL 类型序列化
void func2()
{
    json js;

    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(5);
    js["list"] = vec;

    map<int, string> m;
    m.insert({1, "黄山"});
    m.insert({2, "华山"});
    m.insert({3, "泰山"});
    js["path"] = m;
    cout << js << endl;
}
int main()
{
    string recvBuf = func1();
    // 数据反序列化
    json js = json::parse(recvBuf);

    cout << js["msg_type"] << endl;
    cout << js["from"] << endl;
    cout << js["to"] << endl;
    cout << js["msg"] << endl;

    return 0;
}