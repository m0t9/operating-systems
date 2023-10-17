#!/bin/bash
gcc ~/week06/worker.c -Wall -Wextra -o worker
gcc ~/week06/scheduler_rr.c -Wall -Wextra -o scheduler_rr

~/week06/scheduler_rr $1 

rm ~/week06/worker
rm ~/week06/scheduler_rr

