CC				= gcc
CFLAGS			= -c -fPIC -Wall -Wextra -Wfatal-errors -Wno-unused-function -DMG_ENABLE_SSL
LDFLAGS      	= -shared
SRCDIR			= src
DEBUGOUTPUTDIR 	= build/debug
RELEASEOUTPUTDIR	= build/release
SOURCES			= src/mongoose.c src/he_api_binding.c src/he.c src/he_http.c
INCLUDES		= -I./src -Iinclude -I/usr/local/include/cd
LIBS			= -lcd -pthread
DEPS			= libcd.so
_OBJECTS		= $(SOURCES:.c=.o)
DEBUGOBJECTS	= $(patsubst src/%,$(DEBUGOUTPUTDIR)/%,$(_OBJECTS))
RELEASEOBJECTS	= $(patsubst src/%,$(RELEASEOUTPUTDIR)/%,$(_OBJECTS))
DEBUGTARGET		= build/debug/libhe.so
RELEASETARGET	= build/release/libhe.so

ldc := $(shell sudo ldconfig)
dep := $(shell sudo ldconfig -p | grep libcd.so)

deps:
ifndef dep
$(error "libcd $(dep) is missing, please check Deps in README")
endif

debugprereqs:
		mkdir -p $(DEBUGOUTPUTDIR)

releaseprereqs:
		mkdir -p $(RELEASEOUTPUTDIR)

install-prereqs:
		sudo mkdir -p /usr/local/include/he


debug:	debugprereqs $(SOURCES) deps $(DEBUGTARGET)
release:	releaseprereqs $(SOURCES) deps $(RELEASETARGET)

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
	sudo rm -rf /usr/local/include/he

clean:
	rm -rf $(DEBUGOBJECTS) $(DEBUGTARGET)
	rm -rf $(RELEASEOBJECTS) $(RELEASETARGET)

clean-all: clean examples-clean
