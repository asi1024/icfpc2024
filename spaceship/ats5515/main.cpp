#include <assert.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
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
vector<int> index_map;
vector<int> destX;
vector<int> destY;
int testcase_index = -1;
int N;
struct Solver {
	Timer timer;
	vector<int> load_tsp_solver_order() {
		vector<int> order;
		string testcase_index_str = to_string(testcase_index);
		while (testcase_index_str.size() < 2) {
			testcase_index_str = "0" + testcase_index_str;
		}
		string path = "spaceship/ats5515/tsp_solver_order/" + testcase_index_str + ".txt";
		ifstream ifs(path);

		int _N, _;
		ifs >> _N >> _;

		for (int i = 0; i < _N; i++) {
			int pos;
			ifs >> pos >> _ >> _;
			pos--;
			if (pos == -1) continue;
			pos = index_map[pos];
			if (pos == -1) continue;
			order.push_back(pos);
		}
		assert(destX.size() == order.size());
		return order;
	}
	vector<int> load_order() {
		vector<int> order(N);
		string testcase_index_str = to_string(testcase_index);
		while (testcase_index_str.size() < 2) {
			testcase_index_str = "0" + testcase_index_str;
		}
		string path = "spaceship/ats5515/order/" + testcase_index_str + ".txt";
		ifstream ifs(path);

		for (int i = 0; i < N; i++) {
			ifs >> order[i];
		}
		return order;
	}

	void save_order(const vector<int> &order) {
		string testcase_index_str = to_string(testcase_index);
		while (testcase_index_str.size() < 2) {
			testcase_index_str = "0" + testcase_index_str;
		}
		string path = "spaceship/ats5515/order/" + testcase_index_str + ".txt";
		ofstream ofs(path);
		for (int i = 0; i < order.size(); i++) {
			ofs << order[i] << endl;
		}
	}

	static bool can_be_at_1d(int dx, int vx, int turn) {
		int tri_number = (turn * (turn + 1) / 2);
		int base = vx * turn;
		return (base - tri_number <= dx) && (base + tri_number >= dx);
	}

	static int min_turns(int dx, int dy, int vx, int vy) {
		if (false) {
			int turn = 0;
			while (true) {
				if (can_be_at_1d(dx, vx, turn) && can_be_at_1d(dy, vy, turn)) {
					break;
				}
				turn++;
			}
			return turn;
		} else {
			// O(1)
			vector<int> right_conditions = {
				int(0.5 * (sqrt(max(0, -8 * dx + 4 * vx * vx - 4 * vx + 1)) + 2 * vx - 1)),
				int(0.5 * (sqrt(max(0, 8 * dx + 4 * vx * vx + 4 * vx + 1)) - 2 * vx - 1)),
				int(0.5 * (sqrt(max(0, -8 * dy + 4 * vy * vy - 4 * vy + 1)) + 2 * vy - 1)),
				int(0.5 * (sqrt(max(0, 8 * dy + 4 * vy * vy + 4 * vy + 1)) - 2 * vy - 1)),
			};
			set<int> to_check;
			to_check.insert(0);
			for (int i : right_conditions) {
				for (int j = i - 2; j <= i + 2; j++) {
					if (j >= 0) {
						to_check.insert(j);
					}
				}
			}
			for (auto &t : to_check) {
				if (can_be_at_1d(dx, vx, t) && can_be_at_1d(dy, vy, t)) {
					return t;
				}
			}
			assert(false);
		}
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

	inline double distance(int x1, int y1, int x2, int y2) {
		long long dx = x1 - x2;
		long long dy = y1 - y2;
		// return abs(dx) + abs(dy);
		// return abs(dx) + abs(dy);
		// return pow(dx * dx + dy * dy, 0.68);
		return sqrt(dx * dx + dy * dy);
		// return dx * dx + dy * dy;
	}

	inline double inertia_distance(int x1, int y1, int x2, int y2) {
		long long dx = x1 - x2;
		long long dy = y1 - y2;
		// return max(sqrt(abs(dx)), sqrt(abs(dy)));
		// return (-x1 * x2 - y1 * y2) * 0.1 + dx * dx + dy * dy;
		// return abs(dx) + abs(dy);
		// return pow(dx * dx + dy * dy, 0.68);
		return sqrt(dx * dx + dy * dy);
		// return dx * dx + dy * dy;
	}

	inline double inertia_score(int a1, int a2, int a3, const vector<int> &order) {
		if (a2 >= N || a3 >= N) return 0;
		int px1, px2, px3;
		int py1, py2, py3;
		if (a1 < 0) {
			px1 = 0;
			py1 = 0;
		} else {
			px1 = destX[order[a1]];
			py1 = destY[order[a1]];
		}

		if (a2 < 0) {
			px2 = 0;
			py2 = 0;
		} else {
			px2 = destX[order[a2]];
			py2 = destY[order[a2]];
		}

		px3 = destX[order[a3]];
		py3 = destY[order[a3]];

		return inertia_distance(px2 - px1, py2 - py1, px3 - px2, py3 - py2);
	}

	void order_transit(vector<int> &order, double T, int maxcnt, double coef) {
		double score = 0;
		double bestscore = 0;
		int cnt = 0;
		int total_cnt = 0;
		while (true) {
			total_cnt++;
			double ti = timer.get();
			double r = rnd.nextDouble();
			if (r < 0.8) {
				int a = rnd.nextInt(N);
				int b = rnd.nextInt(N - 1);
				if (a <= b) b++;
				if (a > b) {
					swap(a, b);
				}
				// reverse a ~ b

				double scorediffA = 0;
				if (a == 0) {
					scorediffA -= distance(destX[order[a]], destY[order[a]], 0, 0);
					scorediffA += distance(destX[order[b]], destY[order[b]], 0, 0);
				} else {
					scorediffA -= distance(destX[order[a]], destY[order[a]], destX[order[a - 1]], destY[order[a - 1]]);
					scorediffA += distance(destX[order[b]], destY[order[b]], destX[order[a - 1]], destY[order[a - 1]]);
				}

				if (b < N - 1) {
					scorediffA -= distance(destX[order[b]], destY[order[b]], destX[order[b + 1]], destY[order[b + 1]]);
					scorediffA += distance(destX[order[a]], destY[order[a]], destX[order[b + 1]], destY[order[b + 1]]);
				}
				// else{ // 原点に戻る場合
				// 	scorediff -= distance(destX[order[b]], destY[order[b]], 0, 0);
				// 	scorediff += distance(destX[order[a]], destY[order[a]], 0, 0);
				// }

				double scorediffB = 0;

				scorediffB -= inertia_score(a - 1, a, a + 1, order);
				scorediffB -= inertia_score(a - 2, a - 1, a, order);
				scorediffB += inertia_score(a - 1, b, b - 1, order);
				scorediffB += inertia_score(a - 2, a - 1, b, order);

				scorediffB -= inertia_score(b - 1, b, b + 1, order);
				scorediffB -= inertia_score(b, b + 1, b + 2, order);
				scorediffB += inertia_score(a + 1, a, b + 1, order);
				scorediffB += inertia_score(a, b + 1, b + 2, order);

				double scorediff = scorediffA * (1 - coef) + scorediffB * coef;
				if (scorediff <= -T * rnd.nextLog()) {
					score += scorediff;
					if (bestscore > score) {
						bestscore = score;
					}
					reverse(order.begin() + a, order.begin() + (b + 1));
					cnt++;
					if (cnt >= maxcnt) {
						break;
					}
				}
			} else {
				int a = rnd.nextInt(N);
				int b = rnd.nextInt(N - 1);
				if (a <= b) b += 2;
				if (abs(a - b) <= 2) continue;
				// insert element a to positin b

				double scorediffA = 0;
				if (a == 0) {
					scorediffA -= distance(destX[order[a]], destY[order[a]], 0, 0);
					scorediffA -= distance(destX[order[a]], destY[order[a]], destX[order[a + 1]], destY[order[a + 1]]);
					scorediffA += distance(destX[order[a + 1]], destY[order[a + 1]], 0, 0);
				} else if (a == N - 1) {
					scorediffA -= distance(destX[order[a]], destY[order[a]], destX[order[a - 1]], destY[order[a - 1]]);
				} else {
					scorediffA -= distance(destX[order[a]], destY[order[a]], destX[order[a - 1]], destY[order[a - 1]]);
					scorediffA -= distance(destX[order[a]], destY[order[a]], destX[order[a + 1]], destY[order[a + 1]]);
					scorediffA += distance(destX[order[a - 1]], destY[order[a - 1]], destX[order[a + 1]], destY[order[a + 1]]);
				}

				if (b == 0) {
					scorediffA += distance(destX[order[a]], destY[order[a]], 0, 0);
					scorediffA += distance(destX[order[a]], destY[order[a]], destX[order[b]], destY[order[b]]);
					scorediffA -= distance(destX[order[b]], destY[order[b]], 0, 0);
				} else if (b == N) {
					scorediffA += distance(destX[order[a]], destY[order[a]], destX[order[b - 1]], destY[order[b - 1]]);
				} else {
					scorediffA += distance(destX[order[a]], destY[order[a]], destX[order[b - 1]], destY[order[b - 1]]);
					scorediffA += distance(destX[order[a]], destY[order[a]], destX[order[b]], destY[order[b]]);
					scorediffA -= distance(destX[order[b - 1]], destY[order[b - 1]], destX[order[b]], destY[order[b]]);
				}

				double scorediffB = 0;

				scorediffB -= inertia_score(a - 1, a, a + 1, order);
				scorediffB -= inertia_score(a - 2, a - 1, a, order);
				scorediffB -= inertia_score(a, a + 1, a + 2, order);

				scorediffB += inertia_score(a - 1, a + 1, a + 2, order);
				scorediffB += inertia_score(a - 2, a - 1, a + 1, order);

				scorediffB -= inertia_score(b - 2, b - 1, b, order);
				scorediffB -= inertia_score(b - 1, b, b + 1, order);

				scorediffB += inertia_score(b - 2, b - 1, a, order);
				scorediffB += inertia_score(b - 1, a, b, order);
				scorediffB += inertia_score(a, b, b + 1, order);

				double scorediff = scorediffA * (1 - coef) + scorediffB * coef;

				if (scorediff <= -T * rnd.nextLog()) {
					score += scorediff;
					if (bestscore > score) {
						bestscore = score;
					}
					if (a < b) {
						for (int i = a; i < b - 1; i++) {
							swap(order[i], order[i + 1]);
						}
					} else {
						for (int i = a; i > b; i--) {
							swap(order[i], order[i - 1]);
						}
					}
				}
			}
		}

		dump(total_cnt);
	}

	void inertiaTSP(vector<int> &order) {
		double score = 0;

		// for (int i = 0; i < order.size(); i++) {
		// 	if(i == 0){
		// 		score += distance(0, 0, destX[order[0]], destY[order[0]]);
		// 	}else{
		// 		score += distance(destX[order[i - 1]], destY[order[i - 1]], destX[order[i]], destY[order[i]]);
		// 	}
		// }

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
		double T0 = 100;
		if (testcase_index == 25) {
			T0 = 10;
		} else if (testcase_index == 24) {
			T0 = 450000000;
			T0 = 200000;
			T0 = 1000;
		} else if (testcase_index == 23) {
			T0 = 50000000 / 10000000;
		} else if (testcase_index == 22) {
			T0 = 300 / 100 * 3;
		} else if (testcase_index == 21) {
			T0 = 300 / 60;	// / 300;
		} else if (testcase_index == 20) {
			T0 = 300 / 60;
			// T0 = 300;
			//  T0 = 500 / 20;
		} else if (testcase_index == 19) {
			T0 = 0.5;
		} else if (testcase_index == 18) {
			T0 = 300000 / 2000;
		} else if (testcase_index == 17) {
			T0 = 500 / 100;
		} else if (testcase_index == 16) {
			T0 = 60 / 20;
		} else if (testcase_index == 15) {
			T0 = 1;
		}

		double coefB = 0.7;
		while (true) {
			double ti = timer.get();
			if (ti > timelimit) break;
			// cnt++;
			// if (cnt >= 1000) break;
			T = (timelimit - ti) * tcoef;
			T = T0 * T;
			if (ti > next_log_timing) {
				next_log_timing += log_interval_time;
				cerr << fixed << setprecision(5) << T << " " << score << " " << bestscore << endl;
			}
			double r = rnd.nextDouble();
			if (r < 0.8) {
				int a = rnd.nextInt(N);
				int b = rnd.nextInt(N - 1);
				if (a <= b) b++;
				if (a > b) {
					swap(a, b);
				}
				// reverse a ~ b

				double scorediffA = 0;
				if (a == 0) {
					scorediffA -= distance(destX[order[a]], destY[order[a]], 0, 0);
					scorediffA += distance(destX[order[b]], destY[order[b]], 0, 0);
				} else {
					scorediffA -= distance(destX[order[a]], destY[order[a]], destX[order[a - 1]], destY[order[a - 1]]);
					scorediffA += distance(destX[order[b]], destY[order[b]], destX[order[a - 1]], destY[order[a - 1]]);
				}

				if (b < N - 1) {
					scorediffA -= distance(destX[order[b]], destY[order[b]], destX[order[b + 1]], destY[order[b + 1]]);
					scorediffA += distance(destX[order[a]], destY[order[a]], destX[order[b + 1]], destY[order[b + 1]]);
				}
				// else{ // 原点に戻る場合
				// 	scorediff -= distance(destX[order[b]], destY[order[b]], 0, 0);
				// 	scorediff += distance(destX[order[a]], destY[order[a]], 0, 0);
				// }

				double scorediffB = 0;

				scorediffB -= inertia_score(a - 1, a, a + 1, order);
				scorediffB -= inertia_score(a - 2, a - 1, a, order);
				scorediffB += inertia_score(a - 1, b, b - 1, order);
				scorediffB += inertia_score(a - 2, a - 1, b, order);

				scorediffB -= inertia_score(b - 1, b, b + 1, order);
				scorediffB -= inertia_score(b, b + 1, b + 2, order);
				scorediffB += inertia_score(a + 1, a, b + 1, order);
				scorediffB += inertia_score(a, b + 1, b + 2, order);

				double scorediff = scorediffA * (1 - coefB) + scorediffB * coefB;
				if (scorediff <= -T * rnd.nextLog()) {
					score += scorediff;
					if (bestscore > score) {
						bestscore = score;
					}
					reverse(order.begin() + a, order.begin() + (b + 1));
				}
			} else {
				int a = rnd.nextInt(N);
				int b = rnd.nextInt(N - 1);
				if (a <= b) b += 2;
				if (abs(a - b) <= 2) continue;
				// insert element a to positin b

				double scorediffA = 0;
				if (a == 0) {
					scorediffA -= distance(destX[order[a]], destY[order[a]], 0, 0);
					scorediffA -= distance(destX[order[a]], destY[order[a]], destX[order[a + 1]], destY[order[a + 1]]);
					scorediffA += distance(destX[order[a + 1]], destY[order[a + 1]], 0, 0);
				} else if (a == N - 1) {
					scorediffA -= distance(destX[order[a]], destY[order[a]], destX[order[a - 1]], destY[order[a - 1]]);
				} else {
					scorediffA -= distance(destX[order[a]], destY[order[a]], destX[order[a - 1]], destY[order[a - 1]]);
					scorediffA -= distance(destX[order[a]], destY[order[a]], destX[order[a + 1]], destY[order[a + 1]]);
					scorediffA += distance(destX[order[a - 1]], destY[order[a - 1]], destX[order[a + 1]], destY[order[a + 1]]);
				}

				if (b == 0) {
					scorediffA += distance(destX[order[a]], destY[order[a]], 0, 0);
					scorediffA += distance(destX[order[a]], destY[order[a]], destX[order[b]], destY[order[b]]);
					scorediffA -= distance(destX[order[b]], destY[order[b]], 0, 0);
				} else if (b == N) {
					scorediffA += distance(destX[order[a]], destY[order[a]], destX[order[b - 1]], destY[order[b - 1]]);
				} else {
					scorediffA += distance(destX[order[a]], destY[order[a]], destX[order[b - 1]], destY[order[b - 1]]);
					scorediffA += distance(destX[order[a]], destY[order[a]], destX[order[b]], destY[order[b]]);
					scorediffA -= distance(destX[order[b - 1]], destY[order[b - 1]], destX[order[b]], destY[order[b]]);
				}

				double scorediffB = 0;

				scorediffB -= inertia_score(a - 1, a, a + 1, order);
				scorediffB -= inertia_score(a - 2, a - 1, a, order);
				scorediffB -= inertia_score(a, a + 1, a + 2, order);

				scorediffB += inertia_score(a - 1, a + 1, a + 2, order);
				scorediffB += inertia_score(a - 2, a - 1, a + 1, order);

				scorediffB -= inertia_score(b - 2, b - 1, b, order);
				scorediffB -= inertia_score(b - 1, b, b + 1, order);

				scorediffB += inertia_score(b - 2, b - 1, a, order);
				scorediffB += inertia_score(b - 1, a, b, order);
				scorediffB += inertia_score(a, b, b + 1, order);

				double scorediff = scorediffA * (1 - coefB) + scorediffB * coefB;

				if (scorediff <= -T * rnd.nextLog()) {
					score += scorediff;
					if (bestscore > score) {
						bestscore = score;
					}
					if (a < b) {
						for (int i = a; i < b - 1; i++) {
							swap(order[i], order[i + 1]);
						}
					} else {
						for (int i = a; i > b; i--) {
							swap(order[i], order[i - 1]);
						}
					}
				}
			}
		}
	}

	void TSP(vector<int> &order) {
		double score = 0;

		// for (int i = 0; i < order.size(); i++) {
		// 	if(i == 0){
		// 		score += distance(0, 0, destX[order[0]], destY[order[0]]);
		// 	}else{
		// 		score += distance(destX[order[i - 1]], destY[order[i - 1]], destX[order[i]], destY[order[i]]);
		// 	}
		// }
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
		double T0 = 100;
		if (testcase_index == 25) {
			T0 = 10;
		} else if (testcase_index == 24) {
			T0 = 450000000;
			T0 = 200000;
		} else if (testcase_index == 23) {
			T0 = 50000000 / 10000000;
		} else if (testcase_index == 22) {
			T0 = 300;  // / 60;
		} else if (testcase_index == 21) {
			T0 = 1000;	// / 300;
		} else if (testcase_index == 20) {
			T0 = 300;  // / 80;
			T0 = 10;
					   // T0 = 500 / 20;
		} else if (testcase_index == 19) {
			T0 = 10;
		} else if (testcase_index == 18) {
			T0 = 300000 / 2000;
		} else if (testcase_index == 17) {
			T0 = 500;  // / 10;
		} else if (testcase_index == 16) {
			T0 = 60 / 10;
		} else if (testcase_index == 15) {
			T0 = 7;
		}
		while (true) {
			double ti = timer.get();
			if (ti > timelimit) break;
			// cnt++;
			// if (cnt >= 1000) break;
			T = (timelimit - ti) * tcoef;
			T = T0 * T;
			if (ti > next_log_timing) {
				next_log_timing += log_interval_time;
				cerr << fixed << setprecision(5) << T << " " << score << " " << bestscore << endl;
			}
			double r = rnd.nextDouble();
			if (r < 0.8) {
				int a = rnd.nextInt(N);
				int b = rnd.nextInt(N - 1);
				if (a <= b) b++;
				if (a > b) {
					swap(a, b);
				}
				// reverse a ~ b

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
				// else{ // 原点に戻る場合
				// 	scorediff -= distance(destX[order[b]], destY[order[b]], 0, 0);
				// 	scorediff += distance(destX[order[a]], destY[order[a]], 0, 0);
				// }
				if (scorediff <= -T * rnd.nextLog()) {
					score += scorediff;
					if (bestscore > score) {
						bestscore = score;
					}
					reverse(order.begin() + a, order.begin() + (b + 1));
				}
			} else {
				int a = rnd.nextInt(N);
				int b = rnd.nextInt(N - 1);
				if (a <= b) b += 2;

				// insert element a to positin b

				double scorediff = 0;
				if (a == 0) {
					scorediff -= distance(destX[order[a]], destY[order[a]], 0, 0);
					scorediff -= distance(destX[order[a]], destY[order[a]], destX[order[a + 1]], destY[order[a + 1]]);
					scorediff += distance(destX[order[a + 1]], destY[order[a + 1]], 0, 0);
				} else if (a == N - 1) {
					scorediff -= distance(destX[order[a]], destY[order[a]], destX[order[a - 1]], destY[order[a - 1]]);
				} else {
					scorediff -= distance(destX[order[a]], destY[order[a]], destX[order[a - 1]], destY[order[a - 1]]);
					scorediff -= distance(destX[order[a]], destY[order[a]], destX[order[a + 1]], destY[order[a + 1]]);
					scorediff += distance(destX[order[a - 1]], destY[order[a - 1]], destX[order[a + 1]], destY[order[a + 1]]);
				}

				if (b == 0) {
					scorediff += distance(destX[order[a]], destY[order[a]], 0, 0);
					scorediff += distance(destX[order[a]], destY[order[a]], destX[order[b]], destY[order[b]]);
					scorediff -= distance(destX[order[b]], destY[order[b]], 0, 0);
				} else if (b == N) {
					scorediff += distance(destX[order[a]], destY[order[a]], destX[order[b - 1]], destY[order[b - 1]]);
				} else {
					scorediff += distance(destX[order[a]], destY[order[a]], destX[order[b - 1]], destY[order[b - 1]]);
					scorediff += distance(destX[order[a]], destY[order[a]], destX[order[b]], destY[order[b]]);
					scorediff -= distance(destX[order[b - 1]], destY[order[b - 1]], destX[order[b]], destY[order[b]]);
				}

				if (scorediff <= -T * rnd.nextLog()) {
					score += scorediff;
					if (bestscore > score) {
						bestscore = score;
					}
					if (a < b) {
						for (int i = a; i < b - 1; i++) {
							swap(order[i], order[i + 1]);
						}
					} else {
						for (int i = a; i > b; i--) {
							swap(order[i], order[i - 1]);
						}
					}
				}
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

		void do_eval(int i, const vector<int> &order) {
			eval = turns + rnd.nextDouble() * 1;
			if (i + 1 < N) {
				// int dx = destX[order[i + 1]] - destX[order[i]];
				// int dy = destY[order[i + 1]] - destY[order[i]];
				// eval += min_turns(dx, dy, vx, vy);
				// eval += 1.0 * (abs(vx) + abs(vy));
			}
			// eval += vx * vx + vy * vy;
		}

		bool operator<(const BeamState &right) const {
			return eval < right.eval;
		}
	};

	vector<unordered_map<long long, int> > dp;
	void add_beam(vector<BeamState> &beam, int i, int vx, int vy, int turn, int from, const vector<int> &order) {
		long long hash = vx * (long long)1e9 + vy;
		auto a = dp[i].find(hash);
		if (a == dp[i].end() || (*a).second > turn) {
			dp[i][hash] = turn;
			beam.emplace_back(vx, vy, turn, from);
			beam.back().do_eval(i, order);
		}
	}

	void greedy_order(vector<int> &order) {
		int x = 0;
		int y = 0;
		int vx = 0;
		int vy = 0;
		vector<int> visited(N, 0);
		bool f = false;
		// vector<int> path = {20, 29, 22, 98, 93, 81, 78, 48, 52, 87, 3, 82, 10, 76};
		// vector<int> path = {20, 29, 93, 82, 10, 3, 87, 52, 48, 78, 81, 22, 98, 76};
		// vector<int> path = {20, 29, 22, 98, 82, 10, 3, 87, 52, 48, 78, 81, 93, 76};
		// vector<int> path = {20, 29, 22, 98, 76, 93, 81, 78, 48, 52, 87, 3, 82, 10};
		vector<int> path = {20, 29, 22, 98, 76, 82, 10, 3, 87, 52, 48, 78, 81, 93};
		for (int i = 0; i < N; i++) {
			if (testcase_index == 5) {
				order[i] = i;
				continue;
			}
			double mnscore = 1e18;
			int mnj = -1;
			for (int j = 0; j < N; j++) {
				if (testcase_index == 9) {
					if (i <= 50) {
						if (j == 53 || j == 62) continue;
					}

					if (f && path.size() > 0) {
						if (j != path[0]) continue;
					} else if (!f) {
						bool a = true;
						for (int k = 0; k < path.size(); k++) {
							if (path[k] == j) {
								a = false;
							}
						}
						if (!a) continue;
					}
					if (i > 0 && order[i - 1] != 73) {
						if (j == 28) continue;
					}
				}
				if (!visited[j]) {
					double dx = destX[j] - x;
					double dy = destY[j] - y;
					double score = 0;

					// score = abs(dx) + abs(dy) - 28 * dy;

					score = abs(dx) + abs(dy);
					// score -= 2.1 * max(abs(destX[j]), abs(destY[j]));
					// score += 0.01 * ((long long)dx * dx + (long long)dy * dy);
					// score += ((long long)dx * dx + (long long)dy * dy);
					// score += sqrt(((long long)dx * dx + (long long)dy * dy));
					//  score = sqrt((long long)dx * dx + (long long)dy * dy) + rnd.nextDouble() * 3.0;
					//  score = abs(dx) + abs(dy) + rnd.nextDouble() * 3.0; // ガチャ
					//  score += rnd.nextDouble() * 8.0;
					if (mnscore > score) {
						mnscore = score;
						mnj = j;
					}
				}
			}
			if (testcase_index == 9) {
				if (f && path.size() > 0) {
					cerr << destX[mnj] << " " << destY[mnj] << endl;
				}
				if (f && path.size() > 0) {
					path.erase(path.begin());
				}
				if (mnj == 72) f = true;
			}
			if (testcase_index == 23) {
				if (mnscore > 1000) {
					cerr << i << endl;
					break;
				}
			}
			// if (i % 1000 == 0) {
			// 	cerr << mnscore << endl;
			// }
			order[i] = mnj;
			visited[mnj] = 1;
			x = destX[mnj];
			y = destY[mnj];
			// cerr << x << " " << y << endl;
		}
		if (testcase_index == 23) {
			vector<int> remains;
			for (int i = 0; i < N; i++) {
				if (!visited[i]) {
					remains.push_back(i);
				}
			}
			for (int i = 0; i < remains.size(); i++) {
				double mnscore = 1e18;
				int mnj = -1;
				for (int j = 0; j < N - remains.size() + i - 1; j++) {
					double dx1 = destX[order[j]] - destX[remains[i]];
					double dy1 = destY[order[j]] - destY[remains[i]];
					double dx2 = destX[order[j + 1]] - destX[remains[i]];
					double dy2 = destY[order[j + 1]] - destY[remains[i]];
					double score = abs(dx1) + abs(dx2) + abs(dy1) + abs(dy2);

					if (mnscore > score) {
						mnscore = score;
						mnj = j;
					}
				}
				order.insert((order.begin() + mnj + 1), remains[i]);
				order.pop_back();
				cerr << remains[i] << " " << mnscore << endl;
			}
		}
	}

	BeamState solve_beamsearch(const vector<int> &order, vector<vector<BeamState> > &beams, int width, int max_transition, int extra_turns) {
		int x = 0;
		int y = 0;
		int vx = 0;
		int vy = 0;

		int mnx, mxx;
		int mny, mxy;

		beams.clear();
		beams.resize(N + 1);
		dp.clear();
		dp.resize(N + 1);
		beams[0].emplace_back(0, 0, 0, -1);
		cerr << "beamsearch start" << endl;
		for (int i = 0; i < N; i++) {
			// cerr << i << " " << beams[i].size() << endl;
			if (i % 1000 == 0 && i != 0) {
				cerr << i << " " << beams[i][0].turns << " " << beams[i][0].vx << " " << beams[i][0].vy << endl;
			}
			int pos_index = order[i];
			int dx = destX[pos_index] - x;
			int dy = destY[pos_index] - y;
			int best_turn = (int)1e9;
			beams[i + 1].reserve(beams[i].size() * max_transition * extra_turns);
			for (int j = 0; j < beams[i].size(); j++) {
				vx = beams[i][j].vx;
				vy = beams[i][j].vy;
				int turns_so_far = beams[i][j].turns;
				int turn = min_turns(dx, dy, vx, vy);
				// if (turn == -1) continue;
				if (best_turn > turns_so_far + turn) {
					best_turn = turns_so_far + turn;
				}
				// if (i == 0) {
				// 	cerr << i << " " << j << " " << vx << " " << vy << " " << turns_so_far << " " << turn << endl;
				// }
				for (int k = 0; k < extra_turns; k++) {
					int tt = turn + k;
					if (best_turn + 40 <= turns_so_far + tt) break;
					if (k == 0 || (can_be_at_1d(dx, vx, tt) && can_be_at_1d(dy, vy, tt))) {
						final_velocity_range(dx, vx, tt, mnx, mxx);
						final_velocity_range(dy, vy, tt, mny, mxy);
						if ((mxx - mnx + 1) * (mxy - mny + 1) > max_transition) {
							add_beam(beams[i + 1], i, mnx, mny, turns_so_far + tt, j, order);
							add_beam(beams[i + 1], i, mxx, mny, turns_so_far + tt, j, order);
							add_beam(beams[i + 1], i, mnx, mxy, turns_so_far + tt, j, order);
							add_beam(beams[i + 1], i, mxx, mxy, turns_so_far + tt, j, order);
							for (int z = 0; z < max_transition - 4; z++) {
								add_beam(beams[i + 1], i, rnd.nextInt(mnx, mxx), rnd.nextInt(mny, mxy), turns_so_far + tt, j, order);
							}
						} else {
							for (int xx = mnx; xx <= mxx; xx++) {
								for (int yy = mny; yy <= mxy; yy++) {
									add_beam(beams[i + 1], i, xx, yy, turns_so_far + tt, j, order);
								}
							}
						}
					}
				}
			}
			// if (i > 20) width = 20;
			if (beams[i + 1].size() > width) {
				nth_element(beams[i + 1].begin(), beams[i + 1].begin() + width, beams[i + 1].end());
				// sort(beams[i + 1].begin(), beams[i + 1].end());
				beams[i + 1].resize(width);
			}
			beams[i + 1].shrink_to_fit();
			dp[i].clear();

			x = destX[pos_index];
			y = destY[pos_index];
		}
		auto best = *min_element(beams.back().begin(), beams.back().end());
		return best;
	}

	void construct_from_beam(const vector<int> &order, const vector<vector<BeamState> > &beams, BeamState &best, vector<int> &resX, vector<int> &resY) {
		vector<BeamState> bests;
		bests.push_back(best);
		for (int i = N - 1; i >= 0; i--) {
			best = beams[i][best.from];
			bests.push_back(best);
		}

		reverse(bests.begin(), bests.end());
		int x, y;
		x = 0;
		y = 0;
		for (int i = 0; i < N; i++) {
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
			// cerr << pos_index << " " << turn << endl;
			x = destX[pos_index];
			y = destY[pos_index];
		}
	}

	void solve_order_search(vector<int> &order, vector<int> &resX, vector<int> &resY) {
		vector<vector<BeamState> > beams;
		// int width = 1 << 10;
		// int max_transition = 20;
		// int extra_turns = 10;
		int width = 1 << 11;
		int max_transition = 20;
		int extra_turns = 5;
		BeamState best = solve_beamsearch(order, beams, width, max_transition, extra_turns);
		dump(best.turns);
		int bestscore = best.turns;
		int score = bestscore;
		int score_before = bestscore;
		construct_from_beam(order, beams, best, resX, resY);
		vector<int> best_order = order;
		vector<int> order_before = order;
		double coef;
		double T;
		int maxcnt;
		for (int i = 0; i < 100; i++) {
			if (testcase_index == 16) {
				coef = rnd.nextDouble() * 0.4 + 0.55;
				T = rnd.nextDouble() * 3 + 0.2;
				maxcnt = rnd.nextInt(3, 10);
			} else if (testcase_index == 17) {
				coef = rnd.nextDouble() * 0.3 + 0.6;
				T = rnd.nextDouble() * 3 + 0.5;
				maxcnt = rnd.nextInt(1, 10);
			} else if (testcase_index == 18) {
				coef = rnd.nextDouble() * 0.3 + 0.6;
				T = rnd.nextDouble() * 60 + 20;
				maxcnt = rnd.nextInt(3, 30);
			}else if (testcase_index == 22) {
				coef = rnd.nextDouble() * 0.3 + 0.6;
				T = rnd.nextDouble() * 2 + 0.3;
				maxcnt = rnd.nextInt(3, 10);
			}

			score_before = score;
			order_before = order;

			order_transit(order, T, maxcnt, coef);
			best = solve_beamsearch(order, beams, width, max_transition, extra_turns);
			score = best.turns;
			cerr << score_before << " \t" << score << endl;
			if (bestscore >= score) {
				cerr << "!" << endl;
				bestscore = score;
				best_order = order;
				save_order(best_order);
				if (bestscore > score) {
					dump(score);
				}
				resX.clear();
				resY.clear();
				construct_from_beam(order, beams, best, resX, resY);
			} else {
				if (score > bestscore + 15) {
					// cerr << "undo" << endl;
					order = best_order;
					score = bestscore;
				} else if (score > score_before - 3 * rnd.nextLog()) {
					score = score_before;
					order = order_before;
				}
			}
		}
		order = best_order;
	}

	void solve_one_beam(const vector<int> &order, vector<int> &resX, vector<int> &resY) {
		vector<vector<BeamState> > beams;
		int width = 1 << 12;
		int max_transition = 10;
		int extra_turns = 5;
		BeamState best = solve_beamsearch(order, beams, width, max_transition, extra_turns);

		construct_from_beam(order, beams, best, resX, resY);
	}
	int solve() {
		// int seed = (int)(timer.get() * 1000000);
		// cerr << seed << endl;
		// for (int i = 0; i < seed; i++) {
		// 	rnd.next();
		// }
		// 訪問する点の順番を決める
		vector<int> order(N);

		bool greedy = false;
		greedy = false;
		if (greedy) {
			greedy_order(order);
		} else {
			// order = load_tsp_solver_order();
			order = load_order();
		}
		if (true) {
			// TSP(order);
			inertiaTSP(order);
			save_order(order);
		}
		vector<int> resX;
		vector<int> resY;

		//solve_order_search(order, resX, resY);
		solve_one_beam(order, resX, resY);

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
				// {
				index_map.push_back(destX.size());
				destX.push_back(x);
				destY.push_back(y);
				st.emplace(x, y);
			} else {
				index_map.push_back(-1);
			}
			if (line == 4) {
				line4 = {x, y};
			}
			line++;
		}
		dump(destX.size());
		N = destX.size();
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