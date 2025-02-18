#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <filesystem>

using namespace std;
using namespace filesystem;

mutex mtx;
int totalWords = 0;
int totalChars = 0;
int totalLines = 0;

void CountStats(const path& pth)
{
    ifstream file(pth);
    if (!file)
    {
        cerr << "Failed to open " << pth << endl;
        return;
    }
    
    int wordCount = 0;
    int charCount = 0;
    int lineCount = 0;
    string line;
    
    while(getline(file, line))
    {
        lineCount++;
        charCount += line.size();
        istringstream iss(line);
        string word;
        while(iss >> word)
        {
            wordCount++;
        }
    }
    
    lock_guard<mutex> lock(mtx);
    totalWords += wordCount;
    totalChars += charCount;
    totalLines += lineCount;
    
    cout << pth << " -> Words: " << wordCount
         << ", Chars: " << charCount
         << ", Lines: " << lineCount << endl;
}

int main(int argc, char* argv[])
{
    path dirPath = (argc > 1) ? argv[1] : current_path();
    
    vector<path> files;
    try {
        for (const auto & entry : recursive_directory_iterator(dirPath))
        {
            if (entry.path().extension() == ".txt")
            {
                files.push_back(entry.path());
            }
        }
    }
    catch (const exception &e)
    {
        cerr << "Error accessing directory: " << e.what() << endl;
        return 1;
    }
    
    vector<thread> threads;
    for (const auto& file : files)
    {
        threads.emplace_back(CountStats, file);
    }
    for (auto& t : threads)
    {
        t.join();
    }
    
    cout << "Total -> Words: " << totalWords
         << ", Chars: " << totalChars
         << ", Lines: " << totalLines << endl;
    
    return 0;
}
