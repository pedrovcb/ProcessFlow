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
	@echo "=== Teste 4: Background ==="
	@cd $(TEST_DIR)/teste-background && stdbuf -o0 -e0 ../../$(TARGET) entrada.txt > /tmp/teste-bg-out.txt 2>&1
	@sed -i 's/\[1\] [0-9]*/[1] <PID>/g' /tmp/teste-bg-out.txt
	@diff /tmp/teste-bg-out.txt $(TEST_DIR)/teste-background/saida.txt && echo "PASS" || echo "FAIL"
	@rm -f /tmp/teste-bg-out.txt
	@echo ""
	@echo "=== Teste 5: Erros ==="
	@cd $(TEST_DIR)/teste-erros && stdbuf -o0 -e0 ../../$(TARGET) entrada.txt > /tmp/teste-err-out.txt 2>&1
	@diff /tmp/teste-err-out.txt $(TEST_DIR)/teste-erros/saida.txt && echo "PASS" || echo "FAIL"
	@rm -f /tmp/teste-err-out.txt
	@echo ""
	@echo "=== Teste 6: Workflow sem exit ==="
	@cd $(TEST_DIR)/teste-workflow-sem-exit && stdbuf -o0 -e0 ../../$(TARGET) entrada.txt > /tmp/teste-wf-out.txt 2>&1
	@diff /tmp/teste-wf-out.txt $(TEST_DIR)/teste-workflow-sem-exit/saida.txt && echo "PASS" || echo "FAIL"
	@rm -f /tmp/teste-wf-out.txt
	@echo ""
	@echo "=== Teste 7: Exit Code !== 0 ==="
	@cd $(TEST_DIR)/teste-exit-code && stdbuf -o0 -e0 ../../$(TARGET) entrada.txt > /tmp/teste-ec-out.txt 2>&1
	@diff /tmp/teste-ec-out.txt $(TEST_DIR)/teste-exit-code/saida.txt && echo "PASS" || echo "FAIL"
	@rm -f /tmp/teste-ec-out.txt
	@echo ""
	@echo "Todos os testes concluídos."

run: $(TARGET)
	./$(TARGET)