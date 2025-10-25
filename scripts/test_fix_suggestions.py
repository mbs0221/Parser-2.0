#!/usr/bin/env python3
"""
测试修复建议生成器
根据测试结果生成具体的修复建议和行动计划
"""

import json
import re
from pathlib import Path
from typing import Dict, List, Set
from collections import defaultdict
from dataclasses import dataclass

@dataclass
class FixSuggestion:
    category: str
    priority: int
    description: str
    affected_tests: List[str]
    implementation_steps: List[str]
    estimated_effort: str

class TestFixSuggestions:
    def __init__(self, report_file: str):
        self.report_file = report_file
        self.report_data = self._load_report()
        self.suggestions = []
        
    def _load_report(self) -> Dict:
        """加载测试报告"""
        with open(self.report_file, 'r', encoding='utf-8') as f:
            return json.load(f)
    
    def generate_suggestions(self) -> List[FixSuggestion]:
        """生成修复建议"""
        suggestions = []
        
        # 分析失败测试
        failed_tests = [r for r in self.report_data['results'] if r['status'] != 'PASSED']
        
        # 按错误类型分组
        error_groups = defaultdict(list)
        for test in failed_tests:
            error_type = self._classify_error(test)
            error_groups[error_type].append(test)
        
        # 为每种错误类型生成建议
        for error_type, tests in error_groups.items():
            suggestion = self._create_suggestion(error_type, tests)
            if suggestion:
                suggestions.append(suggestion)
        
        # 按优先级排序
        suggestions.sort(key=lambda x: x.priority)
        self.suggestions = suggestions
        return suggestions
    
    def _classify_error(self, test_result: Dict) -> str:
        """分类错误类型"""
        status = test_result['status']
        error_msg = test_result.get('error_message', '').lower()
        
        if status == 'LEXER_ERROR':
            return 'lexer_issues'
        elif status == 'PARSER_ERROR':
            return 'parser_issues'
        elif status == 'INTERPRETER_ERROR':
            if 'undefined' in error_msg:
                return 'undefined_symbols'
            elif 'type' in error_msg:
                return 'type_system'
            elif 'null' in error_msg or 'segmentation' in error_msg:
                return 'runtime_errors'
            else:
                return 'interpreter_issues'
        elif status == 'TIMEOUT':
            return 'performance_issues'
        else:
            return 'other_issues'
    
    def _create_suggestion(self, error_type: str, tests: List[Dict]) -> FixSuggestion:
        """为特定错误类型创建修复建议"""
        if error_type == 'lexer_issues':
            return FixSuggestion(
                category='词法分析器',
                priority=1,
                description='修复词法分析器问题',
                affected_tests=[t['test_file'] for t in tests],
                implementation_steps=[
                    '检查token定义是否完整',
                    '验证关键字识别逻辑',
                    '检查数字和字符串字面量解析',
                    '测试特殊字符和运算符处理',
                    '添加错误恢复机制'
                ],
                estimated_effort='2-3天'
            )
        
        elif error_type == 'parser_issues':
            return FixSuggestion(
                category='语法分析器',
                priority=1,
                description='修复语法分析器问题',
                affected_tests=[t['test_file'] for t in tests],
                implementation_steps=[
                    '检查语法规则定义',
                    '验证AST节点创建',
                    '测试运算符优先级',
                    '检查表达式解析',
                    '添加语法错误恢复'
                ],
                estimated_effort='3-4天'
            )
        
        elif error_type == 'undefined_symbols':
            return FixSuggestion(
                category='符号表管理',
                priority=2,
                description='修复未定义符号问题',
                affected_tests=[t['test_file'] for t in tests],
                implementation_steps=[
                    '检查作用域管理实现',
                    '验证变量声明处理',
                    '测试函数定义和调用',
                    '检查符号查找逻辑',
                    '添加更好的错误信息'
                ],
                estimated_effort='2-3天'
            )
        
        elif error_type == 'type_system':
            return FixSuggestion(
                category='类型系统',
                priority=2,
                description='修复类型系统问题',
                affected_tests=[t['test_file'] for t in tests],
                implementation_steps=[
                    '检查类型推断逻辑',
                    '验证类型转换规则',
                    '测试类型检查机制',
                    '检查内置类型支持',
                    '添加类型错误处理'
                ],
                estimated_effort='3-5天'
            )
        
        elif error_type == 'runtime_errors':
            return FixSuggestion(
                category='运行时系统',
                priority=3,
                description='修复运行时错误',
                affected_tests=[t['test_file'] for t in tests],
                implementation_steps=[
                    '检查内存管理',
                    '验证指针操作',
                    '测试异常处理',
                    '检查数组边界',
                    '添加安全检查'
                ],
                estimated_effort='2-4天'
            )
        
        elif error_type == 'performance_issues':
            return FixSuggestion(
                category='性能优化',
                priority=4,
                description='解决性能问题',
                affected_tests=[t['test_file'] for t in tests],
                implementation_steps=[
                    '分析算法复杂度',
                    '优化数据结构',
                    '减少不必要的计算',
                    '添加缓存机制',
                    '考虑并行处理'
                ],
                estimated_effort='1-2天'
            )
        
        return None
    
    def generate_action_plan(self) -> str:
        """生成行动计划"""
        if not self.suggestions:
            self.generate_suggestions()
        
        plan = []
        plan.append("# 测试修复行动计划")
        plan.append("")
        
        # 总体概览
        total_tests = self.report_data['total_tests']
        failed_tests = len([r for r in self.report_data['results'] if r['status'] != 'PASSED'])
        plan.append(f"## 总体概览")
        plan.append(f"- 总测试数: {total_tests}")
        plan.append(f"- 失败测试数: {failed_tests}")
        plan.append(f"- 成功率: {(total_tests - failed_tests) / total_tests * 100:.1f}%")
        plan.append("")
        
        # 修复建议
        plan.append("## 修复建议")
        for i, suggestion in enumerate(self.suggestions, 1):
            plan.append(f"### {i}. {suggestion.description}")
            plan.append(f"**类别**: {suggestion.category}")
            plan.append(f"**优先级**: {suggestion.priority}")
            plan.append(f"**影响测试数**: {len(suggestion.affected_tests)}")
            plan.append(f"**预计工作量**: {suggestion.estimated_effort}")
            plan.append("")
            plan.append("**实施步骤**:")
            for step in suggestion.implementation_steps:
                plan.append(f"- {step}")
            plan.append("")
            plan.append("**影响的测试文件**:")
            for test in suggestion.affected_tests[:5]:  # 只显示前5个
                plan.append(f"- {test}")
            if len(suggestion.affected_tests) > 5:
                plan.append(f"- ... 还有 {len(suggestion.affected_tests) - 5} 个测试")
            plan.append("")
        
        # 实施时间线
        plan.append("## 建议实施时间线")
        plan.append("")
        plan.append("### 第一阶段 (高优先级)")
        high_priority = [s for s in self.suggestions if s.priority <= 2]
        for suggestion in high_priority:
            plan.append(f"- {suggestion.description} ({suggestion.estimated_effort})")
        plan.append("")
        
        plan.append("### 第二阶段 (中优先级)")
        medium_priority = [s for s in self.suggestions if s.priority == 3]
        for suggestion in medium_priority:
            plan.append(f"- {suggestion.description} ({suggestion.estimated_effort})")
        plan.append("")
        
        plan.append("### 第三阶段 (低优先级)")
        low_priority = [s for s in self.suggestions if s.priority >= 4]
        for suggestion in low_priority:
            plan.append(f"- {suggestion.description} ({suggestion.estimated_effort})")
        plan.append("")
        
        # 测试策略
        plan.append("## 测试策略建议")
        plan.append("")
        plan.append("### 1. 回归测试")
        plan.append("- 每次修复后运行相关测试")
        plan.append("- 确保修复不会引入新问题")
        plan.append("- 使用持续集成自动测试")
        plan.append("")
        
        plan.append("### 2. 分类测试")
        plan.append("- 按功能模块分组测试")
        plan.append("- 优先修复影响面大的问题")
        plan.append("- 逐步提高测试覆盖率")
        plan.append("")
        
        plan.append("### 3. 性能测试")
        plan.append("- 监控测试执行时间")
        plan.append("- 识别性能瓶颈")
        plan.append("- 优化慢速测试")
        plan.append("")
        
        return "\n".join(plan)
    
    def get_quick_wins(self) -> List[Dict]:
        """获取快速修复建议"""
        if not self.suggestions:
            self.generate_suggestions()
        
        quick_wins = []
        for suggestion in self.suggestions:
            if suggestion.priority <= 2 and len(suggestion.affected_tests) >= 5:
                quick_wins.append({
                    'description': suggestion.description,
                    'impact': len(suggestion.affected_tests),
                    'effort': suggestion.estimated_effort,
                    'steps': suggestion.implementation_steps[:3]  # 只显示前3步
                })
        
        return quick_wins

def main():
    """主函数"""
    import argparse
    
    parser = argparse.ArgumentParser(description='测试修复建议生成器')
    parser.add_argument('report_file', help='测试报告文件')
    parser.add_argument('--output', help='输出行动计划文件')
    parser.add_argument('--quick-wins', action='store_true', help='显示快速修复建议')
    
    args = parser.parse_args()
    
    # 创建建议生成器
    generator = TestFixSuggestions(args.report_file)
    
    if args.quick_wins:
        # 显示快速修复建议
        quick_wins = generator.get_quick_wins()
        print("快速修复建议:")
        print("=" * 50)
        for i, win in enumerate(quick_wins, 1):
            print(f"{i}. {win['description']}")
            print(f"   影响: {win['impact']} 个测试")
            print(f"   工作量: {win['effort']}")
            print(f"   关键步骤: {', '.join(win['steps'])}")
            print()
    else:
        # 生成完整行动计划
        plan = generator.generate_action_plan()
        
        if args.output:
            with open(args.output, 'w', encoding='utf-8') as f:
                f.write(plan)
            print(f"行动计划已保存到: {args.output}")
        else:
            print(plan)

if __name__ == "__main__":
    main()
