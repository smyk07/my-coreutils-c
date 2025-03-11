all: cat echo tail

cat: cat.c
	gcc -g cat.c -o cat

echo: echo.c
	gcc -g echo.c -o echo

tail: tail.c
	gcc -g tail.c -o tail
