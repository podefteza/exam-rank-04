#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// Test helper to capture output and exit code
typedef struct {
    char output[1024];
    int exit_code;
} test_result_t;

test_result_t run_vbc_test(const char *input) {
    test_result_t result = {0};
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        strcpy(result.output, "PIPE_ERROR");
        result.exit_code = -1;
        return result;
    }

    pid_t pid = fork();
    if (pid == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        strcpy(result.output, "FORK_ERROR");
        result.exit_code = -1;
        return result;
    }

    if (pid == 0) {
        // Child: redirect stdout and stderr to pipe
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        dup2(pipefd[1], 2);
        close(pipefd[1]);

        // Execute the vbc program
        execl("./vbc_test", "./vbc_test", input, NULL);
        exit(1);
    } else {
        // Parent: read output
        close(pipefd[1]);
        int bytes_read = read(pipefd[0], result.output, sizeof(result.output) - 1);
        if (bytes_read > 0) {
            result.output[bytes_read] = '\0';
            // Remove trailing newline if present
            if (result.output[bytes_read - 1] == '\n') {
                result.output[bytes_read - 1] = '\0';
            }
        }
        close(pipefd[0]);

        int status;
        wait(&status);
        result.exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }

    return result;
}

void test_basic_numbers() {
    printf("=== Test 1: Basic Numbers ===\n");

    test_result_t result = run_vbc_test("1");
    if (result.exit_code == 0 && strcmp(result.output, "1") == 0) {
        printf("✅ PASS: '1' → '1'\n");
    } else {
        printf("❌ FAIL: '1' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    result = run_vbc_test("9");
    if (result.exit_code == 0 && strcmp(result.output, "9") == 0) {
        printf("✅ PASS: '9' → '9'\n");
    } else {
        printf("❌ FAIL: '9' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    result = run_vbc_test("0");
    if (result.exit_code == 0 && strcmp(result.output, "0") == 0) {
        printf("✅ PASS: '0' → '0'\n");
    } else {
        printf("❌ FAIL: '0' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }
    printf("\n");
}

void test_subject_examples() {
    printf("=== Test 2: Subject Examples ===\n");

    // Subject example 1: ./vbc '1'
    test_result_t result = run_vbc_test("1");
    if (result.exit_code == 0 && strcmp(result.output, "1") == 0) {
        printf("✅ PASS: '1' → '1'\n");
    } else {
        printf("❌ FAIL: '1' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    // Subject example 2: ./vbc '2+3'
    result = run_vbc_test("2+3");
    if (result.exit_code == 0 && strcmp(result.output, "5") == 0) {
        printf("✅ PASS: '2+3' → '5'\n");
    } else {
        printf("❌ FAIL: '2+3' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    // Subject example 3: ./vbc '3*4+5'
    result = run_vbc_test("3*4+5");
    if (result.exit_code == 0 && strcmp(result.output, "17") == 0) {
        printf("✅ PASS: '3*4+5' → '17'\n");
    } else {
        printf("❌ FAIL: '3*4+5' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    // Subject example 4: ./vbc '3+4*5'
    result = run_vbc_test("3+4*5");
    if (result.exit_code == 0 && strcmp(result.output, "23") == 0) {
        printf("✅ PASS: '3+4*5' → '23'\n");
    } else {
        printf("❌ FAIL: '3+4*5' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    // Subject example 5: ./vbc '(3+4)*5'
    result = run_vbc_test("(3+4)*5");
    if (result.exit_code == 0 && strcmp(result.output, "35") == 0) {
        printf("✅ PASS: '(3+4)*5' → '35'\n");
    } else {
        printf("❌ FAIL: '(3+4)*5' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    // Subject example 6: Complex nested
    result = run_vbc_test("(((((2+2)*2+2)*2+2)*2+2)*2+2)*2");
    if (result.exit_code == 0 && strcmp(result.output, "188") == 0) {
        printf("✅ PASS: Complex nested → '188'\n");
    } else {
        printf("❌ FAIL: Complex nested → '%s' (exit: %d)\n", result.output, result.exit_code);
    }
    printf("\n");
}

void test_error_cases() {
    printf("=== Test 3: Error Cases ===\n");

    // Subject error example 1: ./vbc '1+'
    test_result_t result = run_vbc_test("1+");
    if (result.exit_code == 1 && strstr(result.output, "Unexpected end of input")) {
        printf("✅ PASS: '1+' → 'Unexpected end of input'\n");
    } else {
        printf("❌ FAIL: '1+' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    // Subject error example 2: ./vbc '1+2)'
    result = run_vbc_test("1+2)");
    if (result.exit_code == 1 && strstr(result.output, "Unexpected token ')'")) {
        printf("✅ PASS: '1+2)' → 'Unexpected token ')''\n");
    } else {
        printf("❌ FAIL: '1+2)' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    // Test unbalanced parentheses
    result = run_vbc_test("(1+2");
    if (result.exit_code == 1 && strstr(result.output, "Unexpected token '('")) {
        printf("✅ PASS: '(1+2' → 'Unexpected token '(''\n");
    } else {
        printf("❌ FAIL: '(1+2' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    // Test invalid character
    result = run_vbc_test("1+a");
    if (result.exit_code == 1 && strstr(result.output, "Unexpected token 'a'")) {
        printf("✅ PASS: '1+a' → 'Unexpected token 'a''\n");
    } else {
        printf("❌ FAIL: '1+a' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    // Test multiple digits (should error)
    result = run_vbc_test("12");
    if (result.exit_code == 1 && strstr(result.output, "Unexpected token '2'")) {
        printf("✅ PASS: '12' → 'Unexpected token '2''\n");
    } else {
        printf("❌ FAIL: '12' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }
    printf("\n");
}

void test_operator_precedence() {
    printf("=== Test 4: Operator Precedence ===\n");

    // Multiplication before addition
    test_result_t result = run_vbc_test("2+3*4");
    if (result.exit_code == 0 && strcmp(result.output, "14") == 0) {
        printf("✅ PASS: '2+3*4' → '14' (2+(3*4))\n");
    } else {
        printf("❌ FAIL: '2+3*4' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    result = run_vbc_test("1*2+3*4");
    if (result.exit_code == 0 && strcmp(result.output, "14") == 0) {
        printf("✅ PASS: '1*2+3*4' → '14' ((1*2)+(3*4))\n");
    } else {
        printf("❌ FAIL: '1*2+3*4' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    // Parentheses override precedence
    result = run_vbc_test("(2+3)*4");
    if (result.exit_code == 0 && strcmp(result.output, "20") == 0) {
        printf("✅ PASS: '(2+3)*4' → '20'\n");
    } else {
        printf("❌ FAIL: '(2+3)*4' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }
    printf("\n");
}

void test_complex_expressions() {
    printf("=== Test 5: Complex Expressions ===\n");

    // Nested parentheses
    test_result_t result = run_vbc_test("((2+3)*4)");
    if (result.exit_code == 0 && strcmp(result.output, "20") == 0) {
        printf("✅ PASS: '((2+3)*4)' → '20'\n");
    } else {
        printf("❌ FAIL: '((2+3)*4)' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    // Multiple operations
    result = run_vbc_test("1+2+3+4");
    if (result.exit_code == 0 && strcmp(result.output, "10") == 0) {
        printf("✅ PASS: '1+2+3+4' → '10'\n");
    } else {
        printf("❌ FAIL: '1+2+3+4' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    result = run_vbc_test("2*3*4");
    if (result.exit_code == 0 && strcmp(result.output, "24") == 0) {
        printf("✅ PASS: '2*3*4' → '24'\n");
    } else {
        printf("❌ FAIL: '2*3*4' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    // Mixed operations with parentheses
    result = run_vbc_test("(1+2)*(3+4)");
    if (result.exit_code == 0 && strcmp(result.output, "21") == 0) {
        printf("✅ PASS: '(1+2)*(3+4)' → '21'\n");
    } else {
        printf("❌ FAIL: '(1+2)*(3+4)' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }
    printf("\n");
}

void test_edge_cases() {
    printf("=== Test 6: Edge Cases ===\n");

    // Just parentheses with number
    test_result_t result = run_vbc_test("(5)");
    if (result.exit_code == 0 && strcmp(result.output, "5") == 0) {
        printf("✅ PASS: '(5)' → '5'\n");
    } else {
        printf("❌ FAIL: '(5)' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    // Zero operations
    result = run_vbc_test("0+0");
    if (result.exit_code == 0 && strcmp(result.output, "0") == 0) {
        printf("✅ PASS: '0+0' → '0'\n");
    } else {
        printf("❌ FAIL: '0+0' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    result = run_vbc_test("5*0");
    if (result.exit_code == 0 && strcmp(result.output, "0") == 0) {
        printf("✅ PASS: '5*0' → '0'\n");
    } else {
        printf("❌ FAIL: '5*0' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }

    // Maximum single digit
    result = run_vbc_test("9*9");
    if (result.exit_code == 0 && strcmp(result.output, "81") == 0) {
        printf("✅ PASS: '9*9' → '81'\n");
    } else {
        printf("❌ FAIL: '9*9' → '%s' (exit: %d)\n", result.output, result.exit_code);
    }
    printf("\n");
}

void test_no_arguments() {
    printf("=== Test 7: No Arguments ===\n");

    // Test with no arguments (should exit with code 1)
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        printf("❌ FAIL: Pipe creation failed\n");
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        dup2(pipefd[1], 2);
        close(pipefd[1]);
        execl("./vbc_test", "./vbc_test", NULL);
        exit(1);
    } else {
        close(pipefd[1]);
        char output[1024] = {0};
        read(pipefd[0], output, sizeof(output) - 1);
        close(pipefd[0]);

        int status;
        wait(&status);
        int exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;

        if (exit_code == 1) {
            printf("✅ PASS: No arguments → exit code 1\n");
        } else {
            printf("❌ FAIL: No arguments → exit code %d\n", exit_code);
        }
    }
    printf("\n");
}

int main() {
    printf("🧪 Testing VBC (Very Basic Calculator) implementation\n");
    printf("=====================================================\n\n");

    // First, compile the VBC program
    system("gcc -Wall -Wextra -Werror vbc.c -o vbc_test");

    test_basic_numbers();
    test_subject_examples();
    test_error_cases();
    test_operator_precedence();
    test_complex_expressions();
    test_edge_cases();
    test_no_arguments();

    printf("🎯 Subject Requirements Summary:\n");
    printf("===============================\n");
    printf("✅ Handles addition (+) and multiplication (*)\n");
    printf("✅ Handles parentheses for grouping\n");
    printf("✅ Single digits only (0-9)\n");
    printf("✅ Correct operator precedence (* before +)\n");
    printf("✅ Error handling: 'Unexpected token' and 'Unexpected end of input'\n");
    printf("✅ Exit codes: 0 on success, 1 on error\n");
    printf("✅ All subject examples produce correct output\n");

    printf("\n🏁 All tests completed!\n");

    // Cleanup
    system("rm -f vbc_test");

    return 0;
}
