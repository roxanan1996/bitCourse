#include <windows.h>
#include <stdio.h>
#include <stack>

//change the stack
//editbin /STACK:in Tree.exe

//how big is the stack
//link /dump /headers Tree.exe

#define MAX_ELEMENTS				300000

typedef struct __tree
{
	int info;
	__tree * left;
	__tree * right;
	__tree * parent;
}tree;

char * stack = NULL;

int insert_tree(tree ** root, tree *elem)
{
	tree * y = NULL;
	tree * x = *root;
	while (x != NULL)
	{
		y = x;
		if (elem->info < x->info)
			x = x->left;
		else
			x = x->right;
	}
	elem->parent = y;

	if (y == NULL)
	{
		*root = elem;
	}
	else if (elem->info < y->info)
	{
		y->left = elem;
	}
	else
		y->right = elem;

	return 0;
}

int inorder_tree_walk(tree * p, int level)
{
	char current = 0;
	size_t stack_size = stack - &current;
	if (p)
	{

		if (p->left)
			inorder_tree_walk(p->left, level+1);
		printf("%d %d\n",p->info, level);
		if (p->right)
			inorder_tree_walk(p->right, level+1);
	}
	return 0;
}

int inorder_iterative_tree_walk(tree * p)
{
	std::stack<tree *> my_stack;
	tree * crt = p;
	
	do
	{
		while (crt)
		{
			my_stack.push(crt);
			crt = crt->left;
		}
		if (!crt && !my_stack.empty())
		{
			tree * top = my_stack.top();
			my_stack.pop();
			printf("%d\n", top->info);
			crt = top->right;
		}
	}while(crt);

	return 0;

}

int delete_tree(tree * p)
{
	char current = 0;
	size_t stack_size = stack - &current;
	if (p)
	{
		if (p->left)
			delete_tree(p->left);
		if (p->right)
			delete_tree(p->right);
		free(p);
	}
	return 0;

}

tree * search_tree(tree * p, int elem)
{
	if (p == NULL || elem == p->info)
	{
		return p;
	}

	if (elem < p->info)
		return search_tree(p->left, elem);
	else
		return search_tree(p->right, elem);
}

tree * iterative_tree_search(tree * p , int elem)
{
	while(p != NULL && elem != p->info)
	{
		if (elem < p->info)
			p = p->left;
		else 
			p = p->right;
	}
	return p;
}

tree * tree_minimum(tree * p)
{
	if (!p)
		return NULL;
	while (p->left != NULL)
		p = p->left;

	return p;
}

tree * tree_maximum(tree * p)
{
	if (!p)
		return NULL;
	while (p->right != NULL)
		p = p->right;

	return p;
}


tree * tree_successor(tree * p)
{
	if (p->right != NULL)
		return tree_minimum(p->right);
	
	tree * y = p->parent;
	while( y!=NULL && p == y->right)
	{
		p = y;
		y = y->parent;
	}
	return y;
}

int tree_change(tree ** root, tree * p1, tree * p2)
{
	if (p1->parent == NULL)
		*root = p2;
	else if (p1 == p1->parent->left)
		p1->parent->left = p2;
	else
		p1->parent->right = p2;

	if (p2 != NULL)
		p2->parent = p1->parent;

	return 0;
}

int tree_delete_element(tree ** root, tree * p)
{
	if (p->left == NULL)
		tree_change(root, p, p->right);
	else if (p->right == NULL)
		tree_change(root, p, p->left);
	else 
	{
		tree * temp = tree_minimum(p->right);
		if (temp->parent != p)
		{
			tree_change(root, temp, temp->right);
			temp->right = p->right;
			temp->right->parent = temp;
		}
		tree_change(root, p, temp);
		temp->left = p->left;
		temp->left->parent = temp;
	}
	return 0;
}





int main()
{


// tree test
/*	tree * p10 = (tree *)malloc(sizeof(tree));
	tree * p20 = (tree *)malloc(sizeof(tree));
	tree * p30 = (tree *)malloc(sizeof(tree));

	tree * p40 = (tree *)malloc(sizeof(tree));
	tree * p50 = (tree *)malloc(sizeof(tree));
	tree * p60 = (tree *)malloc(sizeof(tree));

	tree * p70 = (tree *)malloc(sizeof(tree));
	tree * p80 = (tree *)malloc(sizeof(tree));
	tree * p85 = (tree *)malloc(sizeof(tree));
	tree * p90 = (tree *)malloc(sizeof(tree));

	tree * p100 = (tree *)malloc(sizeof(tree));
	tree * p110 = (tree *)malloc(sizeof(tree));

	p10->info = 10;
	p10->left = NULL;
	p10->right = NULL;
	p10->parent = p20;
	
	p20->info = 20;
	p20->left = p10;
	p20->right = p70;
	p20->parent = NULL;

	p70->info = 70;
	p70->left = p50;
	p70->right = p90;
	p70->parent = p20;

	p40->info = 40;
	p40->left = NULL;
	p40->right = NULL;
	p40->parent = p50;

	p50->info = 50;
	p50->left = p40;
	p50->right = p60;
	p50->parent = p70;

	p60->info = 60;
	p60->left = NULL;
	p60->right = NULL;
	p60->parent = p50;


	p80->info = 80;
	p80->left = NULL;
	p80->right = p85;
	p80->parent = p90;

	p90->info = 90;
	p90->left = p80;
	p90->right = p100;
	p90->parent = p70;

	p100->info = 100;
	p100->left = p80;
	p100->right = p110;
	p100->parent = p90;


	p85->info = 85;
	p85->left = NULL;
	p85->right = NULL;
	p85->parent = p80;

	p110->info = 110;
	p110->left = NULL;
	p110->right = NULL;
	p110->parent = p100;

	tree_delete_element(&p20, p70);
*/


	char stack_base;
	stack = &stack_base;
	tree * root = NULL;

	for (int i = 0 ; i < MAX_ELEMENTS ; i++)
	{
		tree * new_elem = (tree *)malloc(sizeof(tree));
		new_elem->left = NULL;
		new_elem->right = NULL;
		new_elem->parent = NULL;
		new_elem->info = i;
		insert_tree(&root, new_elem);
	}

	inorder_tree_walk(root,1);

/*	tree * p = search_tree(root, MAX_ELEMENTS/2);

	printf("element %d found on %p with parent_info=%d left_info=%d right_info=%d\n", MAX_ELEMENTS/2, p, 
		p && p->parent? p->parent->info : 0, 
		p && p->left? p->left->info : 0,
		p && p->right? p->right->info : 0);

	if (p)
	{
		tree * suc = tree_successor(p);
		printf("element successor found on %p with info %d and with parent_info=%d left_info=%d right_info=%d\n", suc, suc->info,
			suc && suc->parent? suc->parent->info : 0, 
			suc && suc->left? suc->left->info : 0,
			suc && suc->right? suc->right->info : 0);

		tree_delete_element(&root, suc);
	}
	
	printf("min = %d max = %d\n", tree_minimum(root)->info , tree_maximum(root)->info);


	inorder_iterative_tree_walk(root);

	delete_tree(root);*/

	return 0;
}