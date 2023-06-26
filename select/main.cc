#include "util.hpp"
#include <string>
#include <cstring>
#include <vector>
#include <functional>

using namespace std;
using namespace Util;

using func_t = std::function<void()>;

void show()
{
    cout << "我们正在处理其他的事情" << endl;
}

int main()
{
    std::vector<func_t> funcs;
    funcs.push_back(show);

    SetNonBlock(0);
    // 0
    // string s;
    char buffer[1024];
    while (true)
    {
        buffer[0] = 0;
        // cin >> s; // 0
        int n = scanf("%s", buffer);
        if (n == -1)
        {
            // EWOULDBLOCK
            cout << "errno: " << errno << " desc: " << strerror(errno) << endl;
            for(const auto &f : funcs)
            {
                f();
            }
        }
        else
        {
            cout << "刚刚获取的内容是# " << buffer << " n:" << n << endl;
        }
        sleep(1);
    }
}