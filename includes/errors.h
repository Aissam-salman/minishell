/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 16:31:44 by tibras            #+#    #+#             */
/*   Updated: 2026/02/12 18:04:32 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_H
# define ERRORS_H

typedef enum e_errors {
	CLEAN_EXIT = 0,
	NO_SUCH_FILE_O_DIR = 1,
	SYNTAX_ERROR = 2,
	PARSING_FAIL = 50,
	BUFFER_FAIL = 98,
	MALLOC_FAIL = 99,
} t_errors ;

#endif
