all: cat echo

cat: cat.c
	gcc cat.c -o cat

echo: echo.c 
	gcc echo.c -o echo
