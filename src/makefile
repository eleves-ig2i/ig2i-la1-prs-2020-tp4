CC=gcc
EXE_DIR=../out/
W_FLAGS=-Wall
LINK=-lpthread

all: barbier_01 barbier_02 barbier_03 question_1 question_3

barbier_01: barbier_01.c
	$(CC) $^ $(W_FLAGS) -o $(EXE_DIR)$@.exe $(LINK)

barbier_02: barbier_02.c
	$(CC) $^ $(W_FLAGS) -o $(EXE_DIR)$@.exe $(LINK)
	
barbier_03: barbier_03.c
	$(CC) $^ $(W_FLAGS) -o $(EXE_DIR)$@.exe $(LINK)
	
question_1: question_1.c
	$(CC) $^ $(W_FLAGS) -o $(EXE_DIR)$@.exe $(LINK)
	
question_3: question_3.c
	$(CC) $^ $(W_FLAGS) -o $(EXE_DIR)$@.exe $(LINK)
	
clean :
	rm -f $(EXE_DIR)*.exe
