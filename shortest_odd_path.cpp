#include <algorithm>
#include <iostream>
#include <utility>
#include <map>
#include <queue>
#include <vector>
#include <functional>

using namespace std;

struct edge_t {
    int _v;
    int _cost;
    int _dist = -1;

};

namespace std {
template <>
struct less<edge_t*> {
    bool operator()(edge_t* l, edge_t* r) {
        return (l->_cost + l->_dist) > (r->_cost + r->_dist);
    }
};
    
}
using graph_t = multimap<int, edge_t>;

graph_t G{
    {1, {2,  1}},
    {1, {3,  3}},
    {1, {5, 10}},
    {2, {3,  1}},
    {2, {4,  3}},
    {3, {4,  1}},
    {4, {5,  1}}
};

using Tp = typename decltype(G)::iterator;

std::pair<Tp, Tp> get_nb(graph_t& G, int u) {
    cout << "u = " << u << endl;
    return G.equal_range(u);
}


int get_edge_cost(const graph_t& G, int u, int v)
{
    int _u = std::min(u, v);
    int _v = std::max(u, v);

    auto matches = G.equal_range(u);
    for (auto n = matches.first; n != matches.second; ++n) {
        if ((*n).second._v == _v)
            return (*n).second._cost;
    }

    return -1;
}


int dijkstra(graph_t& G, int u, int v) {
    std::priority_queue<edge_t*,
                        std::vector<edge_t*>,
                        std::less<edge_t*>> seen{};

    auto neigbors = get_nb(G, u);
    for (auto n = neigbors.first; n != neigbors.second; n++) {
        edge_t* edge = &(*n).second;
        edge->_dist = edge->_cost;
        seen.push(edge);
    }

    edge_t* found = nullptr;
    while (seen.size() > 0 && seen.top()->_v != v) {
        edge_t *head = seen.top();
        seen.pop();

        auto nb = get_nb(G, head->_v);
        for (auto n = nb.first; n != nb.second; ++n) {
            edge_t* edge = &(*n).second;
            if (edge->_dist != -1) continue;
            edge->_dist = head->_dist + edge->_cost;
            seen.push(edge);
            
            if (edge->_v == v) {
                found = edge;
                break;
            }
        }
    }

    if (found)
    cout << found->_v << " " << found->_dist << endl;
    return 0;
}


int main(int argc, char **argv)
{
    graph_t G_b{};
    
    for (auto value : G) {
        auto key = value.first;
        auto neighbor = value.second;
        auto v = neighbor._v;
        auto cost = neighbor._cost;
        
        auto even = 2 * key;
        auto odd = 2 * key + 1;
        G_b.insert({even, {2 * v + 1, cost}});
        G_b.insert({odd, {2 * v, cost}});
    }

    dijkstra(G_b, 2, 6);

    return 0;
}
