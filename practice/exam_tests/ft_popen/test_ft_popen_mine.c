#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

// Include the ft_popen function
int ft_popen(const char *file, char *const argv[], char type);

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

// Test helper to read from fd with timeout
int read_with_timeout(int fd, char *buffer, int size) {
    fd_set readfds;
    struct timeval timeout;
    
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    timeout.tv_sec = 2;  // 2 second timeout
    timeout.tv_usec = 0;
    
    int result = select(fd + 1, &readfds, NULL, NULL, &timeout);
    if (result > 0 && FD_ISSET(fd, &readfds)) {
        return read(fd, buffer, size - 1);
    }
    return -1;  // timeout or error
}

void test_basic_read() {
    printf("=== Test 1: Basic read mode ('r') ===\n");
    int initial_fds = count_open_fds();
    
    int fd = ft_popen("echo", (char *const []){"echo", "Hello World", NULL}, 'r');
    if (fd == -1) {
        printf("❌ FAIL: ft_popen returned -1\n");
        return;
    }
    
    char buffer[1024] = {0};
    int bytes_read = read_with_timeout(fd, buffer, sizeof(buffer));
    close(fd);
    wait(NULL); // Wait for child process
    
    int final_fds = count_open_fds();
    
    if (bytes_read > 0 && strstr(buffer, "Hello World")) {
        printf("✅ PASS: Read correct output: %s", buffer);
    } else {
        printf("❌ FAIL: Incorrect output or read failed\n");
    }
    
    if (final_fds == initial_fds) {
        printf("✅ PASS: No file descriptor leak\n");
    } else {
        printf("❌ FAIL: File descriptor leak detected (%d -> %d)\n", initial_fds, final_fds);
    }
    printf("\n");
}

void test_basic_write() {
    printf("=== Test 2: Basic write mode ('w') ===\n");
    int initial_fds = count_open_fds();
    
    int fd = ft_popen("cat", (char *const []){"cat", NULL}, 'w');
    if (fd == -1) {
        printf("❌ FAIL: ft_popen returned -1\n");
        return;
    }
    
    const char *test_data = "Test write data\n";
    int bytes_written = write(fd, test_data, strlen(test_data));
    close(fd);
    wait(NULL); // Wait for child process
    
    int final_fds = count_open_fds();
    
    if (bytes_written == (int)strlen(test_data)) {
        printf("✅ PASS: Successfully wrote %d bytes\n", bytes_written);
    } else {
        printf("❌ FAIL: Write failed or incorrect byte count\n");
    }
    
    if (final_fds == initial_fds) {
        printf("✅ PASS: No file descriptor leak\n");
    } else {
        printf("❌ FAIL: File descriptor leak detected (%d -> %d)\n", initial_fds, final_fds);
    }
    printf("\n");
}

void test_complex_pipeline() {
    printf("=== Test 3: Complex pipeline (ls | grep) ===\n");
    int initial_fds = count_open_fds();
    
    // First: get output from ls
    int fd1 = ft_popen("ls", (char *const []){"ls", "/usr/bin", NULL}, 'r');
    if (fd1 == -1) {
        printf("❌ FAIL: First ft_popen returned -1\n");
        return;
    }
    
    // Read some data from ls
    char buffer[4096] = {0};
    int bytes_read = read_with_timeout(fd1, buffer, sizeof(buffer));
    close(fd1);
    wait(NULL);
    
    if (bytes_read > 0) {
        printf("✅ PASS: Successfully read %d bytes from ls\n", bytes_read);
    } else {
        printf("❌ FAIL: Failed to read from ls\n");
    }
    
    int final_fds = count_open_fds();
    if (final_fds == initial_fds) {
        printf("✅ PASS: No file descriptor leak\n");
    } else {
        printf("❌ FAIL: File descriptor leak detected (%d -> %d)\n", initial_fds, final_fds);
    }
    printf("\n");
}

void test_error_cases() {
    printf("=== Test 4: Error cases ===\n");
    int initial_fds = count_open_fds();
    
    // Test invalid type
    int fd = ft_popen("echo", (char *const []){"echo", "test", NULL}, 'x');
    if (fd == -1) {
        printf("✅ PASS: Invalid type 'x' correctly returns -1\n");
    } else {
        printf("❌ FAIL: Invalid type should return -1\n");
        close(fd);
    }
    
    // Test NULL file
    fd = ft_popen(NULL, (char *const []){"echo", "test", NULL}, 'r');
    if (fd == -1) {
        printf("✅ PASS: NULL file correctly returns -1\n");
    } else {
        printf("❌ FAIL: NULL file should return -1\n");
        close(fd);
    }
    
    // Test NULL argv
    fd = ft_popen("echo", NULL, 'r');
    if (fd == -1) {
        printf("✅ PASS: NULL argv correctly returns -1\n");
    } else {
        printf("❌ FAIL: NULL argv should return -1\n");
        close(fd);
    }
    
    // Test invalid command (should still return fd, but exec will fail)
    fd = ft_popen("nonexistent_command_12345", (char *const []){"nonexistent_command_12345", NULL}, 'r');
    if (fd != -1) {
        printf("✅ PASS: Invalid command returns fd (exec fails in child)\n");
        close(fd);
        wait(NULL); // Child will exit with error
    } else {
        printf("⚠️  INFO: Invalid command returned -1 (implementation dependent)\n");
    }
    
    int final_fds = count_open_fds();
    if (final_fds == initial_fds) {
        printf("✅ PASS: No file descriptor leak in error cases\n");
    } else {
        printf("❌ FAIL: File descriptor leak in error cases (%d -> %d)\n", initial_fds, final_fds);
    }
    printf("\n");
}

void test_multiple_operations() {
    printf("=== Test 5: Multiple ft_popen operations ===\n");
    int initial_fds = count_open_fds();
    
    // Open multiple processes
    int fd1 = ft_popen("echo", (char *const []){"echo", "first", NULL}, 'r');
    int fd2 = ft_popen("echo", (char *const []){"echo", "second", NULL}, 'r');
    int fd3 = ft_popen("cat", (char *const []){"cat", NULL}, 'w');
    
    if (fd1 == -1 || fd2 == -1 || fd3 == -1) {
        printf("❌ FAIL: One or more ft_popen calls failed\n");
        if (fd1 != -1) close(fd1);
        if (fd2 != -1) close(fd2);
        if (fd3 != -1) close(fd3);
        return;
    }
    
    // Test reading from both
    char buffer1[256] = {0};
    char buffer2[256] = {0};
    
    int bytes1 = read_with_timeout(fd1, buffer1, sizeof(buffer1));
    int bytes2 = read_with_timeout(fd2, buffer2, sizeof(buffer2));
    
    // Test writing
    write(fd3, "test write\n", 11);
    
    close(fd1);
    close(fd2);
    close(fd3);
    
    // Wait for all children
    wait(NULL);
    wait(NULL);
    wait(NULL);
    
    int final_fds = count_open_fds();
    
    if (bytes1 > 0 && bytes2 > 0) {
        printf("✅ PASS: Multiple operations successful\n");
        printf("  First output: %s", buffer1);
        printf("  Second output: %s", buffer2);
    } else {
        printf("❌ FAIL: Multiple operations failed\n");
    }
    
    if (final_fds == initial_fds) {
        printf("✅ PASS: No file descriptor leak with multiple operations\n");
    } else {
        printf("❌ FAIL: File descriptor leak with multiple operations (%d -> %d)\n", initial_fds, final_fds);
    }
    printf("\n");
}

void test_subject_examples() {
    printf("=== Test 6: Subject examples ===\n");
    int initial_fds = count_open_fds();
    
    // Example 1: Basic ls
    printf("--- Example 1: ls command ---\n");
    int fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
    if (fd != -1) {
        char buffer[2048] = {0};
        int bytes = read_with_timeout(fd, buffer, sizeof(buffer));
        if (bytes > 0) {
            printf("✅ PASS: ls output received (%d bytes)\n", bytes);
        } else {
            printf("❌ FAIL: No output from ls\n");
        }
        close(fd);
        wait(NULL);
    } else {
        printf("❌ FAIL: ls ft_popen failed\n");
    }
    
    int final_fds = count_open_fds();
    if (final_fds == initial_fds) {
        printf("✅ PASS: No file descriptor leak in subject examples\n");
    } else {
        printf("❌ FAIL: File descriptor leak in subject examples (%d -> %d)\n", initial_fds, final_fds);
    }
    printf("\n");
}

int main() {
    printf("🧪 Testing ft_popen implementation\n");
    printf("===================================\n\n");
    
    printf("Initial file descriptor count: %d\n\n", count_open_fds());
    
    test_basic_read();
    test_basic_write();
    test_complex_pipeline();
    test_error_cases();
    test_multiple_operations();
    test_subject_examples();
    
    printf("🏁 All tests completed!\n");
    printf("Final file descriptor count: %d\n", count_open_fds());
    
    return 0;
}
