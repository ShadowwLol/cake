EXE="cake"
DEFINES=""
DDEFINES="-D_CAKE_DEBUG"

case "$1" in
"--debug"|"-d")
printf "[+] Debug mode\n"
CFLAGS="-O -g -std=c99 $DDEFINES -Wextra -D_FORTIFY_SOURCE=2 -D_GLIBCXX_ASSERTIONS -fstack-clash-protection -Wshadow -Wformat=2 -Wformat-truncation -Wformat-overflow -fno-common -fstack-usage"
;;
*)
printf "[+] Release mode\n"
CFLAGS="-static-libgcc -std=c99 $DEFINES -O3 -Wextra -D_FORTIFY_SOURCE=2 -D_GLIBCXX_ASSERTIONS -fstack-clash-protection -Wshadow -Wformat=2 -Wformat-truncation -Wformat-overflow -fno-common -fstack-usage"
;;
esac

case "$2" in
"windows"|"win")
printf "[#] Windows\n"
CC=i686-w64-mingw32-gcc
;;
*)
printf "[+] GNU/Linux\n"
CC=gcc
;;
esac

if [ ! -d "buildsh/" ]; then
	mkdir "buildsh/"
fi

cd buildsh

cfiles=$(find ../../src/*.c)

printf "[+] Compiling:\n"

for cfile in $cfiles; do
#printf "\t* [$cfile]\n"
compiled=${cfile%.*}.o
compiled=${compiled#"../../src/"}

if [[ -s $compiled ]]; then
	#file exists and is not empty!
	csource=${compiled%.*}.c
	STATUS="$(cmp --silent $csource $cfile; echo $?)"
	if [[ $STATUS -ne 0 ]]; then
		# files differ
		cp $cfile -r .
		printf "$CC -c $cfile $CFLAGS -o $compiled\n"
		$CC -c $cfile $CFLAGS -o $compiled
	fi
else
	cp $cfile -r .
	printf "$CC -c $cfile $CFLAGS -o $compiled\n"
	$CC -c $cfile $CFLAGS -o $compiled
fi

done

cd ..

printf "$CC buildsh/*.o -o $EXE\n"
$CC $(find buildsh/*.o) -o "$EXE"
