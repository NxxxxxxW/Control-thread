1.	operating environment：
OS：Linux
Compiler: g++(GCC)

2.	The description of source files:
ass.h: define ass, generator and operator’s class and initial value
ass.cpp: declare definition of member function. 
ass2_proj.cpp: main function.

3.	Compiling:
Execute following commands:
		make clean
		make

4.	Running:
Input “make” command at command window, then input /Ass2 or /Ass2 x at the same window.
(x is an integer, the range is from 1 to 10, x is used to adjust the number of operators)

Function declaration:

1.	Pause or resume program: After the program run, input “p” for pause the program or input “r” for resuming the program.

2.	Adjust the number of operators

3.	Adjust the number of tools (I used the new restrictions that you have provided from E-mail)

4.	Display the number of materials, material’s buffer, the number of products, and the buffer of product information.

Design ideas:

1.	How to ensure the diversity of the material in a buffer?
The program limited each number of the material, and have 3 types of materials to ensure the buffer of material includes all type of materials. Thus, the program set 4 as a limit value for each material.

2.	How can prevent the tools do not occupied by others?
The operator can only get two tools or nothing at each time.
	
3.	How to ensure the different between two adjacent products in the product cache?
Before the program decides to produce a product, the program can exclude the type of the last product in the current product buffer and the types of products are currently in production.
	
4.	How to prevent product cache is not exceeding 10 in difference between the number of any two products?
When deciding on the type of product to be produced, after satisfying the case of satisfaction (3), the program will select no more than ten products.
