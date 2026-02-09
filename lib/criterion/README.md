# Criterion Testing Framework - Local Installation

This directory contains a local installation of Criterion v2.4.1 for this project.

## Structure
```
lib/criterion/
├── include/        # Header files
└── lib/           # Shared libraries
```

## Usage

To compile and run unit tests:
```bash
make units
./unit_tests
```

## Writing Tests

Create test files in `tests/units/` directory:

```c
#include <criterion/criterion.h>

Test(suite_name, test_name)
{
    cr_assert_eq(2 + 2, 4, "2 + 2 should equal 4");
}
```

## Makefile Configuration

The Makefile is already configured with:
- Include path: `-Ilib/criterion/include`
- Library path: `-Llib/criterion/lib`
- Runtime path: `-Wl,-rpath,lib/criterion/lib`

No system-wide installation needed - everything runs locally!
