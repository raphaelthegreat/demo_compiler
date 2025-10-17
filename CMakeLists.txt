cmake_minimum_required(VERSION 3.15...4.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(compiler VERSION 1.0 LANGUAGES CXX)

# Run bison and flex on frontend parsers
set(FRONTEND_PATH ${CMAKE_CURRENT_SOURCE_DIR}/frontend)
set(LEX_FILENAME "compiler_lex_autogen")
set(BISON_FILENAME "compiler_tab_autogen")

add_custom_command(
    OUTPUT ${FRONTEND_PATH}/${BISON_FILENAME}.h ${FRONTEND_PATH}/${BISON_FILENAME}.cpp
    COMMAND bison --defines=${BISON_FILENAME}.h --output=${BISON_FILENAME}.cpp ${FRONTEND_PATH}/compiler.y
    WORKING_DIRECTORY ${FRONTEND_PATH}
)
add_custom_command(
    OUTPUT ${FRONTEND_PATH}/${LEX_FILENAME}.cpp
    COMMAND flex --outfile=${LEX_FILENAME}.cpp ${FRONTEND_PATH}/compiler.l
    WORKING_DIRECTORY ${FRONTEND_PATH}
)
add_custom_target(compiler_autogen DEPENDS ${FRONTEND_PATH}/${LEX_FILENAME}.cpp
                                           ${FRONTEND_PATH}/${BISON_FILENAME}.h
                                           ${FRONTEND_PATH}/${BISON_FILENAME}.cpp)

add_executable(compiler backend/mix/mix_emitter.cpp
                        backend/mix/mix_emitter.h
                        backend/mix/mix_ir_pass.cpp
                        backend/mix/mix_ir_pass.h
                        backend/mix/mix_item.h
                        backend/mix/mix_opcode.h
                        frontend/ast_node.h
                        frontend/${BISON_FILENAME}.cpp
                        frontend/${BISON_FILENAME}.h
                        frontend/${LEX_FILENAME}.cpp
                        frontend/parse_context.cpp
                        frontend/parse_context.h
                        frontend/translate_pass.cpp
                        frontend/translate_pass.h
                        ir/passes/identity_removal.cpp
                        ir/passes/load_store_elimination.cpp
                        ir/block.cpp
                        ir/block.h
                        ir/function.cpp
                        ir/function.h
                        ir/instruction.cpp
                        ir/instruction.h
                        ir/ir_emitter.cpp
                        ir/ir_emitter.h
                        ir/program.h
                        ir/value.cpp
                        ir/value.h
                        main.cpp)
target_include_directories(compiler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(compiler compiler_autogen)