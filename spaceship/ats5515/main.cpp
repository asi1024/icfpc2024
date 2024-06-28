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

const int DX[4] = {1, 0, -1, 0};
const int DY[4] = {0, 1, 0, -1};
const int REV[4] = {2, 3, 0, 1};
const string DIR = "DRUL";
vector<int> destX;
vector<int> destY;
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

	void TSP(vector<int> &order){


	}

	int solve() {
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
		for (int i = 0; i < destX.size(); i++) {
			int mnj = order[i];

			int dx = destX[mnj] - x;
			int dy = destY[mnj] - y;
			int turn = min_turns(dx, dy, vx, vy);
			vx = generate_moves_1d(dx, vx, turn, resX);
			vy = generate_moves_1d(dy, vy, turn, resY);
			x = destX[mnj];
			y = destY[mnj];
		}

		// 解のvalidation機能の残骸

		// for (int i = 0; i < destX.size(); i++) {
		// 	if (i % 1000 == 0) {
		// 		cerr << i << " " << resX.size() << endl;
		// 	}
		// 	int dx = destX[i] - x;
		// 	int dy = destY[i] - y;
		// 	int turn = min_turns(dx, dy, vx, vy);
		// 	vx = generate_moves_1d(dx, vx, turn, resX);
		// 	vy = generate_moves_1d(dy, vy, turn, resY);
		// 	x = destX[i];
		// 	y = destY[i];
		// }

		// x = 0;
		// y = 0;
		// vx = 0;
		// vy = 0;
		// int cur = 0;
		// for (int i = 0; i < resX.size(); i++) {
		// 	vx += resX[i];
		// 	vy += resY[i];
		// 	x += vx;
		// 	y += vy;
		// 	if (x == destX[cur] && y == destY[cur]) {
		// 		cur++;
		// 	}
		// }

		// cerr << destX.size() << " " << cur << endl;

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
		while (true) {
			getline(cin, str);
			if (str == "") break;
			auto strs = split_naive(str, ' ');
			// intに抑えられる前提
			int x = stoi(strs[0]);
			int y = stoi(strs[1]);
			destX.push_back(x);
			destY.push_back(y);
		}
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