Christopher K. Tarango

September 24, 2017
Shell Lab (Part 1)
README.md

Synopsis:

This program weakly emulates a bash shell, first printing a prompt and taking input. Tokenizes the environment variables to search for the path, then tokenizes the path variable to create a vector of possible paths to locate executables in. The program forks a new process program then attempts to execute the command using the given path, if the command is not found it appends the path to the command in an attempt to find the command. If it fails it prints out an error message the the command cannot be found. If successful the command is executed while the parent process waits for a return from the child process. A return code is given upon return, an error message is printed if the process ends in error. Shell ends by either giving the exit command or through the use of Ctrl-D.

Usage:

./shell

Notes: 

Currentl doesn't play nicely with the testShell.sh

Test Cases:

/bin/pwd
/home/cktarango/shell-bighappee/shell
---------------
/bin/pwd

 /bin/pwd
/home/cktarango/shell-bighappee/shell
$ $ /home/cktarango/shell-bighappee/shell
-----------------------------------------------------
wc shell.c
 92  437 4476 shell.c
--------------------------------------------------------
ps
  PID TTY          TIME CMD
13209 pts/25   00:00:00 bash
13434 pts/25   00:00:00 shell
13442 pts/25   00:00:00 ps
--------------------------------------------------------
/bin/ps
 PID TTY          TIME CMD
13209 pts/25   00:00:00 bash
13434 pts/25   00:00:00 shell
13443 pts/25   00:00:00 ps
-------------------------------------------------------
ps sfdfkj

error: unsupported option (BSD syntax)

Usage:
 ps [options]

 Try 'ps --help <simple|list|output|threads|misc|all>'
  or 'ps --help <s|l|o|t|m|a>'
 for additional help text.

For more details see ps(1).
Program terminated with exit code 1.
--------------------------------------------------------
hello
command not found
---------------------------------------------------------
(no input)
$
$
-------------------------------------------------------
exit
(program exits)
--------------------------------------------------------
Ctrl-D
(program exits
-------------------------------------------------------
wc fdjlfj.txt
wc: fdjlfj.txt: No such file or directory
Program terminated with exit code 1.

