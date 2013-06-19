INSTALL_FILES=\
	asf_rbt_test
	
all : $(INSTALL_FILES)

delete :
	rm -f $(INSTALL_FILE)
	
asf_rbt_test : main.cpp
	$(CXX) -o $@ $<