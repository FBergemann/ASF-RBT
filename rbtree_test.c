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


static void
delete_helper(
		rbtree * t,
		long key)
{
	printf("del(%ld)\n", key);
	rbtree_node returned = rbtree_delete(*t, (void*)key, compare_long);
	printf ("return value = %ld\n", (long)returned->key);;
	free(returned);

	print_tree(*t);
	puts("");
}


int main()
{
	int i;
	rbtree t = NULL;

	{
		puts("=== TREE 0 (empty) - BEGIN ===");

		t = rbtree_create();

		print_tree(t);
		puts("");

		rbtree_delete(t, (void*)19L, compare_long); /* check, it doesn't harm */

		print_tree(t);
		puts("");

		free(t);

		puts("=== TREE 0 (empty) - END ===\n");
	}

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
			long y = (19 - i) * 10;
#ifdef TRACE
			print_tree(t);
			printf("Inserting %d -> %d\n\n", x, y);
#endif
			rbtree_insert(t, (void*)x, (void*)y, compare_long);
			assert(rbtree_lookup(t, (void*)x, compare_long) == (void*)y);
		}

		print_tree(t);
		puts("");

		/*
		 * some lookup tests
		 * calling #2 times, because of #2 different implementations for C++
		 * (to compare the outputs)
		 */

		{
			void * node = rbtree_lookup(t, (void*)19L, compare_long);

			if (NULL == node)
			{
				printf("lookup_it(19) = <not found>\n");
			}
			else
			{
				printf("lookup_it(19) = %ld\n", (long) node);
			}
		}

		{
			void * node = rbtree_lookup(t, (void*)19L, compare_long);

			if (NULL == node)
			{
				printf("lookup(19) = <not found>\n");
			}
			else
			{
				printf("lookup(19) = %ld\n", (long) node);
			}
		}

		/*
		 * some del(ete) tests
		 */

		delete_helper(&t, 8L);
		delete_helper(&t, 7L);
		delete_helper(&t, 12L);
		delete_helper(&t, 19L);
		delete_helper(&t, 18L);
		delete_helper(&t, 17L);
		delete_helper(&t, 11L);
		delete_helper(&t, 0L);

#if 0
		/*
		 * TODO: this has to be changed, it can't be compared with C++-solution
		 */
		for (i = 0; i < 60000; i++)
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
