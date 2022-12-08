COMPILER = clang

GAME_TITLE = \"9\ YEARS\"
GAME_BINARY = "9years"
GAME_VERSION = \"1.0.0\"

PROGRAM_INFO = -DMVERSION=$(GAME_VERSION) -DMTITLE=$(GAME_TITLE)

BUILD_OPTIONS = -g3 -Wpedantic -Ivendor/include/ -D_DEBUG $(PROGRAM_INFO)
RELEASE_OPTIONS = -O2 -Wpedantic -Ivendor/include/ -D_BUILD_MAC_APP $(PROGRAM_INFO)

SOURCE_LIBS = -Ivendor/sources/

CFILES = src/*.c

WORKSPACE = $(shell pwd)

MAC_OPT = -Lvendor/lib/mac/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL vendor/lib/mac/libraylib.a
MAC_OUT = -o "bin/build_mac"
MAC_OUT_RELEASE = -o "bin/$(GAME_TITLE).app/Contents/MacOS/$(GAME_BINARY)"
MAC_ZIP = "$(GAME_TITLE)_v$(GAME_VERSION)_mac.zip"

WIN_OPT = -O2 -Lvendor/lib/win/ ./vendor/lib/win/libraylib.a -lopengl32 -lgdi32 -lwinmm
WIN_OUT = -o "bin/build_win"
WIN_ZIP = "$(GAME_TITLE)_v$(GAME_VERSION)_windows.zip"

LIN_OPT = -O2 -Lvendor/lib/lin/ -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
LIN_OUT = -o "bin/build_lin"
LIN_ZIP = "$(GAME_TITLE)_v$(GAME_VERSION)_linux.zip"

WEB_OPT = -Os -O2 -Wpedantic ./vendor/lib/web/libraylib.a -I. -Ivendor/include/ -s USE_GLFW=3 --shell-file src/minshell.html --preload-file assets -DPLATFORM_WEB
WEB_OUT = -o "bin/index.html"
WEB_ZIP = "$(GAME_TITLE)_v$(GAME_VERSION)_web.zip"

BROWSER = firefox

setup: 
	mkdir bin

build_mac:
	$(COMPILER) $(BUILD_OPTIONS) $(CFILES) $(SOURCE_LIBS) $(MAC_OUT) $(MAC_OPT)

release_mac:
	$(COMPILER) $(RELEASE_OPTIONS) $(CFILES) $(SOURCE_LIBS) $(MAC_OUT_RELEASE) $(MAC_OPT) && \
    rsync -rupE assets bin/$(GAME_TITLE).app/Contents/MacOS/ && \
	/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/LaunchServices.framework/Versions/A/Support/lsregister -f bin/$(GAME_TITLE).app/ && \
	cd bin && zip -r $(MAC_ZIP) $(GAME_TITLE).app

publish_mac:
	butler push bin/$(MAC_ZIP) basil-termini/$(GAME_BINARY):mac --userversion $(GAME_VERSION)


build_win:
	$(COMPILER) $(BUILD_OPTIONS) $(CFILES) $(SOURCE_LIBS) $(WIN_OUT) $(WIN_OPT)

build_lin:
	$(COMPILER) $(BUILD_OPTIONS) $(CFILES) $(SOURCE_LIBS) $(LIN_OUT) $(LIN_OPT)


build_web:
	cd /Users/basil/Documents/emsdk && source "/Users/basil/Documents/emsdk/emsdk_env.sh" && cd "${WORKSPACE}" && \
	emcc $(WEB_OUT) $(CFILES) $(WEB_OPT) && \
	emrun bin/index.html --browser $(BROWSER)

ifeq ([ $? -eq 127 ], 0)
	cd /Users/basil/Documents/emsdk && source ./emsdk_env.sh && cd ${WORKSPACE} && \
	emcc $(WEB_OUT) $(CFILES) $(WEB_OPT) && \
	emrun bin/index.html --browser $(BROWSER)
endif

release_web:
	cd /Users/basil/Documents/emsdk && source ./emsdk_env.sh && cd "${WORKSPACE}" && \
	emcc $(WEB_OUT) $(CFILES) $(WEB_OPT) && \
	cd bin && zip $(WEB_ZIP) index.*

publish_web:
	butler push bin/$(WEB_ZIP) basil-termini/$(GAME_BINARY):web --userversion $(GAME_VERSION)
	