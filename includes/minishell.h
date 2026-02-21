/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 20:06:40 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/21 15:05:37 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

// FAITS MAISON
# include "../libft/libft.h"
# include "errors.h"

// PRODUITS IMPORTES
# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdlib.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>

// MACROS
# define BUFFER_SIZE 5000

# define OPERATORS "|<>"
# define SEPARATORS " $\'\"\t0"

extern int			g_signal_glob;

// ── PARSING ─────────────────────────────────────────
# include "parsing.h"

// ── EXEC ────────────────────────────────────────────
# include "exec.h"

// ── BUILT-INS ───────────────────────────────────────
# include "built_ins.h"

// ── UTILS ───────────────────────────────────────────
# include "utils.h"


void	print_header(void);

#endif
