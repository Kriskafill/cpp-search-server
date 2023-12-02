#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;

    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }

    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    
    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        const int TF_one_word = 1.0 / words.size();
        for (const string& word : words) {
            documents_[word][document_id] += TF_one_word;
        }
        
        ++documents_all_count_;
    }

    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const set<string> query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        sort(matched_documents.begin(), matched_documents.end(), [](const Document& lhs, const Document& rhs) {
                return lhs.relevance > rhs.relevance;
            });

        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        
        return matched_documents;
    }

private:

    map<string, map<int, double>> documents_;
    int documents_all_count_ = 0;
    set<string> stop_words_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;

        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }

        return words;
    }

    set<string> ParseQuery(const string& text) const {
        set<string> query_words;

        for (const string& word : SplitIntoWordsNoStop(text)) {
            query_words.insert(word);
        }

        return query_words;
    }

    float IDF(const string& word) const {
        return static_cast<float>(log(static_cast<float>(documents_all_count_) / documents_.at(word).size()));
    }

    vector<Document> FindAllDocuments(const set<string>& query_words) const {
        vector<Document> matched_documents;
        map<int, double> plus_words;
        
        for (const string& word : query_words) {
            if (documents_.count(word) > 0) {
                for (const auto& i : documents_.at(word)) {
                    plus_words[i.first] += i.second * IDF(word);
                }
            }
        }
        
        for (const string& word : query_words) {
            if (word.at(0) == '-') {
                string str = word;
                str.erase(0);
                if (documents_.count(str) > 0) {
                    for (const auto& i : documents_.at(str)) {
                        plus_words.erase(i.first);
                    }
                }
            }
        }
        
        for (const auto& word : plus_words) {
            matched_documents.push_back({word.first, word.second});
        }
        
        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;

    search_server.SetStopWords(ReadLine());
    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();
    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", " << "relevance = "s << relevance << " }"s << endl;
    }
}