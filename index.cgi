#!/bin/bash

if [ ! -f osz/$QUERY_STRING.osz ]; then
	if [ ! -f osz/$QUERY_STRING.osz.part ]; then
		wget -4 http://osu.ppy.sh/forum/ucp.php?mode=login --post-data="username=osu_dl_new&password=123456&redirect=%2Fd%2F$QUERY_STRING&login=login" -O osz/$QUERY_STRING.osz.part >/dev/null
		mv osz/$QUERY_STRING.osz.part osz/$QUERY_STRING.osz >/dev/null
	else
		while [ -f osz/$QUERY_STRING.osz.part ]; do
			sleep 1s >/dev/null
		done
	fi
fi

LEN=`wc -c osz/$QUERY_STRING.osz`

if [ $LEN -lt 204800 ]; then
	echo "status: 503 Service Temporarily Unavailable"
	echo "content-type: text/plain"
	echo ""
	echo "Error, please wait for a period of time then retry"
	rm -f osz/$QUERY_STRING.osz >/dev/null
	exit 0
fi

echo "status: 301 Moved Permanantly"
echo "location: /osz/$QUERY_STRING.osz"
echo ""
exit 0
