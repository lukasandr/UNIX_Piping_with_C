/*
 
 UNIX Piping implememntation with C
 Created by Lukas Andriejunas
 lukas dot andriejunas at gmail dot com
 2018-1106
 
 */


void parse_piping(char *line, char **piped_lines, int *commands_count);
void parse_command (char *piped_line, char **argv);
void execute_command(char **argv);
void clear_line(char *line, int line_length);
void exec_single(char* command_to_execute);
void exec_multiple(char **piped_lines, int commands_count);
void close_fds(int commands_count);
