#!/bin/bash

# ============================================================
# MINISHELL TEST SCRIPT
# ============================================================

# Configuration
MINISHELL="./minishell"

# Colors
GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[1;33m"
BLUE="\033[0;34m"
CYAN="\033[0;36m"
MAGENTA="\033[0;35m"
BOLD="\033[1m"
RESET="\033[0m"

# Counters
TOTAL=0
PASSED=0
FAILED=0

# Function to print a separator line
print_separator() {
    echo -e "${CYAN}════════════════════════════════════════════════════════════════════════════════${RESET}"
}

# Function to print a box with text
print_box() {
    local text=$1
    echo -e "${BOLD}${BLUE}┌────────────────────────────────────────────────────────────────────────────────┐${RESET}"
    printf "${BOLD}${BLUE}│${RESET} %-78s ${BOLD}${BLUE}│${RESET}\n" "$text"
    echo -e "${BOLD}${BLUE}└────────────────────────────────────────────────────────────────────────────────┘${RESET}"
}

# Main test function
run_test() {
    local cmd=$1
    ((TOTAL++))

    echo ""
    print_separator
    echo -e "${BOLD}${MAGENTA}TEST #${TOTAL}:${RESET} ${YELLOW}${cmd}${RESET}"
    print_separator

    # Get output from Bash
    bash_out=$(echo "$cmd" | bash 2>&1)
    bash_exit=$?

    # Get output from Minishell (adjust prompt removal if needed)
    mini_raw=$(echo "$cmd" | $MINISHELL 2>&1)
    mini_out=$(echo "$mini_raw" | sed 's/foo$> //g' | grep -v "^exit$")
    mini_exit=$(echo "$cmd" | $MINISHELL 2>/dev/null >/dev/null; echo $?)

    # Display side by side
    echo ""
    printf "${BOLD}%-40s %-40s${RESET}\n" "🐚 BASH OUTPUT" "⚡ MINISHELL OUTPUT"
    printf "${BOLD}%-40s %-40s${RESET}\n" "$(printf '─%.0s' {1..38})" "$(printf '─%.0s' {1..38})"

    # Split outputs by lines and display side by side
    IFS=$'\n' read -rd '' -a bash_lines <<< "$bash_out"
    IFS=$'\n' read -rd '' -a mini_lines <<< "$mini_out"

    max_lines=${#bash_lines[@]}
    if [ ${#mini_lines[@]} -gt $max_lines ]; then
        max_lines=${#mini_lines[@]}
    fi

    for ((i=0; i<max_lines; i++)); do
        bash_line="${bash_lines[$i]:-}"
        mini_line="${mini_lines[$i]:-}"
        printf "%-40s %-40s\n" "$bash_line" "$mini_line"
    done

    echo ""
    printf "${BOLD}%-40s %-40s${RESET}\n" "📊 EXIT CODE" "📊 EXIT CODE"
    printf "${BOLD}%-40s %-40s${RESET}\n" "$(printf '─%.0s' {1..38})" "$(printf '─%.0s' {1..38})"
    printf "%-40s %-40s\n" "$bash_exit" "$mini_exit"
    echo ""

    # Compare and show result
    if [ "$bash_out" == "$mini_out" ] && [ "$bash_exit" == "$mini_exit" ]; then
        echo -e "${BOLD}${GREEN}✓ RESULT: PASSED${RESET}"
        ((PASSED++))
    else
        echo -e "${BOLD}${RED}✗ RESULT: FAILED${RESET}"
        ((FAILED++))

        if [ "$bash_out" != "$mini_out" ]; then
            echo -e "${RED}  → Output differs${RESET}"
        fi
        if [ "$bash_exit" != "$mini_exit" ]; then
            echo -e "${RED}  → Exit code differs${RESET}"
        fi
    fi
    echo ""
}

# ============================================================
# START TESTS
# ============================================================

clear
print_box "MINISHELL TEST SUITE"
echo ""

# Add your tests here
run_test "ls"
# run_test "echo hello world"
# run_test "pwd"
# run_test "export VAR=42"
# run_test "echo \$VAR"
# run_test "ls | wc -l"
# run_test "ls -la | grep objs | wc -l"

# ============================================================
# SUMMARY
# ============================================================

print_separator
echo ""
print_box "TEST SUMMARY"
echo ""
echo -e "${BOLD}Total Tests:${RESET}  $TOTAL"
echo -e "${GREEN}${BOLD}Passed:${RESET}       ${GREEN}$PASSED${RESET}"
echo -e "${RED}${BOLD}Failed:${RESET}       ${RED}$FAILED${RESET}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}${BOLD}🎉 ALL TESTS PASSED! 🎉${RESET}"
else
    echo -e "${RED}${BOLD}⚠️  SOME TESTS FAILED ⚠️${RESET}"
fi

echo ""
print_separator
