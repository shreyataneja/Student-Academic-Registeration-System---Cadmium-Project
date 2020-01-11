CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I ../../cadmium/include
INCLUDEDESTIMES=-I ../../DESTimes/include

#CREATE BIN AND BUILD FOLDERS TO SAVE THE COMPILED FILES DURING RUNTIME
bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

#TARGET TO COMPILE ALL THE TESTS TOGETHER (NOT SIMULATOR)

main_top.o: top_model/main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) top_model/main.cpp -o build/main_top.o
	

main_application_test.o: test/main_application_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_application_test.cpp -o build/main_application_test.o
main_applicationProcess_test.o: test/main_applicationProcess_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_applicationProcess_test.cpp -o build/main_applicationProcess_test.o
main_faculty_test.o: test/main_faculty_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_faculty_test.cpp -o build/main_faculty_test.o
main_registrar_test.o: test/main_registrar_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_registrar_test.cpp -o build/main_registrar_test.o
main_webapp_test.o: test/main_webapp_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_webapp_test.cpp -o build/main_webapp_test.o
main_university_test.o: test/main_university_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_university_test.cpp -o build/main_university_test.o

										
tests: main_application_test.o  main_applicationProcess_test.o main_faculty_test.o main_registrar_test.o main_webapp_test.o main_university_test.o
		$(CC) -g -o bin/MAINAPPLICATION build/main_application_test.o
		$(CC) -g -o bin/MAINPROCESSAPPLICATION build/main_applicationProcess_test.o
		$(CC) -g -o bin/MAINFACULTY build/main_faculty_test.o
		$(CC) -g -o bin/MAINREGISTRAR build/main_registrar_test.o
		$(CC) -g -o bin/MAINWEBAPP build/main_webapp_test.o
		$(CC) -g -o bin/MAINUNIVERSITY build/main_university_test.o


#TARGET TO COMPILE ONLY ABP SIMULATOR
simulator: main_top.o 
	$(CC) -g -o bin/SARS build/main_top.o 
	
#TARGET TO COMPILE EVERYTHING (ABP SIMULATOR + TESTS TOGETHER)
all: simulator tests

#CLEAN COMMANDS
clean: 
	rm -f bin/* build/*