#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

vector<pair<int, int>> adj;
vector<int> money;
map<pair<int, int>, int> memo;

int N;

long GetMax(int room)
{
    if(room == N) return 0;
    
    if(memo.count({room, 0}) == 0)
    {
        memo[{room, 0}] = money[room] + GetMax(adj[room].first);
        memo[{room, 1}] = money[room] + GetMax(adj[room].second);
    }
    return max(memo[{room, 0}], memo[{room, 1}]);
}

int main()
{
    cin >> N; 
    
    adj.resize(N);
    money.resize(N);
    
    for (int i = 0; i < N; i++) 
    {
        int room, dollars, r1, r2;
        string a, b;
        
        cin>>room>>dollars>>a>>b;
        
        r1 = (a == "E") ? N : stoi(a);
        r2 = (b == "E") ? N : stoi(b);
        adj[room] = {r1, r2}; 
        money[room] = dollars;
    }
    cout << GetMax(0) << endl;
}