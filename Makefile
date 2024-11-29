# Compiler
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Output Executables
Q1_EXEC = Question1
Q2_EXEC = Question2
Q3_EXEC = Question3

# Source Files
Q1_SRC = Question1.cpp
Q2_SRC = Question2.cpp
Q3_SRC = Question3.cpp

# Object Files
Q1_OBJ = Question1.o
Q2_OBJ = Question2.o
Q3_OBJ = Question3.o

# Default target to build all executables
all: $(Q1_EXEC) $(Q2_EXEC) $(Q3_EXEC)

# Rule to build Question1 executable
$(Q1_EXEC): $(Q1_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to build Question2 executable
$(Q2_EXEC): $(Q2_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to build Question3 executable
$(Q3_EXEC): $(Q3_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule for compiling Question1 source file to object file
$(Q1_OBJ): $(Q1_SRC)
	$(CXX) $(CXXFLAGS) -c $<

# Rule for compiling Question2 source file to object file
$(Q2_OBJ): $(Q2_SRC)
	$(CXX) $(CXXFLAGS) -c $<

# Rule for compiling Question3 source file to object file
$(Q3_OBJ): $(Q3_SRC)
	$(CXX) $(CXXFLAGS) -c $<

# Clean up generated files
clean:
	rm -f $(Q1_OBJ) $(Q2_OBJ) $(Q3_OBJ) $(Q1_EXEC) $(Q2_EXEC) $(Q3_EXEC)

# Run the executables (assuming your executable takes arguments)
run_q1: $(Q1_EXEC)
	./$(Q1_EXEC) "input_file_for_question1" "output_for_question1"

run_q2: $(Q2_EXEC)
	./$(Q2_EXEC) "input_file_for_question2" "output_for_question2"

run_q3: $(Q3_EXEC)
	./$(Q3_EXEC) "input_file_for_question3" "output_for_question3"
