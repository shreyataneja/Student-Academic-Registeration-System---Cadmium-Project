
/**************************/
/****FILES ORGANIZATION****/


README.txt	
StudentAcademicRegistrationSystem.doc
StudentAcademicRegistrationSystem.pdf
ModelDescription-SARS.xml
makefile

atomics [This folder contains atomic models implemented in Cadmium]
	application.hpp
	applicationProcess.hpp
	faculty.hpp
	Registrar.hpp
bin [This folder will be created automatically the first time you compile the poject.
     It will contain all the executables]
build [This folder will be created automatically the first time you compile the poject.
       It will contain all the build files (.o) generated during compilation]
input_data [This folder contains all the input data to run the model and the tests]
	application_input_applicationFillingComplete.txt
	application_input_applicationIN.txt
	applicationProcess_input.txt
	faculty_input.txt
	registrar_input.txt
	sars_input.txt
	university_input.txt
	webApp_input.txt
simulation_results [This folder will be created automatically the first time you compile the poject.
                    It will store the outputs from your simulations and tests]
test [This folder the unit test of the atomic models]
	main_application_test.cpp
	main_applicationProcess_test.cpp
	main_faculty_test.cpp
	main_registrar_test.cpp
	main_university_test.cpp
	main_webapp_test.cpp
top_model [This folder contains the Student Academic Registration System top model]	
	main.cpp
	
/*************/
/****STEPS TO COMPILE AND RUN TESTS****/
/*************/

0 - StudentAcademicRegistrationSystem.doc contains the explanation of this model

1 - Update include path in the makefile in this folder and subfolders. You need to update the following lines:
	INCLUDECADMIUM=-I ../../cadmium/include
	INCLUDEDESTIMES=-I ../../DESTimes/include
    Update the relative path to cadmium/include from the folder where the makefile is. You need to take into account where you copied the folder during the installation process
	Example: INCLUDECADMIUM=-I ../../cadmium/include
	Do the same for the DESTimes library
   
2 - Compile the project and the tests
	1 - Open the terminal (Ubuntu terminal for Linux and Cygwin for Windows) in the project folder. (Bash prompt here)
	3 - To compile the project and the tests, type in the terminal:
			make clean; make all
			
3 - Run atomic/coupled test
	1 - Open the terminal in the bin folder by command.  
		cd bin
	2 - To run the test, type in the terminal "./NAME_OF_THE_COMPILED_FILE" (For windows, "./NAME_OF_THE_COMPILED_FILE.exe"). 
	For this specific test you need to type:
			./MAINAPPLICATION (or ./MAINAPPLICATION.exe for Windows)
			./MAINPROCESSAPPLICATION  (or ./MAINPROCESSAPPLICATION.exe for Windows)
			./MAINREGISTRAR  (or ./MAINREGISTRAR.exe for Windows)
			./MAINFACULTY (or ./MAINFACULTY.exe for Windows)
			./MAINWEBAPP  (or ./MAINWEBAPP.exe for Windows)
			./MAINUNIVERSITY  (or ./MAINUNIVERSITY.exe for Windows)
			
	3 - To check the output of the test, go to the folder simulation_results and open output files.

4 - Run the top model
	1 - Open the terminal (Ubuntu terminal for Linux and Cygwin for Windows) in the bin folder.
	3 - To run the model, type in the terminal "./NAME_OF_THE_COMPILED_FILE NAME_OF_THE_INPUT_FILE". For this test you need to type:
	
	(for Windows: ./SARS.exe ../input_data/sars_input.txt)
	
	4 - To check the output of the model, go to the folder simulation_results and open "SARS_output_messasges.txt" and "SARS_output_state.txt"
