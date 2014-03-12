#!/bin/sh

errors=0
port=0
address=0x01
register=0xE4
total=0
while [ $total -lt 1000 ];
do
  for i in `seq 10 80`;
  do
    pattern="0x$i 0x$(($i+1)) 0x$(($i+2)) 0x$(($i+3))"
    ./kmodtest $port 6 "$address $register $pattern" 0
    response=`./kmodtest $port 2 "$address $register" 4 | sed 's/ $//'`
    if [ "x$pattern" != "x$response" ];
    then
      echo -n "!"
      errors=$(($errors+1))
    else
      echo -n "."
    fi

    total=$(($total+1))
  done
done
echo
echo "report:"
echo "total: $total"
echo "errors: $errors"

