
#include <iostream>
#include <vector>
#include <array>
#include <cstdint>

using namespace std;

struct point{
    int x, y;
};




// def max_x(v, turn):
//     vmax = (turn + v) // 2
//     dec_vmax = (turn + v - 1) // 2
//     max_x = (1 + vmax) * vmax // 2 + (v + dec_vmax) * (dec_vmax - v + 1) // 2
//     return max_x

// def min_x(v, turn):
//     return -max_x(v+2, turn) + (v + 1) * turn


int max_x(int v, int turn){
    int vmax = (turn + v) / 2;
    int dec_vmax = (turn + v - 1) / 2;
    int max_x = (1 + vmax) * vmax / 2 + (v + dec_vmax) * (dec_vmax - v + 1) / 2;
    return max_x;
}
int max_x_from_v1(int v1, int v2, int turn){
    return v1 * turn + max_x(v2 - v1, turn);
}
int min_x(int v, int turn){
    return -max_x(v+2, turn) + (v + 1) * turn;
}
int min_x_from_v1(int v1, int v2, int turn){
    return v1 * turn + min_x(v2 - v1, turn);
}

int cost_1d(int dpos, int v1, int v2){

    for(int t = 0; ; t++){
        if(min_x_from_v1(v1, v2, t) <= dpos && dpos <= max_x_from_v1(v1, v2, t)){
            return t;
        }
    }
}




int cost_between(const point& dpos, const point& v1, const point& v2){

    int res = max(cost_1d(dpos.x, v1.x, v2.x), cost_1d(dpos.y, v1.y, v2.y));
    // std::cerr << "cost_between " << dpos.x << " " << dpos.y << " " << v1.x << " " << v1.y << " " << v2.x << " " << v2.y << " " << res << std::endl;
    return res;
}


std::array<point, 19> pos = {{
    {-8,  7},
    {-10,  4},
    {-6 , -1},
    {-5 , -8},
    {4,  10},
    {0 , -1},
    {-8 , -1},
    {-7 , -10},
    {2,  8},
    {10,  1},
    {5,  4},
    {-8 , -2},
    {-1,  5},
    {6 , -5},
    {-7 , -3},
    {-5 , -4},
    {9 , -5},
    {0,  0},
    {-9 , -4},
}};

// point pos[] = {
//     {1, -1},
//     {1, -3},
//     {2, -5},
//     {2, -8},
//     {3, -10},
// };

// point pos[] = {
//     {-1, 1},
//     {1, 8},
//     {4, 15},
//     {3, 14},
//     {3, 13},
//     {-1, 3},
//     {-1, 0},
//     {2, 11},
//     {0, 5},
//     {5, 16},
// };

constexpr int N = sizeof(pos) / sizeof(pos[0]);

constexpr int MAX_VEL = 4;
array<array<array<array<std::uint16_t, MAX_VEL * 2 + 1>, MAX_VEL * 2 + 1>, N>, 1 << N> dp;


array<array<array<array<array<array<std::uint16_t, MAX_VEL * 2 + 1>, MAX_VEL * 2 + 1>, N>, MAX_VEL * 2 + 1>, MAX_VEL * 2 + 1>, N> cost_between_array;
    
constexpr std::uint16_t INF = 10000;
int main(){
    // vector<point> pos;
    // while(1){
    //     point p;
    //     cin >> p.x >> p.y;
    //     if(cin.eof()) break;
    //     pos.push_back(p);
    // }
    
    for(int i=0; i<N; i++){
        for(int vy=0; vy<MAX_VEL * 2 + 1; vy++){
            for(int vx=0; vx<MAX_VEL * 2 + 1; vx++){
                for(int j= 0; j<N; j++){
                    for(int vky=0; vky<MAX_VEL * 2 + 1; vky++){
                        for(int vkx=0; vkx<MAX_VEL * 2 + 1; vkx++){
                            cost_between_array[i][vy][vx][j][vky][vkx] = cost_between({pos[j].x - pos[i].x, pos[j].y - pos[i].y}, {vx - MAX_VEL, vy - MAX_VEL}, {vkx - MAX_VEL, vky - MAX_VEL});
                            // if(cost_between_array[i][vy][vx][j][vky][vkx] <= 1){
                            //     std::cerr << "cost_between_array " << i << " " << vy << " " << vx << " " << j << " " << vky << " " << vkx << "  = " << cost_between_array[i][vy][vx][j][vky][vkx] << std::endl;
                            // }
                        }
                    }
                }
            }
        }
    }

    std::cerr << "init1 done" << std::endl;

    for (int j = 0; j < N; j++){
        for (int vy = 0; vy < MAX_VEL * 2 + 1; vy++){
            for (int vx = 0; vx < MAX_VEL * 2 + 1; vx++){
                // std::cerr << "j, vy, vx " << j << " " << vy << " " << vx << std::endl;
                // std::cerr << dp.size() << " " << dp[0].size() << " " << dp[0][0].size() << " " << dp[0][0][0].size() << std::endl;
                dp[0][j][vy][vx] = INF;
                dp[1 << j][j][vy][vx] = cost_between(pos[j], {0, 0}, {vx - MAX_VEL, vy - MAX_VEL});
                // if(dp[1 << j][j][vy][vx] <= 1){
                //     std::cerr << "dp[1 << j][j][vy][vx] " << j << " " << vy << " " << vx << " " << dp[1 << j][j][vy][vx] << std::endl;

                // }
            }
        }
    }

    std::cerr << "init done" << std::endl;

    for (int S = 0; S < (1 << N); S++){
        // if ((S & ((1 << 16) - 1)) == 0){
        if(S % 1024 == 0){
            std::cerr << S << " / " << (1 << N) << std::endl;
        }
        // }
        for (int j = 0; j < N; j++){
            if(!(S & (1 << j))) continue;
            if(S == (1 << j)) continue;
            for (int vy = 0; vy < MAX_VEL * 2 + 1; vy++){
                for (int vx = 0; vx < MAX_VEL * 2 + 1; vx++){
                    std::uint16_t min_cost = INF;
                    if(!(S & (1 << j))) continue;
                    // std::cerr << S << " " << j << " " << vy << " " << vx << std::endl;
                    for (int k = 0; k < N; k++){
                        if(!(S & (1 << k))) continue;
                        if(j == k) continue;
                        for (int vky = 0; vky < MAX_VEL * 2 + 1; vky++){
                            for (int vkx = 0; vkx < MAX_VEL * 2 + 1; vkx++){
                                min_cost = min(min_cost, std::uint16_t(dp[S ^ (1 << j)][k][vky][vkx] + cost_between_array[k][vky][vkx][j][vy][vx]));
                            }
                        }
                    }
                    
                    dp[S][j][vy][vx] = min_cost;
                }
            }
        }
    }

    std::uint16_t ans = INF;
    for (int j = 0; j < N; j++){
        for (int vy = 0; vy < MAX_VEL * 2 + 1; vy++){
            for (int vx = 0; vx < MAX_VEL * 2 + 1; vx++){
                ans = min(ans, dp[(1 << N) - 1][j][vy][vx]);
            }
        }
    }
    cout << ans << endl;

}