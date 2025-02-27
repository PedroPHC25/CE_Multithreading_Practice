#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <regex>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

vector<string> read_file(string filepath)
{
    ifstream file(filepath.c_str());

    if (!file)
    {
        cout << "File not found" << endl;
    }

    vector<string> lines;
    string line;

    while (getline(file, line))
    {
        lines.push_back(line);
    }

    file.close();
    
    return vector<string>(lines.begin() + 79, lines.begin() + 196037);
}

string toLowerCase(string str)
{
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

void countLoveAndHate(const vector<string>& text, int start, int end, int& nLove, int& nHate)
{
    for (int i = start; i < end; i++)
    {
        string line = toLowerCase(text[i]);

        regex loveRegex("\\blove\\b");
        regex hateRegex("\\bhate\\b");

        sregex_iterator itLove(line.begin(), line.end(), loveRegex);
        sregex_iterator itHate(line.begin(), line.end(), hateRegex);
        sregex_iterator end;

        nLove += distance(itLove, end);
        nHate += distance(itHate, end);
    }
}

int main()
{
    ofstream file("results.csv");
    file << "NumThreads,BlockSize,Time\n";

    auto startEverything = high_resolution_clock::now();

    auto startLoad = high_resolution_clock::now();
    vector<string> data = read_file("gutenberg.txt");
    auto endLoad = high_resolution_clock::now();

    auto durationLoad = duration_cast<milliseconds>(endLoad - startLoad).count();

    cout << "=================================" << endl;
    cout << "Load duration: " << durationLoad << " ms" << endl;
    cout << "=================================" << endl;

    int maxNumThreads = 10;

    auto startSearch = high_resolution_clock::now();

    for (int numThreads = 1; numThreads <= maxNumThreads; numThreads++)
    {
        int chunkSize = data.size() / numThreads;
        int nLove = 0, nHate = 0;
        vector<thread> threads;
    
        auto startTime = high_resolution_clock::now();
        for (int i = 0; i < numThreads; i++)
        {
            int start = i * chunkSize;
            int end = start + chunkSize;
            if (i == numThreads - 1)
            {
                end = data.size() - 1;
            }
    
            threads.emplace_back(countLoveAndHate, ref(data), start, end, ref(nLove), ref(nHate));
        }
    
        for (int i = 0; i < numThreads; i++)
        {
            threads[i].join();
        }
    
        auto endTime = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(endTime - startTime).count();
    
        file << numThreads << "," << chunkSize << "," << duration << "\n";

        cout << "Number of threads: " << numThreads << endl;
        cout << "Block size: " << chunkSize << endl;
        cout << "Love: " << nLove << ", Hate: " << nHate;
        cout << " - Time: " << duration << " ms" << endl;

        if (nLove > nHate)
        {
            cout << "More love than hate." << endl;
        }
        else if (nLove < nHate)
        {
            cout << "More hate than love." << endl;
        }
        else
        {
            cout << "As much love as hate." << endl;
        }

        cout << "----------------------------------------" << endl;
    }

    auto endSearch = high_resolution_clock::now();
    auto endEverything = high_resolution_clock::now();
    
    auto durationSearch = duration_cast<milliseconds>(endSearch - startSearch).count();
    auto durationEverything = duration_cast<milliseconds>(endEverything - startEverything).count();

    cout << "=================================" << endl;
    cout << "Search duration: " << durationSearch << " ms" << endl;
    cout << "Process duration: " << durationEverything<< " ms" << endl;
    cout << "=================================" << endl;

    return 0;
}
