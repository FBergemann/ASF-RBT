
INSTALL_FILES=\
	asf_rbt_test \
	rbt_test
	
all : $(INSTALL_FILES)

clean :
	rm -f $(INSTALL_FILES)

test : $(INSTALL_FILES)
	./asf_rbt_test > out1
	./rbt_test > out2
	diff out1 out2
	
asf_rbt_test : main.cpp
	$(CXX) -o $@ $<
	
rbt_test : rbtree.c testmain.c
	$(CC) -o $@ $+
