#!/bin/bash

week=604800

links=`find -L $1`

for link in $links
do
	let dif=($(date '+%s') - $(stat -c '%Y' "$link"))
	
	if ! [[ -e "$link" ]] && [ $dif -ge $week ]
	then
		echo "$link"
	fi
done