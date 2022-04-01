#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

enum {
    POS_SIZE = 27
};

enum {
    HASH_MUL = 57575757
};

int cycles12[6][12] = {
        {38, 37, 36, 29, 28, 27, 20, 19, 18, 11, 10, 9},
        {0,  3,  6,  18, 21, 24, 45, 48, 51, 44, 41, 38},
        {6,  7,  8,  27, 30, 33, 47, 46, 45, 17, 14, 11},
        {36, 39, 42, 53, 50, 47, 26, 23, 20, 8,  5,  2},
        {9,  12, 15, 51, 52, 53, 35, 32, 29, 2,  1,  0},
        {15, 16, 17, 24, 25, 26, 33, 34, 35, 42, 43, 44}
};

int cycles8[6][8] = {
        {0,  1,  2,  5,  8,  7,  6,  3},
        {9,  10, 11, 14, 17, 16, 15, 12},
        {18, 19, 20, 23, 26, 25, 24, 21},
        {27, 28, 29, 32, 35, 34, 33, 30},
        {36, 37, 38, 41, 44, 43, 42, 39},
        {45, 46, 47, 50, 53, 52, 51, 48}
};

struct Position {
    char pos_data[POS_SIZE];
    char last_move;
    int dist;
};

struct Position *table;
size_t table_size;

size_t hash(const char *pos_data) {
    uint64_t h = *(uint64_t *) pos_data;
    h = h * HASH_MUL + *(uint64_t *) (pos_data + 8);
    h = h * HASH_MUL + *(uint64_t *) (pos_data + 16);
    return h % table_size;
}

struct Position *find_position(char *pos_data) {
    size_t pos_hash = hash(pos_data);
    for (size_t i = pos_hash; table[i].dist != -1; i = i < table_size - 1 ? i + 1 : 0) {
        if (memcmp(table[i].pos_data, pos_data, POS_SIZE) == 0) {
            return &table[i];
        }
    }
    return NULL;
}

bool store_position(char *pos_data, int last_move, int dist) {
    size_t pos_hash = hash(pos_data);
    size_t i;
    for (i = pos_hash; table[i].dist != -1; i = i < table_size - 1 ? i + 1 : 0) {
        if (memcmp(table[i].pos_data, pos_data, POS_SIZE) == 0) {
            if (table[i].dist <= dist) {
                return false;
            }
            table[i].last_move = (char) last_move;
            table[i].dist = dist;
            return true;
        }
    }
    memcpy(table[i].pos_data, pos_data, POS_SIZE);
    table[i].last_move = (char) last_move;
    table[i].dist = dist;
    return true;
}

int get_color(char *pos_data, int index) {
    uint8_t *pair = (uint8_t *) (pos_data + (index >> 1));
    return index & 1 ? (*pair >> 4) & 15 : *pair & 15;
}

void set_color(char *pos_data, int index, int color) {
    uint8_t *pair = (uint8_t *) (pos_data + (index >> 1));
    *pair = index & 1 ? (*pair & 15) + (color << 4) : color + (*pair - (*pair & 15));
}

void rotate(char *pos_data, int side) {
    int c1 = get_color(pos_data, cycles12[side][9]);
    int c2 = get_color(pos_data, cycles12[side][10]);
    int c3 = get_color(pos_data, cycles12[side][11]);
    for (int i = 8; i >= 0; --i) {
        set_color(pos_data, cycles12[side][i + 3], get_color(pos_data, cycles12[side][i]));
    }
    set_color(pos_data, cycles12[side][0], c1);
    set_color(pos_data, cycles12[side][1], c2);
    set_color(pos_data, cycles12[side][2], c3);
    c1 = get_color(pos_data, cycles8[side][6]);
    c2 = get_color(pos_data, cycles8[side][7]);
    for (int i = 5; i >= 0; --i) {
        set_color(pos_data, cycles8[side][i + 2], get_color(pos_data, cycles8[side][i]));
    }
    set_color(pos_data, cycles8[side][0], c1);
    set_color(pos_data, cycles8[side][1], c2);
}

int depth_precalc, depth_gen;
char cur_pos_data[POS_SIZE];

void precalc(int cur_depth, int last_move) {
    if (!store_position(cur_pos_data, last_move, cur_depth) || cur_depth >= depth_precalc) {
        return;
    }
    for (int side = 0; side < 6; ++side) {
        if (side == (last_move & 15)) continue;
        for (int it = 1; it <= 3; ++it) {
            rotate(cur_pos_data, side);
            precalc(cur_depth + 1, side + ((4 - it) << 4));
        }
        rotate(cur_pos_data, side);
    }
}

int ans_len;
char ans_moves[300], cur_moves[300];

void gen(int cur_depth, int prev_side) {
    size_t cur_moves_size = strlen(cur_moves);
    struct Position *position = find_position(cur_pos_data);
    if (position) {
        char pos_data_copy[POS_SIZE];
        memcpy(pos_data_copy, cur_pos_data, POS_SIZE);
        int cur_len = cur_depth;
        while (position->last_move != -1) {
            int last_side = position->last_move & 15;
            int last_cnt = position->last_move >> 4;
            char c = (char) ('0' + last_side);
            for (int it = 1; it <= last_cnt; ++it) {
                strncat(cur_moves, &c, 1);
                rotate(pos_data_copy, last_side);
            }
            position = find_position(pos_data_copy);
            ++cur_len;
        }
        if (ans_len > cur_len) {
            ans_len = cur_len;
            strcpy(ans_moves, cur_moves);
            printf("IMPROVED: %d %s\n", ans_len, ans_moves);
        }
        cur_moves[cur_moves_size] = 0;
        return;
    }
    if (cur_depth >= depth_gen || cur_depth + depth_precalc >= ans_len - 1) {
        return;
    }
    for (int side = 0; side < 6; ++side) {
        if (side == prev_side) continue;
        for (int it = 1; it <= 3; ++it) {
            rotate(cur_pos_data, side);
            char c = (char) ('0' + side);
            strncat(cur_moves, &c, 1);
            gen(cur_depth + 1, side);
        }
        rotate(cur_pos_data, side);
        cur_moves[cur_moves_size] = 0;
    }
}

size_t table_max_segment() {
    size_t start = 0;
    while (start < table_size && table[start].dist != -1) {
        ++start;
    }
    if (start == table_size) {
        return table_size;
    }
    size_t i = start;
    size_t cur_len = 0;
    size_t max_len = 0;
    do {
        if (table[i].dist == -1) {
            cur_len = 0;
        } else {
            ++cur_len;
        }
        if (max_len < cur_len) {
            max_len = cur_len;
        }
        i = i < table_size - 1 ? i + 1 : 0;
    } while (i != start);
    return max_len;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s depth-gen depth-precalc table-size\n", argv[0]);
        return 1;
    }
    depth_gen = (int) strtol(argv[1], NULL, 10);
    depth_precalc = (int) strtol(argv[2], NULL, 10);
    table_size = (size_t) strtoul(argv[3], NULL, 10);
    if (errno == EINVAL || errno == ERANGE) {
        fprintf(stderr, "usage: %s depth-gen depth-precalc table-size\n", argv[0]);
        return 1;
    }
    table = calloc(table_size, sizeof(*table));
    if (!table) {
        perror("calloc");
        return 1;
    }
    char table_name[50];
    sprintf(table_name, "depth-%d-%zu", depth_precalc, table_size);
    int table_fd = open(table_name, O_RDWR, 0755);
    if (table_fd < 0) {
        printf("file %s does not exist, running precalc\n", table_name);
        memset(table, -1, table_size * sizeof(*table));
        for (int i = 0; i < 54; ++i) {
            set_color(cur_pos_data, i, i / 9);
        }
        precalc(0, -1);
        printf("precalc finished, max segment length is %zu\n", table_max_segment());
        table_fd = open(table_name, O_RDWR | O_CREAT, 0755);
        if (table_fd < 0) {
            perror("open");
            return 1;
        }
        if (ftruncate(table_fd, table_size * sizeof(*table)) < 0) {
            perror("ftruncate");
            return 1;
        }
        void *table_buf = mmap(NULL, table_size * sizeof(*table), PROT_WRITE, MAP_SHARED, table_fd, 0);
        if (table_buf == MAP_FAILED) {
            perror("mmap");
            return 1;
        }
        memcpy(table_buf, table, table_size * sizeof(*table));
        munmap(table_buf, table_size * sizeof(*table));
        close(table_fd);
    } else {
        printf("using file %s\n", table_name);
        void *table_buf = mmap(NULL, table_size * sizeof(*table), PROT_WRITE, MAP_SHARED, table_fd, 0);
        if (table_buf == MAP_FAILED) {
            perror("mmap");
            return 1;
        }
        memcpy(table, table_buf, table_size * sizeof(*table));
        munmap(table_buf, table_size * sizeof(*table));
        close(table_fd);
    }
    while (true) {
        printf("enter your start position:\n");
        for (int i = 0; i < 54; ++i) {
            int c;
            if (scanf("%d", &c) != 1) {
                free(table);
                return 0;
            }
            set_color(cur_pos_data, i, c);
        }
        ans_len = INT_MAX;
        ans_moves[0] = cur_moves[0] = 0;
        gen(0, -1);
        if (ans_len == INT_MAX) {
            printf("NOT FOUND, SORRY :(\n");
            continue;
        }
        printf("%d\n", ans_len);
        printf("%s\n", ans_moves);
    }
}
