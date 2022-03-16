#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>

const int INF = 10000;
const int depth_precalc = 6;
const int depth_gen = 6;

int ans = INF;
const char *fin = "000000000111111111222222222333333333444444444555555555";
std::string cur, seq, ans_seq;

int side[6][12] = {
        {38, 37, 36, 29, 28, 27, 20, 19, 18, 11, 10, 9},
        {0,  3,  6,  18, 21, 24, 45, 48, 51, 44, 41, 38},
        {6,  7,  8,  27, 30, 33, 47, 46, 45, 17, 14, 11},
        {36, 39, 42, 53, 50, 47, 26, 23, 20, 8,  5,  2},
        {9,  12, 15, 51, 52, 53, 35, 32, 29, 2,  1,  0},
        {15, 16, 17, 24, 25, 26, 33, 34, 35, 42, 43, 44}};

int up[6][8] = {
        {0,  1,  2,  5,  8,  7,  6,  3},
        {9,  10, 11, 14, 17, 16, 15, 12},
        {18, 19, 20, 23, 26, 25, 24, 21},
        {27, 28, 29, 32, 35, 34, 33, 30},
        {36, 37, 38, 41, 44, 43, 42, 39},
        {45, 46, 47, 50, 53, 52, 51, 48}};

std::unordered_map<std::string, std::string> all;

void rotate(int pos) {
    for (int i = 8; i >= 0; i--) {
        std::swap(cur[side[pos][i + 3]], cur[side[pos][i]]);
    }
    for (int i = 5; i >= 0; i--) {
        std::swap(cur[up[pos][i + 2]], cur[up[pos][i]]);
    }
}

void precalc(int h, int pr) {
    if (h >= ans) return;
    if (h == depth_precalc) all[cur] = seq;
    if (h >= depth_precalc) return;
    for (int i = 0; i < 6; i++) {
        if (i == pr) continue;
        rotate(i);
        seq += char('0' + i), seq += char('0' + i), seq += char('0' + i);
        precalc(h + 1, i);
        rotate(i);
        seq.pop_back();
        precalc(h + 1, i);
        rotate(i);
        seq.pop_back();
        precalc(h + 1, i);
        rotate(i);
        seq.pop_back();
    }
}

void gen(int h, int pr) {
    if (cur == fin) {
        if (ans > h) {
            std::cout << "Improved: " << h << ' ' << seq << std::endl;
            ans = h;
            ans_seq = seq;
        }
        return;
    }
    if (all.find(cur) != all.end()) {
        if (ans > h + depth_precalc) {
            ans = h + depth_precalc;
            std::string suf = all[cur];
            reverse(suf.begin(), suf.end());
            ans_seq = seq + suf;
            std::cout << "Improved: " << ans << ' ' << ans_seq << std::endl;
        }
    }
    if (h >= depth_gen || h >= ans - 1) return;
    for (int i = 0; i < 6; i++) {
        if (i == pr) continue;
        rotate(i);
        seq += char('0' + i);
        gen(h + 1, i);
        rotate(i);
        seq += char('0' + i);
        gen(h + 1, i);
        rotate(i);
        seq += char('0' + i);
        gen(h + 1, i);
        rotate(i);
        seq.pop_back(), seq.pop_back(), seq.pop_back();
    }
}

int main() {
    cur = fin;
    precalc(0, -1);
    std::cout << "Finished precalc" << std::endl;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 9; j++) {
            std::cin >> cur[9 * i + j];
        }
    }
    gen(0, -1);
    if (ans > depth_precalc + depth_gen) {
        std::cout << -1 << std::endl;
    } else {
        std::cout << ans << std::endl;
        std::cout << ans_seq << std::endl;
    }
}
