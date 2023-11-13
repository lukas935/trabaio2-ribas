cmake_minimum_required(VERSION 3.13)
project(Gerenciador-Filmes-Arvore)
set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.c indices.c operacoesUsuario.c)
set(HEADER_FILES style.h filmes.h indices.h operacoesUsuario.h)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})