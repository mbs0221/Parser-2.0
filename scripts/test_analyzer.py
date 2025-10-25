#!/usr/bin/env python3
"""
测试分析器
用于分析测试结果，识别问题模式，提供修复建议
"""

import json
import re
from pathlib import Path
from typing import Dict, List, Set
from collections import defaultdict, Counter
from dataclasses import dataclass

@dataclass
class ErrorPattern:
    pattern: str
    count: int
    examples: List[str]
    category: str
    suggested_fix: str

class TestAnalyzer:
    def __init__(self, report_file: str):
        self.report_file = report_file
        self.report_data = self._load_report()
        self.error_patterns = []
        self.failed_tests = []
        
    def _load_report(self) -> Dict:
        """加载测试报告"""
        with open(self.report_file, 'r', encoding='utf-8') as f:
            return json.load(f)
    
    def analyze_errors(self) -> List[ErrorPattern]:
        """分析错误模式"""
        error_patterns = defaultdict(list)
        
        # 收集所有失败的测试
        for result in self.report_data['results']:
            if result['status'] != 'PASSED':
                self.failed_tests.append(result)
                error_msg = result.get('error_message', '')
                if error_msg:
                    # 提取错误模式
                    pattern = self._extract_error_pattern(error_msg)
                    if pattern:
                        error_patterns[pattern].append({
                            'file': result['test_file'],
                            'message': error_msg,
                            'status': result['status']
                        })
        
        # 生成错误模式报告
        patterns = []
        for pattern, examples in error_patterns.items():
            if len(examples) >= 2:  # 只关注出现多次的模式
                patterns.append(ErrorPattern(
                    pattern=pattern,
                    count=len(examples),
                    examples=[ex['file'] for ex in examples[:3]],  # 只显示前3个例子
                    category=self._categorize_error(pattern),
                    suggested_fix=self._suggest_fix(pattern)
                ))
        
        # 按出现次数排序
        patterns.sort(key=lambda x: x.count, reverse=True)
        self.error_patterns = patterns
        return patterns
    
    def _extract_error_pattern(self, error_msg: str) -> str:
        """从错误消息中提取模式"""
        # 清理错误消息
        error_msg = error_msg.strip()
        
        # 常见的错误模式
        patterns = [
            r'lexer.*error',
            r'parser.*error',
            r'syntax.*error',
            r'undefined.*variable',
            r'undefined.*function',
            r'type.*mismatch',
            r'division.*by.*zero',
            r'index.*out.*of.*range',
            r'null.*pointer',
            r'segmentation.*fault',
            r'timeout',
            r'compilation.*error'
        ]
        
        for pattern in patterns:
            if re.search(pattern, error_msg, re.IGNORECASE):
                return pattern
        
        # 如果没有匹配的模式，返回前50个字符
        return error_msg[:50] + "..." if len(error_msg) > 50 else error_msg
    
    def _categorize_error(self, pattern: str) -> str:
        """对错误进行分类"""
        if 'lexer' in pattern.lower():
            return '词法分析错误'
        elif 'parser' in pattern.lower() or 'syntax' in pattern.lower():
            return '语法分析错误'
        elif 'undefined' in pattern.lower():
            return '未定义错误'
        elif 'type' in pattern.lower():
            return '类型错误'
        elif 'timeout' in pattern.lower():
            return '超时错误'
        elif 'segmentation' in pattern.lower() or 'null' in pattern.lower():
            return '运行时错误'
        else:
            return '其他错误'
    
    def _suggest_fix(self, pattern: str) -> str:
        """根据错误模式提供修复建议"""
        suggestions = {
            'lexer.*error': '检查词法分析器实现，确保正确识别所有token类型',
            'parser.*error': '检查语法分析器实现，确保语法规则正确',
            'syntax.*error': '检查语法规则定义，确保符合语言规范',
            'undefined.*variable': '检查变量声明和作用域管理',
            'undefined.*function': '检查函数定义和调用机制',
            'type.*mismatch': '检查类型系统和类型转换实现',
            'division.*by.*zero': '添加除零检查和异常处理',
            'index.*out.*of.*range': '添加数组边界检查',
            'null.*pointer': '添加空指针检查和防护',
            'segmentation.*fault': '检查内存管理和指针操作',
            'timeout': '优化算法复杂度或增加超时时间',
            'compilation.*error': '检查编译配置和依赖项'
        }
        
        for p, suggestion in suggestions.items():
            if re.search(p, pattern, re.IGNORECASE):
                return suggestion
        
        return '需要进一步分析具体错误原因'
    
    def generate_analysis_report(self) -> str:
        """生成分析报告"""
        if not self.error_patterns:
            self.analyze_errors()
        
        report = []
        report.append("# 测试分析报告")
        report.append(f"生成时间: {self.report_data['timestamp']}")
        report.append(f"总测试数: {self.report_data['total_tests']}")
        report.append("")
        
        # 总体统计
        stats = self.report_data['statistics']
        report.append("## 总体统计")
        for status, count in stats.items():
            percentage = count / self.report_data['total_tests'] * 100
            report.append(f"- {status}: {count} ({percentage:.1f}%)")
        report.append("")
        
        # 按类别统计
        report.append("## 按类别统计")
        category_stats = self.report_data['category_statistics']
        for category, stats in category_stats.items():
            report.append(f"### {category}")
            for status, count in stats.items():
                report.append(f"- {status}: {count}")
        report.append("")
        
        # 错误模式分析
        if self.error_patterns:
            report.append("## 错误模式分析")
            report.append("### 高频错误模式")
            
            for pattern in self.error_patterns[:10]:  # 显示前10个
                report.append(f"#### {pattern.pattern}")
                report.append(f"- 出现次数: {pattern.count}")
                report.append(f"- 错误类别: {pattern.category}")
                report.append(f"- 修复建议: {pattern.suggested_fix}")
                report.append(f"- 示例文件: {', '.join(pattern.examples)}")
                report.append("")
        
        # 失败测试列表
        report.append("## 失败测试详情")
        for test in self.failed_tests[:20]:  # 显示前20个
            report.append(f"### {test['test_file']}")
            report.append(f"- 状态: {test['status']}")
            report.append(f"- 执行时间: {test['execution_time']:.2f}s")
            if test['error_message']:
                report.append(f"- 错误信息: {test['error_message'][:200]}...")
            report.append("")
        
        return "\n".join(report)
    
    def get_priority_fixes(self) -> List[Dict]:
        """获取优先修复建议"""
        if not self.error_patterns:
            self.analyze_errors()
        
        priority_fixes = []
        
        # 按影响范围排序
        for pattern in self.error_patterns:
            priority_fixes.append({
                'pattern': pattern.pattern,
                'impact': pattern.count,
                'category': pattern.category,
                'suggestion': pattern.suggested_fix,
                'examples': pattern.examples
            })
        
        return priority_fixes

def main():
    """主函数"""
    import argparse
    
    parser = argparse.ArgumentParser(description='测试分析器')
    parser.add_argument('report_file', help='测试报告文件')
    parser.add_argument('--output', help='输出分析报告文件')
    
    args = parser.parse_args()
    
    # 创建分析器
    analyzer = TestAnalyzer(args.report_file)
    
    # 生成分析报告
    report = analyzer.generate_analysis_report()
    
    # 保存报告
    if args.output:
        with open(args.output, 'w', encoding='utf-8') as f:
            f.write(report)
        print(f"分析报告已保存到: {args.output}")
    else:
        print(report)
    
    # 显示优先修复建议
    print("\n" + "="*60)
    print("优先修复建议")
    print("="*60)
    
    priority_fixes = analyzer.get_priority_fixes()
    for i, fix in enumerate(priority_fixes[:5], 1):
        print(f"{i}. {fix['pattern']}")
        print(f"   影响: {fix['impact']} 个测试")
        print(f"   类别: {fix['category']}")
        print(f"   建议: {fix['suggestion']}")
        print()

if __name__ == "__main__":
    main()
