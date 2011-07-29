valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes  -v   --log-file-exactly=memleak.log bin/Toirt_Samhlaigh -size 64x64x64 -file ../data/neghip.raw 
