Authors:
Saba Fathi 260972570
Martin Kruchinski 260915767

Hello,
This is the Demand Paging README for ECSE-427.

This assignment was built on our code from A2, which passed all testcases.

Our program works by running following commands:
make clean
make mysh framesize=<YOUR_VALUE> varmemsize=<YOUR_VALUE>
./mysh < <TESTCASE_FILE>

To change the framesize you must quit the shell and re-run the make commands.

RESETMEM:
There is also the new resetmem command in the shell which serves to wipe all variables stored.
It will print out "Memory was Reset" to let the user know the command has passed.


public testcase notes:

tc1 tc2 intention:
                   testing exec command, there is no page fault in this test case,
                   each program only have two pages max.

tc3 intention:
             testing exec command, page fault involved, when using this test case,
             make sure there are fewer frames than file's pages.

tc4 intention:
               testing exec command. This is the most complex test case,
               make sure there are fewer frames than file's pages.

tc5 intention:
              testing run command.
