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
1. Project was developed using Microsoft Visual Studio Community 2015
Version 14.0.23107.0 D14REL on Windows 7 Professional Version 6.1
2. Implemented pseudo-regression testing facility by enabling to compare
particular input file to an expected output file and verifying that produced
output matches the expected output.
3. Test suite of input/output files are in ./tests subdirectory of main project,
which contains:
- actions1.txt - input file based on the original sample of actions for the testing
- actions2.txt - input file, which expends actions1.txt with book sweeping orders 
- actionsResult2.txt - file with expected outputs , which correspond3 to actions2.txt
  inputs
- actions3.txt - input file, which expends actions2.txt with additional symbol (has
  both IBM and MSFT orders) 
- actionsResult3.txt - file with expected outputs , which correspond to actions3.txt
  inputs
- test.sh - simple script file, which I used to test implementation of SimpleCross 
  project. I used git bash window terminal to execute it
4. Command line to execute SimpleCross.exe
  $Path/SimpleCross.exe <actions file> [<actions_expected>]
  e.g. from the test directory:
  ../Release/SimpleCross.exe actions3.txt actionsResultsExpected3.txt
5. Typical Performance ouput from the "../Release/SimpleCross.exe actions3.txt actionsResultsExpected3.txt"
command is:
		Add/Cancel performance stats:
		count=28
		min=0.85 microsecs
		max=122.30 microsecs
		mean=10.06 microsecs
		min (w/o first measurement)=0.85 microsecs
		max (w/o first measurement)=34.49 microsecs
		mean (w/o first measurement)=5.69 microsecs
First measurement is several magnitudes larger than mean becuase of Pool.h classes allocating resources. If the code
is recompiled with #define __SimpleCrossPrintPerformanceVector__, it will print individual measurements, which
shows that potential optimization targets are string allocations/copies, like in ouput of item [12]
		[0]=122.30 [1]=1.71 [2]=2.85 [3]=34.49 [4]=8.27 [5]=2.56 [6]=1.43 [7]=1.43 [8]=2.57 [9]=1.14 
		[10]=2.57 [11]=0.85 [12]=18.82 [13]=17.96 [14]=2.28 [15]=1.14 [16]=1.14 [17]=5.99 [18]=6.56 
		[19]=2.00 [20]=1.14 [21]=1.14 [22]=1.14 [23]=1.14 [24]=2.00 [25]=1.14 [26]=16.82 [27]=19.10





