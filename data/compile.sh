#!/bin/bash

# Nome do arquivo de código-fonte C++
cpp_file="main.cpp"
hpp_file="main.hpp"

# Nome do arquivo objeto de saída
cpp_output_file="main_cpp.o"
hpp_output_file="main_hpp.o"

# Compilação usando o GCC
#g++ -o "$output_file" "$cpp_file"

g++ -std=c++11 -o "$cpp_output_file" "$cpp_file"
g++ -std=c++11 -o "$hpp_output_file" "$hpp_file"

# Verificar se a compilação foi bem-sucedida
if [ $? -eq 0 ]; then
  echo "Compilação bem-sucedida. O arquivo objeto $output_file foi criado."
else
  echo "Erro durante a compilação."
fi
