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

	RbtNode * grandparent(void)
	{
		assert (this->parent != NULL); /* Not the root node */
		assert (this->parent->parent != NULL); /* Not child of root */
		return this->parent->parent;
	}

	RbtNode * sibling(void)
	{
		assert (this->parent != NULL); /* Root node has no sibling */
		if (this == this->parent->left)
		{
			return this->parent->right;
		}
		else
		{
			return this->parent->left;
		}
	}

	RbtNode * uncle(void)
	{
		assert (this->parent != NULL); /* Root node has no uncle */
		assert (this->parent->parent != NULL); /* Children of root have no uncle */
		return this->parent->sibling();
	}

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

	static void verify_property_4(
			RbtNode * n)
	{
		struct RH_verify_property_4
		{
			typedef RH_verify_property_4 Caller;

			RbtNode *stack_n;

			void exec(Caller * caller, RbtNode * n)
			{
				if (n == NULL)
				{
					return;
				}

				// std::cout << "### verify node with key " << n->key << std::endl;

				stack_n = n;

				if (RbtNode::GetColor(n) == RED)
				{
					assert (RbtNode::GetColor(n->left)   == BLACK);
					assert (RbtNode::GetColor(n->right)  == BLACK);
					if (caller != NULL)
					{
						assert (RbtNode::GetColor(caller->stack_n) == BLACK);
					}
				}
				Caller().exec(this, n->left);
				Caller().exec(this, n->right);
			}
		};

		RH_verify_property_4().exec(NULL, n);
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

		void insert(
				KEY const & key,
				VALUE const & value)
		{
			struct RH_insert_recursive
			{
				typedef RH_insert_recursive Caller;

				RbtNode *stack_n;
				Caller *stack_c;

				// TODO: add asserts here (see above)
				inline RbtNode * current(
						Caller * caller)
				{
					return caller->stack_n;
				}

				inline RbtNode * parent(
						Caller * caller)
				{
					return caller->stack_c->stack_n;
				}

				inline RbtNode * grandparent(
						Caller * caller)
				{
					return caller->stack_c->stack_c->stack_n;
				}

				inline RbtNode * sibling(
						Caller * caller)
				{
					RbtNode * parent_node = parent(caller);
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
						Caller * caller)
				{
					return sibling(caller->stack_c);
				}

				void insert_postop2(
						Caller * caller,
						RbtNode * current_node,
						RbtNode *& root)
				{
					current_node->parent->color = BLACK;
					current_node->grandparent()->color = RED;

					if (current_node == current_node->parent->left && current_node->parent == current_node->grandparent()->left)
					{
						current_node->grandparent()->rotate_right(root);
					}
					else
					{
						current_node->grandparent()->rotate_left(root);
					}
				}

				int insert_postop1_l(
						Caller * caller,
						RbtNode *& root)
				{
					int ret = -1;

					if (current(caller)->color == BLACK)
					{
						; /* Tree is still valid */
					}
					else if (RbtNode::GetColor(sibling(caller)) == RED)
					{
						current(caller)->color = BLACK;
						sibling(caller)->color = BLACK;
						parent(caller)->color = RED;

						ret = 2; // do again two levels back in recursion
					}
					else
					{
						if (current(caller) == parent(caller)->right)
						{
							RbtNode * save = current(caller)->left;
							current(caller)->rotate_right(root);
							insert_postop2(caller, save->right, root);
						}
						else
						{
							insert_postop2(caller, current(caller)->left, root);
						}
					}

					return ret;
				}

				int insert_postop1_r(
						Caller * caller,
						RbtNode *& root)
				{
					int ret = -1;

					if (current(caller)->color == BLACK)
					{
						; /* Tree is still valid */
					}
					else if (RbtNode::GetColor(sibling(caller)) == RED)
					{
						current(caller)->color = BLACK;
						sibling(caller)->color = BLACK;
						parent(caller)->color = RED;

						ret = 2; // do again two levels back in recursion
					}
					else
					{
						if (current(caller) == parent(caller)->left)
						{
							RbtNode * save = current(caller)->right;
							current(caller)->rotate_left(root);
							insert_postop2(caller, save->left, root);
						}
						else
						{
							insert_postop2(caller, current(caller)->right, root);
						}
					}

					return ret;
				}

				int insert_postop1(
						Caller * caller,
						RbtNode *& root)
				{
					int ret = -1;

					if (caller->stack_c == NULL)
					{
						current(caller)->color = BLACK;
					}
					else if (parent(caller)->color == BLACK)
					{
						; /* Tree is still valid */
					}
					else if (RbtNode::GetColor(uncle(caller)) == RED)
					{
						parent(caller)->color = BLACK;
						uncle(caller)->color = BLACK;
						grandparent(caller)->color = RED;

						ret = 2; // do again two levels back in recursion
					}
					else
					{
						if (current(caller) == parent(caller)->right && parent(caller) == grandparent(caller)->left)
						{
							parent(caller)->rotate_left(root);
							insert_postop2(caller, current(caller)->left, root);
						}
						else if (current(caller) == parent(caller)->left && parent(caller) == grandparent(caller)->right)
						{
							parent(caller)->rotate_right(root);
							insert_postop2(caller, current(caller)->right, root);
						}
						else
						{
							insert_postop2(caller, current(caller), root);
						}
					}

					return ret;
				}

				int exec(
					Caller * caller,
					RbtNode *& root_node,
					RbtNode * current_node,
					RbtNode * node_to_insert)
				{
					int ret = -1;

					stack_n = current_node;
					stack_c = caller;

					int comp_result = RbtNode::compare(node_to_insert->key, current_node->key);
					if (comp_result == 0)
					{
						current_node->value = node_to_insert->value;
						/* inserted_node isn't going to be used, don't leak it */
						delete node_to_insert;
					}
					else if (comp_result < 0)
					{
						if (current_node->left == NULL)
						{
							stack_n->left = node_to_insert;
							node_to_insert->parent = stack_n;	// TODO: temporary
							ret = insert_postop1_l(this, root_node);
						}
						else
						{
							ret = Caller().exec(this, root_node, stack_n->left, node_to_insert);
						}
					}
					else
					{
						if (current_node->right == NULL)
						{
							stack_n->right = node_to_insert;
							node_to_insert->parent = stack_n;	// TODO: temporary
							ret = insert_postop1_r(this, root_node);
						}
						else
						{
							ret = Caller().exec(this, root_node, stack_n->right, node_to_insert);
						}
					}

					if (1 == ret)
					{
						ret = insert_postop1(this, root_node);
					}
					else if (ret >= 0)
					{
						ret = ret - 1;
					}

					return ret;
				}
			};

			RbtNode * node_to_insert = new RbtNode(key, value);

			if (this->root == NULL)
			{
				this->root = node_to_insert;
				this->root->color = BLACK;
			}
			else
			{
				RH_insert_recursive().exec(NULL, this->root, this->root, node_to_insert);
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
