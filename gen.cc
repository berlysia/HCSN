#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <vector>
#include <numeric>
#include <random>
#include <iomanip>

using namespace std;

vector<vector<uint32_t> > edges; // [node number][incoming edge's' src node idx]
unsigned long long N, M, steps;
vector<uint32_t> activeNodes; // [idx] = active node number
vector<uint32_t> degrees; // [node idx] = outgoing links
long double A, pSum = 0, pProd = 1, sumProb;
vector<long double> probs;
random_device rdev;
mt19937 mt(rdev());
uniform_real_distribution<> dist(0.0, 1.0);


long double prob (uint32_t deg) {
    return (pProd / pSum) / (A + deg);
}

int main (int argc, char* argv[]) {
    cerr << "input [param M] [param A] [nodes]" << endl;;
    cin >> M;
    cin >> A;
    cin >> N;
    steps = N - M;
    
    edges.resize(M);
    activeNodes.resize(M);
    degrees.resize(M);
    probs.resize(M+1);

    for (int i = M - 1; i >= 0; --i) activeNodes[i] = i;
    for (int i = M - 1; i >= 0; --i) {
        degrees[i] = M - 1;
        pSum += degrees[i] + A;
        pProd *= degrees[i] + A;
    }
    for (int i = M - 1; i >= 0; --i) {
        for (int j = M - 1; j >= 0; --j) {
            if (i == j) continue;
            edges[i].emplace_back(j);
        }
    }

    uint32_t addedNode;
    while (steps--) {
        // 1: いっこふやす
        edges.emplace_back(vector<uint32_t>());
        addedNode = edges.size() - 1;
        degrees.emplace_back(0); // 次数0

        // 2: ふやしたノードiにactiveノードからリンクを張る
        auto& current = edges.back();
        for (int i = M - 1; i >= 0; --i) {
            current.assign(activeNodes.begin(), activeNodes.end());
            ++degrees[activeNodes[i]];
        }

        // 3: ノードiをactivate
        activeNodes.emplace_back(addedNode);
        pSum = 0;
        pProd = 1;
        for (int i = M; i >= 0; --i) {
            pSum += degrees[activeNodes[i]] + A;
            pProd *= degrees[activeNodes[i]] + A;
        }

        // 4: ひとつdeactivate
        for (int i = M; i >= 0; --i) probs[i] = prob(degrees[activeNodes[i]]);
        sumProb = accumulate(probs.begin(), probs.end(), 0l);
        for (int i = M; i >= 0; --i) probs[i] /= sumProb;
        for (int i = 1; i <= M; ++i) probs[i] += probs[i - 1];
        probs.back() = 1l;
        // for (int i = 0; i <= M; ++i) cerr << probs[i] << ", ";
        // cerr << endl;

        auto dropIdx = lower_bound(probs.begin(), probs.end(), dist(mt)) - probs.begin();
        activeNodes.erase(remove(activeNodes.begin(), activeNodes.end(), activeNodes[dropIdx]), activeNodes.end());
    }

    cout << edges.size() << "\n";
    int countEdges = 0;
    for (int i = 0, l = edges.size(); i < l; ++i) {
        countEdges += edges[i].size();
    }
    cout << countEdges << "\n";
    for (int i = 0, l = edges.size(); i < l; ++i) {
        auto& edges_i = edges[i];
        sort(edges_i.begin(), edges_i.end());
        for (int j = 0, ll = edges_i.size(); j < ll; ++j) {
            cout << i << " " << edges_i[j] << "\n";
        }
    }
    // for (int i = 0, l = edges.size(); i < l; ++i) {
    //     auto& edges_i = edges[i];
    //     sort(edges_i.begin(), edges_i.end());
    //     cerr << i << ": #" << edges_i.size() << " - ";
    //     for (int j = 0, ll = edges_i.size(); j < ll; ++j) {
    //         cerr << edges_i[j];
    //         if (j + 1 != ll) cerr << ", ";
    //     }
    //     cerr << "\n";
    // }

    return 0;
}
