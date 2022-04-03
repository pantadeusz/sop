#!/bin/bash -e

NAMES='bitcoin'

if [[ -f ~/.getpricerc ]]; then
	NAMES=$(cat ~/.getpricerc)
fi

curl -X 'GET' \
"https://api.coingecko.com/api/v3/simple/price?ids=$NAMES&vs_currencies=pln" \
-H 'accept: application/json' | sed 's/\}\}/}\n/g' | tr ',' '\n' | \
sed 's/[^a-z]*\([a-z][a-z]*\)[^0-9]*\([0-9.]*\).*/\1 \2/g' | while read line; do 
	# NAME=$(echo $line | awk '{print $1}')
	# PRICE=$(echo $line | awk '{print $2}')
	
	LINIA=($line)
	notify-send "Cena ${LINIA[0]} wynosi ${LINIA[1]}"
#	notify-send "Cena $NAME to $PRICE" 
done
