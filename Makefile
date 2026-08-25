CC = gcc
CFLAGS = -Wall -Wextra -g3
TARGET = processflow

SRCS = main.c parser.c task.c executor.c redirect.c pipe.c jobs.c
OBJS = $(SRCS:.c=.o)

TEST_DIR = testes

.PHONY: all clean test run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) *.o saida.txt

test: $(TARGET)
	@echo "=== Teste 1: Sequential ==="
	@cd $(TEST_DIR)/teste1 && ../../$(TARGET) teste1-entrada.txt > /tmp/teste1-out.txt 2>/dev/null
	@diff /tmp/teste1-out.txt $(TEST_DIR)/teste1/teste1-saida.txt && echo "PASS" || echo "FAIL"
	@rm -f /tmp/teste1-out.txt
	@echo ""
	@echo "=== Teste 2: Pipe ==="
	@cd $(TEST_DIR)/teste2 && ../../$(TARGET) teste2-entrada.txt > /tmp/teste2-out.txt 2>/dev/null
	@diff /tmp/teste2-out.txt $(TEST_DIR)/teste2/teste2-saida.txt && echo "PASS" || echo "FAIL"
	@rm -f /tmp/teste2-out.txt
	@echo ""
	@echo "=== Teste 3: Redirecionamento ==="
	@cd $(TEST_DIR)/teste3 && ../../$(TARGET) teste3-entrada.txt > /tmp/teste3-out.txt 2>&1
	@diff /tmp/teste3-out.txt $(TEST_DIR)/teste3/teste3-saida.txt && echo "PASS" || echo "FAIL"
	@rm -f /tmp/teste3-out.txt saida.txt
	@echo ""
	@echo "Todos os testes concluídos."

run: $(TARGET)
	./$(TARGET)