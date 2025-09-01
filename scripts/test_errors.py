#!/usr/bin/env python3
"""
错误处理测试脚本
专门用于测试语法和词法错误的处理
"""

import os
import sys
import subprocess
from pathlib import Path

def test_error_handling():
    """测试错误处理"""
    parser_path = "build/bin/parser"
    test_dir = "tests"
    
    # 定义错误测试用例
    error_tests = [
        {
            'file': 'test_lexical_error_string.txt',
            'description': '未终止的字符串字面量',
            'expected_error': 'LEXICAL ERROR',
            'expected_code': 1
        },
        {
            'file': 'test_lexical_error_char.txt',
            'description': '未终止的字符字面量',
            'expected_error': 'LEXICAL ERROR',
            'expected_code': 1
        },
        {
            'file': 'test_lexical_error_escape.txt',
            'description': '无效转义字符',
            'expected_error': 'LEXICAL ERROR',
            'expected_code': 1
        },
        {
            'file': 'test_syntax_error_missing_semicolon.txt',
            'description': '缺少分号',
            'expected_error': 'SYNTAX ERROR',
            'expected_code': 1
        },
        {
            'file': 'test_syntax_error_missing_paren.txt',
            'description': '缺少括号',
            'expected_error': 'SYNTAX ERROR',
            'expected_code': 1
        },
        {
            'file': 'test_syntax_error_invalid_operator.txt',
            'description': '无效运算符',
            'expected_error': 'SYNTAX ERROR',
            'expected_code': 1
        }
    ]
    
    print("=" * 60)
    print("错误处理测试")
    print("=" * 60)
    
    passed = 0
    failed = 0
    
    for test in error_tests:
        test_path = os.path.join(test_dir, test['file'])
        
        if not os.path.exists(test_path):
            print(f"❌ {test['description']}: 文件不存在")
            failed += 1
            continue
        
        try:
            # 运行测试
            result = subprocess.run(
                [parser_path, test_path],
                capture_output=True,
                text=True,
                timeout=5
            )
            
            # 检查结果
            success = (
                result.returncode == test['expected_code'] and
                test['expected_error'] in (result.stdout + result.stderr)
            )
            
            if success:
                print(f"✅ {test['description']}: 正确返回错误码 {result.returncode}")
                passed += 1
            else:
                print(f"❌ {test['description']}: 期望错误码 {test['expected_code']}，实际 {result.returncode}")
                print(f"   输出: {result.stdout + result.stderr}")
                failed += 1
                
        except subprocess.TimeoutExpired:
            print(f"❌ {test['description']}: 测试超时")
            failed += 1
        except Exception as e:
            print(f"❌ {test['description']}: 执行错误 - {e}")
            failed += 1
    
    print("=" * 60)
    print(f"测试总结: {passed} 通过, {failed} 失败")
    
    if failed == 0:
        print("🎉 所有错误处理测试都通过了！")
        return True
    else:
        print("💥 有错误处理测试失败")
        return False

if __name__ == "__main__":
    success = test_error_handling()
    sys.exit(0 if success else 1)
