/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_ins.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 09:56:55 by tibras            #+#    #+#             */
/*   Updated: 2026/02/18 10:23:12 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILT_INS_H
# define BUILT_INS_H

# include "minishell.h"

// ── BUILT-INS ───────────────────────────────────────

//BUILT_IN/IS_BUILT_IN.C
int		is_built_in(t_cmd *cmd);
void	run_built_in(t_cmd *cmd, t_minishell *minishell);
//------

//BUILT_IN/ECHO.C
void	ft_echo(char **args);
//------

//BUILT_IN/CD.C
int		ft_cd(t_minishell *minishell, char **args);
//------

//BUILT_IN/PWD.C
int		ft_pwd(void);
//------

//BUILT_IN/EXPORT.C
int		ft_export(t_minishell *minishell, int fd, char **args);
//------

//BUILT_IN/UNSET.C
void	ft_unset(t_env **head_env, char **args);
//------

//BUILT_IN/ENV.C
void	ft_env(t_minishell *minishell);
//------

//BUILT_IN/EXIT.C
void	ft_buildin_exit(t_minishell *minishell, char **args);
//------

//BUILT_IN/EXTRA_BIN.C
void	ft_extra_bin(t_minishell *minishell, int mod);
//------

#endif
