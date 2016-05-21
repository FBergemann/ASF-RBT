/*
 *  asf_rbtree.h
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

// TODO: add support for rvalue references
template <typename KEY, typename VALUE>
struct RbtNode {

    enum Color { RED, BLACK };

	KEY key;

	VALUE value;

	Color color;

	RbtNode* left;

	RbtNode* right;

	RbtNode(
			KEY const & key,
			VALUE const & value)
	: key(key),
	  value(value),
	  color(RED),
	  left(NULL),
	  right(NULL)
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

	void MakeRed(void)
	{
	    this->color = RED;
	    if (this->right != NULL)
	    {
	        this->right->color = BLACK;
	    }
	    if (this->left != NULL)
	    {
	        this->left->color = BLACK;
	    }
	}

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

		inline RbtNode * & current(void)
		{
			return stack_n;
		}

		inline RbtNode * & parent(void)
		{
			assert(stack_c != NULL);
			return stack_c->stack_n;
		}

		inline RbtNode * & grandparent(void)
		{
			assert(stack_c != NULL);
			assert(stack_c->stack_c != NULL);
			return stack_c->stack_c->stack_n;
		}

		inline RbtNode * & sibling(void)
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

		inline RbtNode * & uncle(void)
		{
			RbtNode * grandparent_node = grandparent();

			assert(grandparent_node != NULL);

			if (parent() == grandparent_node->left)
			{
				return grandparent_node->right;
			}
			else
			{
				return grandparent_node->left;
			}
		}

		inline RH_Base * current_caller(void)
		{
			return (RH_Base*)this;
		}

		inline RH_Base * parent_caller(void)
		{
			return stack_c;
		}

		inline RH_Base * grandparent_caller(void)
		{
			assert(stack_c != NULL);
			return stack_c->stack_c;
		}

		/**
		 * get the maximum value of current position down the tree.
		 * it's the leaf at most right position
		 */
		inline RbtNode * maximum_node(void)
		{
			RbtNode *n = stack_n;
			while (NULL != n->right)
			{
				n = n->right;
			}
			return n;
		}

		void replace_node(
				RH_Base * caller,
				RbtNode * newn,
				RbtNode *& root)
		{
			if (caller->stack_c == NULL)
			{
				root = newn;
			}
			else if (caller->stack_n == caller->stack_c->stack_n->left)
			{
				caller->stack_c->stack_n->left = newn;
			}
			else
			{
				caller->stack_c->stack_n->right = newn;
			}
		}

		void rotate_left(
				RH_Base * caller,
				RbtNode *& root)
		{
			RbtNode * R = caller->stack_n->right;
			replace_node(caller, R, root);
			caller->stack_n->right = R->left;
			R->left = caller->stack_n;
		}

		void rotate_right(
				RH_Base * caller,
				RbtNode *& root)
		{
			RbtNode * L = caller->stack_n->left;
			replace_node(caller, L, root);
			caller->stack_n->left = L->right;
			L->right = caller->stack_n;
		}
	};

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
		if (NULL == n)
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

			if (RbtNode::GetColor(this->current()) == RED)
			{
				assert (RbtNode::GetColor(this->current()->left)   == BLACK);
				assert (RbtNode::GetColor(this->current()->right)  == BLACK);
				if (this->stack_c != NULL)
				{
					assert (RbtNode::GetColor(this->stack_c->stack_n) == BLACK);
				}
			}
			Caller(this, this->current()->left).exec();
			Caller(this, this->current()->right).exec();
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


		// Recursion helper for insert operation
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
					this->rotate_right(this->grandparent_caller(), root);
				}
				else
				{
					this->rotate_left(this->grandparent_caller(), root);
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
					this->parent()->MakeRed();
					ret = 2; // do again two levels back in recursion
				}
				else
				{
					if (this->current() == this->parent()->right)
					{
						RbtNode *save = this->current()->left;
						this->rotate_right(this->current_caller(), root);
						std::swap(this->current(), save);
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
					this->parent()->MakeRed();
					ret = 2; // do again two levels back in recursion
				}
				else
				{
					if (this->current() == this->parent()->left)
					{
						RbtNode *save = this->current()->right;
						this->rotate_left(this->current_caller(), root);
						std::swap(this->current(), save);
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
					this->grandparent()->MakeRed();
					ret = 2; // do again two levels back in recursion
				}
				else
				{
					if (this->current() == this->parent()->right && this->parent() == this->grandparent()->left)
					{
						this->rotate_left(this->parent_caller(), root);
						Caller(this, this->current()->left).insert_postop2(root);
					}
					else if (this->current() == this->parent()->left && this->parent() == this->grandparent()->right)
					{
						this->rotate_right(this->parent_caller(), root);
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

				int comp_result = RbtNode::compare(node_to_insert->key, this->current()->key);
				if (comp_result == 0)
				{
					this->current()->value = node_to_insert->value;
					/* inserted_node isn't going to be used, don't leak it */
					delete node_to_insert;
				}
				else if (comp_result < 0)
				{
					if (this->current()->left == NULL)
					{
						this->current()->left = node_to_insert;
						ret = insert_postop1_l(root_node);
					}
					else
					{
						ret = Caller(this, this->current()->left).exec(root_node, node_to_insert);
					}
				}
				else
				{
					if (this->current()->right == NULL)
					{
						this->current()->right = node_to_insert;
						ret = insert_postop1_r(root_node);
					}
					else
					{
						ret = Caller(this, this->current()->right).exec(root_node, node_to_insert);
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
		}; // struct RH_insert

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

		template <typename Caller>
		struct RH_dummy : public RH_Base<RH_dummy<Caller> >
		{
			typedef RH_Base<RH_dummy> base_type;

			RH_dummy(
					Caller * caller,
					RbtNode * node)
			: base_type(caller, node)
			{ }

			// TODO: turn into compile time property
			static bool isDummy()
			{
				return true;
			}

			RbtNode * exec(
					KEY const & key)
			{
				// TODO: should not be used - raise exception
				return NULL;
			}
		};

		// Recursion helper for ASF delete operation
		template <typename Caller>
		struct RH_del : public RH_Base<Caller >
		{
			typedef RH_Base<Caller> base_type;

			RH_del(
					Caller * caller,
					RbtNode * node)
			: base_type(caller, node)
			{ }

			// TODO: turn into compile time property
			static bool isDummy()
			{
				return false;
			}

			RbtNode * exec(
					KEY const & key)
			{
				std::cout << "hello from RH_del::exec()" << std::endl;
				std::cout << "key = " << key << std::endl;
				std::cout << "this->current()->key = " << this->current()->key << std::endl;
				return NULL; // TODO
			}
		};

		/**
		 * Recursion helper for ASF lookup operation.
		 * It uses an optional ASF PostProcessor after lookup.
		 * This makes is suport ASF delete operation.
		 */
		template <bool DeleteIndicator>
		struct RH_lookup : public RH_Base<RH_lookup<DeleteIndicator> >
		{
			typedef RH_lookup Caller;

			typedef RH_Base<RH_lookup> base_type;

			RH_lookup(
					Caller * caller,
					RbtNode * node)
			: base_type(caller, node)
			{ }

			RbtNode * exec(
					KEY const & key)
			{
				if (NULL == this->current())
				{
					return NULL;
				}

				int comp_result = RbtNode::compare(key, this->current()->key);

				if (0 == comp_result)
				{
					/*
					 * element found
					 */

					/*
					 * TODO
					 *  +) turn run-time into compile-time
					 *  +) make more flexible for ANY kind of post-processing
					 */

					if (true == DeleteIndicator)
					{
						return RH_del<Caller>(this, this->current()).exec(key);
					}

					return this->current();
				}

				if (comp_result < 0)
				{
					return Caller(this, this->current()->left).exec(key);
				}

				return Caller(this, this->current()->right).exec(key);
			}
		}; // struct RH_lookup



		// model lookup via ASF
		RbtNode * lookup(
				KEY const & key)
		{
			return RH_lookup<false>(NULL, this->root).exec(key);
		}


		/**
		 *  Delete operation
		 *  This uses RH_lookup
		 *  and adds a trailing processing for RH_delete
		 *  on the stack
		 */
		RbtNode * del(
				KEY const & key)
		{
			return RH_lookup<true>(NULL, this->root).exec(key);

		}

		// iterative version of lookup
		RbtNode * lookup_it(
				KEY const & key)
		{
			RbtNode * n = this->root;
			while (n != NULL)
			{
				int comp_result = RbtNode::compare(key, n->key);
				if (0 == comp_result)
				{
					return n;
				}
				else if (comp_result < 0)
				{
					n = n->left;
				}
				else
				{
					n = n->right;
				}
			}
			return n;
		}

	};

};
