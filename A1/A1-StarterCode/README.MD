Assignment 1 ECSE 427.
Authors:
Martin Kruchinski 260915767
Saba Fathi 260972570

This file contains the explanation of all relevant methods.

echo:
Echo takes as argument 1 alphanumeric element and essentially prints that element on the command line. The argument can also be an existing variable, which needs to be denoted as
${variable_name}.

my_ls:
The my_ls command prints out all files and sub-directories of the present working directory,
including hidden files and parent directory. The alphabetical order of our my_ls command is to print files and directories with upper case letters first, with the strcmp method. We decided to use the order of ASCII in our assignment.

badCommandTooManyTokens:
This command is called when too many arguments are passed to the set method, in which the limit is 5 arguments for 1 variable.

tolowercase:
custom tolowercase because was getting issues with built in
