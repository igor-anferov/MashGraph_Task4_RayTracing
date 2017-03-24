CXX = g++
CXXFLAGS = -std=c++11 -O5 -fopenmp -lz
CXX_SOIL_FLAGS = -O5
PROG = Mashgraph_Task4_RayTracing
WORK_DIR = bin
BUILD_DIR = build

OBJECTS = $(BUILD_DIR)/camera.o \
          $(BUILD_DIR)/cornell_box.o \
	  $(BUILD_DIR)/intersect_info.o \
	  $(BUILD_DIR)/object.o \
	  $(BUILD_DIR)/parallelepiped.o \
          $(BUILD_DIR)/parameters.o \
	  $(BUILD_DIR)/scene.o \
	  $(BUILD_DIR)/texture.o \
	  $(BUILD_DIR)/utilites.o

SOIL_OBJ = $(BUILD_DIR)/SOIL/SOIL.o \
	   $(BUILD_DIR)/SOIL/stb_image_aug.o \
	   $(BUILD_DIR)/SOIL/image_DXT.o \
	   $(BUILD_DIR)/SOIL/image_helper.o

SOIL = src/SOIL
SRC = src

HEADER_ONLY = $(SRC)/LED.h \
	      $(SRC)/rectangle.h \
	      $(SRC)/sphere.h

run: $(WORK_DIR)/$(PROG) 
	cd $(WORK_DIR); \
	./$(PROG) config_fast.txt res.bmp; \
	cd ../..

$(BUILD_DIR)/SOIL/%.o: $(SOIL)/%.c $(SOIL)/%.h
	$(CXX) $(CXXFLAGS) $< -I$(SOIL) -c -o $@

$(WORK_DIR)/$(PROG): $(OBJECTS) $(SOIL_OBJ) $(SRC)/main.cpp $(HEADER_ONLY)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(SOIL_OBJ) $(SRC)/main.cpp -lCGAL -lGL -o $(WORK_DIR)/$(PROG)

$(BUILD_DIR)/SOIL/%.o: $(SOIL)/%.c $(SOIL)/%.h
	$(CXX) $(CXX_SOIL_FLAGS) $< -I$(SOIL) -c -o $@

$(BUILD_DIR)/%.o: $(SRC)/%.cpp $(SRC)/%.h
	$(CXX) $(CXXFLAGS) $< -I$(SOIL) -c -o $@

clean:
	rm $(BUILD_DIR)/*.o; \
        rm $(BUILD_DIR)/SOIL/*.o; \
	rm $(WORK_DIR)/$(PROG)

