/*
 * main.cpp
 *
 *  Created on: Jun 13, 2013
 *      Author: frank
 */

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <iomanip>

#define VERIFY_RBTREE
#define TRACE

// TODO: add support for rvalue references
template <typename KEY, typename VALUE>
struct RbtNode {

    enum Color { RED, BLACK };

	KEY key;

	VALUE value;

	Color color;

	RbtNode* left;

	RbtNode* right;

	RbtNode* parent;    // TODO: make obsolete

	RbtNode(
			KEY const & key,
			VALUE const & value)
	: key(key),
	  value(value),
	  color(RED),
	  left(NULL),
	  right(NULL),
	  parent(NULL)
	{  }

	void print(
			int indent)
	{
		const int indent_step = 2;

		if (this->right != NULL)
		{
			this->right->print(indent + indent_step);
		}


		std::cout << std::string(indent, ' ');

		if (this->color == RED)
		{
			std::cout << this->key << std::endl;
		}
		else
		{
        		std::cout << '<' << this->key << '>' << std::endl;
		}

		if (this->left != NULL)
		{
        		this->left->print(indent + indent_step);
		}
	}

	static Color GetColor(
			RbtNode * n)
	{
		return (n == NULL)?BLACK:n->color;
	}

#if 1 // TODO: make obsolete
	RbtNode * grandparent(void) // TODO: make obsolete
	{
		assert (this->parent != NULL); /* Not the root node */
		assert (this->parent->parent != NULL); /* Not child of root */
		return this->parent->parent;
	}
#endif
	// Recursion Helper Base Class
	template <typename CALLER>
	struct RH_Base
	{
		CALLER *stack_c;
		RbtNode *stack_n;

		RH_Base(
				CALLER * caller,
				RbtNode * node)
		: stack_c(caller),
		  stack_n(node)
		{ }

		inline RbtNode * current(void)
		{
			return stack_n;
		}

		inline RbtNode * parent(void)
		{
			assert(stack_c != NULL);
			return stack_c->stack_n;
		}

		inline RbtNode * grandparent(void)
		{
			assert(stack_c != NULL);
			assert(stack_c->stack_c != NULL);
			return stack_c->stack_c->stack_n;
		}

		inline RbtNode * sibling(void)
		{
			RbtNode * parent_node = parent();

			assert(parent_node != NULL);

			if (current() == parent_node->left)
			{
				return parent_node->right;
			}
			else
			{
				return parent_node->left;
			}
		}

		inline RbtNode * uncle(void)
		{
			RbtNode * grandparent_node = grandparent();

			if (parent() == grandparent_node->left)
			{
				return grandparent_node->right;
			}
			else
			{
				return grandparent_node->left;
			}
		}

		inline RbtNode * current(
				CALLER * caller)
		{
			assert(caller != NULL);
			return caller->stack_n;
		}

		inline RbtNode * parent(
				CALLER * caller)
		{
			assert(caller != NULL);
			return caller->stack_c->stack_n;
		}

		inline RbtNode * grandparent(
				CALLER * caller)
		{
			assert(caller != NULL);
			assert(caller->stack_c != NULL);
			return caller->stack_c->stack_c->stack_n;
		}

		inline RbtNode * sibling(
				CALLER * caller)
		{
			RbtNode * parent_node = parent(caller);

			assert(parent_node != NULL);

			if (current(caller) == parent_node->left)
			{
				return parent_node->right;
			}
			else
			{
				return parent_node->left;
			}
		}

		inline RbtNode * uncle(
				CALLER * caller)
		{
			assert(caller != NULL);
			return sibling(caller->stack_c);
		}
	};

	void replace_node(
			RbtNode *& root,
			RbtNode * newn)
	{
		if (this->parent == NULL)
		{
			root = newn;
		}
		else
		{
			if (this == this->parent->left)
			{
				this->parent->left = newn;
			}
			else
			{
				this->parent->right = newn;
			}
		}
		if (newn != NULL)
		{
			newn->parent = this->parent;
		}
	}

	void rotate_left(
			RbtNode *& root)
	{
		RbtNode * R = this->right;
		this->replace_node(root, R);
		this->right = R->left;
		if (R->left != NULL)
		{
			R->left->parent = this;
		}
		R->left = this;
		this->parent = R;
	}

	void rotate_right(
			RbtNode *& root)
	{
		RbtNode * L = this->left;
		this->replace_node(root, L);
		this->left = L->right;
		if (L->right != NULL)
		{
			L->right->parent = this;
		}
		L->right = this;
		this->parent = L;
	}

	static int compare(
			KEY const & left,
			KEY const & right)
	{
		if (left == right)
		{
			return 0;
		}
		else if (left < right)
		{
			return -1;
		}
		return 1;
	}

	static void verify_property_1(
			RbtNode * n)
	{
		if (n == NULL)
		{
			return;
		}
		verify_property_1(n->left);
		verify_property_1(n->right);
	}

	static void verify_property_2(
			RbtNode * root)
	{
		assert(GetColor(root) == BLACK);
	}

	// Recursion Helper for verify_property_4()
	struct RH_verify_property_4 : RH_Base <RH_verify_property_4>
	{
		typedef RH_verify_property_4 Caller;

		typedef RH_Base<RH_verify_property_4> base_type;

		RH_verify_property_4(
				Caller * caller,
				RbtNode * node)
		: base_type(caller, node)
		{ }

		void exec(void)
		{
			if (this->stack_n == NULL)
			{
				return;
			}

			// std::cout << "### verify node with key " << n->key << std::endl;

			if (RbtNode::GetColor(this->stack_n) == RED)
			{
				assert (RbtNode::GetColor(this->stack_n->left)   == BLACK);
				assert (RbtNode::GetColor(this->stack_n->right)  == BLACK);
				if (this->stack_c != NULL)
				{
					assert (RbtNode::GetColor(this->stack_c->stack_n) == BLACK);
				}
			}
			Caller(this, this->stack_n->left).exec();
			Caller(this, this->stack_n->right).exec();
		}
	};

	static void verify_property_4(
			RbtNode * n)
	{

		RH_verify_property_4(NULL, n).exec();
	}

	static void verify_property_5_helper(
			RbtNode * n,
			int black_count,
			int* path_black_count)
	{
		if (GetColor(n) == BLACK)
		{
			black_count++;
		}
		if (n == NULL)
		{
			if (*path_black_count == -1)
			{
				*path_black_count = black_count;
			}
			else
			{
				assert (black_count == *path_black_count);
			}
			return;
		}
		verify_property_5_helper(n->left,  black_count, path_black_count);
		verify_property_5_helper(n->right, black_count, path_black_count);
	}

	static void verify_property_5(
			RbtNode * n)
	{
		int black_count_path = -1;
		verify_property_5_helper(n, 0, &black_count_path);
	}

	struct Tree
	{
		RbtNode * root;

		Tree()
		: root(NULL)
		{ }

		void print(void)
		{
			if (root == NULL)
			{
				std::cout << "<empty tree>" << std::endl;
				return;
			}

			this->root->print(0);
			std::cout << std::endl;
		}

		void verify_properties(void)
		{
#ifdef VERIFY_RBTREE
			verify_property_1(this->root);
			verify_property_2(this->root);
			/* Property 3 is implicit */
			verify_property_4(this->root);
			verify_property_5(this->root);
#endif
		}


		// Recursion Helper for insert operation
		struct RH_insert : public RH_Base<RH_insert>
		{
			typedef RH_insert Caller;

			typedef RH_Base<RH_insert> base_type;

			RH_insert(
					Caller * caller,
					RbtNode * node)
			: base_type(caller, node)
			{ }

			void insert_postop2(
					RbtNode *& root)
			{
				this->parent()->color = BLACK;
				this->grandparent()->color = RED;

				if (this->current() == this->parent()->left && this->parent() == this->grandparent()->left)
				{
					this->grandparent()->rotate_right(root);
				}
				else
				{
					this->grandparent()->rotate_left(root);
				}
			}

			int insert_postop1_l(
					RbtNode *& root)
			{
				int ret = -1;

				if (this->current()->color == BLACK)
				{
					; /* Tree is still valid */
				}
				else if (RbtNode::GetColor(this->sibling()) == RED)
				{
					this->current()->color = BLACK;
					this->sibling()->color = BLACK;
					this->parent()->color = RED;

					ret = 2; // do again two levels back in recursion
				}
				else
				{
					if (this->current() == this->parent()->right)
					{
						RbtNode *save = this->current()->left;
						this->current()->rotate_right(root);
						std::swap(this->stack_n, save);
						Caller(this, save).insert_postop2(root);
					}
					else
					{
						Caller(this, this->current()->left).insert_postop2(root);
					}
				}

				return ret;
			}

			int insert_postop1_r(
					RbtNode *& root)
			{
				int ret = -1;

				if (this->current()->color == BLACK)
				{
					; /* Tree is still valid */
				}
				else if (RbtNode::GetColor(this->sibling()) == RED)
				{
					this->current()->color = BLACK;
					this->sibling()->color = BLACK;
					this->parent()->color = RED;

					ret = 2; // do again two levels back in recursion
				}
				else
				{
					if (this->current() == this->parent()->left)
					{
						RbtNode *save = this->current()->right;
						this->current()->rotate_left(root);
						std::swap(this->stack_n, save);
						Caller(this, save).insert_postop2(root);
					}
					else
					{
						Caller(this, this->current()->right).insert_postop2(root);
					}
				}

				return ret;
			}

			int insert_postop1(
					RbtNode *& root)
			{
				int ret = -1;

				if (this->stack_c == NULL)
				{
					this->current()->color = BLACK;
				}
				else if (this->parent()->color == BLACK)
				{
					; /* Tree is still valid */
				}
				else if (RbtNode::GetColor(this->uncle()) == RED)
				{
					this->parent()->color = BLACK;
					this->uncle()->color = BLACK;
					this->grandparent()->color = RED;

					ret = 2; // do again two levels back in recursion
				}
				else
				{
					if (this->current() == this->parent()->right && this->parent() == this->grandparent()->left)
					{
						this->parent()->rotate_left(root);
						Caller(this, this->current()->left).insert_postop2(root);
					}
					else if (this->current() == this->parent()->left && this->parent() == this->grandparent()->right)
					{
						this->parent()->rotate_right(root);
						Caller(this, this->current()->right).insert_postop2(root);
					}
					else
					{
						insert_postop2(root);
					}
				}

				return ret;
			}

			int exec(
				RbtNode *& root_node,
				RbtNode * node_to_insert)
			{
				int ret = -1;

				int comp_result = RbtNode::compare(node_to_insert->key, this->stack_n->key);
				if (comp_result == 0)
				{
					this->stack_n->value = node_to_insert->value;
					/* inserted_node isn't going to be used, don't leak it */
					delete node_to_insert;
				}
				else if (comp_result < 0)
				{
					if (this->stack_n->left == NULL)
					{
						this->stack_n->left = node_to_insert;
						node_to_insert->parent = this->stack_n;	// TODO: temporary
						ret = insert_postop1_l(root_node);
					}
					else
					{
						ret = Caller(this, this->stack_n->left).exec(root_node, node_to_insert);
					}
				}
				else
				{
					if (this->stack_n->right == NULL)
					{
						this->stack_n->right = node_to_insert;
						node_to_insert->parent = this->stack_n;	// TODO: temporary
						ret = insert_postop1_r(root_node);
					}
					else
					{
						ret = Caller(this, this->stack_n->right).exec(root_node, node_to_insert);
					}
				}

				if (1 == ret)
				{
					ret = insert_postop1(root_node);
				}
				else if (ret >= 0)
				{
					ret = ret - 1;
				}

				return ret;
			}
		};

		void insert(
				KEY const & key,
				VALUE const & value)
		{

			RbtNode * node_to_insert = new RbtNode(key, value);

			if (this->root == NULL)
			{
				this->root = node_to_insert;
				this->root->color = BLACK;
			}
			else
			{
				RH_insert(NULL, this->root).exec(this->root, node_to_insert);
			}

			verify_properties();

		}

	};

};

int main(
	int,
	char**)
{
	RbtNode<int, int>::Tree tree;

	for(int i=0; i<20; i++)
	{
		int x = rand() % 10000;
		int y = rand() % 10000;
		printf("Inserting %d -> %d\n\n", x, y);
		tree.insert(x, y);
		// assert(tree->lookup(x) == y);
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
