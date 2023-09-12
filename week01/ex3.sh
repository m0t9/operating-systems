date
sleep 3
mkdir ~/week01/rootf
date
sleep 3
touch ~/week01/rootf/root.txt
date
sleep 3
mkdir ~/week01/homef
date
sleep 3
touch ~/week01/homef/home.txt
date
ls / -t -r > ~/week01/rootf/root.txt
ls ~ -t -r > ~/week01/homef/home.txt
echo Contents of \"root.txt\"
cat ~/week01/rootf/root.txt
echo Contents of \"home.txt\"
cat ~/week01/homef/home.txt
echo Contents of new \"rootf\" folder
ls ~/week01/rootf
echo Contents of new \"homef\" folder
ls ~/week01/homef

