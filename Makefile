all:
	cc -Weverything -Wno-padded lstailq.c 
	./a.out

debug:
	lldb ./a.out
