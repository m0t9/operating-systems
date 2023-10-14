gcc ~/week06/worker.c -Wall -Wextra -o worker
gcc ~/week06/scheduler.c -Wall -Wextra -o scheduler

~/week06/scheduler $1 

rm ~/week06/worker
rm ~/week06/scheduler

