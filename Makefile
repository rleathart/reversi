.PHONY: clean Clean run

PROGNAME = Reversi

$(PROGNAME): main.o
	$(CC) main.o -o $(PROGNAME)

clean:
	rm -f *.o
Clean: clean
	rm -f $(PROGNAME) save.dat

run: $(PROGNAME)
	./$(PROGNAME)
