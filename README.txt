README.txt

General information about the projects:

The goals for the project's implementation were:
------------------------------------------------
1. Break up programming task into separate modules with an eye
toward further optimization as project matures, but program more
toward interfaces interactions instead of optimization at this stage
2. Implement each module keeping performance in mind, in some case
provided alternatives to the same task (e.g. OrderBook.h)
3. As a general rule, implemented polymorphism at compile time (through
templates and preprocessor directives) instead of runtime through virtual
functions


Additional information:
------------------------------------------------
4. Implemented pseudo-regression testing facility by enabling to compare
particular input file to an expected output file and verifying that produced
output matches the expected output.
