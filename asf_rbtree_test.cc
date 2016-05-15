#include <assert.h>
#include <cstdlib> // rand()
#include <iostream>

#define VERIFY_RBTREE
#define TRACE

#include "asf_rbtree.h"

int main(
		int,
		char**)
{
	typedef RbtNode<int, int> Node;

	{
		std::cout << "=== TREE 1 - BEGIN ===" << std::endl;

		Node::Tree tree;

		for(int i=0; i<20; i++)
		{
			int x = rand() % 10000;
			int y = rand() % 10000;
			// printf("Inserting %d -> %d\n\n", x, y);
			tree.insert(x, y);
			Node * node = tree.lookup(x);
			assert(node != NULL);
			assert(node->value == y);
			// tree.print();
		}
		tree.print();

		std::cout << "=== TREE 1 - END ===" << std::endl << std::endl;
	}

	{
		std::cout << "=== TREE 2 - BEGIN ===" << std::endl;

		Node::Tree tree;

		for(int i=0; i<20; i++)
		{
			int x = i;
			int y = i;
			// printf("Inserting %d -> %d\n\n", x, y);
			tree.insert(x, y);
			Node * node = tree.lookup(x);
			assert(node != NULL);
			assert(node->value == y);
			// tree.print();
		}
		tree.print();

		std::cout << "=== TREE 2 - END ===" << std::endl << std::endl;
	}

	{
		std::cout << "=== TREE 3 - BEGIN ===" << std::endl;

		Node::Tree tree;

		for(int i=0; i<20; i++)
		{
			int x = 19-i;
			int y = 19-i;
			// printf("Inserting %d -> %d\n\n", x, y);
			tree.insert(x, y);
			Node * node = tree.lookup(x);
			assert(node != NULL);
			assert(node->value == y);
			// tree.print();
		}
		tree.print();

		std::cout << "=== TREE 3 - END ===" << std::endl << std::endl;
	}

#if 0
	for (int i=0; i<60000; i++)
	{
		int x = rand() % 10000;
#ifdef TRACE
		print_tree(t);
		printf("Deleting key %d\n\n", x);
#endif
		rbtree_delete(t, (void*)x, compare_int);
	}
#endif

	return 0;
}
