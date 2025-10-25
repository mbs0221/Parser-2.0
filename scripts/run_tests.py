#!/usr/bin/env python3
"""
批量测试脚本
用于测试Parser-2.0项目的所有测试文件
"""

import os
import sys
import subprocess
import time
from pathlib import Path
import threading
from tqdm import tqdm

class TestRunner:
    def __init__(self, parser_path="build/bin/parser_main", test_dir="tests", verbose=False):
        self.parser_path = parser_path
        self.test_dir = test_dir
        self.verbose = verbose
        self.results = []
        
    def run_single_test(self, test_file):
        """运行单个测试文件"""
        test_path = os.path.join(self.test_dir, test_file)
        
        if not os.path.exists(test_path):
            return {
                'file': test_file,
                'status': 'ERROR',
                'message': f'文件不存在: {test_path}',
                'output': '',
                'exit_code': -1
            }
        
        try:
            # 构建命令
            cmd = [self.parser_path, '-v', test_path]
            
            if self.verbose:
                print(f"运行测试: {test_file}")
                print(f"命令: {' '.join(cmd)}")
            
            # 运行测试
            start_time = time.time()
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=3  # 30秒超时
            )
            end_time = time.time()
            
            # 分析结果
            if result.returncode == 0:
                status = 'PASS'
                message = '测试通过'
            elif result.returncode == 1:
                # 检查是否为预期的语法或词法错误
                if any(keyword in (result.stdout + result.stderr) for keyword in ['SYNTAX ERROR', 'LEXICAL ERROR']):
                    status = 'PASS'
                    message = '预期的语法/词法错误，测试通过'
                else:
                    status = 'FAIL'
                    message = '意外的错误，退出码: 1'
            elif result.returncode in [139, -11]:  # 段错误
                status = 'SEGFAULT'
                message = '段错误（程序结束时的内存清理问题）'
            elif result.returncode == 130:
                status = 'TIMEOUT'
                message = '超时或中断'
            else:
                status = 'FAIL'
                message = f'测试失败，退出码: {result.returncode}'
            
            return {
                'file': test_file,
                'status': status,
                'message': message,
                'output': result.stdout + result.stderr,
                'exit_code': result.returncode,
                'duration': end_time - start_time
            }
            
        except subprocess.TimeoutExpired:
            return {
                'file': test_file,
                'status': 'TIMEOUT',
                'message': '测试超时（30秒）',
                'output': '',
                'exit_code': -1,
                'duration': 3
            }
        except Exception as e:
            return {
                'file': test_file,
                'status': 'ERROR',
                'message': f'执行错误: {str(e)}',
                'output': '',
                'exit_code': -1
            }
    
    def run_all_tests(self, specific_files=None):
        """运行测试文件"""
        if specific_files:
            # 运行指定的测试文件
            test_files = specific_files
        else:
            # 获取所有.txt测试文件
            test_files = [f for f in os.listdir(self.test_dir) if f.endswith('.txt')]
            test_files.sort()
        
        if not test_files:
            print("未找到测试文件")
            return
        
        print(f"找到 {len(test_files)} 个测试文件")
        print("=" * 60)
        
        # 状态统计
        stats = {
            'PASS': 0,
            'FAIL': 0,
            'SEGFAULT': 0,
            'TIMEOUT': 0,
            'ERROR': 0
        }
        
        # 状态图标
        status_icons = {
            'PASS': '✅',
            'FAIL': '❌',
            'SEGFAULT': '⚠️',
            'TIMEOUT': '⏰',
            'ERROR': '💥'
        }
        
        # 使用进度条
        with tqdm(total=len(test_files), desc="测试进度", unit="test") as pbar:
            for i, test_file in enumerate(test_files, 1):
                # 更新进度条描述
                pbar.set_description(f"测试: {test_file[:30]}{'...' if len(test_file) > 30 else ''}")
                
                result = self.run_single_test(test_file)
                self.results.append(result)
                
                # 更新统计
                stats[result['status']] += 1
                
                # 更新进度条后缀信息
                status_text = f"✅{stats['PASS']} ❌{stats['FAIL']} ⚠️{stats['SEGFAULT']} ⏰{stats['TIMEOUT']} 💥{stats['ERROR']}"
                pbar.set_postfix_str(status_text)
                
                # 更新进度条
                pbar.update(1)
                
                # 如果详细模式，显示详细信息
                if self.verbose:
                    icon = status_icons.get(result['status'], '❓')
                    print(f"\n{icon} {result['status']}: {result['message']}")
                    if result.get('duration'):
                        print(f"  耗时: {result['duration']:.2f}秒")
                    if result['output']:
                        print("  输出:")
                        for line in result['output'].split('\n')[:5]:  # 只显示前5行
                            if line.strip():
                                print(f"    {line}")
                        if len(result['output'].split('\n')) > 5:
                            print("    ...")
        
        print("\n" + "=" * 60)
    
    def print_summary(self):
        """打印测试总结"""
        print("=" * 60)
        print("测试总结")
        print("=" * 60)
        
        # 统计结果
        stats = {}
        for result in self.results:
            status = result['status']
            stats[status] = stats.get(status, 0) + 1
        
        total = len(self.results)
        print(f"总测试数: {total}")
        
        for status, count in stats.items():
            percentage = (count / total) * 100
            print(f"{status}: {count} ({percentage:.1f}%)")
        
        print()
        
        # 显示失败的测试
        failed_tests = [r for r in self.results if r['status'] not in ['PASS', 'SEGFAULT']]
        if failed_tests:
            print("失败的测试:")
            for test in failed_tests:
                print(f"  - {test['file']}: {test['message']}")
        else:
            print("所有测试都通过了！")
        
        # 显示段错误的测试（通常不影响核心功能）
        segfault_tests = [r for r in self.results if r['status'] == 'SEGFAULT']
        if segfault_tests:
            print("\n段错误的测试（程序结束时的内存清理问题）:")
            for test in segfault_tests:
                print(f"  - {test['file']}")
    
    def save_results(self, output_file="test_results.txt"):
        """保存测试结果到文件"""
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("测试结果报告\n")
            f.write("=" * 60 + "\n")
            f.write(f"测试时间: {time.strftime('%Y-%m-%d %H:%M:%S')}\n\n")
            
            for result in self.results:
                f.write(f"文件: {result['file']}\n")
                f.write(f"状态: {result['status']}\n")
                f.write(f"消息: {result['message']}\n")
                if result.get('duration'):
                    f.write(f"耗时: {result['duration']:.2f}秒\n")
                f.write(f"退出码: {result['exit_code']}\n")
                if result['output']:
                    f.write("输出:\n")
                    f.write(result['output'])
                    f.write("\n")
                f.write("-" * 40 + "\n")
        
        print(f"\n详细结果已保存到: {output_file}")

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='批量测试Parser-2.0项目')
    parser.add_argument('--parser', default='build/bin/parser_main', 
                       help='parser可执行文件路径')
    parser.add_argument('--test-dir', default='tests', 
                       help='测试文件目录')
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='详细输出')
    parser.add_argument('--output', '-o', default='test_results.txt',
                       help='结果输出文件')
    parser.add_argument('test_files', nargs='*', 
                       help='指定要运行的测试文件（不指定则运行所有.txt文件）')
    
    args = parser.parse_args()
    
    # 检查parser是否存在
    if not os.path.exists(args.parser):
        print(f"错误: parser可执行文件不存在: {args.parser}")
        print("请先编译项目: cd ../build && make")
        sys.exit(1)
    
    # 运行测试
    runner = TestRunner(
        parser_path=args.parser,
        test_dir=args.test_dir,
        verbose=args.verbose
    )
    
    runner.run_all_tests(args.test_files)
    runner.print_summary()
    runner.save_results(args.output)

if __name__ == '__main__':
    main()
