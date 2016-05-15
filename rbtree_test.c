/* Copyright (c) 2013 the authors listed at the following URL, and/or
the authors of referenced articles or incorporated external code:
http://en.literateprograms.org/Red-black_tree_(C)?action=history&offset=20120524204657

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Retrieved from: http://en.literateprograms.org/Red-black_tree_(C)?oldid=18555
*/

/*
 * ATTENTION:
 *
 *  This module is just a demo
 *  It stores long values directly at void *
 *  instead of allocating and releasing memory for long values
 *
 */

#include "rbtree.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h> /* rand() */

#define INDENT_STEP	(2)
#undef TRACE

static int compare_long(
		void * leftp,
		void * rightp)
{
	long left  = (long)leftp;
	long right = (long)rightp;

	if (left < right)
	{
		return -1; /* early exit */
	}
	else if (left > right)
	{
    	return 1; /* early exit */
	}

	assert (left == right);
	return 0;
}

static void print_tree_helper(
		rbtree_node n,
		int indent)
{
	int i;

	if (NULL == n)
	{
    	fputs("<empty tree>", stdout);
        return; /* early exit */
	}

	if (n->right != NULL)
	{
		print_tree_helper(n->right, indent + INDENT_STEP);
	}

	for(i=0; i<indent; i++)
	{
    	fputs(" ", stdout);
	}

	if (n->color == RED)
	{
    	printf("%ld\n", (long)n->key);
	}
	else
	{
    	printf("<%ld>\n", (long)n->key);
	}

	if (n->left != NULL)
	{
    	print_tree_helper(n->left, indent + INDENT_STEP);
	}
}

static void print_tree(
		rbtree t)
{
	print_tree_helper(t->root, 0);
}

int main()
{
	int i;
	rbtree t = NULL;

	{
		puts("=== TREE 1 - BEGIN ===");
		t = rbtree_create();

		for (i=0; i<20; i++)
		{
			long x = rand() % 10000;
			long y = rand() % 10000;
#ifdef TRACE
			print_tree(t);
			printf("Inserting %d -> %d\n\n", x, y);
#endif
			rbtree_insert(t, (void*)x, (void*)y, compare_long);
			assert(rbtree_lookup(t, (void*)x, compare_long) == (void*)y);
		}

		print_tree(t);
		puts("");

		free(t);

		puts("=== TREE 1 - END ===\n");
	}

	{
		puts("=== TREE 2 - BEGIN ===");

		t = rbtree_create();

		for(i=0; i<20; i++)
		{
			long x = i;
			long y = i;
#ifdef TRACE
			print_tree(t);
			printf("Inserting %d -> %d\n\n", x, y);
#endif
			rbtree_insert(t, (void*)x, (void*)y, compare_long);
			assert(rbtree_lookup(t, (void*)x, compare_long) == (void*)y);
		}

		print_tree(t);
		puts("");

		free(t);

		puts("=== TREE 2 - END ===\n");
	}

	{
		puts("=== TREE 3 - BEGIN ===");

		t = rbtree_create();

		for(i=0; i<20; i++)
		{
			long x = 19 - i;
			long y = 19 - i;
#ifdef TRACE
			print_tree(t);
			printf("Inserting %d -> %d\n\n", x, y);
#endif
			rbtree_insert(t, (void*)x, (void*)y, compare_long);
			assert(rbtree_lookup(t, (void*)x, compare_long) == (void*)y);
		}

		print_tree(t);
		puts("");

#if 0
		for(i=0; i<60000; i++)
		{
			int x = rand() % 10000;
#ifdef TRACE
			print_tree(t);
			printf("Deleting key %d\n\n", x);
#endif
			rbtree_delete(t, (void*)x, compare_long);
		}
#endif

		free(t);

		puts("=== TREE 3 - END ===\n");

	}

    return 0;
}
