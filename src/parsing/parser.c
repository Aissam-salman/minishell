/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 18:26:24 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/07 23:20:41 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include <stdio.h>
#include <unistd.h>

void ft_create_cmd_lst(t_minishell *minishell);
t_elements *create_mocks_element();

int ft_check_flags(char *str)
{
	int i;

	i = 0;
	if (str[i] != '-' || !str[i])
		return (0);
	else
		i++;
	if (str[i] == '-')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isalnum(str[i]))
			return(0);
		i++;
	}
	return (1);
}

int ft_check_redirection(char *str)
{
    if (!str)
	    return (0);
    if (str[0] == ' ')
	    return (0);
    else if ((str[0] == '<' && !str[1]) || (str[0] == '>' && !str[1]))
	    return (1);
    else if ((str[0] == '<' && str[1] == '<' && !str[2]) ||
		    (str[0] == '>' && str[1] == '>' && !str[2]))
	    return (1);
    return (0);
}

int ft_check_heredoc_end(char *str)
{
	//NOTE: maybe handle " " and ""
	if (!str)
		return (0);
	return (1);
}

int ft_check_file(t_elements *ele)
{
	//NOTE: if < try to access R mode 
	//     else if > try to open O_CREAT | O_TRUNCT | O_W
	//     else if << check_heredoc_end
	//     else if >> try to open O_W | O_APPEND | O_CREAT if not exist
	if (ele->type == IN_CHEVRON)
	{
		if (access(ele->next->str, R_OK) == -1)
			return (0);
	}
	else if (ele->type == IN_DCHEVRON)
	{
		if (ft_check_heredoc_end(ele->next->str) == 0)
			return (0);
	}
	return (1);
}

int ft_check_cmd(t_minishell *minishell, t_elements *ele)
{
    (void)minishell;
    (void)ele;
    return (0);
}
int ft_check_expends(t_minishell *minishell, t_elements *ele)
{
    (void)minishell;
    (void)ele;
    return (0);
}
int ft_check_pipe(t_minishell *minishell, t_elements *ele)
{
    (void)minishell;
    (void)ele;
    return (0);
}
void cmd_append(t_minishell *minishell, t_elements *ele, char **args)
{
    (void)minishell;
    (void)ele;
    (void)args;
}


void error_parsing_redirection()
{
	printf("Error Redirection");
	exit(12);
}

void error_parsing_files()
{
	printf("Error file");
	exit(12);
}

void error_parsing_cmd()
{
	printf("Error cmd");
	exit(13);
}

void error_parsing_expends()
{
	printf("Error expends");
	exit(14);
}

void error_parsing_pipe()
{
	printf("Error pipe");
	exit(15);
}

void error_parsing_flags()
{
	printf("Error flags");
	exit(16);
}

int is_redirection(t_elements *ele)
{
    if (ele->type == IN_CHEVRON ||
        ele->type == OUT_CHEVRON ||
        ele->type == IN_DCHEVRON ||
        ele->type == OUT_DCHEVRON)
        return (1);
    return (0);
}

void ft_create_cmd_lst(t_minishell *minishell)
{
	(void)minishell;
	t_elements *ele = NULL;
	char **args;
	t_elements *e;
	int i;
	int len_ele;

	ele = create_mocks_element();
	minishell->elements = ele;
	len_ele = 6; //FIX: make lst_size for t_element
	args = ft_gc_malloc(sizeof(char *) * len_ele, &minishell->gc);
	if (!args)
		return;
	ft_bzero(void *s, size_t n)
	while (ele)
	{
		printf("%s\n", ele->str);
		if (ele->is_taken)
		{
			ele = ele->next;
			continue;
		}
		if (is_redirection(ele))
		{
			//NOTE: check word after if is file ??
			if (ft_check_redirection(ele->str) == 0)
				error_parsing_redirection();
			if (ele->next)
			{
				if (ft_check_file(ele) == 0)
					error_parsing_files();
			}
		}
		else if (ele->type == WORD && (ft_strchr(ele->str, '\'') || ft_strchr(ele->str, '\"')))
		{
			if (!ft_check_expends(minishell, ele))
				error_parsing_expends();
		}
		else if (ele->type == WORD)
		{
			//NOTE: set type CMD if X_OK
			if (!ft_check_cmd(minishell, ele))
				error_parsing_cmd();
			e = ele;
			i = 0;
			while (e && e->type != PIPE)
			{
				if (is_redirection(e) && e->next && e->next->next)
				{
							e->is_taken = 1;
							e->next->is_taken = 1;
				}
				if ((!args[i] || e->type == WORD) && e->is_taken == 0)
				{
					args[i] = ft_gc_malloc(ft_strlen(e->str) + 1, &minishell->gc);
					if (e->type == CMD)
						args[i] = e->str;
					else
					{
						if (ft_check_flags(e->str) == 0)
							error_parsing_flags();
						args[i] = e->str;
					}
					e->is_taken = 1;
					i++;
				}
				e = e->next;
			}
		}
		else if (ele->type == PIPE)
		{
			if (!ft_check_pipe(minishell, ele))
				error_parsing_pipe();
		}
		cmd_append(minishell, ele, args);
		ele = ele->next;
	}
}

t_elements *create_mocks_element()
{
	t_elements *head = calloc(1,sizeof(t_elements));
	t_elements *element2 = calloc(1,sizeof(t_elements));
	t_elements *element3 = calloc(1,sizeof(t_elements));
	t_elements *element4 = calloc(1,sizeof(t_elements));
	t_elements *element5 = calloc(1,sizeof(t_elements));
	t_elements *element6 = calloc(1,sizeof(t_elements));
	// PIPE,
	// IN_CHEVRON,
	// IN_DCHEVRON,
	// OUT_CHEVRON,
	// OUT_DCHEVRON,
	// WORD,
	// NBR_TYPES,
	// FILE ??
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
