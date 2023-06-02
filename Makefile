# Especifica o compilador a ser usado
CC = g++
# Especifica as flags de compilação a serem usadas
CFLAGS = -Wall -Wextra -Werror -std=c++20 -pedantic -pthread -march=native -mtune=native 

# Especifica os diretórios de origem, objeto e binário
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Usa a função wildcard para obter uma lista de todos os arquivos-fonte na pasta de origem
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
# Substitui o diretório de origem pelo diretório de objeto para obter uma lista de todos os arquivos-objeto necessários
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))
# Especifica o nome do executável a ser gerado e onde deve ser salvo
EXECUTABLE = $(BINDIR)/meu_programa

# Regra padrão para executar tudo
all: $(EXECUTABLE)

# Regra para gerar o executável
$(EXECUTABLE): $(OBJECTS) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

# Regra para gerar cada arquivo-objeto individualmente
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para criar o diretório de objeto, se ainda não existir
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Regra para criar o diretório de binário, se ainda não existir
$(BINDIR):
	mkdir -p $(BINDIR)

# Regra para limpar tudo
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Especifica quais alvos não correspondem a arquivos
.PHONY: all clean
