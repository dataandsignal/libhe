CC			= gcc
CFLAGS			= -c -Wall -Wextra -Wfatal-errors -Wno-unused-function
SRCDIR 			= src
DEBUGOUTPUTDIR 		= build/debug
RELEASEOUTPUTDIR	= build/release
EXAMPLE_HTTP_GET_ENDPOINT_SOURCES	= http_get_endpoint.c
EXAMPLE_HTTP_POST_ENDPOINT_SOURCES	= http_post_endpoint.c
EXAMPLE_HTTP_GET_POST_CALLBACK_SOURCES	= http_get_post_callback.c
INCLUDES		= -I. -I../include
INCLUDES		= -I./src -Iinclude -I/usr/local/include/he
LIBS			= -lhe -lcjson -lssl -pthread

_EXAMPLE_HTTP_GET_ENDPOINT_OBJECTS		= $(EXAMPLE_HTTP_GET_ENDPOINT_SOURCES:.c=.o)
EXAMPLE_HTTP_GET_ENDPOINT_DEBUGOBJECTS 		= $(patsubst src/%,$(DEBUGOUTPUTDIR)/%,$(_EXAMPLE_HTTP_GET_ENDPOINT_OBJECTS))
EXAMPLE_HTTP_GET_ENDPOINT_RELEASEOBJECTS	= $(patsubst src/%,$(RELEASEOUTPUTDIR)/%,$(_EXAMPLE_HTTP_GET_ENDPOINT_OBJECTS))
EXAMPLE_HTTP_GET_ENDPOINT_DEBUGTARGET		= build/debug/httpgetendpoint
EXAMPLE_HTTP_GET_ENDPOINT_RELEASETARGET	= build/release/httpgetendpoint

_EXAMPLE_HTTP_POST_ENDPOINT_OBJECTS		= $(EXAMPLE_HTTP_POST_ENDPOINT_SOURCES:.c=.o)
EXAMPLE_HTTP_POST_ENDPOINT_DEBUGOBJECTS 		= $(patsubst src/%,$(DEBUGOUTPUTDIR)/%,$(_EXAMPLE_HTTP_POST_ENDPOINT_OBJECTS))
EXAMPLE_HTTP_POST_ENDPOINT_RELEASEOBJECTS	= $(patsubst src/%,$(RELEASEOUTPUTDIR)/%,$(_EXAMPLE_HTTP_POST_ENDPOINT_OBJECTS))
EXAMPLE_HTTP_POST_ENDPOINT_DEBUGTARGET		= build/debug/httppostendpoint
EXAMPLE_HTTP_POST_ENDPOINT_RELEASETARGET	= build/release/httppostendpoint

_EXAMPLE_HTTP_GET_POST_CALLBACK_OBJECTS		= $(EXAMPLE_HTTP_GET_POST_CALLBACK_SOURCES:.c=.o)
EXAMPLE_HTTP_GET_POST_CALLBACK_DEBUGOBJECTS 		= $(patsubst src/%,$(DEBUGOUTPUTDIR)/%,$(_EXAMPLE_HTTP_GET_POST_CALLBACK_OBJECTS))
EXAMPLE_HTTP_GET_POST_CALLBACK_RELEASEOBJECTS	= $(patsubst src/%,$(RELEASEOUTPUTDIR)/%,$(_EXAMPLE_HTTP_GET_POST_CALLBACK_OBJECTS))
EXAMPLE_HTTP_GET_POST_CALLBACK_DEBUGTARGET		= build/debug/httpgetpostcallback
EXAMPLE_HTTP_GET_POST_CALLBACK_RELEASETARGET	= build/release/httpgetpostcallback

debugprereqs:
		mkdir -p $(DEBUGOUTPUTDIR)

releaseprereqs:
		mkdir -p $(RELEASEOUTPUTDIR)

debugall:	debugprereqs $(EXAMPLE_HTTP_GET_ENDPOINT_SOURCES) $(EXAMPLE_HTTP_GET_ENDPOINT_DEBUGTARGET) $(EXAMPLE_HTTP_POST_ENDPOINT_SOURCES) $(EXAMPLE_HTTP_POST_ENDPOINT_DEBUGTARGET) $(EXAMPLE_HTTP_GET_POST_CALLBACK_SOURCES) $(EXAMPLE_HTTP_GET_POST_CALLBACK_DEBUGTARGET)
releaseall:	releaseprereqs $(EXAMPLE_HTTP_GET_ENDPOINT_SOURCES) $(EXAMPLE_HTTP_GET_ENDPOINT_RELEASETARGET) $(EXAMPLE_HTTP_POST_ENDPOINT_SOURCES) $(EXAMPLE_HTTP_POST_ENDPOINT_RELEASETARGET) $(EXAMPLE_HTTP_GET_POST_CALLBACK_SOURCES) $(EXAMPLE_HTTP_GET_POST_CALLBACK_RELEASETARGET)

# additional flags
examples-debug:		CFLAGS += -DDEBUG -g -ggdb3 -O0
examples-debug:		debugall

examples-release:	CFLAGS +=
examples-release: 	releaseall

examples:		examples-release


$(EXAMPLE_HTTP_GET_ENDPOINT_DEBUGTARGET): $(EXAMPLE_HTTP_GET_ENDPOINT_DEBUGOBJECTS) 
	$(CC) $(LDFLAGS) $(EXAMPLE_HTTP_GET_ENDPOINT_DEBUGOBJECTS) $(LIBS) -o $@

$(EXAMPLE_HTTP_GET_ENDPOINT_RELEASETARGET): $(EXAMPLE_HTTP_GET_ENDPOINT_RELEASEOBJECTS) 
	$(CC) $(LDFLAGS) $(EXAMPLE_HTTP_GET_ENDPOINT_RELEASEOBJECTS) $(LIBS) -o $@

$(EXAMPLE_HTTP_POST_ENDPOINT_DEBUGTARGET): $(EXAMPLE_HTTP_POST_ENDPOINT_DEBUGOBJECTS) 
	$(CC) $(LDFLAGS) $(EXAMPLE_HTTP_POST_ENDPOINT_DEBUGOBJECTS) $(LIBS) -o $@

$(EXAMPLE_HTTP_POST_ENDPOINT_RELEASETARGET): $(EXAMPLE_HTTP_POST_ENDPOINT_RELEASEOBJECTS) 
	$(CC) $(LDFLAGS) $(EXAMPLE_HTTP_POST_ENDPOINT_RELEASEOBJECTS) $(LIBS) -o $@

$(EXAMPLE_HTTP_GET_POST_CALLBACK_DEBUGTARGET): $(EXAMPLE_HTTP_GET_POST_CALLBACK_DEBUGOBJECTS) 
	$(CC) $(LDFLAGS) $(EXAMPLE_HTTP_GET_POST_CALLBACK_DEBUGOBJECTS) $(LIBS) -o $@

$(EXAMPLE_HTTP_GET_POST_CALLBACK_RELEASETARGET): $(EXAMPLE_HTTP_GET_POST_CALLBACK_RELEASEOBJECTS) 
	$(CC) $(LDFLAGS) $(EXAMPLE_HTTP_GET_POST_CALLBACK_RELEASEOBJECTS) $(LIBS) -o $@

$(DEBUGOUTPUTDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

$(RELEASEOUTPUTDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

all: examples-release

.DEFAULT_GOAL = examples-release

clean:
	rm -rf $(EXAMPLE_HTTP_GET_ENDPOINT_DEBUGOBJECTS) $(EXAMPLE_HTTP_GET_ENDPOINT_DEBUGTARGET)
	rm -rf $(EXAMPLE_HTTP_GET_ENDPOINT_RELEASEOBJECTS) $(EXAMPLE_HTTP_GET_ENDPOINT_RELEASETARGET)
	rm -rf $(EXAMPLE_HTTP_POST_ENDPOINT_DEBUGOBJECTS) $(EXAMPLE_HTTP_POST_ENDPOINT_DEBUGTARGET)
	rm -rf $(EXAMPLE_HTTP_POST_ENDPOINT_RELEASEOBJECTS) $(EXAMPLE_HTTP_POST_ENDPOINT_RELEASETARGET)
	rm -rf $(EXAMPLE_HTTP_GET_POST_CALLBACK_DEBUGOBJECTS) $(EXAMPLE_HTTP_GET_POST_CALLBACK_DEBUGTARGET)
	rm -rf $(EXAMPLE_HTTP_GET_POST_CALLBACK_RELEASEOBJECTS) $(EXAMPLE_HTTP_GET_POST_CALLBACK_RELEASETARGET)
