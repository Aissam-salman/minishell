
#include "../../includes/minishell.h"

t_token	*create_mocks_element1(void)
{
	t_token	*head;
	t_token	*element2;
	t_token	*element3;
	t_token	*element4;
	t_token	*element5;
	t_token	*element6;

	head = calloc(1, sizeof(t_token));
	element2 = calloc(1, sizeof(t_token));
	element3 = calloc(1, sizeof(t_token));
	element4 = calloc(1, sizeof(t_token));
	element5 = calloc(1, sizeof(t_token));
	element6 = calloc(1, sizeof(t_token));
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

t_token	*create_mocks_element2(void)
{
	t_token	*head;
	t_token	*element2;
	t_token	*element3;
	t_token	*element4;
	t_token	*element5;
	t_token	*element6;

	head = calloc(1, sizeof(t_token));
	element2 = calloc(1, sizeof(t_token));
	element3 = calloc(1, sizeof(t_token));
	element4 = calloc(1, sizeof(t_token));
	element5 = calloc(1, sizeof(t_token));
	element6 = calloc(1, sizeof(t_token));
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
