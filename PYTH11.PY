1. Structural Anatomy of a Function
Every function consists of two distinct operational phases: the Definition and the Call.

def function_name(parameter1, parameter2):
     """Optional Docstring: Documents what this specific function does."""
     # Indented code block logic execution
     result = parameter1 + parameter2
     return result
def: The system keyword notifying Python that a new functional code block is being defined.
Parameters: Variable placeholders defined within the parentheses that accept data inputs.
Docstring: A string literal placed directly underneath the definition line used to describe your function’s operational purpose.
Inbuilt vs. Custom Functions
Python balances its core engine between pre-built native utility functions and custom functions you craft yourself:

# 1. Native Built-in Array Functions
dataset = [1, 2, 3, 4, 5, -78, 324]
print(sum(dataset))  # Output: 281
print(max(dataset))  # Output: 324

# 2. Custom Function
def check_divisible_by_3():
     number = int(input("Enter A Number: "))
     if number % 3 == 0:
          print("Yes, it's divisible by 3")
     else:
          print("No, it's not divisible by 3")

# Activating the machine by calling it
check_divisible_by_3()
2. Navigating Parameters vs. Arguments
While often used interchangeably, these terms have precise individual meanings:

Parameters: The variable names declared inside the function’s structural definition layout.
Arguments: The real values passed into the function when you invoke it.
Positional Arguments
Arguments are matched to parameters based on their exact ordering position. Shifting positions changes how data map to variables.

def display_info(name, age):
     print(f"Name: {name}, Age: {age}")

display_info("Charlie", 25) # Output: Name: Charlie, Age: 25
display_info(25, "Charlie") # Output: Name: 25, Age: Charlie (Oops!)
Keyword Arguments
You can specify the parameter names explicitly during the function call. When using keyword arguments, ordering position no longer matters.

display_info(age=25, name="Charlie") # Output: Name: Charlie, Age: 25
Mixing Rules: You can combine positional and keyword arguments in a single call, but positional arguments must always come first. Placing a positional argument after a keyword argument triggers a SyntaxError.

3. Default Parameters
You can provide a default fallback value for parameters. This makes those arguments optional during function invocation.

def display_profile(name, age, city="Jaipur", course="Python"):
     print(f"Name: {name}, Age: {age}, City: {city}, Course: {course}")

# Missing city and course variables will automatically adopt fallback defaults
display_profile("Vipin", 23) # Output: Name: Vipin, Age: 23, City: Jaipur, Course: Python
Rule: All mandatory positional parameters must be declared before your optional default parameters in the definition layout.

4. Variable-Length Arguments (*args and kwargs)
When building dynamic tools, you may not know exactly how many elements a user will pass into your function. Python solves this with two unpacking mechanisms:

A. Non-Keyword Optional Arrays (*args)
The * symbol packs an arbitrary amount of comma-separated positional arguments into a single tuple.

def calculate_average(*numbers):
     # numbers is received internally as a tuple: (10, 20, 30, 40)
     return sum(numbers) / len(numbers)

print(calculate_average(10, 20, 30, 40)) # Output: 25.0
B. Keyword Optional Dictionaries (kwargs)
The `` symbol packs variable named keyword parameters into a clean dictionary.

def show_profile(**user_info):
     # user_info is processed internally as a dictionary
     print(user_info)

show_profile(a=1, b=2, c=3) # Output: {'a': 1, 'b': 2, 'c': 3}
5. Variable Scope: Local vs. Global
Variable visibility depends entirely on where that variable was declared inside your application workspace.

a = 20 # Global variable scope: Visible everywhere

def test():
     a = 34 # Local variable scope: Confined purely to this function space
     print(f"Local environment variable: {a}")

test()
print(f"Global master framework variable: {a}")
Overwriting Global States via global
To change a global variable from inside a function, explicitly declare it using the global keyword:

a = 20

def modify_globally():
     global a
     a = 34 # Permanently overwrites the global variable 'a'

modify_globally()
print(a) # Output: 34