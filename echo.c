#include "echo.h"

int echo_call(char (*args)[MAX_COMMAND_LENGTH], int num)
{
    if (num == 1)
    {
        printf("\n");
        return 0;
    }
    for (int i = 1; i < num; i++)
    {
        printf("%s ", args[i]);
    }
    printf("\n");
    return 0;
}