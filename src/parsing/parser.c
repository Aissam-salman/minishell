/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 18:26:24 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/10 18:23:21 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

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

int ft_check_file_of_redirection(t_token *token)
{
	if (token->type == IN_CHEVRON)
	{
		if (access(token->next->str, R_OK) == -1)
			return (0);
	}
	else if (token->type == IN_DCHEVRON)
	{
		if (ft_check_heredoc_end(token->next->str) == 0)
			return (0);
	}
	return (1);
}

int ft_check_file(t_token *token)
{
	if (access(token->str, R_OK | W_OK) == -1)
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

int ft_test_path(t_minishell *minishell, char **envp, t_token *token)
{
	char *tmp;
	char *cur_path;
	int i;
	struct stat stat_file;

	i = 0;
	while (envp[i])
	{
		tmp = ft_strjoin_gc(envp[i], "/", &minishell->gc);
		cur_path = ft_strjoin_gc(tmp, token->str, &minishell->gc);

		if (stat(cur_path, &stat_file) == 0 && S_ISREG(stat_file.st_mode))
		{
			if (cur_path && access(cur_path, X_OK) == 0)
			{
				token->path = cur_path;
				return (1);
			}
		}
		i++;
	}
    return (0);
}

int ft_check_cmd(t_minishell *minishell, t_token *token)
{
	char **envp;
	int res;
	struct stat stat_file;

	if (!token || !token->str || !*token->str)
		return (0);
	if (token->str[0] == '/' && !token->str[1])
		return (0);
	if (stat(token->str, &stat_file) == 0 && S_ISREG(stat_file.st_mode))
	{
		if (access(token->str, X_OK) == 0)
		{
			token->path = token->str;
			return (1);
		}
	}
	envp = ft_get_path(minishell);
	if  (!envp)
		return (0);
	res = ft_test_path(minishell, envp, token);
	if (res == 1)
		return 1;
    return (0);
}

int ft_check_expends(t_minishell *minishell, t_token *token)
{
	// $""HOME
    (void)minishell;
    (void)token;
    return (1);
}

int ft_check_pipe(char *str)
{
	if (!str)
		return (0);
	if (str[0] == '|' && !str[1])
		return (1);
    return (0);
}

int is_redirection(t_token *token)
{
    if (token->type == IN_CHEVRON ||
        token->type == OUT_CHEVRON ||
        token->type == IN_DCHEVRON ||
        token->type == OUT_DCHEVRON)
        return (1);
    return (0);
}

int ft_strlen_without_quote(char *str, char type_quote)
{
	int i;
	int len;

	i = 0;
	len = 0;
	while (str[i])
	{
		if (str[i] != type_quote)
			len++;
		i++;
	}
	return (len);
}

char *ft_strdup_without(char *str, char type_quote, t_minishell *minishell)
{
	char *clear_word;
	int i;
	int j;

	clear_word = ft_gc_malloc(ft_strlen_without_quote(str, type_quote) + 1, 
						   &minishell->gc);
	if (!clear_word)
		return (NULL);
	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] != type_quote)
			clear_word[j++] = str[i];
		i++;
	}
	return (clear_word);
}

void ft_filter_quote(t_token *token, t_minishell *minishell)
{
	char type_quote;

	if (!token || !*token->str)
		return;
	type_quote = '\0';
	if (ft_strchr(token->str, '\"'))
		type_quote = '\"';
	else if (ft_strchr(token->str, '\''))
		type_quote = '\'';
	token->str = ft_strdup_without(token->str, type_quote, minishell);
	printf("t: %s\n", token->str);
}

void checker_token(t_minishell *minishell)
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
				if (ft_check_file_of_redirection(token) == 0)
					token->code_error = 299;
				else
					token->next->type = R_FILE;
			}
		}
		else if (token->type == WORD && ft_strchr(token->str, '$'))
		{
			//NOTE: while find $ parcours jusqu'a pas alnum
			// 		si rien trouver mettre chaine vide
			// 		sinon remplacer
			if (ft_check_expends(minishell, token) == 0)
					token->code_error = 300;
		}
		else if (token->type == WORD)
		{
			ft_filter_quote(token, minishell);
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
			if (!ft_check_pipe(token->str))
					token->code_error = 301;
			cmd_find = 0;
		}
		token = token->next;
	}
}
