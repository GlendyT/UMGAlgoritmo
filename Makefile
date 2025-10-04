CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = envios
SOURCES = main.cpp Usuario.cpp Paquete.cpp SistemaEnvios.cpp
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del *.o $(TARGET).exe

.PHONY: clean