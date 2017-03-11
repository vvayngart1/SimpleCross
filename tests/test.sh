echo "+++++++++++++++++++++++++++++++++ RUNNING regression test1"
../$1/SimpleCross.exe actions2.txt actionsResultsExpected2.txt
echo "+++++++++++++++++++++++++++++++++ done RUNNING regression test1"
echo ""
echo "+++++++++++++++++++++++++++++++++ RUNNING regression test2"
../$1/SimpleCross.exe actions3.txt actionsResultsExpected3.txt
echo "+++++++++++++++++++++++++++++++++ done RUNNING regression test2"
