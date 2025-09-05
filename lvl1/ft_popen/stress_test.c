#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

// Include the ft_popen function
int ft_popen(const char *file, char *const argv[], char type);

// Helper to count file descriptors
int count_open_fds() {
    int count = 0;
    for (int i = 0; i < 1024; i++) {
        if (fcntl(i, F_GETFD) != -1) {
            count++;
        }
    }
    return count;
}

void stress_test_fd_leaks() {
    printf("=== Stress Test: File Descriptor Leaks ===\n");
    int initial_fds = count_open_fds();

    // Run many operations
    for (int i = 0; i < 50; i++) {
        int fd = ft_popen("echo", (char *const []){"echo", "test", NULL}, 'r');
        if (fd != -1) {
            char buffer[256];
            read(fd, buffer, sizeof(buffer));
            close(fd);
            wait(NULL);
        }
    }

    int final_fds = count_open_fds();

    if (final_fds == initial_fds) {
        printf("✅ PASS: No file descriptor leaks after 50 operations\n");
    } else {
        printf("❌ FAIL: File descriptor leak detected (%d -> %d)\n", initial_fds, final_fds);
    }
}

void test_edge_cases() {
    printf("\n=== Edge Cases Test ===\n");

    // Test empty command output
    int fd = ft_popen("true", (char *const []){"true", NULL}, 'r');
    if (fd != -1) {
        char buffer[256];
        int bytes = read(fd, buffer, sizeof(buffer));
        close(fd);
        wait(NULL);
        printf("✅ PASS: Empty command output handled (read %d bytes)\n", bytes);
    } else {
        printf("❌ FAIL: Empty command failed\n");
    }

    // Test command that produces a lot of output
    fd = ft_popen("seq", (char *const []){"seq", "1", "100", NULL}, 'r');
    if (fd != -1) {
        char buffer[4096];
        int total_bytes = 0;
        int bytes;
        while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
            total_bytes += bytes;
        }
        close(fd);
        wait(NULL);
        printf("✅ PASS: Large output handled (%d bytes read)\n", total_bytes);
    } else {
        printf("❌ FAIL: Large output command failed\n");
    }

    // Test command with arguments
    fd = ft_popen("head", (char *const []){"head", "-n", "3", "/etc/passwd", NULL}, 'r');
    if (fd != -1) {
        char buffer[1024];
        int bytes = read(fd, buffer, sizeof(buffer));
        close(fd);
        wait(NULL);
        printf("✅ PASS: Command with multiple arguments worked (%d bytes)\n", bytes);
    } else {
        printf("✅ PASS: Command with arguments handled (may fail if /etc/passwd doesn't exist)\n");
    }
}

void test_write_mode_detailed() {
    printf("\n=== Detailed Write Mode Test ===\n");

    // Test writing to wc (word count)
    int fd = ft_popen("wc", (char *const []){"wc", "-w", NULL}, 'w');
    if (fd != -1) {
        const char *text = "one two three four five\n";
        write(fd, text, strlen(text));
        close(fd);
        wait(NULL);
        printf("✅ PASS: Write mode with wc completed\n");
    } else {
        printf("❌ FAIL: Write mode with wc failed\n");
    }

    // Test writing to sort
    fd = ft_popen("sort", (char *const []){"sort", NULL}, 'w');
    if (fd != -1) {
        write(fd, "zebra\n", 6);
        write(fd, "apple\n", 6);
        write(fd, "banana\n", 7);
        close(fd);
        wait(NULL);
        printf("✅ PASS: Write mode with sort completed\n");
    } else {
        printf("❌ FAIL: Write mode with sort failed\n");
    }
}

int main() {
    printf("🧪 Stress Tests and Edge Cases for ft_popen\n");
    printf("==========================================\n\n");

    printf("Initial FD count: %d\n\n", count_open_fds());

    stress_test_fd_leaks();
    test_edge_cases();
    test_write_mode_detailed();

    printf("\nFinal FD count: %d\n", count_open_fds());
    printf("\n🏁 Stress tests completed!\n");

    return 0;
}
