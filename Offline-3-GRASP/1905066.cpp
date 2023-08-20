#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

#define RANDOM_CUT_ITERATION 20
#define GRASP_RECORD_INTERVAL 20
#define INF 1e9

vector<vector<pair<int, int>>> adj;
vector<pair<int, pair<int, int>>> edgelist;
int max_edge_weight = -INF;
int min_edge_weight = INF;
int dummy = 0;
int grasp_max_iteration = 500;

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

struct Solution
{
    vector<int> vertex_set_map;
    int cut_value;

    Solution(vector<int> vertex_set_map, int cut_value)
    {
        this->vertex_set_map = vertex_set_map;
        this->cut_value = cut_value;
    }

    Solution(vector<int> vertex_set_map)
    {
        this->vertex_set_map = vertex_set_map;
        this->cut_value = 0;
        for (int i = 1; i <= n; i++)
        {
            if (vertex_set_map[i] == 1) this->cut_value += new_vertex_cut_calc(vertex_set_map, 1, i);
        }
    }

    Solution()
    {
        
    }

};


Solution random_cut()
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

    return Solution(vertex_set_map, cut);
}

// https://www.researchgate.net/publication/228569559_On_greedy_construction_heuristics_for_the_MAX-CUT_problem
// The greedy heuristic provided in the problem specification is similar to SG1 of this paper
// score(i) = max(w(i, S1), w(i, S2))
// SG3 gives a slightly better result
// score(i) = |w(i, S1) − w(i, S2)|

Solution semi_greedy(int heuristic = 2, bool greedy = false)
{
    double alpha = 1;
    if(!greedy) alpha = random_int(0, 100)*1.0/100;

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
        vector<int> score(n + 1, 0);
        vector<int> rcl;

        
        int wmax = -INF;
        int wmin = INF;

        for(int i = 1; i<=n; i++){
            
            if(vertex_set_map[i] == 0){
                int cut1 = new_vertex_cut_calc(vertex_set_map, 1, i);
                int cut2 = new_vertex_cut_calc(vertex_set_map, 2, i);
                
                if(heuristic == 2) score[i] = abs(cut1 - cut2);
                else score[i] = max(cut1, cut2);

                wmax = max(wmax, score[i]);
                wmin = min(wmin, score[i]);

                sigmax[i] = cut1;
                sigmay[i] = cut2;
            }

        }
        
        threshold = wmin + alpha * (wmax - wmin);
        for(int i = 1; i<=n; i++){
            if(vertex_set_map[i] == 0 && score[i] >= threshold) {
                rcl.push_back(i);
            }
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
    
    return Solution(vertex_set_map, cut);
}



Solution local_search(Solution s, int &iteration_count = dummy){
    bool change = true;
    iteration_count = 0;

    vector<int> vertex_set_map = s.vertex_set_map;
    
    vector<int> sigma[3];
    sigma[1].resize(n+1, 0);
    sigma[2].resize(n+1, 0);

    for(int i = 1; i<=n; i++){
        sigma[1][i] = new_vertex_cut_calc(vertex_set_map, 1, i); // if the vertex is in set 1
        sigma[2][i] = new_vertex_cut_calc(vertex_set_map, 2, i); // if the vertex is in set 2
    }

    int start_index = 0;
    while (change)
    {
        change = false;
        iteration_count++;
        for(int cnt = 0; cnt<n; cnt++){
            int i = (start_index + cnt) % n + 1;
            int own_set = vertex_set_map[i];
            int other_set = (own_set == 1) ? 2 : 1;
            
            if(sigma[own_set][i] < sigma[other_set][i]){
                vertex_set_map[i] = other_set;
                change = true;
                start_index = cnt + 1;
                for(int j = 0; j<adj[i].size(); j++){
                    int v = adj[i][j].first;
                    int w = adj[i][j].second;

                    sigma[own_set][v] += w;
                    sigma[other_set][v] -= w;
                }
                break;
            }
        }
    }

    return Solution(vertex_set_map);
    
}

vector<pair<int,int>> grasp_record;


Solution GRASP(int max_iteration){
    
    Solution best_solution;
    grasp_record.clear();
    
    int local_search_iteration, total_local_search_iteration = 0;
    int total_improvement = 0;
    int local_search_value = 0;

    for(int i = 1; i<=max_iteration; i++){
        Solution s = semi_greedy();
        int semi_greedy_cut = s.cut_value;

        s = local_search(s, local_search_iteration);
        total_local_search_iteration += local_search_iteration;
        total_improvement += s.cut_value - semi_greedy_cut;
        local_search_value += s.cut_value;

        if(i==1 || s.cut_value > best_solution.cut_value) best_solution = s;

        if(i%GRASP_RECORD_INTERVAL == 0){
            // cout<<"GRASP_ITERATION "<<i<<" "<<best_solution.cut_value<<endl;
            grasp_record.push_back({i, best_solution.cut_value});
        }
    }

    int avarage_local_search_iteration = total_local_search_iteration/max_iteration;
    int avarage_improvement = total_improvement/max_iteration;
    int avarage_local_search_value = local_search_value/max_iteration;

    // cout<<"Local_Search "<<avarage_local_search_iteration<<" "<<avarage_improvement<<" "<<avarage_local_search_value<<endl;
    //cout<<"GRASP "<<best_solution.cut_value<<endl;

    cout<<avarage_local_search_iteration<<","<<avarage_improvement<<","<<avarage_local_search_value<<",";

    return best_solution;
}


int main(int argc, char *argv[])
{
    freopen(argv[1], "r", stdin);
    //freopen(argv[2], "w", stdout);

    string problem_name = argv[1];
    problem_name = problem_name.substr(problem_name.find_last_of("/\\") + 1);
    problem_name = problem_name.substr(0, problem_name.find_last_of("."));
    problem_name[0] = toupper(problem_name[0]);

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

    fclose(stdin);

    freopen("known_best.txt", "r", stdin);
    string str;
    int known_best, known_found = 0;
    while (cin >> str >> known_best){
        if(str == problem_name){
            known_found = 1;
            break;
        }
    }
    fclose(stdin);
    

    // cout<<"Problem "<<problem_name<<endl;
    // cout<<"|V|  "<<n<<endl;
    // cout<<"|E| "<<m<<endl;
    // cout<<"W ["<<min_edge_weight<<","<<max_edge_weight<<"]"<<endl;


    // random cut
    int simple_random_cut = 0;
    for(int i = 0; i<RANDOM_CUT_ITERATION; i++) simple_random_cut += random_cut().cut_value;
    simple_random_cut = simple_random_cut/RANDOM_CUT_ITERATION;
    // cout<<"Ranomized "<<simple_random_cut<<endl;

    // greedy
    int greedy_cut = semi_greedy(true).cut_value;
    // cout<<"Greedy "<<greedy_cut<<endl;

    // semi greedy
    int semi_greedy_cut = 0;
    for(int i = 0; i<RANDOM_CUT_ITERATION; i++) semi_greedy_cut += semi_greedy().cut_value;
    semi_greedy_cut = semi_greedy_cut/RANDOM_CUT_ITERATION;
    // cout<<"Semi_Greedy "<<semi_greedy_cut<<endl;
    

    //-----------------GRASP-----------------
    
    cout<<problem_name<<","<<n<<","<<m<<","<<min_edge_weight<<","<<max_edge_weight<<",";
    cout<<simple_random_cut<<",";
    cout<<greedy_cut<<",";
    cout<<semi_greedy_cut<<",";

    Solution s = GRASP(grasp_max_iteration);
    //cout<<grasp_record[0].first<<","<<grasp_record[0].second<<","<<s.cut_value;
    cout<<grasp_max_iteration<<","<<s.cut_value;
    if(known_found) cout<<","<<known_best;
    cout<<endl;
    
    // for(int i = 0; i<grasp_record.size(); i++){
    //     // cout<<"GRASP_ITERATION "<<grasp_record[i].first<<" "<<grasp_record[i].second<<endl;
    //     cout<<",,,,,,,,,,,"<<grasp_record[i].first<<","<<grasp_record[i].second<<endl;
    // }

    return 0;

}