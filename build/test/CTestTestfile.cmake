# CMake generated Testfile for 
# Source directory: /Users/u1530331/Documents/Adv.Embedded Lab/Lab8/Lab8_Varun_Tyler/test
# Build directory: /Users/u1530331/Documents/Adv.Embedded Lab/Lab8/Lab8_Varun_Tyler/build/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(simulate_mytest "RENODE-NOTFOUND" "--disable-xwt" "--port" "-2" "--pid-file" "renode.pid" "--console" "-e" "\$ELF=@/Users/u1530331/Documents/Adv.Embedded Lab/Lab8/Lab8_Varun_Tyler/build/test/mytest.elf; \$WORKING=@/Users/u1530331/Documents/Adv.Embedded Lab/Lab8/Lab8_Varun_Tyler; include @/Users/u1530331/Documents/Adv.Embedded Lab/Lab8/Lab8_Varun_Tyler/test/simulate.resc; start")
set_tests_properties(simulate_mytest PROPERTIES  _BACKTRACE_TRIPLES "/Users/u1530331/Documents/Adv.Embedded Lab/Lab8/Lab8_Varun_Tyler/test/CMakeLists.txt;39;add_test;/Users/u1530331/Documents/Adv.Embedded Lab/Lab8/Lab8_Varun_Tyler/test/CMakeLists.txt;0;")
