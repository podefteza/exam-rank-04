#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int picoshell(char **cmds[]);

int main()
{
    printf("=== Testing sequence like in the main test ===\n");
    
    // First run the invalid command test (like in the main test)
    printf("--- Testing completely invalid command ---\n");
    char *cmd_invalid[] = {"this_command_absolutely_does_not_exist_xyz123", NULL};
    char **cmds_invalid[] = {cmd_invalid, NULL};

    int result = picoshell(cmds_invalid);
    printf("Invalid command result: %d %s\n", result, result == 1 ? "✅" : "❌");

    // Test pipeline with invalid command in middle
    printf("--- Testing pipeline with invalid command in middle ---\n");
    char *cmd_good1[] = {"echo", "test", NULL};
    char *cmd_bad[] = {"invalid_command_xyz", NULL};
    char *cmd_good2[] = {"cat", NULL};
    char **cmds_mixed[] = {cmd_good1, cmd_bad, cmd_good2, NULL};

    result = picoshell(cmds_mixed);
    printf("Mixed pipeline result: %d %s\n", result, result == 1 ? "✅" : "❌");

    // NOW test the multi-stage pipeline (exactly as in the main test)
    printf("--- Testing with multi-stage pipeline ---\n");
    char *cmd_slow1[] = {"echo", "slow test", NULL};
    char *cmd_slow2[] = {"cat", NULL};  // Pass through the input
    char *cmd_slow3[] = {"cat", NULL};  // Another pass through
    char **cmds_slow[] = {cmd_slow1, cmd_slow2, cmd_slow3, NULL};

    result = picoshell(cmds_slow);
    printf("Multi-stage pipeline result: %d %s\n", result, result == 0 ? "✅" : "❌");
    
    return 0;
}
