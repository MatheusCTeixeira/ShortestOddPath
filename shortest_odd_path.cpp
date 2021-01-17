#include <algorithm>
#include <iostream>
#include <utility>
#include <map>
#include <queue>
#include <vector>
#include <functional>

using namespace std;


class node_t {

public:
    node_t(int id): _id{id}, _neighborhood{}, _distance{-1} {
        
    }

    void id() {
        cout << _id << endl;
    }
    
    void print() {
        _distance = 0;
        for (auto n: _neighborhood) {
            cout << _id << " --> " << n.first->_id << endl;
        }

        cout << "------" << endl;

        for (auto n: _neighborhood) {
            if (n.first->_distance < 0)
                n.first->print();
        }
            
    }
    
    void reset() {
        _neighborhood.clear();
        _distance = -1;
    }
    
    void add_edge(node_t * to, int cost) {
        _neighborhood.push_back(std::make_pair(to, cost));
        to->_neighborhood.push_back(std::make_pair(this, cost));
    }

    std::vector<std::pair<node_t*, int>> get_neighborhood() const {
        return _neighborhood;
    }

    void set_dist(int dist) {
        _distance = dist;
    }

    int get_dist() const {
        return _distance;
    }
        

    bool is_new() const {
        return _distance == -1;
    }

private:
    int _id;
    std::vector<std::pair<node_t*, int>> _neighborhood;
    int _distance;
};


std::vector<std::tuple<int, int, int>> transform(
                                 std::vector<std::tuple<int, int, int>> edges)
{
    decltype(edges) result{};

    for (auto e: edges) {
        int u = std::get<0>(e), v = std::get<1>(e), cost = std::get<2>(e);
        result.push_back({2 * u, 2 * v + 1, cost});
        result.push_back({2 * u + 1, 2 * v, cost});
    }

    return result;
}

std::pair<node_t*, node_t*> build(std::vector<std::tuple<int, int, int>> edges,
                                  int start, int end) {
    std::map<int, node_t*> cache{};
    
    for (auto edge: edges) {
        node_t *u, *v;

        // verify if u is in cache else create u.
        if (cache.find(std::get<0>(edge)) != cache.end()) {
            u = cache.at(std::get<0>(edge));
        } else {
            u = new node_t{std::get<0>(edge)};
            cache.insert({std::get<0>(edge), u});
        }

        // do the same with v.
        if (cache.find(std::get<1>(edge)) != cache.end()) {
            v = cache.at(std::get<1>(edge));
        } else {
            v = new node_t{std::get<1>(edge)};
            cache.insert({std::get<1>(edge), v});
        }

        // add v to neigborhood of u, and hence u to neigborhood of v.
        u->add_edge(v, std::get<2>(edge));
    }

    return {cache.at(start), cache.at(end)}; // first node or root.
}

namespace std {
    using Tp = std::pair<node_t*, int>;
    template<>
    struct greater<Tp> {
        bool operator()(Tp l, Tp r) {
            return l.second > r.second;
        }
    };
}

using data_t = std::pair<node_t*, int>;
using alloc_t = std::vector<data_t>;
using pqueue_t = std::priority_queue<data_t, alloc_t, std::greater<data_t>>;
int dijkstra(node_t *start, node_t *end) {
    pqueue_t seen{};
    start->set_dist(0);
    seen.push({start, 0});

    while (seen.size() > 0 && seen.top().first != end) {
        std::pair<node_t*, int> head = seen.top();
        node_t *top = head.first;
        top->set_dist(head.second);
        seen.pop();

        auto neighborhood = top->get_neighborhood();
        for (auto nb: neighborhood) {
            int dist = nb.second;
            node_t *nd = nb.first;
            if (!nd->is_new()) continue;
            seen.push({nd, top->get_dist() + dist});

        }
    }

    if (seen.size() > 0)
        return seen.top().second;
    else
        return -1;
}


int main(int argc, char **argv)
{
    int N = 0, V = 0;
    cin >> N >> V;
    std::vector<std::tuple<int, int, int>> graph{};
    for (int i = 0; i < V; ++i) {
        int u, v, w;
        cin >> u >> v >> w;
        graph.push_back({u, v, w});
    }
  
    graph = transform(graph);
    std::pair<node_t*, node_t*> se = build(graph, 3, 2 * N);
    
    int dist = dijkstra(se.first, se.second);
    if (dist > 0)
        cout << dist << endl;
    else
        cout << ":(" << endl;

    return 0;
}
