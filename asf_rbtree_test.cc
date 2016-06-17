#include <assert.h>
#include <cstdlib> // rand()
#include <iostream>
#include <sstream>

#define VERIFY_RBTREE
#define TRACE			1 // #0: off, higher value, more verbose

#include "asf_rbtree.h"

typedef RbtNode<int, int> Node;

static void
delete_helper(
		Node::Tree & tree,
		int key)
{
	std::cout << "del(" << key << ")" << std::endl;
	Node * returned = tree.del(key);
	std::cout << "return value = " << returned->key << std::endl;
	delete returned;
	tree.print();
}

int main(
		int,
		char**)
{

	{
		std::cout << "=== TREE 0 (empty) - BEGIN ===" << std::endl;
		Node::Tree tree;
		tree.print();
		tree.del(19L); // check, it doesn't harm
		tree.print();
		std::cout << "=== TREE 0 (empty) - END ===" << std::endl << std::endl;
	}

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
			int x = 19 - i;
			int y = (19 - i) * 10;
			// printf("Inserting %d -> %d\n\n", x, y);
			tree.insert(x, y);
			Node * node = tree.lookup(x);
			assert(node != NULL);
			assert(node->value == y);
			// tree.print();
		}
		tree.print();

		/*
		 * some lookup tests
		 */

		{
			Node * node = tree.lookup_it(19);
			std::string value = "<not found>";
			if (NULL != node)
			{
				std::stringstream oss;
				oss << node->value;
				value = oss.str();
			}

			std::cout << "lookup_it(19) = " << value << std::endl;
		}

		{
			Node * node = tree.lookup(19);
			std::string value = "<not found>";
			if (NULL != node)
			{
				std::stringstream oss;
				oss << node->value;
				value = oss.str();
			}

			std::cout << "lookup(19) = " << value << std::endl;
		}

		/*
		 * some del(ete) tests
		 */

		delete_helper(tree, 8);
		delete_helper(tree, 7);
		delete_helper(tree, 12);
		delete_helper(tree, 19);

		delete_helper(tree, 18);
		delete_helper(tree, 17);
		delete_helper(tree, 11);
		delete_helper(tree, 0);

#if 0
		/*
		 * TODO: this has to be changed, it can't be compared with C-solution
		 */
		for (int i = 0; i < 60000; i++)
		{
			int x = rand() % 10000;
#ifdef TRACE
			print_tree(t);
			printf("Deleting key %d\n\n", x);
#endif
			rbtree_delete(t, (void*)x, compare_int);
		}
#endif

		std::cout << "=== TREE 3 - END ===" << std::endl << std::endl;
	}


	return 0;
}
