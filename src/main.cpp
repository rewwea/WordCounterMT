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

void CountStats(const path& pth) {
    ifstream file(pth);
    if (!file) {
        lock_guard<mutex> lock(mtx);
        cerr << "Не удалось открыть файл: " << pth << endl;
        return;
    }

    int wordCount = 0;
    int charCount = 0;
    int lineCount = 0;
    string line;

    while (getline(file, line)) {
        lineCount++;
        charCount += line.size();
        istringstream iss(line);
        string word;
        while (iss >> word) {
            wordCount++;
        }
    }

    lock_guard<mutex> lock(mtx);
    totalWords += wordCount;
    totalChars += charCount;
    totalLines += lineCount;

    cout << pth << " -> Слов: " << wordCount
         << ", Символов: " << charCount
         << ", Строк: " << lineCount << endl;
}

path GetDirectory(int argc, char* argv[]) {
    if (argc > 1) {
        path dirPath = argv[1];
        if (exists(dirPath) && is_directory(dirPath)) {
            return dirPath;
        } else {
            cerr << "Указанный путь не существует или не является директорией." << endl;
            return current_path();
        }
    }

    cout << "Введите путь к директории: ";
    string input;
    getline(cin, input);
    path dirPath = input.empty() ? current_path() : path(input);

    if (!exists(dirPath) || !is_directory(dirPath)) {
        cerr << "Указанный путь не существует или не является директорией." << endl;
        return current_path();
    }

    return dirPath;
}

int main(int argc, char* argv[]) {
    path dirPath = GetDirectory(argc, argv);

    vector<path> files;
    try {
        for (const auto& entry : recursive_directory_iterator(dirPath)) {
            if (entry.path().extension() == ".txt") {
                files.push_back(entry.path());
            }
        }
    } catch (const exception& e) {
        cerr << "Ошибка при доступе к директории: " << e.what() << endl;
        return 1;
    }

    vector<thread> threads;
    for (const auto& file : files) {
        threads.emplace_back(CountStats, file);
    }

    for (auto& t : threads) {
        t.join();
    }

    cout << "\nОбщая статистика:" << endl;
    cout << "  Всего слов: " << totalWords << endl;
    cout << "  Всего символов: " << totalChars << endl;
    cout << "  Всего строк: " << totalLines << endl;

    return 0;
}