### Makefile for 'Neural Processing Unit' ###

#Simulation tool
SIM = iverilog

#Simulation flags
SIMFLAGS = -Wall

#Source file(s)
SRCS = rtl/model_reader.v test/model_reader_tb.v

#Output file(s)
OUTPUT = model_reader_sim

all: simulate

#Compile and simulate
simulate: $(SRCS)
	$(SIM) $(SIMFLAGS) -o $(OUTPUT) $(SRCS)
	./$(OUTPUT)

#Clean up
clean:
	rm -f $(OUTPUT) *.vcd

.PHONY: all simulate clean