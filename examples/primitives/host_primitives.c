/*
File: host_primitives.c

This file is part of the Epiphany BSP library.

Copyright (C) 2014 Buurlage Wits
Support e-mail: <info@buurlagewits.nl>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License (LGPL)
as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
and the GNU Lesser General Public License along with this program,
see the files COPYING and COPYING.LESSER. If not, see
<http://www.gnu.org/licenses/>.
*/

#include <host_bsp.h>
#include <stdio.h>

// Data to be processed by the epiphany cores
#define data_count (16*1000)
float data[data_count];

int main(int argc, char **argv)
{
    // Initialize the BSP system
    if (!bsp_init("bin/e_primitives.srec", argc, argv))
    {
        fprintf(stderr, "ERROR: bsp_init() failed\n");
        return -1;
    }

    // Get the number of processors available
    int nprocs = bsp_nprocs();

    printf("bsp_nprocs(): %i\n", nprocs);

    // Initialize the epiphany system, and load the e-program
    if (!bsp_begin(nprocs))
    {
        fprintf(stderr, "ERROR: bsp_begin() failed\n");
        return -1;
    }

    //
    // Send some initial data to the processors
    // (matrix data for example)
    //

    // Give it a tag. For example an integer
    int tag;
    int tagsize = sizeof(int);

    // Payload
    for (int i = 0; i < data_count; i++)
        data[i] = (float)(1+i);

    // Send the data
    // We divide it in nprocs parts
    int chunk_count = (data_count + nprocs - 1)/nprocs;

    ebsp_set_tagsize(&tagsize);
    for (int p = 0; p < nprocs; p++)
    {
        tag = 100+p;
        ebsp_senddown(p, &tag,
                &data[p*chunk_count],
                sizeof(float)*chunk_count);
    }

    // Run the SPMD on the e-cores
    ebsp_spmd();

    // Finalize
    bsp_end();
}
