CXXFLAGS += -O3 -std=c++20
OBJDIR = build
SRCDIR = src

OBJ = build/main.o build/lodepng.o

all: imgconv

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

imgconv: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $ $(LINKFLAGS)

clean:
	rm -rf ./build ./imgconv
