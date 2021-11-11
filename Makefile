BINDIR = bin
OBJDIR = obj
SRCDIR = src
DEPDIR = d

$(shell mkdir -p ${BINDIR})
$(shell mkdir -p ${OBJDIR})
$(shell mkdir -p ${DEPDIR})

BINNAME = cckuc
TESTNAME = test
BINPATH = ${BINDIR}/${BINNAME}
TESTPATH = ${BINDIR}/${TESTNAME}

CC = cc
CSTANDARD = 11
OPTIMIZATION = 3
CPPFLAGS = -MMD -MP -MF
CFLAGS = -Wall -Wextra -Wconversion -Werror -std=c${CSTANDARD} -O${OPTIMIZATION}

${BINPATH}: ${OBJDIR}/cmd.o ${OBJDIR}/source.o ${OBJDIR}/scanner.o \
${OBJDIR}/token.o ${OBJDIR}/str.o ${OBJDIR}/fatal.o ${OBJDIR}/slice.o \
${OBJDIR}/ast.o ${OBJDIR}/parser.o ${OBJDIR}/byte_reader.o ${OBJDIR}/position.o \
${OBJDIR}/charset.o
	${CC} -o $@ $^

.PHONY: test
test: ${TESTPATH}
	${TESTPATH} 

${TESTPATH}: ${OBJDIR}/test.o ${OBJDIR}/source.o ${OBJDIR}/scanner.o \
${OBJDIR}/token.o ${OBJDIR}/str.o ${OBJDIR}/slice.o ${OBJDIR}/byte_reader.o \
${OBJDIR}/position.o ${OBJDIR}/charset.o ${OBJDIR}/fatal.o
	${CC} -o $@ $^

${OBJDIR}/cmd.o: ${SRCDIR}/cmd.c
	${CC} ${CPPFLAGS} ${DEPDIR}/cmd.d ${CFLAGS} -o $@ -c $<
-include ${DEPDIR}/cmd.d

${OBJDIR}/test.o: ${SRCDIR}/test.c
	${CC} ${CPPFLAGS} ${DEPDIR}/test.d ${CFLAGS} -o $@ -c $<
-include ${DEPDIR}/test.d

${OBJDIR}/source.o: ${SRCDIR}/source.c
	${CC} ${CPPFLAGS} ${DEPDIR}/source.d ${CFLAGS} -o $@ -c $<
-include ${DEPDIR}/source.d

${OBJDIR}/scanner.o: ${SRCDIR}/scanner.c
	${CC} ${CPPFLAGS} ${DEPDIR}/scanner.d ${CFLAGS} -o $@ -c $<
-include ${DEPDIR}/scanner.d

${OBJDIR}/token.o: ${SRCDIR}/token.c
	${CC} ${CPPFLAGS} ${DEPDIR}/token.d ${CFLAGS} -o $@ -c $<
-include ${DEPDIR}/token.d

${OBJDIR}/str.o: ${SRCDIR}/str.c
	${CC} ${CPPFLAGS} ${DEPDIR}/str.d ${CFLAGS} -o $@ -c $<
-include ${DEPDIR}/str.d

${OBJDIR}/byte_reader.o: ${SRCDIR}/byte_reader.c
	${CC} ${CPPFLAGS} ${DEPDIR}/byte_reader.d ${CFLAGS} -o $@ -c $<
-include ${DEPDIR}/byte_reader.d

${OBJDIR}/position.o: ${SRCDIR}/position.c
	${CC} ${CPPFLAGS} ${DEPDIR}/position.d ${CFLAGS} -o $@ -c $<
-include ${DEPDIR}/position.d

${OBJDIR}/slice.o: ${SRCDIR}/slice.c
	${CC} ${CPPFLAGS} ${DEPDIR}/slice.d ${CFLAGS} -o $@ -c $<
-include ${DEPDIR}/slice.d

${OBJDIR}/ast.o: ${SRCDIR}/ast.c
	${CC} ${CPPFLAGS} ${DEPDIR}/ast.d ${CFLAGS} -o $@ -c $<
-include ${DEPDIR}/ast.d

${OBJDIR}/parser.o: ${SRCDIR}/parser.c
	${CC} ${CPPFLAGS} ${DEPDIR}/parser.d ${CFLAGS} -o $@ -c $<
-include ${DEPDIR}/parser.d

${OBJDIR}/fatal.o: ${SRCDIR}/fatal.c
	${CC} ${CPPFLAGS} ${DEPDIR}/fatal.d ${CFLAGS} -o $@ -c $<
-include ${DEPDIR}/fatal.d

${OBJDIR}/charset.o: ${SRCDIR}/charset.c
	${CC} ${CPPFLAGS} ${DEPDIR}/charset.d ${CFLAGS} -o $@ -c $<
-include ${DEPDIR}/charset.d

.PHONY: clean
clean:
	rm -rf ${BINDIR} ${OBJDIR} ${DEPDIR}
