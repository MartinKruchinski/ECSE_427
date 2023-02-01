# ECSE_427

**Implementation of an operating system using C programming.**


After implementing the memory manager, I built a simple, simulated OS. So far, our simulated OS supports simple Shell commands, and is capable to do process management according to different scheduling techniques. 

On a high level, in this assignment we will allow programs larger than the shell memory size to be run by our OS. We will split the program into pages; only the necessary pages will be loaded into memory and old pages will be switched out when the shell memory gets full. Programs executed through both the run and the exec commands need to use paging. In addition, we will further relax the assumptions of exec by allowing the same program to be executed multiple times by exec
