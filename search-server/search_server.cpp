#include "search_server.h"

using namespace std;

void SearchServer::AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
    if (TextContainsSpecialCharacters(document)) {
        throw invalid_argument("special character in the document"s);
    }
    if (document_id < 0) {
        throw invalid_argument("negative ID"s);
    }
    for (const auto& id : documents_) {
        if (document_id == id.first) {
            throw invalid_argument("existing ID"s);
        }
    }

    const vector<string> words = SplitIntoWordsNoStop(document);
    const double TF_one_word = 1.0 / words.size();
    for (const string& word : words) {
        word_to_document_freqs_[word][document_id] += TF_one_word;
    }

    documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status });
    identifiers_.push_back(document_id);
}

vector<Document> SearchServer::FindTopDocuments(const string& raw_query, DocumentStatus status) const {
    return FindTopDocuments(raw_query, [&status](
        [[maybe_unused]] int document_id,
        [[maybe_unused]] DocumentStatus document_status,
        [[maybe_unused]] int rating) {
            return document_status == status;
        });
}

int SearchServer::GetDocumentId(int index) const {
    if (index < 0 || index > identifiers_.size() - 1) {
        throw out_of_range("the ID is out of acceptable values"s);
    }

    return identifiers_[index];
}

int SearchServer::GetDocumentCount() const {
    return documents_.size();
}

tuple<vector<string>, DocumentStatus> SearchServer::MatchDocument(const string& raw_query, int document_id) const {
    const Query query = ParseQuery(raw_query);

    vector<string> matched_words;

    for (const string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }

    for (const string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }

    return { matched_words, documents_.at(document_id).status };
}

bool SearchServer::IsStopWord(const string& word) const {
    return stop_words_.count(word) > 0;
}

vector<string> SearchServer::SplitIntoWordsNoStop(const string& text) const {
    vector<string> words;

    for (const string& word : SplitIntoWords(text)) {
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }

    return words;
}

int SearchServer::ComputeAverageRating(const vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }

    int rating_sum = accumulate(ratings.begin(), ratings.end(), 0);

    return rating_sum / static_cast<int>(ratings.size());
}

SearchServer::QueryWord SearchServer::ParseQueryWord(string text) const {
    if (TextContainsSpecialCharacters(text)) {
        throw invalid_argument("special character in the request"s);
    }

    bool is_minus = false;
    if (text[0] == '-') {
        is_minus = true;
        text = text.substr(1);
    }

    if (text == ""s || text.at(0) == '-') {
        throw invalid_argument("invalid request"s);
    }

    return { text, is_minus, IsStopWord(text) };
}

SearchServer::Query SearchServer::ParseQuery(const string& text) const {
    Query query;

    for (const string& word : SplitIntoWords(text)) {
        const QueryWord query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                query.minus_words.insert(query_word.data);
            }
            else {
                query.plus_words.insert(query_word.data);
            }
        }
    }

    return query;
}

double SearchServer::ComputeWordInverseDocumentFreq(const string& word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}