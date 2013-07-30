if [ -f vn ]; then
	expr `cat vn` + 1 > vn
else
	echo 0 > vn
fi
if [ -z "$USER" ]; then
       USER=`whoami`
fi
HOST=`hostname`
VN=`cat vn`
PLATFORM=$1
echo "($PLATFORM Xinu) #$VN ("$USER"@"$HOST") "`date` > version
