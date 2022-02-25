#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <cassert>

using namespace std;
using sint = short int;

const sint INF = 10000;
const int depth_precalc = 6;
const int depth_gen = 6;

sint ans = INF;
const string fin = "000000000111111111222222222333333333444444444555555555";
string cur = fin, seq = "", ans_seq = "";

sint side[6][12] = {
{38, 37, 36, 29, 28, 27, 20, 19, 18, 11, 10, 9},
{0, 3, 6, 18, 21, 24, 45, 48, 51, 44, 41, 38},
{6, 7, 8, 27, 30, 33, 47, 46, 45, 17, 14, 11},
{36, 39, 42, 53, 50, 47, 26, 23, 20, 8, 5, 2},
{9, 12, 15, 51, 52, 53, 35, 32, 29, 2, 1, 0},
{15, 16, 17, 24, 25, 26, 33, 34, 35, 42, 43, 44} };

sint up[6][8] = {
{0, 1, 2, 5, 8, 7, 6, 3},
{9, 10, 11, 14, 17, 16, 15, 12},
{18, 19, 20, 23, 26, 25, 24, 21},
{27, 28, 29, 32, 35, 34, 33, 30},
{36, 37, 38, 41, 44, 43, 42, 39},
{45, 46, 47, 50, 53, 52, 51, 48} };

unordered_map<string, string> all;

void rotate(sint pos) {
    for (sint i = 8; i >= 0; i--) {
        swap(cur[side[pos][i + 3]], cur[side[pos][i]]);
    }
    for (sint i = 5; i >= 0; i--) {
        swap(cur[up[pos][i + 2]], cur[up[pos][i]]);
    }
}

void precalc(sint h, sint pr) {
    if (h >= ans) return;
    if (h == depth_precalc) all[cur] = seq;
    if (h >= depth_precalc) return;
    for (int i = 0; i < 6; i++) {
        if (i == pr) continue;
        rotate(i), seq += '0' + i, seq += '0' + i, seq += '0' + i;
        precalc(h + 1, i);
        rotate(i), seq.pop_back();
        precalc(h + 1, i);
        rotate(i), seq.pop_back();
        precalc(h + 1, i);
        rotate(i), seq.pop_back();
    }
}

void gen(sint h, sint pr) {
    if (cur == fin) {
        if (ans > h) {
            cout << "Improved: " << h << ' ' << seq << endl;
            ans = h;
            ans_seq = seq;
        }
        return;
    }
    if (all.find(cur) != all.end()) {
        if (ans > sint(h + depth_precalc)) {
            ans = sint(h + depth_precalc);
            string suf = all[cur];
            reverse(suf.begin(), suf.end());
            ans_seq = seq + suf;
            cout << "Improved: " << ans << ' ' << ans_seq << endl;
        }
    }
    if (h >= depth_gen || h >= ans - 1) return;
    for (sint i = 0; i < 6; i++) {
        if (i == pr) continue;
        rotate(i), seq += '0' + i;
        gen(h + 1, i);
        rotate(i), seq += '0' + i;
        gen(h + 1, i);
        rotate(i), seq += '0' + i;
        gen(h + 1, i);
        rotate(i), seq.pop_back(), seq.pop_back(), seq.pop_back();
    }
}

int main()
{
    double start = clock();
    precalc(0, -1);
    cout << "Finished precalc. Time: " << (clock() - start) / CLOCKS_PER_SEC << endl;
    for (sint i = 0; i < 6; i++) {
        for (sint j = 0; j < 9; j++) {
            cin >> cur[9 * i + j];
        }
    }
    gen(0, -1);
    if (ans > depth_precalc + depth_gen) {
        cout << -1 << endl;
    }
    else {
        cout << ans << endl;
        cout << ans_seq << endl;
    }
    cout << "Time: " << (clock() - start) / CLOCKS_PER_SEC << endl;
}
