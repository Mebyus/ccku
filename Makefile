-include .env

BIN_DIR = bin
OBJ_DIR = obj
SRC_DIR = src
DEP_DIR = d

BIN_NAME = cckuc
TEST_NAME = test

RELEASE_DIR = release
DEBUG_DIR = debug

CC = cc
CSTANDARD = 11
OPTIMIZATION = 2
CPPFLAGS = -MMD -MP -MF
WARNINGS = -Wall -Wextra -Wconversion -Wunreachable-code -Wshadow -Wundef -Wfloat-equal -Wformat -Wpointer-arith -Winit-self

ifeq (${DEBUG}, true)
	TARGET_BIN_DIR = ${BIN_DIR}/${DEBUG_DIR}
	TARGET_OBJ_DIR = ${OBJ_DIR}/${DEBUG_DIR}
	CFLAGS = ${WARNINGS} -Werror -std=c${CSTANDARD} -g
else
	TARGET_BIN_DIR = ${BIN_DIR}/${RELEASE_DIR}
	TARGET_OBJ_DIR = ${OBJ_DIR}/${RELEASE_DIR}
	CFLAGS = ${WARNINGS} -Werror -std=c${CSTANDARD} -O${OPTIMIZATION}
endif

$(shell mkdir -p ${TARGET_BIN_DIR})
$(shell mkdir -p ${TARGET_OBJ_DIR})
$(shell mkdir -p ${DEP_DIR})

BIN_PATH = ${TARGET_BIN_DIR}/${BIN_NAME}
TEST_PATH = ${TARGET_BIN_DIR}/${TEST_NAME}


${BIN_PATH}: ${TARGET_OBJ_DIR}/cmd.o ${TARGET_OBJ_DIR}/source.o ${TARGET_OBJ_DIR}/scanner.o \
${TARGET_OBJ_DIR}/token.o ${TARGET_OBJ_DIR}/str.o ${TARGET_OBJ_DIR}/fatal.o ${TARGET_OBJ_DIR}/slice.o \
${TARGET_OBJ_DIR}/ast.o ${TARGET_OBJ_DIR}/parser.o ${TARGET_OBJ_DIR}/byte_reader.o ${TARGET_OBJ_DIR}/position.o \
${TARGET_OBJ_DIR}/charset.o ${TARGET_OBJ_DIR}/map.o
	${CC} -o $@ $^

.PHONY: test
test: ${TEST_PATH}
	${TEST_PATH} tests/scanner/1.test

${TEST_PATH}: ${TARGET_OBJ_DIR}/test.o ${TARGET_OBJ_DIR}/source.o ${TARGET_OBJ_DIR}/scanner.o \
${TARGET_OBJ_DIR}/token.o ${TARGET_OBJ_DIR}/str.o ${TARGET_OBJ_DIR}/slice.o ${TARGET_OBJ_DIR}/byte_reader.o \
${TARGET_OBJ_DIR}/position.o ${TARGET_OBJ_DIR}/charset.o ${TARGET_OBJ_DIR}/fatal.o \
${TARGET_OBJ_DIR}/split_test_scanner.o ${TARGET_OBJ_DIR}/map.o ${TARGET_OBJ_DIR}/strop.o
	${CC} -o $@ $^

${TARGET_OBJ_DIR}/cmd.o: ${SRC_DIR}/cmd.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/cmd.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/cmd.d

${TARGET_OBJ_DIR}/test.o: ${SRC_DIR}/test.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/test.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/test.d

${TARGET_OBJ_DIR}/source.o: ${SRC_DIR}/source.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/source.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/source.d

${TARGET_OBJ_DIR}/scanner.o: ${SRC_DIR}/scanner.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/scanner.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/scanner.d

${TARGET_OBJ_DIR}/token.o: ${SRC_DIR}/token.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/token.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/token.d

${TARGET_OBJ_DIR}/str.o: ${SRC_DIR}/str.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/str.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/str.d

${TARGET_OBJ_DIR}/byte_reader.o: ${SRC_DIR}/byte_reader.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/byte_reader.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/byte_reader.d

${TARGET_OBJ_DIR}/position.o: ${SRC_DIR}/position.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/position.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/position.d

${TARGET_OBJ_DIR}/slice.o: ${SRC_DIR}/slice.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/slice.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/slice.d

${TARGET_OBJ_DIR}/ast.o: ${SRC_DIR}/ast.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/ast.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/ast.d

${TARGET_OBJ_DIR}/parser.o: ${SRC_DIR}/parser.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/parser.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/parser.d

${TARGET_OBJ_DIR}/fatal.o: ${SRC_DIR}/fatal.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/fatal.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/fatal.d

${TARGET_OBJ_DIR}/charset.o: ${SRC_DIR}/charset.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/charset.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/charset.d

${TARGET_OBJ_DIR}/timer.o: ${SRC_DIR}/timer.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/timer.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/timer.d

${TARGET_OBJ_DIR}/map.o: ${SRC_DIR}/map.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/map.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/map.d

${TARGET_OBJ_DIR}/strop.o: ${SRC_DIR}/strop.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/strop.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/strop.d

${TARGET_OBJ_DIR}/split_test_scanner.o: ${SRC_DIR}/split_test_scanner.c
	${CC} ${CPPFLAGS} ${DEP_DIR}/split_test_scanner.d ${CFLAGS} -o $@ -c $<
-include ${DEP_DIR}/split_test_scanner.d

.PHONY: clean
clean:
	rm -rf ${BIN_DIR} ${OBJ_DIR} ${DEP_DIR}
