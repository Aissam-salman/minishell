/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 18:26:24 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/09 19:12:36 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void ft_create_cmd_lst(t_minishell *minishell);
t_token *create_mocks_element();

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

int ft_check_file_of_redirection(t_token *ele)
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

int ft_check_file(t_token *ele)
{
	if (access(ele->str, R_OK | W_OK) == -1) 
		return (0);
	return (1);
}

int ft_check_cmd(t_minishell *minishell, t_token *ele)
{
    (void)minishell;
    (void)ele;
    return (0);
}
int ft_check_expends(t_minishell *minishell, t_token *ele)
{
    (void)minishell;
    (void)ele;
    return (0);
}
int ft_check_pipe(t_minishell *minishell, t_token *ele)
{
    (void)minishell;
    (void)ele;
    return (0);
}

int is_redirection(t_token *ele)
{
    if (ele->type == IN_CHEVRON ||
        ele->type == OUT_CHEVRON ||
        ele->type == IN_DCHEVRON ||
        ele->type == OUT_DCHEVRON)
        return (1);
    return (0);
}

typedef struct s_checker 
{
	int exit_code;
	char *msg;
	t_token *token;
} t_checker;

t_checker *checker_token(t_minishell *minishell)
{
	t_token *token;
	
	token = minishell->head_token;
	while (token)
	{
		if (is_redirection(token))
		{
			if (ft_check_redirection(token->str) == 0)
				ft_error();
			if (token->next)
			{
				if (ft_check_file_of_redirection(token) == 0)
					error_parsing_files();
				token->next->type = R_FILE;
			}
		}
		else if (token->type == WORD && ft_strchr(token->str, '\"'))
		{
			if (ft_check_expends(minishell, token) == 0)
				error_parsing_expends();
			token->type = EXPEND;
		}
		else if (token->type == WORD)
		{
			//NOTE: set type CMD if X_OK
			if (ft_check_cmd(minishell, token) == 1)
				token->type = CMD;
			if (ft_check_flags(token->str) == 1)
				token->type = FLAG;
			if (ft_check_file(token))
				token->type = R_FILE;
		}
		else if (token->type == PIPE)
		{
			if (!ft_check_pipe(minishell, token))
				error_parsing_pipe();
		}
		token = token->next;
	}
	return (NULL);
}

void ft_create_cmd_lst(t_minishell *minishell)
{
	t_checker *res;

	res = checker_token(minishell);
		// if (token->is_taken)
		// {
		// 	token = token->next;
		// 	continue;
		// }
			// e = token;
			// i = 0;
			// while (e && e->type != PIPE)
			// {
			// 	if (is_redirection(e) && e->next && e->next->next)
			// 	{
			// 				e->is_taken = 1;
			// 				e->next->is_taken = 1;
			// 	}
			// 	if ((!args[i] || e->type == WORD) && e->is_taken == 0)
			// 	{
			// 		args[i] = ft_gc_malloc(ft_strlen(e->str) + 1, &minishell->gc);
			// 		if (e->type == CMD)
			// 			args[i] = e->str;
			// 		else
			// 		{
			// 			if (ft_check_flags(e->str) == 0)
			// 				error_parsing_flags();
			// 			args[i] = e->str;
			// 		}
			// 		e->is_taken = 1;
			// 		i++;
			// 	}
			// 	e = e->next;
			// }
}
