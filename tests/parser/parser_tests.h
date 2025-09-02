#ifndef PARSER_TESTS_H
#define PARSER_TESTS_H

// 声明所有测试函数
int test_parser_comprehensive();
int test_parser();
int test_parser_simple();
int test_parser_helpers();
int test_expression_parsing();
int test_ast_only();
int test_constant_expression();
int test_leaf_expression();
int test_assignment_binary();
int test_simplified_binary();
int test_simplified_expressions();
int test_cast_expression();
int test_str_simple();
int test_str_method();
int test_debug_let();
int test_debug_lexer();

// 主函数，调用所有测试
int main() {
    test_parser_comprehensive();
    test_parser();
    test_parser_simple();
    test_parser_helpers();
    test_expression_parsing();
    test_ast_only();
    test_constant_expression();
    test_leaf_expression();
    test_assignment_binary();
    test_simplified_binary();
    test_simplified_expressions();
    test_cast_expression();
    test_str_simple();
    test_str_method();
    test_debug_let();
    test_debug_lexer();
    return 0;
}

#endif // PARSER_TESTS_H
