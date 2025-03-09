CXXFLAGS += -O3 -std=c++20
OBJDIR = build
SRCDIR = src

OBJ = build/main.o

all: probeconv

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

probeconv: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $ $(LINKFLAGS)

clean:
	rm -rf ./build ./probeconv
