#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <filesystem>

using namespace std;
using namespace filesystem;

int tconut = 0;
mutex mtx;

void WordCount(const path& pth)
{
    ifstream f(pth);
    int count = 0;
    string word;
    while (f >> word)
    {
        count++;
    }
    lock_guard<mutex> lock (mtx);
    tconut += count;
    cout << pth << "Words count: " << count << endl;
}

int main()
{
    path pth = current_path();
    vector <path> files;
    for (const auto & entry : directory_iterator(pth))
    {
        if (entry.path().extension() == ".txt")
        {
            files.push_back(entry.path());
        }
    }

    vector<thread> thrs;
    for (const auto& fl: files)
    {
        thrs.emplace_back(WordCount, fl);
    }
    for (auto& i : thrs)
    {
        i.join();
    }
    cout << "Total words count: " << tconut << endl;
}