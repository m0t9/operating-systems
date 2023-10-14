gcc ~/week06/worker.c -Wall -Wextra -o worker
gcc ~/week06/scheduler_sjf.c -Wall -Wextra -o scheduler_sjf

~/week06/scheduler_sjf $1 

rm ~/week06/worker
rm ~/week06/scheduler_sjf

