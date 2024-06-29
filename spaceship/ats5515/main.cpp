#include <assert.h>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#define dump(x) cerr << #x << " = " << (x) << endl;
#define debug(x) cerr << #x << " = " << (x) << " (L" << __LINE__ << ")" << " " << __FILE__ << endl;

using namespace std;

const int64_t CYCLES_PER_SEC = 2800000000;
const double TIMELIMIT = 2.95;
struct Timer {
	int64_t start;
	Timer() { reset(); }
	void reset() { start = getCycle(); }
	void plus(double a) { start -= (a * CYCLES_PER_SEC); }
	inline double get() { return (double)(getCycle() - start) / CYCLES_PER_SEC; }
	inline int64_t getCycle() {
		uint32_t low, high;
		__asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
		return ((int64_t)low) | ((int64_t)high << 32);
	}
};
class XorShift {
   public:
	unsigned int x, y, z, w;
	double nL[65536];

	XorShift() {
		init();
	}

	void init() {
		x = 314159265;
		y = 358979323;
		z = 846264338;
		w = 327950288;
		double n = 1 / (double)(2 * 65536);
		for (int i = 0; i < 65536; i++) {
			nL[i] = log(((double)i / 65536) + n);
		}
	}

	inline unsigned int next() {
		unsigned int t = x ^ x << 11;
		x = y;
		y = z;
		z = w;
		return w = w ^ w >> 19 ^ t ^ t >> 8;
	}

	inline double nextLog() {
		return nL[next() & 0xFFFF];
	}

	inline int nextInt(int m) {
		return (int)(next() % m);
	}

	int nextInt(int min, int max) {
		return min + nextInt(max - min + 1);
	}

	inline double nextDouble() {
		return (double)next() / ((long long)1 << 32);
	}
};
XorShift rnd;

struct FastSet {
	vector<int> list;
	vector<int> pos;
	void init(int N) {
		pos.resize(N, -1);
		list.reserve(N);
	}
	void insert_all() {
		list.clear();
		list.resize(pos.size());
		for (int i = 0; i < list.size(); i++) {
			pos[i] = list[i] = i;
		}
	}
	void insert(int a) {
		if (pos[a] == -1) {
			pos[a] = list.size();
			list.push_back(a);
		}
	}
	void erase(int a) {
		if (pos[a] >= 0) {
			swap(list[pos[a]], list.back());
			pos[list[pos[a]]] = pos[a];
			pos[a] = -1;
			list.pop_back();
		}
	}
	void flip(int a) {
		if (pos[a] == -1) {
			pos[a] = list.size();
			list.push_back(a);
		} else {
			swap(list[pos[a]], list.back());
			pos[list[pos[a]]] = pos[a];
			pos[a] = -1;
			list.pop_back();
		}
	}
	inline int size() {
		return list.size();
	}
	inline int rand() {
		return list[rnd.nextInt(list.size())];
	}
	inline void erase_all() {
		for (int i : list) {
			pos[i] = -1;
		}
		list.clear();
	}
};
struct FastSet2 {
	vector<int> list;
	vector<int> pos;
	int sz;
	void init(int N) {
		sz = 0;
		pos.resize(N);
		list.resize(N);
		for (int i = 0; i < N; i++) {
			pos[i] = list[i] = i;
		}
	}
	void insert_all() {
		sz = list.size();
	}
	int getAny() {
		return list[sz++];
	}
	void insert(int a) {
		if (pos[a] >= sz) {
			pos[list[sz]] = pos[a];
			swap(list[pos[a]], list[sz]);
			pos[a] = sz;
			sz++;
		}
	}
	void erase(int a) {
		if (pos[a] < sz) {
			sz--;
			pos[list[sz]] = pos[a];
			swap(list[pos[a]], list[sz]);
			pos[a] = sz;
		}
	}
	inline int size() {
		return sz;
	}
	inline int rand() {
		return list[rnd.nextInt(sz)];
	}
	inline void erase_all() {
		sz = 0;
	}
};

vector<int> destX;
vector<int> destY;
int testcase_index = -1;
struct Solver {
	Timer timer;

	void init() {
	}

	void SA() {
	}
	bool can_be_at_1d(int dx, int vx, int turn) {
		int tri_number = (turn * (turn + 1) / 2);
		int base = vx * turn;
		return (base - tri_number <= dx) && (base + tri_number >= dx);
	}
	int min_turns(int dx, int dy, int vx, int vy) {
		int turn = 0;
		while (true) {
			if (can_be_at_1d(dx, vx, turn) && can_be_at_1d(dy, vy, turn)) {
				break;
			}
			turn++;
		}
		return turn;
	}

	int generate_moves_1d(int dx, int vx, int turn, vector<int> &res) {
		int diff = vx * turn - dx;
		int dir;
		if (diff > 0) {
			dir = -1;
		} else {
			dir = 1;
			diff = abs(diff);
		}
		for (int t = 0; t < turn; t++) {
			int turn_remains = turn - t;
			if (diff >= turn_remains) {
				diff -= turn_remains;
				res.push_back(dir);
				vx += dir;
			} else {
				res.push_back(0);
			}
		}
		return vx;
	}

	void generate_moves_1d_with_final_velocity(int dx, int vx1, int vx2, int turn, vector<int> &res) {
		int curx = 0;
		int curvx = vx1;
		for (int t = 0; t < turn; t++) {
			int turn_remains = turn - t;
			if (vx2 - curvx == turn_remains) {
				res.push_back(1);
			} else if (vx2 - curvx == -turn_remains) {
				res.push_back(-1);
			} else {
				int tt = turn_remains - 1;
				int yy = vx2 - curvx;
				int mxdx = curx + curvx * turn_remains;
				if ((yy + tt) % 2 == 0) {
					int tmp = (yy + tt) / 2;
					mxdx += tmp * tmp - (yy * (yy - 1) / 2);

				} else {
					int tmp = (yy + tt - 1) / 2;
					mxdx += tmp * (tmp + 1) - (yy * (yy - 1) / 2);
				}

				if (dx > mxdx) {
					res.push_back(1);
				} else {
					int mndx = curx + curvx * turn_remains;
					if ((yy + tt) % 2 == 0) {
						int tmp = (-yy + tt) / 2;
						mndx -= tmp * tmp - (yy * (yy + 1) / 2);

					} else {
						int tmp = (-yy + tt - 1) / 2;
						mndx -= tmp * (tmp + 1) - (yy * (yy + 1) / 2);
					}
					if (dx < mndx) {
						res.push_back(-1);
					} else {
						res.push_back(0);
					}
				}
			}

			curvx += res.back();
			curx += curvx;
		}
		assert(curvx == vx2);
		assert(curx == dx);
	}

	bool is_possible_final_velocity_stupid(int dx, int vx1, int vx2, int turn) {
		// O(turn)かけて判定
		if (abs(vx1 - vx2) > turn) return false;
		int mxdx = 0;
		int mxv = vx1;
		int mndx = 0;
		int mnv = vx1;
		for (int i = 0; i < turn; i++) {
			int turn_remained = turn - i;
			for (int j = 1; j >= -1; j--) {
				if (mxv + j - (turn_remained - 1) <= vx2) {
					mxv += j;
					break;
				}
			}
			mxdx += mxv;

			for (int j = -1; j <= 1; j++) {
				if (mnv + j + (turn_remained - 1) >= vx2) {
					mnv += j;
					break;
				}
			}
			mndx += mnv;
		}
		assert(mxv == vx2);
		assert(mnv == vx2);
		return mndx <= dx && dx <= mxdx;
	}

	void final_velocity_range(int dx, int vx, int turn, int &mnx, int &mxx) {
		int area = dx - vx * turn;
		double turn_half = turn + 0.5;

		double mnx_d = 1 + turn - sqrt(max(0.0, 0.5 - 4 * area + 2 * turn + 2 * turn * turn));
		int lower = floor(mnx_d) - 2;
		int upper = lower + 5;
		for (int i = lower; i <= upper; i++) {
			if (abs(i) > turn) continue;

			int a;
			if ((i + turn) % 2 == 0) {
				int tmp = (i + turn) / 2;
				a = tmp * tmp - (i * (i - 1) / 2);

			} else {
				int tmp = (i + turn - 1) / 2;
				a = tmp * (tmp + 1) - (i * (i - 1) / 2);
			}
			if (a >= area) {
				mnx = vx + i;
				break;
			}
		}

		double mxx_d = -(1 + turn - sqrt(max(0.0, 0.5 + 4 * area + 2 * turn + 2 * turn * turn)));

		lower = floor(mxx_d) - 2;
		upper = lower + 5;
		for (int i = upper; i >= lower; i--) {
			if (abs(i) > turn) continue;
			int a;
			if ((i + turn) % 2 == 0) {
				int tmp = (-i + turn) / 2;
				a = -tmp * tmp + (-i * (-i - 1) / 2);

			} else {
				int tmp = (-i + turn - 1) / 2;
				a = -tmp * (tmp + 1) + (-i * (-i - 1) / 2);
			}
			if (a <= area) {
				mxx = vx + i;
				break;
			}
		}
	}

	double distance(int x1, int y1, int x2, int y2) {
		long long dx = x1 - x2;
		long long dy = y1 - y2;
		return sqrt(dx * dx + dy * dy);
	}
	void TSP(vector<int> &order) {
		int N = destX.size();
		double score = 0;
		double bestscore = 0;
		double st = timer.get();
		double timelimit = st + 10;
		double tcoef = 1.0 / (timelimit - st);
		int cnt = 0;
		double log_interval_time = (timelimit - st) / 10;
		double next_log_timing = st;
		bool updated = false;
		double score_coef = 0;
		double max_score_coef = 0.5;
		double r;
		double T;
		double T0 = 100000;
		if (testcase_index == 19) {
			T0 = 1000;
		} else if (testcase_index == 18) {
			T0 = 300000;
		} else if (testcase_index == 17) {
			T0 = 500;
		} else if (testcase_index == 16) {
			T0 = 60;
		} else if (testcase_index == 15) {
			T0 = 10;
		}
		while (true) {
			double ti = timer.get();
			if (ti > timelimit) break;

			T = (timelimit - ti) * tcoef;
			T = T0 * T;
			if (ti > next_log_timing) {
				next_log_timing += log_interval_time;
				cerr << fixed << setprecision(5) << T << " " << score << " " << bestscore << endl;
			}

			int a = rnd.nextInt(N);
			int b = rnd.nextInt(N - 1);
			if (a <= b) b++;
			if (a > b) {
				swap(a, b);
			}
			// reverse a~b

			double scorediff = 0;
			if (a == 0) {
				scorediff -= distance(destX[order[a]], destY[order[a]], 0, 0);
				scorediff += distance(destX[order[b]], destY[order[b]], 0, 0);
			} else {
				scorediff -= distance(destX[order[a]], destY[order[a]], destX[order[a - 1]], destY[order[a - 1]]);
				scorediff += distance(destX[order[b]], destY[order[b]], destX[order[a - 1]], destY[order[a - 1]]);
			}

			if (b < N - 1) {
				scorediff -= distance(destX[order[b]], destY[order[b]], destX[order[b + 1]], destY[order[b + 1]]);
				scorediff += distance(destX[order[a]], destY[order[a]], destX[order[b + 1]], destY[order[b + 1]]);
			}
			if (scorediff <= -T * rnd.nextLog()) {
				score += scorediff;
				if (bestscore > score) {
					bestscore = score;
				}
				reverse(order.begin() + a, order.begin() + (b + 1));
			}
		}
	}

	struct BeamState {
		int vx;
		int vy;
		int turns;
		int from;
		double eval;
		BeamState() {}
		BeamState(int _vx, int _vy, int _turns, int _from) {
			vx = _vx;
			vy = _vy;
			turns = _turns;
			from = _from;
		}

		void do_eval() {
			eval = turns + rnd.nextDouble() * 0.1;
		}

		bool operator<(const BeamState &right) const {
			return eval < right.eval;
		}
	};

	vector<map<long long, int> > dp;
	void add_beam(vector<BeamState> &beam, int i, int vx, int vy, int turn, int from) {
		long long hash = vx * (long long)1e9 + vy;
		auto a = dp[i].find(hash);
		if (a == dp[i].end() || (*a).second > turn) {
			dp[i][hash] = turn;
			beam.emplace_back(vx, vy, turn, from);
			beam.back().do_eval();
		}
	}
	int solve() {
		dp.resize(destX.size() + 1);
		// 答えを格納する変数
		// それぞれx軸、y軸の加速度(-1, 0, 1のいずれか)の列が入る
		vector<int> resX;
		vector<int> resY;

		// 訪問する点の順番を決める
		vector<int> visited(destX.size(), 0);
		vector<int> order(destX.size());
		int x = 0;
		int y = 0;
		int vx = 0;
		int vy = 0;
		for (int i = 0; i < destX.size(); i++) {
			long long mnscore = 1e18;
			int mnj = -1;
			for (int j = 0; j < destX.size(); j++) {
				if (!visited[j]) {
					int dx = destX[j] - x;
					int dy = destY[j] - y;
					long long score = 0;
					// score = min_turns(dx, dy, vx, vy);
					score = (long long)dx * dx + (long long)dy * dy;
					if (mnscore > score) {
						mnscore = score;
						mnj = j;
					}
				}
			}
			order[i] = mnj;
			visited[mnj] = 1;
			x = destX[mnj];
			y = destY[mnj];
		}

		TSP(order);

		// 定められた順序で訪問していく
		// 現在の速度と目標点との相対座標が求まればその目標点に行くまでの最小ターン数は簡単に計算できる
		// 最小ターンで目標点に行く操作列のうち、与える加速度を与える回数が最も小さいものを選ぶ
		x = 0;
		y = 0;
		vx = 0;
		vy = 0;

		int mnx, mxx;
		int mny, mxy;

		int width = 1000;

		vector<vector<BeamState> > beams(destX.size() + 1);
		beams[0].emplace_back(0, 0, 0, -1);

		for (int i = 0; i < destX.size(); i++) {
			if (i % 1000 == 0) {
				cerr << i << endl;
			}
			int pos_index = order[i];
			int dx = destX[pos_index] - x;
			int dy = destY[pos_index] - y;
			for (int j = 0; j < beams[i].size(); j++) {
				vx = beams[i][j].vx;
				vy = beams[i][j].vy;
				int turns_so_far = beams[i][j].turns;
				int turn = min_turns(dx, dy, vx, vy);
				// if (i == 0) {
				// 	cerr << i << " " << j << " " << vx << " " << vy << " " << turns_so_far << " " << turn << endl;
				// }
				for (int k = 0; k < 3; k++) {
					int tt = turn + k;
					if (k == 0 || (can_be_at_1d(dx, vx, tt) && can_be_at_1d(dy, vy, tt))) {
						final_velocity_range(dx, vx, tt, mnx, mxx);
						final_velocity_range(dy, vy, tt, mny, mxy);

						for (int xx = mnx; xx <= mxx; xx++) {
							for (int yy = mny; yy <= mxy; yy++) {
								add_beam(beams[i + 1], i, xx, yy, turns_so_far + tt, j);
							}
						}
					}
				}
			}
			if (beams[i + 1].size() > width) {
				nth_element(beams[i + 1].begin(), beams[i + 1].begin() + width, beams[i + 1].end());
				beams[i + 1].resize(width);
			}

			x = destX[pos_index];
			y = destY[pos_index];
		}
		auto best = *min_element(beams.back().begin(), beams.back().end());
		vector<BeamState> bests;
		bests.push_back(best);
		for (int i = destX.size() - 1; i >= 0; i--) {
			best = beams[i][best.from];
			bests.push_back(best);
		}

		reverse(bests.begin(), bests.end());

		x = 0;
		y = 0;
		for (int i = 0; i < destX.size(); i++) {
			int pos_index = order[i];
			int dx = destX[pos_index] - x;
			int dy = destY[pos_index] - y;
			int vx1 = bests[i].vx;
			int vy1 = bests[i].vy;
			int vx2 = bests[i + 1].vx;
			int vy2 = bests[i + 1].vy;
			int turn = bests[i + 1].turns - bests[i].turns;
			// int turn = min_turns(dx, dy, vx, vy);
			// cerr << dx << " " << vx1 << " " << vx2 << " " << turn << endl;
			generate_moves_1d_with_final_velocity(dx, vx1, vx2, turn, resX);
			generate_moves_1d_with_final_velocity(dy, vy1, vy2, turn, resY);

			x = destX[pos_index];
			y = destY[pos_index];
		}

		cerr << "score = " << resX.size() << endl;

		output(resX, resY);
		return 0;
	}

	vector<string> split_naive(const string &s, char delim) {
		vector<string> elems;
		string item;
		for (char ch : s) {
			if (ch == delim) {
				if (!item.empty())
					elems.push_back(item);
				item.clear();
			} else {
				item += ch;
			}
		}
		if (!item.empty())
			elems.push_back(item);
		return elems;
	}
	void input() {
		cin.tie(0);
		ios::sync_with_stdio(false);
		string str;
		set<pair<int, int> > st;
		int line = 0;
		pair<int, int> line4;
		while (true) {
			getline(cin, str);
			if (str == "") break;
			auto strs = split_naive(str, ' ');
			// intに抑えられる前提
			int x = stoi(strs[0]);
			int y = stoi(strs[1]);
			if (st.find({x, y}) == st.end()) {
				destX.push_back(x);
				destY.push_back(y);
				st.emplace(x, y);
			}
			if (line == 4) {
				line4 = {x, y};
			}
			line++;
		}
		dump(destX.size());
		map<pair<int, int>, int> destxy4_to_testcase_index;
		destxy4_to_testcase_index[pair<int, int>(3, -10)] = 1;
		destxy4_to_testcase_index[pair<int, int>(0, 7)] = 2;
		destxy4_to_testcase_index[pair<int, int>(3, 13)] = 3;
		destxy4_to_testcase_index[pair<int, int>(-199, -171)] = 4;
		destxy4_to_testcase_index[pair<int, int>(-9, 6)] = 5;
		destxy4_to_testcase_index[pair<int, int>(1, 38)] = 6;
		destxy4_to_testcase_index[pair<int, int>(-10, -11)] = 7;
		destxy4_to_testcase_index[pair<int, int>(2, -8)] = 8;
		destxy4_to_testcase_index[pair<int, int>(35, 44)] = 9;
		destxy4_to_testcase_index[pair<int, int>(-9, -38)] = 10;
		destxy4_to_testcase_index[pair<int, int>(-10, -8)] = 11;
		destxy4_to_testcase_index[pair<int, int>(-91534, -6662)] = 12;
		destxy4_to_testcase_index[pair<int, int>(52346, 84891)] = 13;
		destxy4_to_testcase_index[pair<int, int>(873, 720)] = 14;
		destxy4_to_testcase_index[pair<int, int>(4, 10)] = 15;
		destxy4_to_testcase_index[pair<int, int>(-100, -25)] = 16;
		destxy4_to_testcase_index[pair<int, int>(-61, 76)] = 17;
		destxy4_to_testcase_index[pair<int, int>(-1733, -958)] = 18;
		destxy4_to_testcase_index[pair<int, int>(-15, -6)] = 19;
		destxy4_to_testcase_index[pair<int, int>(714, 825)] = 20;
		destxy4_to_testcase_index[pair<int, int>(431, 536)] = 21;
		destxy4_to_testcase_index[pair<int, int>(256, 503)] = 22;
		destxy4_to_testcase_index[pair<int, int>(-6419, -198134)] = 23;
		destxy4_to_testcase_index[pair<int, int>(-322399, 203475)] = 24;
		destxy4_to_testcase_index[pair<int, int>(-10000, 2111)] = 25;

		testcase_index = destxy4_to_testcase_index[line4];
		dump(testcase_index)
	}

	void output(const vector<int> &resX, const vector<int> &resY) {
		for (int i = 0; i < resX.size(); i++) {
			cout << (resY[i] + 1) * 3 + resX[i] + 1 + 1;
		}
		cout << endl;
	}
};

Solver solver;
int main(int argc, char *argv[]) {
	solver.input();
	solver.solve();
	cerr << "timer = " << solver.timer.get() << endl;
	return 0;
}