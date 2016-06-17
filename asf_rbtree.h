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

#if TRACE >= 2
#define PRINT_STACK(a)		this->PrintStack(a);
#else
#define PRINT_STACK(a)
#endif

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
		return (n == NULL) ? BLACK : n->color;
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

	/**
	 * get the maximum value of current positistack_n->on down the tree.
	 * it's the leaf at most right position
	 */
	inline RbtNode * maximum_node(void)
	{
		RbtNode *n = this;
		while (NULL != n->right)
		{
			n = n->right;
		}
		return n;
	}


	// Recursion Helper Base Class
	struct RH_Base
	{
		RH_Base *stack_c;
		RbtNode *stack_n;

		RH_Base(
				RH_Base * caller,
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
		 * replaces the node of *caller by new node *newn
		 *
		 * Note, that this changes *caller's PARENTs left of right node
		 * unless there is no parent and caller is the root itself.
		 */
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

		/*
		 * see e.g. https://de.wikipedia.org/wiki/Rot-Schwarz-Baum
		 *
		 *           P                                    S
		 *       /      \                             /       \
		 *      N        (S)                        (P)       SR
		 *    /  \     /    \           =>        /    \     /  \
		 *   1    2   SL     SR                  N      SL  5    6
		 *           / \     / \               /  \    / \
		 *          3   4   5   6             1    2  3   4
		 *
		 *  When used for delete operation in delete_case2 then TODO...
		 */
		void rotate_left(
				RH_Base * caller,
				RbtNode *& root)
		{
			RbtNode * R = caller->stack_n->right;   // R = (S)
			replace_node(caller, R, root);          // (S) to the top replacing P
			caller->stack_n->right = R->left;       // P->right = (S)->left = SL
			R->left = caller->stack_n;              // (S)->left = P
		}

		void rotate_left_adjust_stack()
		{
			assert (NULL != NULL); // TODO: not yet implemented
#if 0
			/*
			 * Rotation shifted down the node to delete
			 * But the structure up to the parent is still the same
			 * However the parent and the current node values itself changed.
			 * -> update the stack
			 */
			// was the parent on the left side of the grandparent?
			if (this->grandparent()->key > this->parent()->key)
			{
				this->parent()  = this->grandparent()->left;
			}
			else
			{
				this->parent()  = this->grandparent()->right;
			}

			this->current() = this->parent()->left; // TODO: not sure about that - to be verified
#endif
		}

		/*
		 * see e.g. https://de.wikipedia.org/wiki/Rot-Schwarz-Baum
		 *
		 *           P                             S
		 *       /      \                     /         \
		 *      S        N                  SL           P
		 *    /    \     /\      =>        /  \       /     \
		 *   SL     SR  5  6              1     2    SR      N
		 *  / \     / \                             /  \    /  \
 		 * 1   2   3   4                           3    4   5   6
		 *
		 *  When used for delete operation in delete_case2 then the node to delete is N.
		 *  And it does NOT have a right node (6).
		 *  Because we looked for some right-most ancestor before.
		 *
		 */
		void rotate_right(
				RH_Base * caller,
				RbtNode *& root)
		{
			RbtNode * L = caller->stack_n->left;	// L = S
			replace_node(caller, L, root);			// S to the top replacing P
			caller->stack_n->left = L->right;		// P->left = S->right = SR
			L->right = caller->stack_n;				// S->right = P
		}

		void rotate_right_adjust_stack()
		{
			/*
			 * Rotation shifted down the node to delete
			 * But the structure up to the parent is still the same
			 * However the parent and the current node values itself changed.
			 * -> update the stack
			 */
			// was the parent on the left side of the grandparent?
			if (this->grandparent()->key > this->parent()->key)
			{
				this->parent() = this->grandparent()->left;
			}
			else
			{
				this->parent()  = this->grandparent()->right;
			}

			this->current() = this->parent()->right;
		}

		void PrintStack(
				std::string const & headline = "")
		{
			RH_Base *cur = this;

			std::cout << "stack-trace: " << headline << ": ";

			do
			{
				std::cout << cur->stack_n->key << " - ";
				cur = cur->stack_c;
			} while (NULL != cur);

			std::cout << std::endl;
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
		assert(BLACK == GetColor(root));
	}

	// Recursion Helper for verify_property_4()
	struct RH_verify_property_4 : public RH_Base
	{
		RH_verify_property_4(
				RH_Base * caller,
				RbtNode * node)
		: RH_Base(caller, node)
		{ }

		void exec(void)
		{
			if (this->stack_n == NULL)
			{
				return;
			}

			// std::cout << "### verify node with key " << n->key << std::endl;

			if (GetColor(this->current()) == RED)
			{
				assert (BLACK == GetColor(this->current()->left));
				assert (BLACK == GetColor(this->current()->right));
				if (this->stack_c != NULL)
				{
					assert (BLACK == GetColor(this->stack_c->stack_n));
				}
			}
			RH_verify_property_4(this, this->current()->left).exec();	// recursion
			RH_verify_property_4(this, this->current()->right).exec();	// recursion
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

		Tree(RbtNode * node)
		: root(node)
		{ }

		static void print(RbtNode * node)
		{
			Tree(node).print();
		}

		void print(void)
		{
			if (root == NULL)
			{
				std::cout << "<empty tree>" << std::endl;
				return;
			}

			this->root->print(0);
			std::cout << std::endl << "-----" << std::endl;
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
		struct RH_insert : public RH_Base
		{
			RH_insert(
					RH_Base * caller,
					RbtNode * node)
			: RH_Base(caller, node)
			{ }

			void insert_postop2(
					RbtNode *& root)
			{
				this->parent()->color      = BLACK;
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
				else if (GetColor(this->sibling()) == RED)
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
						RH_insert(this, save).insert_postop2(root); // recursion
					}
					else
					{
						RH_insert(this, this->current()->left).insert_postop2(root); // recursion
					}
				}

				return ret;
			}

			int insert_postop1_r(
					RbtNode *& root)
			{
				int ret = -1;

				if (BLACK == this->current()->color)
				{
					; /* Tree is still valid */
				}
				else if (RED == GetColor(this->sibling()))
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
						RH_insert(this, save).insert_postop2(root); // recursion
					}
					else
					{
						RH_insert(this, this->current()->right).insert_postop2(root); // recursion
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
				else if (BLACK == this->parent()->color)
				{
					; /* Tree is still valid */
				}
				else if (RED == GetColor(this->uncle()))
				{
					this->grandparent()->MakeRed();
					ret = 2; // do again two levels back in recursion
				}
				else
				{
					if (this->current() == this->parent()->right && this->parent() == this->grandparent()->left)
					{
						this->rotate_left(this->parent_caller(), root);
						RH_insert(this, this->current()->left).insert_postop2(root); // recursion
					}
					else if (this->current() == this->parent()->left && this->parent() == this->grandparent()->right)
					{
						this->rotate_right(this->parent_caller(), root);
						RH_insert(this, this->current()->right).insert_postop2(root); // recursion
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
					if (NULL == this->current()->left)
					{
						this->current()->left = node_to_insert;
						ret = insert_postop1_l(root_node);
					}
					else
					{
						ret = RH_insert(this, this->current()->left).exec(root_node, node_to_insert); // recursion
					}
				}
				else
				{
					if (NULL == this->current()->right)
					{
						this->current()->right = node_to_insert;
						ret = insert_postop1_r(root_node);
					}
					else
					{
						ret = RH_insert(this, this->current()->right).exec(root_node, node_to_insert); // recursion
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

			if (NULL == this->root)
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

		/**
		 * Extended base class for lookup and delete
		 * It introduces function chaining.
		 * A next function can be given a tail-end successor function.
		 * This enables implementing "delete" based on "lookup"
		 */
		struct RH_Base_Ext : public RH_Base
		{
			RH_Base_Ext * successor;

			RH_Base_Ext(
				RH_Base * caller,
				RbtNode * node,
				RH_Base_Ext * successor)
			: RH_Base(caller, node),
			  successor(successor)
			{ }

			/*
			 * offline setters for successor support
			 */
			inline void SetCaller(
					RH_Base * caller)
			{
				RH_Base::stack_c = caller;
			}

			inline void SetNode(
					RbtNode * node)
			{
				RH_Base::stack_n = node;
			}

			virtual RbtNode * exec(
					RbtNode *& root_node,
					KEY const & key) = 0;
		};

		/**
		 * Recursion helper for ASF lookup operation.
		 * It uses an optional ASF PostProcessor after lookup.
		 * This makes is suport ASF delete operation.
		 */
		struct RH_lookup : public RH_Base_Ext
		{
			RH_lookup(
					RH_Base * caller,
					RbtNode * node,
					RH_Base_Ext * successor = NULL)
			: RH_Base_Ext(caller, node, successor)
			{ }

			RbtNode * exec(
					RbtNode *& root_node,
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

					if (NULL != RH_Base_Ext::successor)
					{
						/*
						 * merge with successor: but back to the caller level to avoid an additional stack level
						 */
						RH_Base_Ext::successor->SetCaller(this->parent_caller());
						RH_Base_Ext::successor->SetNode(this->current());

						return RH_Base_Ext::successor->exec(root_node, key);
					}

					return this->current();
				}

				if (comp_result < 0)
				{
					return RH_lookup(this, this->current()->left, RH_Base_Ext::successor).exec(root_node, key);
				}

				return RH_lookup(this, this->current()->right, RH_Base_Ext::successor).exec(root_node, key);

			}
		}; // struct RH_lookup


		// model lookup via ASF
		RbtNode * lookup(
				KEY const & key)
		{
			return RH_lookup(NULL, this->root, NULL).exec(this->root, key);
		}

		/**
		 * A RH_Base::replace_node post-processing helper
		 */
		struct RH_replace_node : public RH_Base_Ext
		{
			RbtNode * _newNode;

			RH_replace_node(
					RH_Base * caller,
					RbtNode * node,
					RH_Base_Ext * successor,
					RbtNode * newNode)
			: RH_Base_Ext(caller, node, successor),
			  _newNode(newNode)
			{

			}

			RbtNode * exec(
					RbtNode *& root_node,
					KEY const & key)
			{
				RbtNode * oldNode = this->current();
				this->replace_node(this->current_caller(), _newNode, root_node);
				return oldNode;
			}
		};

		// Recursion helper #1 for ASF delete operation
		struct RH_del_1 : public RH_Base_Ext
		{
			RH_del_1(
					RH_Base * caller,
					RbtNode * node,
					RH_Base_Ext * successor = NULL)
			: RH_Base_Ext(caller, node, successor)
			{ }

			RbtNode * exec(
					RbtNode *& root_node,
					KEY const & key)
			{
				RbtNode * current = this->current();

				if (NULL != current->left and NULL != current->right)
				{
					/* Copy key/value from predecessor node and then delete predecessor node instead */
					RbtNode * pred = current->left->maximum_node();

					std::swap(current->key, pred->key);
					std::swap(current->value, pred->value);

					/* WALK to predecessor value and continue with next successor function there */
					{
						/*
						 * delete operation successor for RH_lookup again
						 * (note, that we could even STACK successors,
						 *  the last argument of the successor could be *another* successor again)
						 */
						RH_del_2 successor(NULL, NULL, NULL);
						return RH_lookup(this, this->current()->left, &successor).exec(root_node, pred->key);
					}
				}

				/*
				 * merge with RH_del_2 w/o introducing another stack level
				 */
				return RH_del_2(this->parent_caller(), this->current(), NULL).exec(root_node, key);
			}

		};

		// Recursion helper #2 for ASF delete operation
		struct RH_del_2 : public RH_Base_Ext
		{
			RH_del_2(
					RH_Base * caller,
					RbtNode * node,
					RH_Base_Ext * successor)
			: RH_Base_Ext(caller, node, successor)
			{ }

			/*
			 * helper adopted from the C reference implementation
			 */
			void delete_case1(
					RbtNode *& root_node)
			{
				std::cout << "delete_case1" << std::endl;

			    if (NULL == this->parent())
			    {
			        return;
			    }
			    else
			    {
			    	delete_case2(root_node);
			    }
			}

			void delete_case2(
					RbtNode *& root_node)
			{
				std::cout << "delete_case2" << std::endl;

			    if (RED == GetColor(this->sibling()))
			    {
			    	std::cout << " rotate for " << this->current()->key << std::endl;
			    	Tree::print(root_node);

			    	PRINT_STACK("stack");

			        this->parent()->color  = RED;
			        this->sibling()->color = BLACK;

			        if (this->current() == this->parent()->left) // TODO: introduce IsLeft() / IsRight()
			        {
			        	std::cout << " rotate left" << std::endl;
			            this->rotate_left(this->parent_caller(), root_node);
			            this->rotate_left_adjust_stack();
			            Tree::print(root_node);

			            PRINT_STACK("stack")

			            // the node to delete is now the left node of current()
			            // -> add another level for next operation
			            RH_del_2(this, this->current()->left, NULL).delete_case3(root_node);
			        }
			        else
			        {
			        	std::cout << " rotate right" << std::endl;
			            this->rotate_right(this->parent_caller(), root_node);
			            this->rotate_right_adjust_stack();
			            Tree::print(root_node);

			            PRINT_STACK("stack")

			            // the node to delete is now the right node of current()
			            // -> add another level for next operation
			            RH_del_2(this, this->current()->right, NULL).delete_case3(root_node);
			        }
			    }
			    else
			    {
			    	delete_case3(root_node);
			    }
			}

			void delete_case3(
					RbtNode *& root_node)
			{
				std::cout << "delete_case3" << std::endl;

				PRINT_STACK("stack");

			    if (   BLACK == GetColor(this->parent())
			    	&& BLACK == GetColor(this->sibling())
			    	&& BLACK == GetColor(this->sibling()->left)
			    	&& BLACK == GetColor(this->sibling()->right))
			    {
			        this->sibling()->color = RED;
			        std::cout << "critical implementation" << std::endl << std::flush;
			        static_cast<RH_del_2*>(this->parent_caller())->delete_case1(root_node); // TODO: i am not sure, if this works
			    }
			    else
			    {
			        delete_case4(root_node);
			    }
			}

			void delete_case4(
					RbtNode *& root_node)
			{
				std::cout << "delete_case4" << std::endl;

				PRINT_STACK("stack");

				Tree::print(root_node);

			    if (   RED   == GetColor(this->parent())
			    	&& BLACK == GetColor(this->sibling())
			    	&& BLACK == GetColor(this->sibling()->left)
			    	&& BLACK == GetColor(this->sibling()->right))
			    {
			    	std::cout << " change color" << std::endl;
			        this->sibling()->color = RED;
			        this->parent()->color = BLACK;
			        Tree::print(root_node);
			    }
			    else
			    {
			        delete_case5(root_node);
			    }
			}

			void delete_case5(
					RbtNode *& root_node)
			{
				std::cout << "delete_case5" << std::endl;

				PRINT_STACK("stack");

			    if (   this->current() == this->parent()->left
			    	&& BLACK == GetColor(this->sibling())
			    	&& RED   == GetColor(this->sibling()->left)
			    	&& BLACK == GetColor(this->sibling()->right))
			    {
			        this->sibling()->color       = RED;
			        this->sibling()->left->color = BLACK;
			        std::cout << "implementation incomplete!" << std::endl << std::flush;
			        // TODO: with delete_case6 tailer
			        // rotate_right(t, sibling(n));
			    }
			    else if (   this->current() == this->parent()->right
			             && BLACK == GetColor(this->sibling())
			             && RED   == GetColor(this->sibling()->right)
			             && BLACK == GetColor(this->sibling()->left))
			    {
			        this->sibling()->color = RED;
			        this->sibling()->right->color = BLACK;
			        std::cout << "implementation incomplete!" << std::endl << std::flush;
			        // TODO: with delete_case6 tailer
			        // rotate_left(t, sibling(n));
			    }
			    else
			    {
			    	delete_case6(root_node);
			    }
			}

			void delete_case6(
					RbtNode *& root_node)
			{
				std::cout << "delete_case6" << std::endl;
				Tree::print(root_node);

			    this->sibling()->color = GetColor(this->parent());
			    this->parent()->color  = BLACK;

			    if (this->current() == this->parent()->left)
			    {
			        assert (RED == GetColor(this->sibling()->right));
			        this->sibling()->right->color = BLACK;
			        this->rotate_left(this->parent_caller(), root_node);
			    }
			    else
			    {
			        assert (RED == GetColor(this->sibling()->left));
			        this->sibling()->left->color = BLACK;
			        this->rotate_right(this->parent_caller(), root_node);
			    }
			}

			RbtNode * exec(
					RbtNode *& root_node,
					KEY const & key)
			{
			    assert(NULL == this->stack_n->left || NULL == this->stack_n->right);

			    RbtNode *child = (NULL == this->stack_n->right) ? this->stack_n->left : this->stack_n->right;
			    RbtNode *old_curr_node = this->current();

			    if (BLACK == this->current()->color)
			    {
			    	this->current()->color = GetColor(child);
			    	delete_case1(root_node);
			    }

			    child = (NULL == old_curr_node->right) ? old_curr_node->left : old_curr_node->right;

			    /*
			     * search for old_curr_node
			     * but merge current level with 1st RH_lookup level
			     * When found let it replace the node.
			     */
			    RbtNode * oldNode = NULL;
			    {
			    	RH_replace_node replaceNode(NULL, NULL, NULL, child);
			    	oldNode = RH_lookup(this->parent_caller(), this->current(), &replaceNode).exec(root_node, old_curr_node->key);
			    }

			    if (NULL == this->parent_caller() && NULL != child)
			    {
			        child->color = BLACK;
			    }

			    return oldNode;
			}

		};

		/**
		 *  Delete operation
		 *  This uses RH_lookup
		 *  and adds a trailing processing for RH_delete
		 *  on the stack
		 */
		RbtNode * del(
				KEY const & key)
		{
			RH_del_1 successor(NULL, NULL, NULL);
			RbtNode *node = RH_lookup(NULL, this->root, &successor).exec(this->root, key);

			Tree::print(this->root);

		    verify_properties();

		    return node;
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
