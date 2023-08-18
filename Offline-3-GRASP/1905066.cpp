#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

#define RANDOM_CUT_ITERATION 20
#define INF 1e9

vector<vector<pair<int, int>>> adj;
vector<pair<int, pair<int, int>>> edgelist;
int max_edge_weight = -INF;
int min_edge_weight = INF;

int n, m;

int random_int(int min, int max)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(min, max);

    return dis(gen);
}

int new_vertex_cut_calc(vector<int> &vertex_set_map, int own_set, int new_vertex)
{
    int cut = 0;
    int other_set = (own_set == 1) ? 2 : 1;


    for (int i = 0; i < adj[new_vertex].size(); i++){
        if (vertex_set_map[adj[new_vertex][i].first] == other_set){
             cut += adj[new_vertex][i].second;
        }
    }

    return cut;
}

int random_cut()
{
    vector<int> vertex_set_map(n + 1, 0);

    for (int i = 1; i <= n; i++)
        vertex_set_map[i] = (random_int(1, 2));

    int cut = 0;
    for (int i = 1; i <= n; i++)
    {
        if (vertex_set_map[i] == 1)
            cut += new_vertex_cut_calc(vertex_set_map, 1, i);
    }

    return cut;
}

int semi_greedy(int alpha = 0.8)
{

    int wmax = max_edge_weight;
    int wmin = min_edge_weight;
    int threshold = wmin + alpha * (wmax - wmin);

    vector<int> rcl;
    vector<int> vertex_set_map(n + 1, 0);
    
    for(int i = 0; i<edgelist.size(); i++)
        if(edgelist[i].first >= threshold) rcl.push_back(i);

    
    int initial_edge = rcl[random_int(0, rcl.size()-1)];
    vertex_set_map[edgelist[initial_edge].second.first] = 1;
    vertex_set_map[edgelist[initial_edge].second.second] = 2;

    
    int vertex_count = 2;

    while (vertex_count < n)
    {
        
        vector<int> sigmax(n + 1, 0);
        vector<int> sigmay(n + 1, 0);
        vector<int> rcl;

        int wmax = -INF;
        int wmin = INF;

        for(int i = 1; i<=n; i++){
            
            if(vertex_set_map[i] == 0){
                int cut1 = new_vertex_cut_calc(vertex_set_map, 1, i);
                int cut2 = new_vertex_cut_calc(vertex_set_map, 2, i);

                wmax = max(wmax, max(cut1, cut2));
                wmin = min(wmin, min(cut1, cut2));

                sigmax[i] = cut1;
                sigmay[i] = cut2;
            }
        }


        threshold = wmin + alpha * (wmax - wmin);
        for(int i = 1; i<=n; i++){
            if(sigmax[i] >= threshold || sigmay[i] >= threshold) rcl.push_back(i);
        }

        int new_vertex = rcl[random_int(0, rcl.size()-1)];
        vertex_set_map[new_vertex] = (sigmax[new_vertex] >= sigmay[new_vertex]) ? 1 : 2;
    
        vertex_count++;
    }


    int cut = 0;
    for (int i = 1; i <= n; i++)
    {
        if (vertex_set_map[i] == 1)
            cut += new_vertex_cut_calc(vertex_set_map, 1, i);
    }
    return cut;
}

int main()
{

    freopen("Graphs/g1.rud", "r", stdin);
    //freopen("output.out", "w", stdout);

    cin >> n >> m;

    adj.resize(n + 1);

    for (int i = 0; i < m; i++)
    {
        int u, v, w;
        cin >> u >> v >> w;
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
        edgelist.push_back({w, {u, v}});
        max_edge_weight = max(max_edge_weight, w);
        min_edge_weight = min(min_edge_weight, w);
    }

   
    // ----------------- random cut -----------------
    // int random_cut_result = 0;
    // for (int i = 0; i < RANDOM_CUT_ITERATION; i++) random_cut_result += random_cut();
    // random_cut_result /= RANDOM_CUT_ITERATION;
    // cout<<random_cut_result<<endl;

    // ----------------- greedy cut -----------------
    int greedy_cut_result = semi_greedy(0.5);
    cout << greedy_cut_result << endl;
}