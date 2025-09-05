#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int picoshell(char **cmds[]);

int main()
{
    printf("=== Debug: Multi-stage pipeline test ===\n");
    
    // Test with command that would normally hang (but we'll use timeout in shell script)
    char *cmd_slow1[] = {"echo", "slow test", NULL};
    char *cmd_slow2[] = {"cat", NULL};  // Pass through the input
    char *cmd_slow3[] = {"cat", NULL};  // Another pass through
    char **cmds_slow[] = {cmd_slow1, cmd_slow2, cmd_slow3, NULL};

    int result = picoshell(cmds_slow);
    printf("Multi-stage pipeline result: %d %s\n", result, result == 0 ? "✅" : "❌");
    
    return result;
}
