
INSTALL_FILES=\
	asf_rbt_test
	
all : $(INSTALL_FILES)

clean :
	rm -f $(INSTALL_FILES)

test : $(INSTALL_FILES)
	./asf_rbt_test
	
asf_rbt_test : main.cpp
	$(CXX) -o $@ $<
