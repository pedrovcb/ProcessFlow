# ProcessFlow

Orquestrador de processos feito em C pra atividade da disciplina. O programa cria e gerencia processos filhos pra executar tarefas, com suporte a execução sequencial, paralela, pipes, redirecionamento de input/output e background.

## Arquivos do projeto

- `main.c` — entry point, trata os dois modos (interativo e workflow) e os handlers dos comandos
- `parser.c/.h` — tokeniza a linha de comando e identifica qual comando foi digitado
- `task.c/.h` — guarda as tarefas cadastradas (nome + programa + argumentos)
- `executor.c/.h` — faz o fork/exec/waitpid pro run sequential e run parallel
- `pipe.c/.h` — conecta stdout de uma tarefa no stdin da outra usando pipe() e dup2()
- `redirect.c/.h` — redireciona entrada/saída pra arquivos (input, output, append)
- `jobs.c/.h` — gerencia os jobs em background (start, jobs, wait)

## Como compilar

```
make
```

Isso gera o executável `processflow`. Pra limpar os .o e o executável:

```
make clean
```

## Como executar

Modo interativo (digita os comandos no prompt):
```
./processflow
```

Modo workflow (passa um arquivo .pf com os comandos):
```
./processflow arquivo.pf
```

## Testes

O projeto vem com os 3 testes do professor mais 4 que eu adicionei pra cobrir mais casos:

```
make test
```

Roda os testes de sequential, pipe, redirecionamento, background, erros, workflow sem exit e exit code diferente de zero.

## Comandos

- `task <nome> <programa> [args]` — cadastra uma tarefa
- `run sequential <t1> [t2 ...]` — roda em ordem, uma depois da outra
- `run parallel <t1> [t2 ...]` — roda todas ao mesmo tempo
- `run pipe <t1> <t2> [t3 ...]` — encadeia via pipe (saida de um vira entrada do outro)
- `run <tarefa>` — atalho pro sequential
- `input <tarefa> <arquivo>` — tarefa lê do arquivo em vez do stdin
- `output <tarefa> <arquivo>` — tarefa escreve no arquivo (sobrescreve)
- `append <tarefa> <arquivo>` — tarefa escreve no arquivo (adiciona no final)
- `workdir <diretorio>` — muda o diretório de trabalho das próximas tarefas
- `start <tarefa>` — roda em background, volta pro prompt na hora
- `jobs` — mostra os jobs em background
- `wait <id>` — espera um job terminar
- `exit` — sai do programa

## Sistema Operacional

Esse projeto foi feito com o Linux, mais especificamente na distro Omarchy (Arch Linux). 
A implementação utiliza exclusivamente system calls POSIX { fork(), execvp(), waitpid(), pipe(), dup2(), chdir() }, o que garante compatibilidade com qualquer sistema que siga o padrão POSIX, incluindo outras distribuições Linux, macOS e sistemas Unix em geral. O código foi compilado com o GCC.
