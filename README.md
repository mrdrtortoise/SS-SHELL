This is the README file for the s3 implementation of Jad Saad and Maximilian Heuberger

This shell has the following features:

Features that are not part of extra work:
1. Basic Commands
2. Commands with Redirection (only on redirection per command)
3. Support for cd (including special builtins for cd with no arguments and cd with argument '-' -> returns to previous working directory)
4. Commands with Pipes
5. Batched Commands

Features that are extra work:
1. Commands with Redirection (can have as many redirections as possible as long as the input does not exceed MAX_LINE)
2. Subshells (with nesting)

Notes: 

the banner S3 SHELL is only printed once when the shell is launched. Color coding on the prompt shows the user more easily in which directory they are.

all commands that are passed into the shell are parsed based on a ' ' delimiter, hence the input must seperate arguments with a space.

arguments include not only arguments of standard binaries
(e.g. cat foo.txt must have a space between filename "cat" and its argument), but also to seperate special characters that determine the output of the command.

In a pipelined command, commands between pipe stages must have a space before and after the pipe operator '|'.
(e.g. cat foo.txt | wc)

in a command with redireciton, the output/input file and the operation must be seperated by a space before and after the redirection operators: '<', '>', '>>'. 
(e.g. cat < foo1.txt >> foo2.txt)

when using batched commands, each command must also have a space before and after the ';' operator. 
(e.g. cat foo.txt ; cat foo1.txt)

when using subshells, each subshell must be enclosed in parentheses, and the parentheses (being an operator) must also have a space before and after they are used. 
(e.g. echo "start" ; ( cd foodir ; cat foo.txt | wc ) ; echo "end")

when nesting, subshells, the same space separation must be respected.
e.g. ->( ( cd txt ; cat calendar.txt ) | grep June ) | wc > myfile2.txt
