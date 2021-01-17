#include <algorithm>
#include <iostream>
#include <utility>
#include <map>
#include <queue>
#include <vector>
#include <functional>

using namespace std;

// A classe node_t foi criada para facilitar a obtenção da vizinhança de um dado
// vértice e para indicar a distância no qual ele foi visitado (mantendo todas
// essas informações localizadas). Também é facilmente possível adicionar um
// pointeiro para o parent, mas isso não contribui para esse exercício.
class node_t {
public:
    // initialize the object.
    node_t(int id)
        :
        _id{id},
        _neighborhood{},
        _distance{-1}
    {        
    }

    // return the given id.
    void id() {
        cout << _id << endl;
    }

    // for debug proposes: print id and neighborhood.
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

    // reset the state of this object.
    void reset() {
        _neighborhood.clear();
        _distance = -1;
    }

    // add "to" to neighborhood and add this to its neighborhood.
    void add_edge(node_t * to, int cost) {
        _neighborhood.push_back(std::make_pair(to, cost));
        to->_neighborhood.push_back(std::make_pair(this, cost));
    }

    // return the neighborhood.
    std::vector<std::pair<node_t*, int>> get_neighborhood() const {
        return _neighborhood;
    }

    // update distance, ie, cost from root.
    void set_dist(int dist) {
        _distance = dist;
    }

    // return the distance from root.
    int get_dist() const {
        return _distance;
    }
        
    // indicates if this node wasn't visited yet.
    bool is_new() const {
        return _distance == -1;
    }

private:
    int _id;
    std::vector<std::pair<node_t*, int>> _neighborhood;
    int _distance;
};

// Essa função lê o grafo e transforma em um novo grafo com o dobro do tamanho,
// mas com a garantia que caso haja um caminho entre dois vértices de paridade
// diferente, esse caminho é impar.
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

// Essa função cria os vetores de indices pares e impares e também já associa as
// arestas de modo apropriado, isto é, associa um dado vértice aos seus vizinhos
// de paridade diferente da sua. Por exemplo, se o vértice (1) era conectado ao
// (2), agora o vértice (1) possui uma versão par: (2) e se conecta a versão
// ímpar de (2): (5).
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

// Adapta a função da std para o tipo std::pair<...>.
namespace std {
    using Tp = std::pair<node_t*, int>;
    template<>
    struct greater<Tp> {
        bool operator()(Tp l, Tp r) {
            return l.second > r.second;
        }
    };
}

// Implementação da função de dijkstra, que identifica o caminho mínimo entre
// dois vértices. Essa função retorna a distância mínima se um caminho for
// encontrado e -1 caso contrário, uma vez que, -1 não é uma solução válida.
using data_t = std::pair<node_t*, int>;
using alloc_t = std::vector<data_t>;
using pqueue_t = std::priority_queue<data_t, alloc_t, std::greater<data_t>>;
int dijkstra(node_t *start, node_t *end) {
    pqueue_t seen{}; // priority queue.
    start->set_dist(0);
    seen.push({start, 0});

    while (seen.size() > 0 && seen.top().first != end) {
        std::pair<node_t*, int> head = seen.top();
        node_t *top = head.first;
        top->set_dist(head.second);
        seen.pop();

        // extend neighborhood if not seen before.
        auto neighborhood = top->get_neighborhood();
        for (auto nb: neighborhood) {
            int dist = nb.second;
            node_t *nd = nb.first;
            if (!nd->is_new()) continue;
            seen.push({nd, top->get_dist() + dist});
        }
    }
    
    return (seen.size() > 0) ? seen.top().second : -1;
}


int main(int argc, char **argv)
{
    // lê a entrada do usuário.
    int N = 0, V = 0;
    cin >> N >> V;
    std::vector<std::tuple<int, int, int>> graph{};
    for (int i = 0; i < V; ++i) {
        int u, v, w;
        cin >> u >> v >> w;
        graph.push_back({u, v, w});
    }

    // adapta a entrada do usuário para o formato do sistema.
    graph = transform(graph);

    // contrói o gráfico de paridade.
    std::pair<node_t*, node_t*> se = build(graph, 3, 2 * N);

    // calcula a distância mínima.
    int dist = dijkstra(se.first, se.second);
    if (dist > 0)
        cout << dist << endl; // caso haja algum caminho.
    else
        cout << ":(" << endl; // caso contrário.

    return 0;
}
