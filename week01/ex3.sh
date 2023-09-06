date
sleep 3
mkdir rootf
date
sleep 3
touch rootf/root.txt
date
sleep 3
mkdir homef
date
sleep 3
touch homef/home.txt
date
ls / -t -r > rootf/root.txt
ls ~ -t -r > homef/home.txt
echo Contents of \"root.txt\"
cat rootf/root.txt
echo Contents of \"home.txt\"
cat homef/home.txt
echo Contents of new \"rootf\" folder
ls rootf
echo Contents of new \"homef\" folder
ls homef

