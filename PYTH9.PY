MASTER GUIDES TO FILE HANDLING IN PYTHON:PATHS, MODES, STRESM OPTIONS

File handling allows your programs to read data from and write data to files stored permanently on your computer. Without file handling, all variables, user inputs, and calculated results are instantly lost the moment your script stops running.

1: ABSOLUTE vs RELATIVE SYSTEM PATHS:

Before opening a file, you must understand how to point Python to its exact location on your computer’s storage drive:

Absolute Path: The full path starting from your system root directory (/ on Mac/Linux or C:\ on Windows).

Example: /home/user/projects/data/test.txt

Relative Path: A path that is relative to the directory where your Python script is currently running.

Example: data/test.txt (or simply test.txt if it is in the exact same folder)

OPENING AND CLOSING OF STREAMS:

To interact with a file, you use the built-in open() function. It sets up an active communications stream and returns a file object.

THE MODERN SOLUTION: THE with CONTEXT MANAGER - 

To completely eliminate the risk of forgetting to close files, Python provides the with statement. It serves as a context manager that automatically closes the file the moment execution steps outside the indented block—even if your code encounters a runtime error or crashes midway through.

