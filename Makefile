CC=g++
CCFLAGS=-c -fPIC
LDFLAGS=-ldl -lrt -shared -s
BUILD_DIR=build
RPC_DIR=rpc
PYTHON_DIR=python
SHMEM_DIR=libeggnogg_shmem
LIB_DIR=lib
SOURCE_DIR=src
BIN_DIR=bin
LIBRARY=libeggnogg.so
PYTHON_LIBRARY=pyeggnogg
SOURCE_EXT=cpp
SOURCES=$(shell find $(SOURCE_DIR) -type f -name *.$(SOURCE_EXT))
LIBRARIES=$(shell find $(LIB_DIR) -type f -name *.$(SOURCE_EXT))
OBJECTS=$(patsubst $(SOURCE_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.$(SOURCE_EXT)=.o))
OBJECTS_LIB=$(patsubst $(LIB_DIR)/%,$(BUILD_DIR)/$(LIB_DIR)/%,$(LIBRARIES:.$(SOURCE_EXT)=.o))

all: $(LIBRARY) $(PYTHON_LIBRARY)

$(LIBRARY): $(OBJECTS) $(OBJECTS_LIB)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $(BIN_DIR)/$@ $(LDFLAGS)

$(BUILD_DIR)/$(LIB_DIR)/%.o: $(LIB_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $< -o $@ $(CCFLAGS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $< -o $@ $(CCFLAGS)

$(PYTHON_LIBRARY):
	@mkdir -p $(BIN_DIR)/$(PYTHON_LIBRARY)
	@mkdir -p $(BUILD_DIR)/$(PYTHON_LIBRARY)
	cd python && python3.8 setup.py build --build-base=../$(BUILD_DIR)/$(PYTHON_LIBRARY) install --prefix=../$(BIN_DIR)/$(PYTHON_LIBRARY)

install: $(LIBRARY)
	@mkdir -p $(BUILD_DIR)/$(PYTHON_LIBRARY)
	cd python && python3.8 setup.py build --build-base=../$(BUILD_DIR) install

uninstall:
	rm -rf /usr/local/lib/python3.8/dist-packages/*pyeggnogg*

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

