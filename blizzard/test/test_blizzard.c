/**
 * Copyright (c) 2015 The Blizzard Open Source Project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdio.h>
#include <stdlib.h>

#include "blizzard.h"

int main(int argc, char *argv[])
{
    struct mpq_hash_table table;

    blizzard_init(&table, 21);

    printf("the size of index table is: %llu\n", table.mht_index_table_size);
    int i;
    table.insert("test", &table);
    for (i = 0; i < 20; ++i) {
        char buf[32];
        sprintf(buf, "abcdefg%d." , i);
        u_64 index = table.insert(buf, &table);
        index != -1 ? printf("hash ok for: %s, %lld\n", buf, index) :
                printf("hash failed for: %s\n", buf);
    }

    for (i = 0; i < 20; i++)
    {
        char buf[32];
        sprintf(buf, "abcdefg%d." , i);
        u_64 index = table.query(buf, &table);
        index != -1 ? printf("%s index is %lld\n", buf, index) :
                printf("%s index is -1\n", buf);
    }

    blizzard_final(&table);

    return 0;
}
