/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 10:40:00 by githubcopil       #+#    #+#             */
/*   Updated: 2026/02/18 10:17:28 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_H
# define PARSING_H

# include "minishell.h"

// ── PARSING ─────────────────────────────────────────

//PARSING/LEXER.C
void	ft_state_detect(char c, t_minishell *minishell);
int		ft_buffer_add(char *buffer, char c);
int		ft_tokenize(t_minishell *minishell);
//------

//PARSING/PARSER.C
int		checker_token(t_minishell *minishell);
//------

//PARSING/CHECK.C
int		ft_check_flags(char *str);
int		ft_check_redirection(char *str);
//------

//PARSING/CHECK2.C
void	ft_cmd_find_path(t_minishell *minishell, t_token *token);
int		is_redirection(t_token *token);
int		ft_check_pipe(char *str);
//------

//UTILS/PARSER_UTILS.C
void	ft_quotes_handle(t_minishell *minishell, t_token *token);
//------

#endif
//------
