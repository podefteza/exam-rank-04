#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

// Include the ft_popen function
int ft_popen(const char *file, char *const argv[], char type);

// Test the exact example from the subject
void test_subject_pipeline_example() {
    printf("=== Testing Subject Pipeline Example ===\n");
    
    // This is the example from the subject:
    int fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
    if (fd == -1) {
        printf("❌ FAIL: ls ft_popen failed\n");
        return;
    }
    
    // Redirect the output to stdin for next command
    dup2(fd, 0);
    close(fd);
    
    fd = ft_popen("grep", (char *const []){"grep", "c", NULL}, 'r');
    if (fd == -1) {
        printf("❌ FAIL: grep ft_popen failed\n");
        return;
    }
    
    char buffer[1024];
    int bytes_read;
    printf("Pipeline output (ls | grep c):\n");
    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }
    
    close(fd);
    wait(NULL);
    wait(NULL);
    
    printf("✅ PASS: Pipeline example completed\n");
}

// Test with a simple get_next_line simulation
char* simple_get_next_line(int fd) {
    static char buffer[1024];
    static int pos = 0;
    static int bytes_read = 0;
    static int eof_reached = 0;
    
    if (eof_reached) return NULL;
    
    char *line = malloc(1024);
    int line_pos = 0;
    
    while (1) {
        if (pos >= bytes_read) {
            bytes_read = read(fd, buffer, sizeof(buffer));
            pos = 0;
            if (bytes_read <= 0) {
                eof_reached = 1;
                if (line_pos == 0) {
                    free(line);
                    return NULL;
                }
                break;
            }
        }
        
        char c = buffer[pos++];
        line[line_pos++] = c;
        
        if (c == '\n' || line_pos >= 1023) {
            break;
        }
    }
    
    line[line_pos] = '\0';
    return line;
}

void test_with_get_next_line() {
    printf("\n=== Testing with get_next_line simulation ===\n");
    
    int fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
    if (fd == -1) {
        printf("❌ FAIL: ft_popen failed\n");
        return;
    }
    
    char *line;
    int line_count = 0;
    printf("Lines from ls:\n");
    while ((line = simple_get_next_line(fd)) != NULL) {
        printf("Line %d: %s", ++line_count, line);
        free(line);
        if (line_count > 10) break; // Limit output
    }
    
    close(fd);
    wait(NULL);
    
    printf("✅ PASS: get_next_line simulation completed (%d lines)\n", line_count);
}

int main() {
    printf("🧪 Additional ft_popen tests (Subject Examples)\n");
    printf("===============================================\n\n");
    
    test_subject_pipeline_example();
    test_with_get_next_line();
    
    printf("\n🏁 Additional tests completed!\n");
    return 0;
}
