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

int pid;
int pipeX[1024][2];

//parse whole user input into pipe lines
void parse_piping(char *line, char **piped_lines, int *commands_count)
{
    *commands_count = 0;
    while (*line != '\0') //if not end of line
    {
        while (*line == '|') *line++ = '\0'; //replace '|' with \0
        *piped_lines++ = line; //save as aargument
        *commands_count += 1;
        while (*line != '|' && *line != '\0' && *line != '\n') line++; //skip until next '|'
    }
    *piped_lines = NULL; //end of argv list
}


//parse one piping line into command + arguments
void parse_command (char *piped_line, char **argv)
{
    while (*piped_line != '\0') //if not end of line
    {
        while (*piped_line == ' ' || *piped_line == '\n') *piped_line++ = '\0'; //replace spaces with \0
        *argv++ = piped_line; //save as argument
        while (*piped_line != ' ' && *piped_line != '\0' && *piped_line != '\n') piped_line++; //skip until next space
    }
    *argv = NULL; //end of argv list
}


// clean input line
void clear_line(char *line, int line_length)
{
    for(int i = 0; i < line_length; i++)
    {
        *line++ = '\0';
    }
}


// execute single command
void exec_single(char* command_to_execute)
{
    char *argv[1024];
    parse_command(command_to_execute, argv);
    
    // fork
    if ((pid = fork()) == -1) {
        perror("bad fork");
        exit(1);
    } else if (pid == 0) {
        execvp(*argv, argv);
        perror("bad exec");
        _exit(1);
    }
    wait(NULL);
}

//execute multiple commands
void exec_multiple(char **piped_lines, int commands_count)
{
    char *argv[1024];
    // first command execution
    // create pipe0
    if (pipe(pipeX[0]) == -1) {
        perror("bad pipe");
        exit(1);
    }
    // fork
    if ((pid = fork()) == -1) {
        perror("bad fork");
        exit(1);
    } else if (pid == 0) {
        parse_command(piped_lines[0], argv);
        // input from stdin (already done)
        // output to pipe1
        dup2(pipeX[0][1], 1);
        // close fds
        close(pipeX[0][0]);
        close(pipeX[0][1]);
        // exec
        execvp(*argv, argv);
        // exec didn't work, exit
        perror("bad exec");
        _exit(1);
    }
    
    // parent
    // managing inner commands
    wait(NULL);
    for(int i = 1; i < commands_count - 1; i++)
    {
        // create pipe i
        if (pipe(pipeX[i]) == -1) {
            perror("bad pipe1");
            exit(1);
        }
        if ((pid = fork()) == -1) {
            perror("bad fork3");
            exit(1);
        } else if (pid == 0) {
            parse_command(piped_lines[i], argv);
            // input from previous pipe
            dup2(pipeX[i-1][0], 0);
            // output to next pipe
            dup2(pipeX[i][1], 1);
            // close fds
            fprintf(stderr,"here: %i",i);
            for(int j=0; j<i; j++)
            {
                close(pipeX[j][0]);
                close(pipeX[j][1]);
            }
            // exec
            execvp(*argv, argv);
            // exec didn't work, exit
            perror("bad exec");
            _exit(1);
        }
        // parent
        wait(NULL);
    }
    
    //wait(NULL);
    // last command execution
    if ((pid = fork()) == -1) {
        perror("bad fork last");
        exit(1);
    } else if (pid == 0) {
        //fprintf(stderr,"\nhere");
        parse_command(piped_lines[commands_count-1], argv);
        // input from last pipe
        dup2(pipeX[commands_count-2][0], 0);
        // output to stdout (already done)
        // close fds
        for(int j=0; j<commands_count-1; j++)
        {
            close(pipeX[j][0]);
            close(pipeX[j][1]);
        }
        // exec
        execvp(*argv, argv);
        // exec didn't work, exit
        perror("bad exec");
        _exit(1);
    }
    wait(NULL);
    // parent
    
    for(int j=0; j<commands_count-1; j++)
    {
        close(pipeX[j][0]);
        close(pipeX[j][1]);
    }
   
    
}

void close_fds(int commands_count)
{
    for(int j=0; j<commands_count-1; j++)
    {
        close(pipeX[j][0]);
        close(pipeX[j][1]);
    }
}
