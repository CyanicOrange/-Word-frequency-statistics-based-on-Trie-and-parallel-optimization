#include <iostream>
#include<thread>
#include <vector>
#include <fstream>
#include <thread>
#include <mutex>
#include <utility>
#include <sstream>
#include <algorithm>
//Originated by Cyanic_Orange,github
using namespace std;
mutex mtx;
vector<pair<string, int>> wordsFreq;
class TrieNode {			
 public:  
    TrieNode* childNode[26];
    int frequency;
    

    void insert(TrieNode* root, string word) {
        TrieNode* node = root;
        for (char c : word) {
            if (node->childNode[c - 'a'] == NULL) {
                node->childNode[c - 'a'] = new TrieNode();
            }
            node = node->childNode[c - 'a'];
        }
        node->frequency++;
    }
   TrieNode() {
        frequency = 0;
        for (int i = 0; i < 26; i++) {
            childNode[i] = NULL;
        }
    }
    
};
TrieNode* root = new TrieNode();

void readWords(TrieNode* root, string filename) {
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        for (char& c : line) {
           c = isalpha(c) ? tolower(c) : ' ';
        }
        stringstream ss(line);
        string word;
        while (ss >> word) {
            root->insert(root, word);
        }
    }
    file.close();
}
void dfs(TrieNode* node, string word) {
    if (node == NULL) {
        return;
    }
    if (node->frequency > 0) {
        lock_guard<mutex> lock(mtx);
        wordsFreq.push_back({word, node->frequency});
    }
    for (char c = 'a'; c <= 'z'; c++) {
        dfs(node->childNode[c - 'a'], word + c);
    }
}
void dfsMultiThread(TrieNode* root) {
    vector<thread> threads;
    for (char c = 'a'; c <= 'z'; c++) {
        threads.push_back(thread(dfs, root->childNode[c - 'a'], string(1, c)));
    }
    for (auto& t : threads) {
        t.join();
    }
}
void topKFrequentWords(TrieNode* root, int k) {
    dfsMultiThread(root);
    sort(wordsFreq.begin(), wordsFreq.end(), [](pair<string, int>& a, pair<string, int>& b) {
        if (a.second == b.second) {
            return a.first < b.first;
        }
        return a.second > b.second;
    });
    ofstream file2("out.txt");
    for (int i = 0; i < k; i++) {
        cout << wordsFreq[i].first << " " << wordsFreq[i].second << endl;
        file2 << wordsFreq[i].first << " " << wordsFreq[i].second << endl;
    }
    file2.close(); 
}
int main( int argc, char* argv[]) {
    
    string filename = "in.txt";
    
    readWords(root, filename);
    topKFrequentWords(root, 100);
    return 0;
}
