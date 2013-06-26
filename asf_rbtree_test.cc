#include <cstdlib> // rand()

#define VERIFY_RBTREE
#define TRACE

#include "asf_rbtree.h"

int main(
  int,
	char**)
{
	{
		RbtNode<int, int>::Tree tree;

		for(int i=0; i<20; i++)
		{
			int x = rand() % 10000;
			int y = rand() % 10000;
			// printf("Inserting %d -> %d\n\n", x, y);
			tree.insert(x, y);
			// assert(tree->lookup(x) == y);
			// tree.print();
		}
		tree.print();
	}
	{
		RbtNode<int, int>::Tree tree;

		for(int i=0; i<20; i++)
		{
			int x = i;
			int y = i;
			// printf("Inserting %d -> %d\n\n", x, y);
			tree.insert(x, y);
			// assert(tree->lookup(x) == y);
			// tree.print();
		}
		tree.print();
	}
	{
		RbtNode<int, int>::Tree tree;

		for(int i=0; i<20; i++)
		{
			int x = 19-i;
			int y = 19-i;
			// printf("Inserting %d -> %d\n\n", x, y);
			tree.insert(x, y);
			// assert(tree->lookup(x) == y);
			// tree.print();
		}
		tree.print();
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
