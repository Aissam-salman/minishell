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

char **ft_get_path(t_minishell *minishell)
{
	char *env;
	char **envp;

	env = getenv("PATH");
	if (!env)
		return (NULL);
	envp = ft_split_sep_gc(env, ':', &minishell->gc);
	if  (!envp)
		return (NULL);
	return (envp);
}

int ft_test_path(t_minishell *minishell, char **envp, t_token *ele)
{
	char *tmp;
	char *cur_path;
	int i;

	i = 0;
	while (envp[i])
	{
		tmp = ft_strjoin_gc(envp[i], "/", &minishell->gc);
		cur_path = ft_strjoin_gc(tmp, ele->str, &minishell->gc);
		if (cur_path && access(cur_path, X_OK) == 0)
		{
			ele->path = cur_path;
			return (1);
		}
		i++;
	}
    return (0);
}

int ft_check_cmd(t_minishell *minishell, t_token *ele)
{
	char **envp;
	int res;

	if (!ele || !ele->str || !*ele->str)
		return (0);
	if (access(ele->str, X_OK) == 0)
	{
		ele->path = ele->str;
		return (1);
	}
	envp = ft_get_path(minishell);
	if  (!envp)
		return (0);
	res = ft_test_path(minishell, envp, ele);
	if (res == 1)
		return 1;
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
	int		cmd_find = 0;
	
	token = minishell->head_token;
	while (token)
	{
		if (is_redirection(token))
		{
			if (ft_check_redirection(token->str) == 0)
				token->code_error = SYNTAX_ERROR;
			if (token->next)
			{
				//FIX: recup code error de open
				if (ft_check_file_of_redirection(token) == 0)
					token->code_error = NO_SUCH_FILE_O_DIR;
				else 
					token->next->type = R_FILE;
			}
		}
		else if (token->type == WORD && (ft_strchr(token->str, '\"') ||
		   ft_strchr(token->str, '$')))
		{
			//NOTE: while find $ parcours jusqu'a pas alnum
			// 		si rien trouver mettre chaine vide
			// 		sinon remplacer
			if (ft_check_expends(minishell, token) == 0)
				return ft_error(1, "Error expension");
		}
		else if (token->type == WORD)
		{
			//NOTE: set type CMD if X_OK
			//FIX: !!!! une seul cmd par slot de pipe
			if (ft_check_cmd(minishell, token) == 1 && cmd_find == 0)
			{
				token->type = CMD;
				cmd_find = 1;
			}
			if (ft_check_flags(token->str) == 1)
				token->type = FLAG;
			if (ft_check_file(token) == 1)
				token->type = R_FILE;
		}
		else if (token->type == PIPE)
		{
			if (!ft_check_pipe(minishell, token))
				return ft_error(1, "Error pipe");
			cmd_find = 0;
		}
		token = token->next;
	}
	return (0);
}

void ft_create_cmd_lst(t_minishell *minishell)
{
	checker_token(minishell);
}
