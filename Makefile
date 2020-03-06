CC=g++
CCFLAGS=-c -fPIC
LDFLAGS=-ldl -shared -s
BUILD_DIR=build
RPC_DIR=rpc
PYTHON_DIR=python
SHMEM_DIR=shmem
SOURCE_DIR=src
BIN_DIR=bin
LIBRARY=libeggnogg.so
PYTHON_LIBRARY=pyeggnogg

all: directories $(LIBRARY) $(PYTHON_LIBRARY)
	@> /dev/null

$(LIBRARY): directories libeggnogg.o $(RPC_DIR)/libeggnogg_rpc_svc.o
	$(CC) $(BUILD_DIR)/libeggnogg.o $(BUILD_DIR)/$(RPC_DIR)/libeggnogg_rpc_svc.o -o $(BIN_DIR)/$(LIBRARY) $(LDFLAGS)

%.o:
	$(CC) $(SOURCE_DIR)/$*.cpp -o $(BUILD_DIR)/$@ $(CCFLAGS)

directories:
	-mkdir -p $(BUILD_DIR)/$(RPC_DIR)
	-mkdir -p $(BUILD_DIR)/$(PYTHON_LIBRARY)
	-mkdir -p $(BIN_DIR)/$(PYTHON_LIBRARY)

$(PYTHON_LIBRARY): directories
	cd python && python3.8 setup.py build --build-base=../$(BUILD_DIR)/$(PYTHON_LIBRARY) install --prefix=../$(BIN_DIR)/$(PYTHON_LIBRARY)

install: directories
	cd python && python3.8 setup.py build --build-base=../$(BUILD_DIR)/$(PYTHON_LIBRARY) install

uninstall:
	rm -rf /usr/local/lib/python3.8/dist-packages/*pyeggnogg* 
	
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

