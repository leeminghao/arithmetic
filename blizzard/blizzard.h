/**
 * Copyright (c) 2015 The Blizzard Open Source Project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _BLIZZARD_H_
#define _BLIZZARD_H_

#ifdef LINUX
typedef long long          s_64;
typedef unsigned long long u_64;
typedef int                s_32;
typedef unsigned int       u_32;
typedef short              s_16;
typedef unsigned short     u_16;
typedef char               s_8;
typedef unsigned char      u_8;
#else /* ! LINUX */
#endif /* LINUX */

#define HASH_0  0x0
#define HASH_1  0x1
#define HASH_2  0x2

struct mpq_hash_index {
    s_64 mhi_hash_1;
    s_64 mhi_hash_2;
    int  mhi_exists;
};

#define TABLE_SIZE 0x500
#define TABLE_UNIT 0x100

struct mpq_hash_table {
    u_64 mht_table[TABLE_SIZE];

    s_64 (*insert)(char *item, struct mpq_hash_table *table);
    s_64 (*query)(char *item, struct mpq_hash_table *table);

    u_64 mht_index_table_size;
    struct mpq_hash_index *mht_index_table;
};

int  blizzard_init(struct mpq_hash_table *table, u_64 index_table_size);
void blizzard_final(struct mpq_hash_table *table);

#endif /* _BLIZZARD_H_ */
