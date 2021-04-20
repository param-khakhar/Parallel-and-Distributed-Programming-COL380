.PHONY := execute clean

compile:
	@(cd src; $(MAKE))
	@(cd obj; $(MAKE))

execute:
	@(cd bin; $(MAKE))

distributed:
	@(cd bin; make -s distribute)

test: 
	@(cd bin; make -s tests)

diagonal:
	@(cd src; make -s generate)
	@(cd bin; make -s generate)

clean: 
	@rm -f obj/main.o
	@rm -f bin/A2
	


		
