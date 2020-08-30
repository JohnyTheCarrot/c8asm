.PHONY: default
.DEFAULT_GOAL: default

SRCDIR=$(CURDIR)/src
OBJDIR=$(CURDIR)/build

SOURCES=$(wildcard $(SRCDIR)/*.cpp)
OBJECTS=$(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))
DEPENDS=$(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.d, $(SOURCES))

CXXFLAGS=-g -O1 -Wno-unused-result

default: $(OBJDIR)/c8asm

$(OBJDIR)/%.d: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -MM -MT $(patsubst %.d, %.o, $@) -MF $@ $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OBJDIR)/c8asm: $(OBJECTS) | $(OBJDIR)
	$(CXX) -o $@ $(OBJECTS)

-include $(DEPENDS)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)
