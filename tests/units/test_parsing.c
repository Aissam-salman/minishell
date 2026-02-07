#include "../../includes/minishell.h"
#include "../../includes/testing.h"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>

t_elements *create_mocks_element1();
// t_elements *create_mocks_element2();

Test(Parser, test_check_flags) {
    cr_expect(ft_check_flags("-l") == 1);
    cr_expect(ft_check_flags("-la") == 1);
    cr_expect(ft_check_flags("-lamsdsdsd") == 1);
    cr_expect(ft_check_flags("--format") == 1);
    cr_expect(ft_check_flags("--foo") == 1);
    cr_expect(ft_check_flags("--l0912") == 1);
    cr_expect(ft_check_flags("--1") == 1);
    cr_expect(ft_check_flags("-1") == 1);

    cr_expect(ft_check_flags("-") == 0);
    cr_expect(ft_check_flags("- ") == 0);
    cr_expect(ft_check_flags("") == 0);
    cr_expect(ft_check_flags("--") == 0);
    cr_expect(ft_check_flags("-- ") == 0);
    cr_expect(ft_check_flags("--l -la") == 0);
}

Test(Parser, test_check_redirection)
{
	// <
	t_elements* ele = create_mocks_element1();
	cr_expect(ft_check_redirection(ele) == 1);
	ele->str = ft_strdup(">");
	cr_expect(ft_check_redirection(ele) == 1);
	ele->str = ft_strdup(">>");
	cr_expect(ft_check_redirection(ele) == 1);
	ele->str = ft_strdup("<<");
	cr_expect(ft_check_redirection(ele) == 1);

	ele->str = ft_strdup("");
	cr_expect(ft_check_redirection(ele) == 0);
	ele->str = ft_strdup(" ");
	cr_expect(ft_check_redirection(ele) == 0);
	ele->str = ft_strdup("      ");
	cr_expect(ft_check_redirection(ele) == 0);
	ele->str = ft_strdup("  <");
	cr_expect(ft_check_redirection(ele) == 0);
	ele->str = ft_strdup("><");
	cr_expect(ft_check_redirection(ele) == 0);
	ele->str = ft_strdup(">>>");
	cr_expect(ft_check_redirection(ele) == 0);
	ele->str = ft_strdup("<<<");
	cr_expect(ft_check_redirection(ele) == 0);
	ele->str = ft_strdup("<<>>>>>>");
	cr_expect(ft_check_redirection(ele) == 0);
	ele->str = ft_strdup("<< >>>>>>");
	cr_expect(ft_check_redirection(ele) == 0);
	ele->str = ft_strdup("< >>");
	cr_expect(ft_check_redirection(ele) == 0);
	ele->str = ft_strdup("> >>");
	cr_expect(ft_check_redirection(ele) == 0);
	ele->str = ft_strdup("> > > >");
	cr_expect(ft_check_redirection(ele) == 0);
}
