#include "../../includes/testing.h"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>

Test(Parser, test_check_flags) {
    cr_expect(ft_check_flags("-l") == 1);
    cr_expect(ft_check_flags("-la") == 1);
    cr_expect(ft_check_flags("-lamsdsdsd") == 1);
    cr_expect(ft_check_flags("--format") == 1);
    cr_expect(ft_check_flags("--foo") == 1);
    cr_expect(ft_check_flags("-") == 0);
    cr_expect(ft_check_flags("- ") == 0);
    cr_expect(ft_check_flags("") == 0);
}
