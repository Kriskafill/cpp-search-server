#include "search_server.h"
#include "request_queue.h"

using namespace std;

vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
    if (requests_.size() == min_in_day_) {
        requests_.pop_back();
    }

    const vector<Document> documents = search_server_.FindTopDocuments(raw_query);
    const QueryResult result = { raw_query, documents.empty() };
    requests_.push_front(result);

    return documents;
}

int RequestQueue::GetNoResultRequests() const {
    return count_if(requests_.begin(), requests_.end(), [](const QueryResult& query) {
        return query.isEmptyResult;
    });
}