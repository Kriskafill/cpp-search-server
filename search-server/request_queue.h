#pragma once

#include <deque>
#include <string>
#include <vector>

class RequestQueue {
public:

    explicit RequestQueue(const SearchServer& search_server) : search_server_(search_server) { }

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;

private:

    struct QueryResult {
        std::string query;
        std::vector<Document> result;
    };

    std::deque<QueryResult> requests_;
    const SearchServer& search_server_;
    const static int min_in_day_ = 1440;
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    if (requests_.size() == min_in_day_) {
        requests_.pop_back();
    }

    const QueryResult result = { raw_query, search_server_.FindTopDocuments(raw_query, document_predicate) };
    requests_.push_front(result);

    return result.result;
}