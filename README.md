# Manual-Accounting
A C project using multi-threading to achieve parallel programming and compares the results to sequential algorithms related to generating and selling tickets, as well as counting profits.

# Problem description
Given 4 sellers, each generates a given number of tickets they sold and counts profits while selling, also taking account of what type of ticket they sold. After finishing, they also write their total profits in a shared file. One of the sellers makes a mistake and notes down the wrong type of ticket for one of the ones they sold, but counts their profits correctly. The boss requests that they tally up their earnings based on both the types of tickets sold, and the profits they counted while selling. The two will not match, because of one of the workers' errors. Thus, the boss himself processes the files alone to spot the mistake. 

# Technical information
The seller making a mistake is randomly determined with a threadsafe method.<br/>
All actions are performed both sequentially and using parallel programming, making use of threads created using OpenMP in C. The critical directive provided by the library allows for the safe modification of specific data. The time it took for the program to execute sequentially and in parallel is noted down, and used to calculate acceleration and efficiency.
All memory used by arrays is allocated dynamically and freed at the end of the program.<br/>

Tested on Linux ( Ubuntu WSL 2.0 )<br/>
Compile with: ~gcc manualAccounting.c -Wall -fopenmp -o executableName<br/>
Run with: ./executableName nr where nr is an integer specifying the number of tickets sold by each seller<br/>
Output: profit of each seller on screen, types of tickets sold in "x_kassza" files, where x is the ID of the seller, overall profit in "bevetel" file, boss's calculations, acceleration and efficiency on screen<br/>
