#!/bin/bash
echo hello

for i in 1 2 3 4
do
	echo $i
done

for i in `ls`
do
	echo here $i
done

s1 = "abacaba"
s2 = "abacaba"

if test "$s1" = "$s2"
then
	s2 = "kek"
	echo ok
	echo $s1
	echo $s2
fi

let A=123
echo value $((3 + 1 - 2 + $A))

square() {
	echo sq $(($1 * $1))
}

for i in 1 2 3
do
	square $i
done

for i in 1 2 3 4
do
	case $i in
		1)
			echo case one
			;;
		2)
			echo case two
			;;
		*)
			echo case too big
			;;
	esac
done

#while ####return 0 -- true
#do
	
#done
