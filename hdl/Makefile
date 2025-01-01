### Makefile for 'Neural Processing Unit' ###

#Simulation tool
SIM = iverilog

#Simulation flags
SIMFLAGS = -Wall

#Source file(s)
SRCS_NEURON     = rtl/neuron.v test/neuron_tb.v
SRCS_MODEL_READ = rtl/model_reader.v test/model_reader_tb.v

#Output build folder
BUILD = build

#Output file(s)
NEURON_OUTPUT     = $(BUILD)/neuron_sim
MODEL_READ_OUTPUT = $(BUILD)/model_reader_sim

#Model file to use for the HDL simulation
#Will be used to produce the 'model_params.vh' header file during compilation
HEADER_FILE = $(BUILD)/model_params.vh
MODEL_FILE  = ../models/xor_quantized.weights

all: model_read_simulate

#Create build directory
$(BUILD):
	mkdir -p $(BUILD)

#Create the model_params.vh header file to help set parameters during compile time
$(HEADER_FILE): $(BUILD) $(MODEL_FILE)
	python scripts/gen_model_reader_params.py $(MODEL_FILE)

#Compile and simulate model reader
model_read_simulate: $(BUILD) $(HEADER_FILE) $(SRCS_MODEL_READ)
	$(SIM) $(SIMFLAGS) -o $(MODEL_READ_OUTPUT) $(SRCS_MODEL_READ)
	./$(MODEL_READ_OUTPUT)

#Compile and simulate neuron
neuron_simulate: $(BUILD) $(HEADER_FILE) $(SRCS_NEURON)
	$(SIM) $(SIMFLAGS) -o $(NEURON_OUTPUT) $(SRCS_NEURON)
	./$(NEURON_OUTPUT)

#Clean up
clean:
	rm -rf $(BUILD)

.PHONY: all model_read_simulate neuron_simulate clean