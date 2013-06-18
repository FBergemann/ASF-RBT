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

        // TODO
        for (int i=0; i<indent; i++)
        {
            std::cout << ' ';
        }

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

    void insert_postprocess(
            RbtNode *& root)
    {
        if (this->parent == NULL)
        {
            this->color = BLACK;
        }
        else if (this->parent->color == BLACK)
        {
            return; /* Tree is still valid */
        }
        else if (GetColor(this->uncle()) == RED)
        {
            this->parent->color = BLACK;
            this->uncle()->color = BLACK;
            this->grandparent()->color = RED;
            this->grandparent()->insert_postprocess(root); // pop->pop->again
        }
        else
        {
            if (this == this->parent->right && this->parent == this->grandparent()->left)
            {
                this->parent->rotate_left(root);
                this->left->insert_postprocess2(root);
            }
            else if (this == this->parent->left && this->parent == this->grandparent()->right)
            {
                this->parent->rotate_right(root);
                this->right->insert_postprocess2(root);
            }
            else
            {
                this->insert_postprocess2(root);;
            }
        }
    }

    void insert_postprocess2(
            RbtNode *& root)
    {
            this->parent->color = BLACK;
            this->grandparent()->color = RED;

            if (this == this->parent->left && this->parent == this->grandparent()->left)
            {
                this->grandparent()->rotate_right(root);
            }
            else
            {
                this->grandparent()->rotate_left(root);
            }
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

#if 0

    // original version
    static void verify_property_4(
            RbtNode * n)
    {
        if (n == NULL)
        {
            return;
        }

        std::cout << "### verify node with key " << n->key << std::endl;

        if (GetColor(n) == RED)
        {
            assert (GetColor(n->left)   == BLACK);
            assert (GetColor(n->right)  == BLACK);
            assert (GetColor(n->parent) == BLACK); // want to make this obsolete
        }
        verify_property_4(n->left);
        verify_property_4(n->right);
    }

#else

    // new version
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

                std::cout << "### verify node with key " << n->key << std::endl;

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

#endif

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
            } else
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
            RbtNode * inserted_node = new RbtNode(key, value);
            if (this->root == NULL)
            {
                this->root = inserted_node;
            }
            else
            {
                RbtNode * n = this->root;
                while (1)
                {
                    int comp_result = compare(key, n->key);
                    if (comp_result == 0)
                    {
                        n->value = value;
                        /* inserted_node isn't going to be used, don't leak it */
                        delete inserted_node;
                        return;
                    }
                    else if (comp_result < 0)
                    {
                        if (n->left == NULL)
                        {
                            n->left = inserted_node;
                            break;
                        }
                        else
                        {
                            n = n->left;
                        }
                    }
                    else
                    {
                        if (n->right == NULL)
                        {
                            n->right = inserted_node;
                            break;
                        }
                        else
                        {
                            n = n->right;
                        }
                    }
                }
                inserted_node->parent = n;
            }
            inserted_node->insert_postprocess(root);
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
    for(int i=0; i<60000; i++)
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
