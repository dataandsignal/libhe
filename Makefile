CC				= gcc
CFLAGS			= -c -fPIC -Wall -Wextra -Wfatal-errors -Wno-unused-function
LDFLAGS      	= -shared
SRCDIR			= src
DEBUGOUTPUTDIR 	= build/debug
RELEASEOUTPUTDIR	= build/release
SOURCES			= src/mongoose.c src/he_api_binding.c src/he.c src/he_http.c
INCLUDES		= -I./src -Iinclude -I/usr/local/include/cd
LIBS			= -lcd -pthread
_OBJECTS		= $(SOURCES:.c=.o)
DEBUGOBJECTS	= $(patsubst src/%,$(DEBUGOUTPUTDIR)/%,$(_OBJECTS))
RELEASEOBJECTS	= $(patsubst src/%,$(RELEASEOUTPUTDIR)/%,$(_OBJECTS))
DEBUGTARGET		= build/debug/libhe.so
RELEASETARGET	= build/release/libhe.so

debugprereqs:
		mkdir -p $(DEBUGOUTPUTDIR)

releaseprereqs:
		mkdir -p $(RELEASEOUTPUTDIR)

install-prereqs:
		mkdir -p /usr/local/include/he


debug:	debugprereqs $(SOURCES) $(DEBUGTARGET)
release:	releaseprereqs $(SOURCES) $(RELEASETARGET)

test-debug:		debugall
		cd test && make test-debug

test-release:		releaseall
		cd test && make test-release

test:		test-release

test-clean:
		cd test && make clean

examples-debug:		debug install-debug
		cd examples && make examples-debug

examples-release:		release install-release
		cd examples && make examples-release

examples:		examples-release

examples-clean:
		cd examples && make clean

$(DEBUGTARGET): $(DEBUGOBJECTS) 
	$(CC) $(DEBUGOBJECTS) $(LIBS) -o $@ $(LDFLAGS)

$(RELEASETARGET): $(RELEASEOBJECTS) 
	$(CC) $(RELEASEOBJECTS) $(LIBS) -o $@ $(LDFLAGS)

$(DEBUGOUTPUTDIR)/%.o: CFLAGS += -g -ggdb3 -O0
$(DEBUGOUTPUTDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

$(RELEASEOUTPUTDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

all: release

.DEFAULT_GOAL = release

install-headers: install-prereqs include/he.h
	sudo cp include/* /usr/local/include/he/

install-debug: $(DEBUGTARGET) install-headers
	sudo cp $(DEBUGTARGET) /lib/

install-release: $(RELEASETARGET) install-headers
	sudo cp $(RELEASETARGET) /lib/

install: install-release

uninstall:
	sudo rm /lib/libhe.so

clean:
	sudo rm -rf $(DEBUGOBJECTS) $(DEBUGTARGET)
	sudo rm -rf $(RELEASEOBJECTS) $(RELEASETARGET)

clean-all: clean examples-clean test-clean
