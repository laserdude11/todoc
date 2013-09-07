#Scons buildfile for Todoc.
# Written by Ben Davenport-Ray.

# Set up the environment that we are using.
dbgenv = Environment(CPPPATH="include", CFLAGS=["-g"])

todolib = dbgenv.Library("todotxt", ["src/task.c", 'src/tasklist.c', 'src/linkedlist.c'])
mainprogram = dbgenv.Program("todoc", 'src/todo.c', LIBS=[todolib]) 

testprogram = dbgenv.Program("tests/todoc_test", ["tests/seatest.c", "tests/todo_test.c", "tests/task_test.c", "tests/tasklist_test.c", "tests/lltest.c"], LIBS=[todolib])
# Make a test runner that automagically runs all the tests.
testrunner = dbgenv.Alias("tests/todoc_test", [testprogram], testprogram[0].path)
AlwaysBuild(testrunner)

