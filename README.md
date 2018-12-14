# Submarine-Movement

Project completed through Visual Basic Community 2017 on Windows 10.

Files used to get project working:
	1) FreeGLUT MSVC Package found at http://www.transmissionzero.co.uk/software/freeglut-devel/
	
Steps to get Project running (this is if the FreeGLUT MSVC Package found above is being used):
	1) Extract freeglut folder to easily accessible location.
	2) Open project in Visual Studio.
	3) Right click on Project name on the right panel, below Solutions
		<Project Name> and click on Properties.
	4) Go to Configuration Properties, VC++ Directories and modify
		the Include Directories, adding in pathfile to the include
		folder located in the freeglut folder that was extracted earlier.
	5) Do the same for Library Directories.
	6) Click on Linker found under Configuration Properties, Input, and add
		opengl32.lib to Additional Dependencies.
	7) Back out to Visual Studio (by clicking on OK to ensure changes are saved).
	8) Run project.

Requirement 1 (Submarine is constructed by five parts, minimum):
	1 Submarine Body
	1 Submarine Sail
	2 Submarine Sail Wings
	1 Submarine Tail
	2 Submarine Tail Wings
	4 Submarine Propellers (done using for statement)
	
Requirement 2 (Submarine movement):
	Submarine is capable of moving moving forwards ("w") and backwards ("s"),
		rotating ("d" and "a"), and lowering/raising ("up arrow" and "down arrow").

Requirement 3 (Help key):
	Implemented a help key to print out button functions to console.
	
Optional Bonus:
	Used "e" and "q" for moving submarine forwards/backwards on a timer.
