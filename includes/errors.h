/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 16:31:44 by tibras            #+#    #+#             */
/*   Updated: 2026/02/09 14:52:56 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_H
# define ERRORS_H

typedef enum e_errors {
	CLEAN_EXIT = 0,
	NO_SUCH_FILE_O_DIR = 1,
	SYNTAX_ERROR = 2,
	PARSING_FAIL = 50,
	MALLOC_FAIL = 99,
} t_errors ;

#endif
