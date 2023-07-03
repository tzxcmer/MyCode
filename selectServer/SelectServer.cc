#include <iostream>
#include <sys/select.h>
#include "Sock.hpp"

int fdsArray[sizeof(fd_set) * 8] = {0}; // 保存历史上所有的合法fd
int gnum = sizeof(fdsArray) / sizeof(fdsArray[0]);

#define DFL -1

using namespace std;

static void showArray(int arr[], int num)
{
    cout << "当前合法sock list# ";
    for (int i = 0; i < num; i++)
    {
        if (arr[i] == DFL)
            continue;
        else
            cout << arr[i] << " ";
    }
    cout << endl;
}

static void usage(std::string process)
{
    cerr << "\nUsage: " << process << " port\n"
         << endl;
}