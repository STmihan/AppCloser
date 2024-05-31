CC = cl
LIBS = user32.lib Advapi32.lib kernel32.lib

SRC = src/hate.c
RES = src/resource.rc
OUT = out
EXE_NAME = Vintage_Story_1.19.8_stable_by_OverF1X.exe

all: build

# Цель для создания папки out
$(OUT):
	if not exist $(OUT) mkdir $(OUT)

res: $(OUT)
	rc /fo $(OUT)/res.res $(RES)

build: res
	$(CC) $(SRC) $(OUT)/res.res /Fe:$(OUT)/$(EXE_NAME) /Fo:$(OUT)\ /link $(LIBS)

clean:
	if exist $(OUT) rd /s /q $(OUT)
