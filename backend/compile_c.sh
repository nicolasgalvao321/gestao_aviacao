#!/bin/bash
# compile_c.sh — Script para compilar o módulo C

echo "Compilando módulo C para validação de assentos..."

# Verificar se o compilador gcc está disponível
if ! command -v gcc &> /dev/null; then
    echo "Erro: gcc não está instalado. Instale com: sudo apt-get install build-essential"
    exit 1
fi

# Compilar como biblioteca compartilhada
gcc -shared -o libseat_rules.so -fPIC seat_rules.c

if [ $? -eq 0 ]; then
    echo "✓ Compilação bem-sucedida!"
    echo "✓ Arquivo gerado: libseat_rules.so"
    ls -lh libseat_rules.so
else
    echo "Erro durante a compilação."
    exit 1
fi
