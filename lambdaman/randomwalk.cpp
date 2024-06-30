//
// Usage:
// ./randomwalk problem_id [stride]
// stride: 2 for tree-like (4, 11-15), 1 for others (default)
//

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <set>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

using i64 = long long;

char board[310][310];
int H, W, sy, sx;

const int DY[] = {0, 1, 0, -1};
const int DX[] = {1, 0, -1, 0};
const char DIRNAME[] = "L>FO";  //"RDLU";

constexpr int MAX_THREADS = 32;
bool visited[MAX_THREADS][310][310];

std::string to_base94(i64 x) {
    std::string ret;
    for (; x > 0; x /= 94) {
        ret.push_back(x % 94 + 33);
    }
    ret.push_back('I');
    std::reverse(ret.begin(), ret.end());
    return ret;
}

struct Rng {
    i64 s, k, prime;

    Rng(i64 s, i64 k, i64 prime) : s(s), k(k), prime(prime) {}

    i64 next() {
        i64 ret = s % 4;
        s = (s * k) % prime;
        return ret;
    }

    bool exhausted() {
        return s == 1;
    }
};

std::pair<std::vector<int>, std::vector<i64>> make_random_seq(i64 s, i64 k, i64 prime) {
    Rng rng(s, k, prime);

    std::vector<int> ret;
    std::vector<i64> seeds;
    for (;;) {
        if (rng.exhausted()) {
            break;
        }
        seeds.push_back(rng.s);
        ret.push_back(rng.next());
    }
    std::reverse(ret.begin(), ret.end());
    std::reverse(seeds.begin(), seeds.end());

    return {ret, seeds};
}

std::pair<bool, int> run_randomwalk(i64 s, i64 k, i64 prime, int steps, int stride, int tid) {
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            visited[tid][y][x] = false;
        }
    }

    int y = sy, x = sx;
    visited[tid][y][x] = true;

    std::vector<int> dirs = make_random_seq(s, k, prime).first;
    if (dirs.size() != steps) {
        fprintf(stderr, "steps mismatch (%d vs %d)\n", (int)dirs.size(), steps);
        throw 42;
    }

    for (int dir : dirs) {
        for (int d = 1; d <= stride; ++d) {
            int y2 = y + DY[dir];
            int x2 = x + DX[dir];
            if (0 <= y2 && y2 < H && 0 <= x2 && x2 < W && board[y2][x2] != '#') {
                y = y2;
                x = x2;
                visited[tid][y][x] = true;
            } else {
                break;
            }
        }
    }

    int n_visited = 0;
    int n_cells = 0;
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            if (visited[tid][y][x]) {
                ++n_visited;
            }
            if (board[y][x] != '#') {
                ++n_cells;
            }
        }
    }

    return {n_visited == n_cells, n_visited};
}

i64 first_visit_time[MAX_THREADS][310][310];
int first_visit_time_step[MAX_THREADS][310][310];

void aux_dfs(int tid, int y, int x, std::string& buf) {
    for (int d = 0; d < 4; ++d) {
        int y2 = y + DY[d];
        int x2 = x + DX[d];
        if (0 <= y2 && y2 < H && 0 <= x2 && x2 < W && board[y2][x2] != '#' && first_visit_time[tid][y2][x2] == -1) {
            first_visit_time[tid][y2][x2] = -2;
            buf.push_back(DIRNAME[d]);
            aux_dfs(tid, y2, x2, buf);
            buf.push_back(DIRNAME[d ^ 2]);
        }
    }
}

std::optional<std::string> rectify_randomwalk(i64 s, i64 k, i64 prime, int stride, int tid) {
    auto [dirs, seeds] = make_random_seq(s, k, prime);

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            first_visit_time[tid][y][x] = -1;
        }
    }
    int y = sy, x = sx;
    first_visit_time[tid][y][x] = 0;
    for (int i = 0; i < (int)dirs.size(); ++i) {
        int dir = dirs[i];
        int y2 = y + DY[dir];
        int x2 = x + DX[dir];

        for (int d = 1; d <= stride; ++d) {
            int y2 = y + DY[dir];
            int x2 = x + DX[dir];
            if (0 <= y2 && y2 < H && 0 <= x2 && x2 < W && board[y2][x2] != '#') {
                y = y2;
                x = x2;
                if (first_visit_time[tid][y][x] < 0) {
                    first_visit_time[tid][y][x] = i;
                    first_visit_time_step[tid][y][x] = d;
                }
            } else {
                break;
            }
        }
    }

    std::map<i64, std::vector<std::pair<int, std::string>>> insertion_map;

    std::ostringstream oss;
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            if (board[y][x] == '#' || first_visit_time[tid][y][x] < 0 || first_visit_time[tid][y][x] == dirs.size() - 1) {
                continue;
            }

            bool has_bad_neighbor = false;
            for (int d = 0; d < 4; ++d) {
                int y2 = y + DY[d];
                int x2 = x + DX[d];
                if (0 <= y2 && y2 < H && 0 <= x2 && x2 < W && board[y2][x2] != '#' && first_visit_time[tid][y2][x2] == -1) {
                    has_bad_neighbor = true;
                    break;
                }
            }

            if (!has_bad_neighbor) {
                continue;
            }

            std::string buf;
            aux_dfs(tid, y, x, buf);

            insertion_map[first_visit_time[tid][y][x]].emplace_back(first_visit_time_step[tid][y][x], buf);
            // oss << "? B= vs " << to_base94(seeds[first_visit_time[y][x]]) << " S" << buf << " ";
        }
    }
    //if (insertion_map.size() >= 6) {
    //    fprintf(stderr, "insertion map too large: %d\n", (int)insertion_map.size());
    //    return std::nullopt;
    //}
    for (auto [t, vec] : insertion_map) {
        std::sort(vec.begin(), vec.end());
        oss << "? B= vs " << to_base94(seeds[t]) << " S";
        std::vector<std::string> timings(stride);
        for (auto [t, v] : vec) {
            timings[t - 1] = v;
        }
        for (int i = 0; i < stride; ++i) {
            oss << DIRNAME[dirs[t]] << timings[i];
        }
        oss << " ";
    }

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            if (board[y][x] != '#' && first_visit_time[tid][y][x] == -1) {
                return std::nullopt;
            }
        }
    }

    return oss.str();
}

i64 modpow(i64 a, i64 b, i64 mod) {
    i64 ret = 1;
    for (; b > 0; b >>= 1) {
        if (b & 1) {
            ret = (ret * a) % mod;
        }
        a = (a * a) % mod;
    }
    return ret;
}

i64 modinv(i64 a, i64 mod) {
    return modpow(a, mod - 2, mod);
}

i64 miller_rabin(i64 n, i64 a) {
    if (n % a == 0) {
        return false;
    }
    i64 d = n - 1;
    int s = 0;

    while (d % 2 == 0) {
        d /= 2;
        ++s;
    }

    i64 m = modpow(a, d, n);
    if (m == 1) {
        return true;
    }
    for (int i = 0; i <= s; ++i) {
        if (m == n - 1) {
            return true;
        }
        m = (m * m) % n;
    }
    return false;
}

bool is_prime(i64 n) {
    if (n == 2 || n == 7 || n == 61) return true;
    return miller_rabin(n, 2) && miller_rabin(n, 7) && miller_rabin(n, 61);
}

i64 is_primitive_root(i64 k, i64 p, std::vector<i64>& testers) {
    for (i64 a : testers) {
        if (modpow(k, a, p) == 1) {
            return false;
        }
    }
    return true;
}

std::vector<i64> get_testers(i64 p) {
    std::vector<i64> ret;
    for (i64 i = 1; i * i <= p - 1; ++i) {
        if ((p - 1) % i == 0) {
            ret.push_back(i);
            if (i != 1 && i * i != p - 1) {
                ret.push_back((p - 1) / i);
            }
        }
    }
    return ret;
}

std::pair<i64, std::vector<i64>> gen_prime_and_primitive_roots(std::mt19937& rng, int n_roots, bool eco_mode) {
    i64 p;

    i64 prime_lo, prime_hi;
    if (eco_mode) {
        prime_lo = 730583;
        prime_hi = 830583;
    } else {
        prime_lo = 1e9;
        prime_hi = 1e9 + 1e8;
    }
    while (true) {
        p = std::uniform_int_distribution<i64>(prime_lo, prime_hi)(rng);
        if (is_prime(p)) {
            break;
        }
    }

    std::vector<i64> testers = get_testers(p);
    std::set<i64> roots;

    if (eco_mode) {
        for (i64 k = 2; k < 94; ++k) {
            if (is_primitive_root(k, p, testers)) {
                roots.insert(k);
            }
        }
    } else {
        while (roots.size() < n_roots) {
            i64 k = std::uniform_int_distribution<i64>(2, p - 1)(rng);
            if (is_primitive_root(k, p, testers)) {
                roots.insert(k);
            }
        }
    }

    return {p, std::vector<i64>(roots.begin(), roots.end())};
}

i64 decide_s(i64 p, i64 k, int steps) {
    return modpow(modinv(k, p), steps, p);
}

std::string problem_id_to_string(int n) {
    std::string ret;
    for (; n > 0; n /= 10) {
        ret.push_back(n % 10 + 'U');
    }
    std::reverse(ret.begin(), ret.end());
    return ret;
}

int main(int argc, char** argv) {
    if (argc < 6) {
        fprintf(stderr, "Usage: %s problem_id stride eco_mode num_threads seed\n", argv[0]);
        return 1;
    }

    H = 0;
    for (;;) {
        if (scanf("%s", board[H]) != 1) break;
        if (strlen(board[H]) == 0) break;
        ++H;
    }
    W = strlen(board[0]);

    int problem_id = atoi(argv[1]);
    int stride;
    if (argc >= 3) {
        stride = atoi(argv[2]);
    } else {
        stride = 1;
    }
    if (stride != 1 && stride != 2) {
        fprintf(stderr, "stride must be 1 or 2\n");
        return 1;
    }
    bool eco_mode = false;
    if (argc >= 4) {
        eco_mode = atoi(argv[3]);
    }

    int n_sp = 0;
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            if (board[y][x] == 'L') {
                sy = y;
                sx = x;
            }
            if (board[y][x] != '#') {
                ++n_sp;
            }
        }
    }
    fprintf(stderr, "H: %d, W: %d, start: (%d, %d)\n", H, W, sy, sx);

    int steps = (eco_mode ? 500000 : 999000) / stride;
    int sota = 0;
    int last_rectify_len = 1000;
    std::mutex mtx;

    std::vector<std::thread> threads;
    int num_threads = atoi(argv[4]);
    if (num_threads > MAX_THREADS) {
        fprintf(stderr, "too many threads\n");
        return 1;
    }
    i64 seed_base = atoll(argv[5]);

    for (int i = 0; i < num_threads; ++i) {
        threads.push_back(std::thread([&, i] {
            std::mt19937 rng(seed_base * 123456789 + i);
            for (;;) {
                auto [p, roots] = gen_prime_and_primitive_roots(rng, 100, eco_mode);
                for (i64 k : roots) {
                    if (sota == n_sp) {
                        return;
                    }
                    i64 s = decide_s(p, k, steps);
                    auto [isok, n_visited] = run_randomwalk(s, k, p, steps, stride, i);

                    mtx.lock();
                    if (sota < n_visited) {
                        sota = n_visited;
                        fprintf(stderr, "current best: %d / %d\n", n_visited, n_sp);
                    }
                    mtx.unlock();

                    if (n_sp - n_visited <= 100) {
                        std::optional<std::string> rectified = rectify_randomwalk(s, k, p, stride, i);
                        if (rectified && rectified->size() < last_rectify_len) {
                            mtx.lock();
                            last_rectify_len = rectified->size();
                            fprintf(stderr, "p: %s (%lld)\n", to_base94(p).c_str(), p);
                            fprintf(stderr, "k: %s (%lld)\n", to_base94(k).c_str(), k);
                            fprintf(stderr, "s: %s (%lld)\n", to_base94(s).c_str(), s);
                            if (stride == 1) {
                                printf("B. S3/,6%%},!-\"$!-!.%s} B$ B$ Lf B$ vf vf Lf Ls ? B= vs I\" S B. B$ B$ vf vf B%% B* vs %s %s %sBT I\" BD B%% vs I%% SL>FO %s\n",
                                    problem_id_to_string(problem_id).c_str(), to_base94(k).c_str(), to_base94(p).c_str(), rectified->c_str(), to_base94(s).c_str()
                                );
                            } else {
                                printf("B. S3/,6%%},!-\"$!-!.%s} B$ B$ Lf B$ vf vf Lf Ls ? B= vs I\" S B. B$ B$ vf vf B%% B* vs %s %s %sBT I# BD B* I# B%% vs I%% SLL>>FFOO %s\n",
                                    problem_id_to_string(problem_id).c_str(), to_base94(k).c_str(), to_base94(p).c_str(), rectified->c_str(), to_base94(s).c_str()
                                );
                            }
                            mtx.unlock();
                        }
                    }
                }
            }
        }));
    }

    for (auto& th : threads) {
        th.join();
    }

    fprintf(stderr, "done!\n");

    return 0;
}
