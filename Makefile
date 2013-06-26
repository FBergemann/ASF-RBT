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
	
asf_rbt_test : asf_rbtree_test.cc asf_rbtree.h
	$(CXX) -o $@ $<
	
rbt_test : rbtree.c rbtree_test.c
	$(CC) -o $@ $+
