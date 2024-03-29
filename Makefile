CC				= gcc
CFLAGS			= -c -fPIC -Wall -Wextra -Wfatal-errors -Wno-unused-function -DMG_ENABLE_SSL
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


deps:
	$(eval depcjson := $(shell sudo dpkg -l | grep libcjson-dev))
	$(eval depcjson := $(if $(depcjson), $(depcjson), $(shell sudo ldconfig -p | grep libcjson)))
	$(if $(depcjson), , $(error "libcjson is missing, please install libcjson (sudo apt install libcjson-dev)"))

	$(eval depssl := $(shell sudo dpkg -l | grep libssl-dev))
	$(eval depssl := $(if $(depssl), $(depssl), $(shell sudo ldconfig -p | grep libssl-dev)))
	$(if $(depssl), , $(error "libssl-dev is missing, please install libssl-dev (sudo apt install libssl-dev)"))

	$(eval lcd := $(shell sudo ldconfig -p | grep libcd.so))
	$(if $(lcd), , $(error "libcd is missing, please install libcd (https://github.com/dataandsignal/libcd)"))

debugprereqs:
		mkdir -p $(DEBUGOUTPUTDIR)

releaseprereqs:
		mkdir -p $(RELEASEOUTPUTDIR)

install-prereqs:
		sudo mkdir -p /usr/local/include/he

install-post:
	$(eval ldconf := $(shell sudo which ldconfig))
	$(if $(ldconf), $(shell sudo ldconfig), $(shell echo "Warning: no ldconfig on this system (make sure your linker is updated with libhe)"))

debug:	CFLAGS += -g -ggdb3 -O0
debug:	debugprereqs $(SOURCES) deps $(DEBUGTARGET)
release:	CFLAGS +=
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
	make install-post

install-release: $(RELEASETARGET) install-headers
	sudo cp $(RELEASETARGET) /lib/
	make install-post

install: install-release

uninstall:
	sudo rm /lib/libhe.so
	sudo rm -rf /usr/local/include/he
	make install-post

clean:
	rm -rf $(DEBUGOBJECTS) $(DEBUGTARGET)
	rm -rf $(RELEASEOBJECTS) $(RELEASETARGET)

clean-all: clean examples-clean
