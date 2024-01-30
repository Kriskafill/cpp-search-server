#include "search_server.h"
#include "request_queue.h"

using namespace std;

vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
    if (requests_.size() == min_in_day_) {
        requests_.pop_back();
    }

    const QueryResult result = { raw_query, search_server_.FindTopDocuments(raw_query) };
    requests_.push_front(result);

    return result.result;
}

int RequestQueue::GetNoResultRequests() const {
    return count_if(requests_.begin(), requests_.end(), [](const QueryResult& query) {
        return query.result.empty();
        });
}