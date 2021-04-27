 hexdump -s `hexdump $1 -s 10 -n 1 -e '/4 "%d"'` -v -e '16/1 "0x%02XU," "\n"' $1
