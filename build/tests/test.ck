// this is a program made with cake!

// these are logging functions:
err("This is an error!");
warn("This is a warning!");
info("This is an info!\n\n\nthese were a couple of new lines and\tthisisatab!\r\n");
// * * * * * * * * * * * * * * *

// this is a comment!
print("in file: _here()\n");
print("1\n");
print("22\n");
print("333\n");
print("4444\n");
print("55555\n");
print("666666\n");
print("7777777\n");
print("88888888\n");
print("999999999\n");
print("1010101010\n");

i32 myint = 12;
print("myint is: ");
print(myint);
print("\n");

myint = 2;
print("myint is: ");
print(myint);
print("\n");

myint = 69;
print("myint is: ");
print(myint);
print("\n");

myint = 2000000000;
print("myint is: ");
print(myint);
print("\n");

// call file as function
print("Calling _ here() function in _here()\n");
use("_here()"); // %{here} gets expanded to <current_filename>
