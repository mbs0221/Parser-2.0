#!/usr/bin/env python3
"""
系统化测试运行器
用于执行所有unit-tests，记录词法/语法分析错误，处理超时问题
"""

import os
import sys
import subprocess
import time
import json
import re
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Tuple, Optional
from dataclasses import dataclass, asdict
from enum import Enum

class TestStatus(Enum):
    PASSED = "PASSED"
    FAILED = "FAILED"
    TIMEOUT = "TIMEOUT"
    LEXER_ERROR = "LEXER_ERROR"
    PARSER_ERROR = "PARSER_ERROR"
    INTERPRETER_ERROR = "INTERPRETER_ERROR"
    SKIPPED = "SKIPPED"

@dataclass
class TestResult:
    test_file: str
    status: TestStatus
    execution_time: float
    error_message: str = ""
    output: str = ""
    category: str = ""

class SystematicTestRunner:
    def __init__(self, project_root: str, timeout: int = 30):
        self.project_root = Path(project_root)
        self.timeout = timeout
        self.results: List[TestResult] = []
        self.categories = {
            'arithmetic': ['arithmetic', 'math', 'plus', 'minus', 'multiply', 'divide'],
            'function': ['function', 'method', 'call'],
            'class': ['class', 'struct', 'object'],
            'control_flow': ['if', 'while', 'for', 'switch', 'condition'],
            'variable': ['var', 'let', 'assignment', 'declaration'],
            'string': ['string', 'str'],
            'array': ['array', 'list', 'collection'],
            'debug': ['debug', 'print', 'output'],
            'basic': ['basic', 'simple'],
            'comprehensive': ['comprehensive', 'complete', 'full']
        }
        
        # 可执行文件路径
        self.executables = {
            'lexer': self.project_root / 'build' / 'bin' / 'lexer_main',
            'parser': self.project_root / 'bin' / 'parser_main',
            'interpreter': self.project_root / 'build' / 'bin' / 'interpreter_main'
        }
        
        # 检查可执行文件是否存在
        self._check_executables()
    
    def _check_executables(self):
        """检查可执行文件是否存在"""
        for name, path in self.executables.items():
            if not path.exists():
                print(f"警告: {name} 可执行文件不存在: {path}")
                # 尝试其他可能的路径
                alt_paths = [
                    self.project_root / 'build' / 'bin' / f'{name}_main',
                    self.project_root / 'build' / 'tests' / name,
                    self.project_root / 'build' / 'src' / name
                ]
                for alt_path in alt_paths:
                    if alt_path.exists():
                        self.executables[name] = alt_path
                        print(f"找到替代路径: {alt_path}")
                        break
    
    def categorize_test(self, test_file: str) -> str:
        """根据文件名对测试进行分类"""
        filename = Path(test_file).stem.lower()
        
        for category, keywords in self.categories.items():
            if any(keyword in filename for keyword in keywords):
                return category
        
        return 'other'
    
    def run_test_with_timeout(self, test_file: str, executable: str) -> Tuple[TestStatus, str, str, float]:
        """使用超时运行测试"""
        start_time = time.time()
        
        try:
            # 构建命令
            cmd = [str(executable), str(test_file)]
            
            # 运行命令，设置超时
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=self.timeout,
                cwd=self.project_root
            )
            
            execution_time = time.time() - start_time
            
            # 分析结果
            if result.returncode == 0:
                return TestStatus.PASSED, result.stdout, result.stderr, execution_time
            else:
                # 分析错误类型
                error_output = result.stderr + result.stdout
                if 'lexer' in error_output.lower() or 'token' in error_output.lower():
                    return TestStatus.LEXER_ERROR, result.stdout, result.stderr, execution_time
                elif 'parser' in error_output.lower() or 'syntax' in error_output.lower():
                    return TestStatus.PARSER_ERROR, result.stdout, result.stderr, execution_time
                else:
                    return TestStatus.INTERPRETER_ERROR, result.stdout, result.stderr, execution_time
                    
        except subprocess.TimeoutExpired:
            execution_time = time.time() - start_time
            return TestStatus.TIMEOUT, "", f"测试超时 ({self.timeout}秒)", execution_time
        except Exception as e:
            execution_time = time.time() - start_time
            return TestStatus.FAILED, "", f"执行错误: {str(e)}", execution_time
    
    def run_single_test(self, test_file: str) -> TestResult:
        """运行单个测试文件"""
        print(f"运行测试: {test_file}")
        
        # 确定使用哪个可执行文件
        executable = self.executables['interpreter']  # 默认使用解释器
        
        # 如果解释器不存在，尝试使用parser
        if not executable.exists():
            executable = self.executables['parser']
        
        # 如果parser也不存在，使用lexer
        if not executable.exists():
            executable = self.executables['lexer']
        
        if not executable.exists():
            return TestResult(
                test_file=test_file,
                status=TestStatus.SKIPPED,
                execution_time=0,
                error_message="没有可用的可执行文件"
            )
        
        # 运行测试
        status, stdout, stderr, exec_time = self.run_test_with_timeout(test_file, executable)
        
        # 创建测试结果
        result = TestResult(
            test_file=test_file,
            status=status,
            execution_time=exec_time,
            error_message=stderr,
            output=stdout,
            category=self.categorize_test(test_file)
        )
        
        # 打印结果
        status_emoji = {
            TestStatus.PASSED: "✅",
            TestStatus.FAILED: "❌",
            TestStatus.TIMEOUT: "⏰",
            TestStatus.LEXER_ERROR: "🔤",
            TestStatus.PARSER_ERROR: "📝",
            TestStatus.INTERPRETER_ERROR: "⚙️",
            TestStatus.SKIPPED: "⏭️"
        }
        
        print(f"  {status_emoji.get(status, '❓')} {status.value} ({exec_time:.2f}s)")
        if stderr:
            print(f"    错误: {stderr[:100]}...")
        
        return result
    
    def run_all_tests(self, test_dir: str = "unit-tests") -> List[TestResult]:
        """运行所有测试"""
        test_dir_path = self.project_root / test_dir
        
        if not test_dir_path.exists():
            print(f"错误: 测试目录不存在: {test_dir_path}")
            return []
        
        # 获取所有测试文件
        test_files = list(test_dir_path.glob("test_*.txt"))
        test_files.sort()
        
        print(f"找到 {len(test_files)} 个测试文件")
        print("=" * 60)
        
        # 运行所有测试
        for test_file in test_files:
            result = self.run_single_test(str(test_file))
            self.results.append(result)
        
        return self.results
    
    def generate_report(self, output_file: str = None) -> str:
        """生成测试报告"""
        if not output_file:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_file = f"test_report_{timestamp}.json"
        
        # 统计结果
        stats = {}
        for status in TestStatus:
            stats[status.value] = len([r for r in self.results if r.status == status])
        
        # 按类别统计
        category_stats = {}
        for result in self.results:
            category = result.category
            if category not in category_stats:
                category_stats[category] = {}
            status = result.status.value
            category_stats[category][status] = category_stats[category].get(status, 0) + 1
        
        # 生成报告
        report = {
            'timestamp': datetime.now().isoformat(),
            'total_tests': len(self.results),
            'statistics': stats,
            'category_statistics': category_stats,
            'results': [asdict(result) for result in self.results]
        }
        
        # 转换TestStatus枚举为字符串
        for result_dict in report['results']:
            result_dict['status'] = result_dict['status'].value
        
        # 保存报告
        with open(output_file, 'w', encoding='utf-8') as f:
            json.dump(report, f, indent=2, ensure_ascii=False)
        
        return output_file
    
    def print_summary(self):
        """打印测试摘要"""
        if not self.results:
            print("没有测试结果")
            return
        
        print("\n" + "=" * 60)
        print("测试摘要")
        print("=" * 60)
        
        # 总体统计
        total = len(self.results)
        passed = len([r for r in self.results if r.status == TestStatus.PASSED])
        failed = total - passed
        
        print(f"总测试数: {total}")
        print(f"通过: {passed} ({passed/total*100:.1f}%)")
        print(f"失败: {failed} ({failed/total*100:.1f}%)")
        
        # 按状态统计
        print("\n按状态分类:")
        for status in TestStatus:
            count = len([r for r in self.results if r.status == status])
            if count > 0:
                print(f"  {status.value}: {count}")
        
        # 按类别统计
        print("\n按类别分类:")
        category_counts = {}
        for result in self.results:
            category_counts[result.category] = category_counts.get(result.category, 0) + 1
        
        for category, count in sorted(category_counts.items()):
            print(f"  {category}: {count}")
        
        # 显示失败的测试
        failed_tests = [r for r in self.results if r.status != TestStatus.PASSED]
        if failed_tests:
            print(f"\n失败的测试 ({len(failed_tests)}个):")
            for result in failed_tests[:10]:  # 只显示前10个
                print(f"  {result.test_file}: {result.status.value}")
            if len(failed_tests) > 10:
                print(f"  ... 还有 {len(failed_tests) - 10} 个失败测试")

def main():
    """主函数"""
    import argparse
    
    parser = argparse.ArgumentParser(description='系统化测试运行器')
    parser.add_argument('--timeout', type=int, default=30, help='测试超时时间（秒）')
    parser.add_argument('--test-dir', default='unit-tests', help='测试目录')
    parser.add_argument('--output', help='输出报告文件')
    parser.add_argument('--single', help='运行单个测试文件')
    
    args = parser.parse_args()
    
    # 创建测试运行器
    runner = SystematicTestRunner('.', timeout=args.timeout)
    
    if args.single:
        # 运行单个测试
        result = runner.run_single_test(args.single)
        runner.results = [result]
    else:
        # 运行所有测试
        runner.run_all_tests(args.test_dir)
    
    # 生成报告
    report_file = runner.generate_report(args.output)
    print(f"\n测试报告已保存到: {report_file}")
    
    # 打印摘要
    runner.print_summary()

if __name__ == "__main__":
    main()
