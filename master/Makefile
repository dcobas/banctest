LANG=C
CPU=L865

include /acc/src/dsc/co/Make.auto

ifeq ($(CPU),L865)
SPECINC=-I/acc/src/dsc/drivers/pcidrivers/include -I/acc/src/dsc/drivers/pcidrivers/mil1553/include
LDLIBS=/acc/src/dsc/drivers/pcidrivers/mil1553/lib/2.6.24.7-rt27_dbg/libmil1553.a
endif
ifeq ($(CPU),ppc4)
SPECINT= -I/acc/src/dsc/drivers/mil1553vme/include
LDLIBS=-lquick
endif

CFLAGS=-Wall -O3 $(SPECINC)
LDFLAGS=
SRCS = main.c print.c bounce.c broadcst.c buffer.c tst_csr.c csr.c mode_code.c mode_data.c procedures.c choix_rt.c signat.c
SRCS += missing.c
OBJS=$(SRCS:.c=.$(CPU).o)

all:    mdaccept.$(CPU)

mdaccept.$(CPU): $(OBJS)
	@rm -f /tmp/$@ $@
	$(CC) -o /tmp/$@ $^ $(LDFLAGS) $(LOADLIBES) $(LDLIBS)
	@mv /tmp/$@ $@
	rm -f $(OBJS)


clean:
	$(RM) mdacc.h *.o *.$(CPU) $(BAKS)

# Generate global prototype file + check for print format
mdacc.h:
	@cproto -e  $(SPECINC) $(STDFLAGS) $(SRCS) 2>/dev/null | egrep -v print >$@
	@echo '/* print.c */' >>$@
	@echo 'extern void print (int flg, char *str, ...)  __attribute__ ((__format__ (__printf__, 2, 3)));' >>$@

# Regenerate dependacies on includes but not global ones
depend: $(SRCS)
	mkdepend -o .'$$(CPU)'.o -- $(SPECINC) $(STDFLAGS)  -- $(SRCS) 2>/dev/null

# DO NOT DELETE

main.$(CPU).o: 1553def.h mdacc.h
bounce.$(CPU).o: 1553def.h mdacc.h
broadcst.$(CPU).o: 1553def.h mdacc.h
buffer.$(CPU).o: 1553def.h mdacc.h
tst_csr.$(CPU).o: 1553def.h mdacc.h
csr.$(CPU).o: 1553def.h mdacc.h
mode_code.$(CPU).o: 1553def.h mdacc.h
mode_data.$(CPU).o: 1553def.h mdacc.h
procedures.$(CPU).o: 1553def.h mdacc.h
choix_rt.$(CPU).o: 1553def.h mdacc.h
signat.$(CPU).o: 1553def.h mdacc.h
missing.$(CPU).o: 1553def.h
