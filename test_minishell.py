#!/usr/bin/env python3
import csv
import subprocess
import os
import sys
import datetime

import tempfile
import shutil

# Configuration
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
TEST_MINISHELL_DIR = os.path.join(SCRIPT_DIR, "test_minishell")
CSV_FILE = os.path.join(TEST_MINISHELL_DIR, "TESTER MINISHELL - Feuille 1.csv")

def get_log_filename():
    timestamp = datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
    return os.path.join(TEST_MINISHELL_DIR, f"minishell_test_{timestamp}.log")

def clean_command(cmd_str):
    if not cmd_str:
        return ""
    if cmd_str.startswith("$> "):
        return cmd_str[3:]
    if cmd_str.startswith("$>"):
        return cmd_str[2:]
    if "touche entrée" in cmd_str:
        return ""
    if "[que des espaces]" in cmd_str:
        return "   "
    if "[que des tabulations]" in cmd_str:
        return "\t\t\t"
    return cmd_str

def run_test_case(minishell_path, command, cwd):
    try:
        # Run minishell with input command via stdin
        result = subprocess.run(
            [minishell_path],
            input=command + "\n",
            capture_output=True,
            cwd=cwd,
            text=True,
            timeout=2  # Prevent hanging tests
        )
        return {
            "stdout": result.stdout,
            "stderr": result.stderr,
            "returncode": result.returncode,
            "error": None
        }
    except subprocess.TimeoutExpired:
        return {
            "stdout": "",
            "stderr": "Timeout",
            "returncode": -1,
            "error": "Timeout"
        }
    except Exception as e:
        return {
            "stdout": "",
            "stderr": str(e),
            "returncode": -1,
            "error": str(e)
        }

def main():
    if len(sys.argv) < 2:
        minishell_path = "./minishell"
    else:
        minishell_path = sys.argv[1]
        
    minishell_path = os.path.abspath(minishell_path)
    if not os.path.isfile(minishell_path):
        print(f"Error: Minishell executable not found at {minishell_path}")
        sys.exit(1)

    print(f"Loading tests from {CSV_FILE}...")
    
    tests_by_category = {}
    current_category = "General"
    
    try:
        with open(CSV_FILE, newline='', encoding='utf-8') as csvfile:
            reader = csv.reader(csvfile)
            for i, row in enumerate(reader):
                if i < 24: continue # Skip initial metadata
                
                # Check for category header (Col 0 not empty)
                if row[0].strip():
                    current_category = row[0].strip()
                    if current_category not in tests_by_category:
                        tests_by_category[current_category] = []
                
                # Check for test case (Col 1 not empty)
                if len(row) > 1 and row[1].strip():
                    raw_cmd = row[1].strip()
                    
                    # Filter 'NON GERE'
                    is_non_gere = False
                    for col in row:
                        if "NON GERE" in col:
                            is_non_gere = True
                            break
                    if is_non_gere:
                        continue
                        
                    # Basic check if it's likely a command (starts with $> or looks like one)
                    # Some commands might be just symbols like '|' but row[1] has '$> |'
                    if not raw_cmd.startswith("$>"):
                        # If it doesn't start with $> and is in Col 1, it might still be a command
                        # verify if it's not a header row (which we check with row[0])
                        # Row 24: Col 0="CATEGORY", Col 1="$> \n"
                        pass

                    cmd = clean_command(raw_cmd)
                    expected_output = row[7] if len(row) > 7 else ""
                    expected_exit = row[8] if len(row) > 8 else ""
                    notes = row[9] if len(row) > 9 else ""

                    tests_by_category.setdefault(current_category, []).append({
                        "command": cmd,
                        "raw_command": raw_cmd,
                        "expected_output": expected_output,
                        "expected_exit": expected_exit,
                        "notes": notes,
                        "line": i + 1
                    })

    except FileNotFoundError:
        print(f"Error: CSV file not found at {CSV_FILE}")
        sys.exit(1)

    print(f"Found {sum(len(t) for t in tests_by_category.values())} tests across {len(tests_by_category)} categories.\n")
    
    total_tests = 0
    total_success = 0
    total_failed = 0
    log_entries = []
    
    LOG_FILE = get_log_filename()
    
    # Create temporary execution directory (e.g., /tmp/minishell_runs_12345)
    temp_test_dir = tempfile.mkdtemp(prefix="minishell_runs_")
    print(f"Test Execution Directory: {temp_test_dir}")
    print(f"Log File Location: {LOG_FILE}\n")

    for category, tests in tests_by_category.items():
        if not tests: continue
        
        print(f"Category: {category}")
        cat_success = 0
        cat_failed = 0
        
        for test in tests:
            res = run_test_case(minishell_path, test["command"], cwd=temp_test_dir)
            
            passed = True
            reasons = []
            
            # 1. Exit Code Check
            if test["expected_exit"].strip().isdigit():
                exp = int(test["expected_exit"].strip())
                if res["returncode"] != exp:
                    passed = False
                    reasons.append(f"Exit Code Mismatch: Expected {exp}, Got {res['returncode']}")

            # 2. Output Check (Heuristic)
            # If expected output suggests an error (contains 'bash:' or 'error'), check for error in stderr
            exp_out_lower = test["expected_exit"].lower() # Wait, using expected_exit for text check? No, expected_output.
            exp_out_text = test["expected_output"]
            
            # Simple check: if expected output is specific, look for it
            # But "bash: syntax error" vs "minishell: syntax error"
            if "bash:" in exp_out_text and passed:
                 # Check if stderr contains "syntax error" or similar keywords if expected output has them
                 if "syntax error" in exp_out_text and "syntax error" not in (res["stderr"] + res["stdout"]):
                      passed = False
                      reasons.append("Missing syntax error message")
                 elif "command not found" in exp_out_text and "command not found" not in (res["stderr"] + res["stdout"]):
                      passed = False
                      reasons.append("Missing 'command not found' message")

            if passed:
                cat_success += 1
                print(".", end="", flush=True)
            else:
                cat_failed += 1
                print("F", end="", flush=True)
                log_entries.append(f"""
[FAIL] Category: {category} | Line: {test['line']}
Command: {test['raw_command']} -> '{test['command']}'
Expected Exit: {test['expected_exit']} | Actual Exit: {res['returncode']}
Expected Output: {test['expected_output']}
Actual Output (Combined):
{res['stdout']}{res['stderr']}
Notes: {test['notes']}
Reasons: {', '.join(reasons)}
--------------------------------------------------
""")

        print(f"\nSuccess: {cat_success} | Failed: {cat_failed}\n")
        total_tests += len(tests)
        total_success += cat_success
        total_failed += cat_failed

    # Write Log
    with open(LOG_FILE, "w", encoding='utf-8') as f:
        f.write("\n".join(log_entries))
        
    print(f"SUMMARY:\nTotal: {total_tests}\nSuccess: {total_success}\nFailed: {total_failed}")
    print(f"Detailed failures written to {LOG_FILE}")

if __name__ == "__main__":
    main()
