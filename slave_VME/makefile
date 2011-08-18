#
CFLAGS =  -ix -M=20K -V=/h0/DEFS

OBJS   = main.r testbuff.r testcsr.r testremote.r

make: $(OBJS)
   cc $(CFLAGS) $(OBJS) -l=/h0/USR/charrue/1553/lib/libmil.l -f=VMEos9

