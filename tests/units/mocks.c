
#include "../../includes/minishell.h"

t_elements *create_mocks_element1()
{
	t_elements *head = calloc(1,sizeof(t_elements));
	t_elements *element2 = calloc(1,sizeof(t_elements));
	t_elements *element3 = calloc(1,sizeof(t_elements));
	t_elements *element4 = calloc(1,sizeof(t_elements));
	t_elements *element5 = calloc(1,sizeof(t_elements));
	t_elements *element6 = calloc(1,sizeof(t_elements));

	head->str = ft_strdup("<");
	head->type = IN_CHEVRON;
	element2->str = ft_strdup("logs.txt");
	element2->type = WORD;
	element3->str = ft_strdup("cat");
	element3->type = WORD;
	element4->str = ft_strdup("|");
	element4->type = PIPE;
	element5->str = ft_strdup("grep");
	element5->type = WORD;
	element6->str = ft_strdup("\"error\"");
	element6->type = WORD;
	head->next = element2;
	head->next->next = element3;
	head->next->next->next = element4;
	head->next->next->next->next = element5;
	head->next->next->next->next->next = element6;
	head->next->next->next->next->next->next = NULL;
	return (head);
}

t_elements *create_mocks_element2()
{
	t_elements *head = calloc(1,sizeof(t_elements));
	t_elements *element2 = calloc(1,sizeof(t_elements));
	t_elements *element3 = calloc(1,sizeof(t_elements));
	t_elements *element4 = calloc(1,sizeof(t_elements));
	t_elements *element5 = calloc(1,sizeof(t_elements));
	t_elements *element6 = calloc(1,sizeof(t_elements));

	head->str = ft_strdup(">");
	head->type = IN_CHEVRON;
	element2->str = ft_strdup("logs.txt");
	element2->type = WORD;
	element3->str = ft_strdup("cat");
	element3->type = WORD;
	element4->str = ft_strdup("|");
	element4->type = PIPE;
	element5->str = ft_strdup("grep");
	element5->type = WORD;
	element6->str = ft_strdup("\"error\"");
	element6->type = WORD;
	head->next = element2;
	head->next->next = element3;
	head->next->next->next = element4;
	head->next->next->next->next = element5;
	head->next->next->next->next->next = element6;
	head->next->next->next->next->next->next = NULL;
	return (head);
}
