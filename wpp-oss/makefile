.PHONY: all 

CC = cc
CFLAGS = -std=c90 -Wall -Wextra -Wpedantic -Werror -Os -ggdb
LDFLAGS =
BUILDDIR = build/

all: $(BUILDDIR) $(BUILDDIR)wpp
clean:
	rm $(BUILDDIR)*.o $(BUILDDIR)wpp

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)wpp: $(BUILDDIR)wpp.o $(BUILDDIR)lexer.o $(BUILDDIR)arena.o $(BUILDDIR)exec.o $(BUILDDIR)object.o $(BUILDDIR)exec_print.o $(BUILDDIR)exec_fn.o $(BUILDDIR)exec_var.o $(BUILDDIR)exec_copy.o $(BUILDDIR)exec_scanln.o $(BUILDDIR)exec_ret_stack.o $(BUILDDIR)exec_obj.o $(BUILDDIR)exec_misc.o $(BUILDDIR)exec_cast.o $(BUILDDIR)exec_namespace.o $(BUILDDIR)exec_if.o $(BUILDDIR)exec_loop.o
	$(CC) $(LDFLAGS) -o $@ $^

# objects
$(BUILDDIR)wpp.o: wpp.c lexer.h exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR)lexer.o: lexer.c lexer.h wpp.h cfwd.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR)arena.o: arena.c arena.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR)exec.o: exec.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR)exec_print.o: exec_print.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR)exec_fn.o: exec_fn.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR)exec_var.o: exec_var.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR)exec_copy.o: exec_copy.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR)exec_scanln.o: exec_scanln.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR)exec_ret_stack.o: exec_ret_stack.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR)exec_obj.o: exec_obj.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR)exec_misc.o: exec_misc.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR)exec_cast.o: exec_cast.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR)exec_namespace.o: exec_namespace.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR)exec_if.o: exec_if.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR)exec_loop.o: exec_loop.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR)object.o: object.c object.h
	$(CC) -c -o $@ $(CFLAGS) $<

# headers
lexer.h: arena.h
	@touch $@

exec.h: lexer.h object.h wpp.h cfwd.h
	@touch $@
