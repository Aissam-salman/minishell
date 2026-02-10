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
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
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
	//NOTE: open >  and >>
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
	char *env = getenv("PATH");
	char **envp = ft_split_sep_gc(env, ':', &minishell->gc);

	int i = 0;
	while (envp[i])
	{
		if (access(ele->str, X_OK) == 0)
			return (1);
		char *path = ft_strjoin_gc("/", ele->str, &minishell->gc);
		char *cur_path = ft_strjoin_gc(envp[i], path, &minishell->gc);
		printf("envp[%d]= %s\n",i , cur_path);
		if (access(cur_path, X_OK) == 0)
			return (1);
		i++;
	}
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

int checker_token(t_minishell *minishell)
{
	t_token *token;
	
	token = minishell->head_token;
	while (token)
	{
		if (is_redirection(token))
		{
			if (ft_check_redirection(token->str) == 0)
				//FIX: stocker code error 
				return ft_error(2, "Error redirection");
			if (token->next)
			{
				//FIX: recup code error de open
				if (ft_check_file_of_redirection(token) == 0)
					return ft_error(1, "Error file of redirection");
				token->next->type = R_FILE;
			}
		}
		else if (token->type == WORD && (ft_strchr(token->str, '\"') ||
		   ft_strchr(token->str, '$')))
		{
			//NOTE: while find $ parcours jusqu'a pas alnum
			// 		si rien trouver mettre chaine vide
			if (ft_check_expends(minishell, token) == 0)
				return ft_error(1, "Error expension");
		}
		else if (token->type == WORD)
		{
			//NOTE: set type CMD if X_OK
			if (ft_check_cmd(minishell, token) == 1)
				token->type = CMD;
			if (ft_check_flags(token->str) == 1)
				token->type = FLAG;
			if (ft_check_file(token) == 1)
				token->type = R_FILE;
		}
		else if (token->type == PIPE)
		{
			if (!ft_check_pipe(minishell, token))
				return ft_error(1, "Error pipe");
		}
		token = token->next;
	}
	return (0);
}

void ft_create_cmd_lst(t_minishell *minishell)
{
	checker_token(minishell);
}
