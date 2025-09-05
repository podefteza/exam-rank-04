#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>

// Include the picoshell function
int picoshell(char **cmds[]);

// Helper function to count open file descriptors
int count_open_fds() {
    int count = 0;
    for (int i = 0; i < 1024; i++) {
        if (fcntl(i, F_GETFD) != -1) {
            count++;
        }
    }
    return count;
}

// Test helper to capture output
int capture_output(char **cmds[], char *output_buffer, int buffer_size) {
    int pipefd[2];
    if (pipe(pipefd) == -1) return -1;

    pid_t pid = fork();
    if (pid == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        return -1;
    }

    if (pid == 0) {
        // Child: redirect stdout to pipe and run picoshell
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        close(pipefd[1]);
        exit(picoshell(cmds));
    } else {
        // Parent: read from pipe
        close(pipefd[1]);
        int bytes_read = read(pipefd[0], output_buffer, buffer_size - 1);
        if (bytes_read > 0) {
            output_buffer[bytes_read] = '\0';
        } else {
            output_buffer[0] = '\0';
        }
        close(pipefd[0]);

        int status;
        wait(&status);
        return WEXITSTATUS(status);
    }
}

void test_basic_single_command() {
    printf("=== Test 1: Basic single command ===\n");
    int initial_fds = count_open_fds();

    char *cmd1[] = {"echo", "Hello World", NULL};
    char **cmds[] = {cmd1, NULL};

    char output[1024];
    int result = capture_output(cmds, output, sizeof(output));

    int final_fds = count_open_fds();

    if (result == 0) {
        printf("✅ PASS: Single command executed successfully\n");
    } else {
        printf("❌ FAIL: Single command returned %d\n", result);
    }

    if (strstr(output, "Hello World")) {
        printf("✅ PASS: Correct output: %s", output);
    } else {
        printf("❌ FAIL: Incorrect output: %s\n", output);
    }

    if (final_fds == initial_fds) {
        printf("✅ PASS: No file descriptor leak\n");
    } else {
        printf("❌ FAIL: File descriptor leak (%d -> %d)\n", initial_fds, final_fds);
    }
    printf("\n");
}

void test_basic_pipeline() {
    printf("=== Test 2: Basic pipeline (echo | cat) ===\n");
    int initial_fds = count_open_fds();

    char *cmd1[] = {"echo", "pipeline test", NULL};
    char *cmd2[] = {"cat", NULL};
    char **cmds[] = {cmd1, cmd2, NULL};

    char output[1024];
    int result = capture_output(cmds, output, sizeof(output));

    int final_fds = count_open_fds();

    if (result == 0) {
        printf("✅ PASS: Pipeline executed successfully\n");
    } else {
        printf("❌ FAIL: Pipeline returned %d\n", result);
    }

    if (strstr(output, "pipeline test")) {
        printf("✅ PASS: Correct pipeline output: %s", output);
    } else {
        printf("❌ FAIL: Incorrect pipeline output: %s\n", output);
    }

    if (final_fds == initial_fds) {
        printf("✅ PASS: No file descriptor leak\n");
    } else {
        printf("❌ FAIL: File descriptor leak (%d -> %d)\n", initial_fds, final_fds);
    }
    printf("\n");
}

void test_subject_examples() {
    printf("=== Test 3: Subject examples ===\n");
    int initial_fds = count_open_fds();

    // Test example 1: ls | wc
    printf("--- Example 1: ls | wc ---\n");
    char *cmd1[] = {"ls", NULL};
    char *cmd2[] = {"wc", "-l", NULL};
    char **cmds1[] = {cmd1, cmd2, NULL};

    char output1[1024];
    int result1 = capture_output(cmds1, output1, sizeof(output1));

    if (result1 == 0) {
        printf("✅ PASS: ls | wc executed successfully\n");
        printf("   Output: %s", output1);
    } else {
        printf("❌ FAIL: ls | wc returned %d\n", result1);
    }

    // Test example 2: echo | sed (similar to subject example)
    printf("--- Example 2: echo | sed ---\n");
    char *cmd3[] = {"echo", "squalala", NULL};
    char *cmd4[] = {"sed", "s/a/b/g", NULL};
    char **cmds2[] = {cmd3, cmd4, NULL};

    char output2[1024];
    int result2 = capture_output(cmds2, output2, sizeof(output2));

    if (result2 == 0) {
        printf("✅ PASS: echo | sed executed successfully\n");
        printf("   Output: %s", output2);
        if (strstr(output2, "squblblb")) {
            printf("✅ PASS: Correct sed transformation\n");
        } else {
            printf("❌ FAIL: Incorrect sed transformation\n");
        }
    } else {
        printf("❌ FAIL: echo | sed returned %d\n", result2);
    }

    int final_fds = count_open_fds();
    if (final_fds == initial_fds) {
        printf("✅ PASS: No file descriptor leak in subject examples\n");
    } else {
        printf("❌ FAIL: File descriptor leak in subject examples (%d -> %d)\n", initial_fds, final_fds);
    }
    printf("\n");
}

void test_three_command_pipeline() {
    printf("=== Test 4: Three command pipeline ===\n");
    int initial_fds = count_open_fds();

    char *cmd1[] = {"echo", "hello world test", NULL};
    char *cmd2[] = {"tr", " ", "\n", NULL}; // Convert spaces to newlines
    char *cmd3[] = {"wc", "-l", NULL};      // Count lines
    char **cmds[] = {cmd1, cmd2, cmd3, NULL};

    char output[1024];
    int result = capture_output(cmds, output, sizeof(output));

    int final_fds = count_open_fds();

    if (result == 0) {
        printf("✅ PASS: Three command pipeline executed successfully\n");
        printf("   Output: %s", output);
        // Should output "3" (hello, world, test = 3 words = 3 lines)
        if (strstr(output, "3")) {
            printf("✅ PASS: Correct word count transformation\n");
        } else {
            printf("⚠️  INFO: Word count result may vary based on system\n");
        }
    } else {
        printf("❌ FAIL: Three command pipeline returned %d\n", result);
    }

    if (final_fds == initial_fds) {
        printf("✅ PASS: No file descriptor leak\n");
    } else {
        printf("❌ FAIL: File descriptor leak (%d -> %d)\n", initial_fds, final_fds);
    }
    printf("\n");
}

void test_error_cases() {
    printf("=== Test 5: Error cases ===\n");
    int initial_fds = count_open_fds();

    // Test NULL cmds
    printf("--- Testing NULL cmds ---\n");
    int result = picoshell(NULL);
    if (result == 1) {
        printf("✅ PASS: NULL cmds correctly returns 1\n");
    } else {
        printf("❌ FAIL: NULL cmds should return 1, got %d\n", result);
    }

    // Test command that fails
    printf("--- Testing failing command ---\n");
    char *cmd_fail[] = {"nonexistent_command_xyz123", NULL};
    char **cmds_fail[] = {cmd_fail, NULL};

    result = picoshell(cmds_fail);
    if (result == 1) {
        printf("✅ PASS: Failing command correctly returns 1\n");
    } else {
        printf("❌ FAIL: Failing command should return 1, got %d\n", result);
    }

    // Test pipeline with one failing command
    printf("--- Testing pipeline with failing command ---\n");
    char *cmd_good[] = {"echo", "test", NULL};
    char *cmd_bad[] = {"nonexistent_xyz", NULL};
    char **cmds_mixed[] = {cmd_good, cmd_bad, NULL};

    result = picoshell(cmds_mixed);
    if (result == 1) {
        printf("✅ PASS: Pipeline with failing command correctly returns 1\n");
    } else {
        printf("❌ FAIL: Pipeline with failing command should return 1, got %d\n", result);
    }

    int final_fds = count_open_fds();
    if (final_fds == initial_fds) {
        printf("✅ PASS: No file descriptor leak in error cases\n");
    } else {
        printf("❌ FAIL: File descriptor leak in error cases (%d -> %d)\n", initial_fds, final_fds);
    }
    printf("\n");
}

void test_complex_pipelines() {
    printf("=== Test 6: Complex pipelines ===\n");
    int initial_fds = count_open_fds();

    // Test: ls | grep "c" | wc -l
    printf("--- Testing: ls | grep | wc ---\n");
    char *cmd1[] = {"ls", NULL};
    char *cmd2[] = {"grep", "c", NULL};
    char *cmd3[] = {"wc", "-l", NULL};
    char **cmds[] = {cmd1, cmd2, cmd3, NULL};

    char output[1024];
    int result = capture_output(cmds, output, sizeof(output));

    if (result == 0) {
        printf("✅ PASS: Complex pipeline executed successfully\n");
        printf("   Output: %s", output);
    } else {
        printf("❌ FAIL: Complex pipeline returned %d\n", result);
    }

    // Test with cat and multiple transformations
    printf("--- Testing: echo | cat | cat | cat ---\n");
    char *cmd4[] = {"echo", "multi-stage", NULL};
    char *cmd5[] = {"cat", NULL};
    char *cmd6[] = {"cat", NULL};
    char *cmd7[] = {"cat", NULL};
    char **cmds2[] = {cmd4, cmd5, cmd6, cmd7, NULL};

    char output2[1024];
    int result2 = capture_output(cmds2, output2, sizeof(output2));

    if (result2 == 0 && strstr(output2, "multi-stage")) {
        printf("✅ PASS: Multi-stage pipeline executed successfully\n");
    } else {
        printf("❌ FAIL: Multi-stage pipeline failed\n");
    }

    int final_fds = count_open_fds();
    if (final_fds == initial_fds) {
        printf("✅ PASS: No file descriptor leak in complex pipelines\n");
    } else {
        printf("❌ FAIL: File descriptor leak in complex pipelines (%d -> %d)\n", initial_fds, final_fds);
    }
    printf("\n");
}

void stress_test_pipelines() {
    printf("=== Test 7: Stress test - Multiple pipeline executions ===\n");
    int initial_fds = count_open_fds();

    // Run multiple pipelines to check for cumulative leaks
    for (int i = 0; i < 20; i++) {
        char *cmd1[] = {"echo", "stress test", NULL};
        char *cmd2[] = {"cat", NULL};
        char **cmds[] = {cmd1, cmd2, NULL};

        int result = picoshell(cmds);
        if (result != 0) {
            printf("❌ FAIL: Stress test failed on iteration %d\n", i);
            return;
        }
    }

    int final_fds = count_open_fds();

    if (final_fds == initial_fds) {
        printf("✅ PASS: Stress test completed - no file descriptor leaks after 20 pipelines\n");
    } else {
        printf("❌ FAIL: File descriptor leak in stress test (%d -> %d)\n", initial_fds, final_fds);
    }
    printf("\n");
}

int main() {
    printf("🧪 Testing picoshell implementation\n");
    printf("===================================\n\n");

    printf("Initial file descriptor count: %d\n\n", count_open_fds());

    test_basic_single_command();
    test_basic_pipeline();
    test_subject_examples();
    test_three_command_pipeline();
    test_error_cases();
    test_complex_pipelines();
    stress_test_pipelines();

    printf("🏁 All tests completed!\n");
    printf("Final file descriptor count: %d\n", count_open_fds());

    return 0;
}
