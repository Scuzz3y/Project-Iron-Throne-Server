# Define the C compiler to use
CC = g++

# Define any compile-time flags
CFLAGS = -g -Wall -Wextra -Werror -std=c++11 -pedantic

# Define any libraries to link into executable
LIBS = 

# Define the C source files
SRCS = src/logic.cpp src/blackIce.cpp

INC = -Ishared/schema-include -Iinclude/ -I lib/flatbuffers/include/
# Define the C object files
OBJS = $(SRCS:.cpp=.o)

# Define the executable file
MAIN = bin/ironThrone.out

.PHONY: depend clean

all:	$(MAIN)
		@echo Project $(MAIN) has been compiled

$(MAIN): $(OBJS)
		$(CC) $(LIBS) $(CFLAGS) $(INC) -o $(MAIN) $(OBJS) 

.cpp.o:
		$(CC) $(INC) $(CFLAGS) -c $< -o $@

clean:
		$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
		makedepend $^

# DO NOT DELETE THIS LINE -- make depend needs it