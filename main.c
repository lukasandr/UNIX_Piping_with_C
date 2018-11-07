/*
 
 UNIX Piping implememntation with C
 Created by Lukas Andriejunas
 lukas dot andriejunas at gmail dot com
 2018-1106
 
 */
 

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "piping.h"


int main()
{
    #define MAX_LINE 1024
    int line_length = 0;
    int commands_count = 0;
    char line[MAX_LINE];
    char *piped_lines[1024];
    char c;
    
    printf("\n>> ");
    // the main loop of application
    while(1)
    {
        clear_line(line, MAX_LINE);
        line_length = 0;
        
        // read input from stdin (screen) until enter
        while ((c = getchar()) != '\n' && line_length < MAX_LINE - 1) {
            if(c != '\xff') //ignore the '\xff' symbol
            {
                line[line_length] = c;
                line_length++;
            }
            if(line_length==0)
            {
                printf("\n>> ");
            }
        }
        
        parse_piping(line, piped_lines, &commands_count);
        
        // if exit was entered
        if (piped_lines[0] && (strcmp(piped_lines[0], "exit") == 0)) exit(0);
        
        
        // execution according to the lenght of the command
        if (commands_count <= 0) continue;
        else if (commands_count == 1)
        {
            exec_single(piped_lines[0]);
            continue;
        }
        else if (commands_count > 1)
        {
            exec_multiple(piped_lines, commands_count);
            continue;
        }

    }
    
    return 0;
}

