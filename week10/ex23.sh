#!/bin/bash
# Contents of gen.sh
# for ((i=0; i<$1; i++)) do echo $RANDOM >> $2; done

bash gen.sh 10 ex1.txt
ln ex1.txt ex11.txt
ln ex1.txt ex12.txt

diff ex1.txt ex11.txt
diff ex11.txt ex12.txt
# no output, files are identical

ls -i ex1.txt ex11.txt ex12.txt > output.txt
cat output.txt
#575893 ex11.txt
#575893 ex12.txt
#575893 ex1.txt

# I-nodes are same because of creation of hardlinks
ln ex1.txt ex13.txt
mv ex13.txt /tmp/ex13.txt

find ~ -inum 575893
#/home/m0t9/week10/ex1.txt
#/home/m0t9/week10/ex12.txt
#/home/m0t9/week10/ex11.txt
find / -inum 575893 2> /dev/null
#/tmp/ex13.txt
#/home/m0t9/week10/ex1.txt
#/home/m0t9/week10/ex12.txt
#/home/m0t9/week10/ex11.txt

find / -inum 575893 -exec rm {} \; 2> /dev/null
