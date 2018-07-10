1.	operating environment：
OS：Linux
Compiler: g++(GCC)

2.	The file of source code description:
ass.h: define Ass, Generator and Operator’s class and initial value
ass.cpp: declare a member function definition.
ass2_proj.cpp : include main() function.

3.	Compiling:
Execute the following command
	make clean
	make

4.	Running:
Use “make” command first then use ./Ass2 or ./Ass2 x (x is an integer, the range is from 1 to 10, x is used to adjust the number of operators)

Function declaration:

1.	Pause or resume program: After running program, you can type “p” then press “Enter” immediately for pause the program. The same is you can type “r” then press “Enter” immediately for resume the program.

2.	Operators adjustable

3.	Tools adjustable (I used the new restrictions that you have provided from E-mail)

4.	Display the number of materials, material’s buffer, the number of products, and the buffer of product information.

Design ideas:

1.	How to ensure the diversity of the material in a buffer?
Base on the size of material’s buffer, and the type of materials, I set a value to limit each number of the material. Such as the size of the buffer is 10, I have 3 type of materials. I have to ensure the buffer of material includes all type of materials. Thus, I set 4 as a limit value for each material.

2.	How can I prevent another does not occupy the tools?
The operator can only get two tools or nothing at each time; this way can avoid deadlock.
	
3.	No same products can be next to each other in this queue
Before the program decides to produce a product, the program can exclude the type of the last product in the current product buffer and the types of products that are currently in production.
	
4.	 The difference of the number of any two kinds of products produced should be less than 10.
When deciding on the type of product to be produced, after satisfying the case of satisfaction (3), the program will select the least number of products with no more than ten products.
