/**
 * Copyright (c) 2015 The Blizzard Open Source Project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "blizzard.h"

static void
prepare(u_64 *mht_table)
{
    u_64 seed = 0x00100001;
    u_64 i, j, k;

    for (i = 0; i < TABLE_UNIT; ++i) {
        for (j = i, k = 0; k < (TABLE_SIZE / TABLE_UNIT);
             ++k, j += TABLE_UNIT) {
            u_64 a, b;
            seed = (seed * 125 + 3) % 0x2AAAB;
            a = (seed & 0xFFFF) << 0x10;
            seed = (seed * 125 + 3) % 0x2AAAB;
            b = (seed & 0xFFFF);
            mht_table[j] = (a | b);
        }
    }
}

static u_64
hash(char *item, u_64 hash_type, u_64 *mht_table)
{
    u_8 *key = (u_8 *)item;
    u_64 seed1 = 0x7FED7FED;
    u_64 seed2 = 0xEEEEEEEE;

    while (*key != 0) {
        int c = toupper(*key++);
        seed1 = mht_table[(hash_type << 8) + c] ^ (seed1 + seed2);
        seed2 = c + seed1 + seed2 + (seed2 << 5) + 3;
    }

    return seed1;
}

static s_64
insert(char *item, struct mpq_hash_table *table)
{
    u_64 hash_0 = hash(item, HASH_0, table->mht_table);
    u_64 hash_1 = hash(item, HASH_1, table->mht_table);
    u_64 hash_2 = hash(item, HASH_2, table->mht_table);
    u_64 start = hash_0 % table->mht_index_table_size;
    u_64 index = start;
    struct mpq_hash_index *index_table = table->mht_index_table;

    while (index_table[index].mhi_exists) {
        index = (index + 1) % table->mht_index_table_size;
        if (index == start) {
            return -1;
        }
    }
    index_table[index].mhi_exists = 1;
    index_table[index].mhi_hash_1 = hash_1;
    index_table[index].mhi_hash_2 = hash_2;

    return index;
}

static s_64
query(char *item, struct mpq_hash_table *table)
{
    u_64 hash_0 = hash(item, HASH_0, table->mht_table);
    u_64 hash_1 = hash(item, HASH_1, table->mht_table);
    u_64 hash_2 = hash(item, HASH_2, table->mht_table);
    u_64 start = hash_0 % table->mht_index_table_size;
    u_64 index = start;
    struct mpq_hash_index *index_table = table->mht_index_table;

    while (index_table[index].mhi_exists) {
        if (index_table[index].mhi_hash_1 == hash_1 &&
            index_table[index].mhi_hash_2 == hash_2) {
            return index;
        } else {
            index = (index + 1) % table->mht_index_table_size;
        }
        if (index == start) {
            break;
        }
    }

    return -1;
}

int blizzard_init(struct mpq_hash_table *table, u_64 index_table_size)
{
    int i;

    prepare(table->mht_table);

    table->insert = insert;
    table->query = query;

    table->mht_index_table_size = index_table_size;
    table->mht_index_table = malloc(sizeof (struct mpq_hash_index)
                                    * index_table_size);
    if (table->mht_index_table == NULL) {
        return -1;
    }
    for (i = 0; i < table->mht_index_table_size; ++i) {
        table->mht_index_table[i].mhi_hash_1 = -1;
        table->mht_index_table[i].mhi_hash_2 = -1;
        table->mht_index_table[i].mhi_exists = 0;
    }

    return 0;
}

void blizzard_final(struct mpq_hash_table *table)
{
    if (table != NULL && table->mht_index_table != NULL) {
        free(table->mht_index_table);
    }
    memset(table, 0, sizeof (*table));
}
