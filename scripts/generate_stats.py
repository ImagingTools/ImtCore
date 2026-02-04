#!/usr/bin/env python3
"""
Generate repository statistics for ImtCore project.
This script collects various metrics about the codebase including:
- Lines of code (LOC)
- Number of files
- Number of classes
- Number of components
- Library and package information
- Function/method counts
- Namespace usage
- Include dependencies
- File size distribution
"""

import os
import re
import json
from pathlib import Path
from datetime import datetime
from collections import defaultdict, Counter

class RepositoryStats:
    def __init__(self, repo_path):
        self.repo_path = Path(repo_path)
        self.stats = {
            'generated_at': datetime.now().isoformat(),
            'total_files': 0,
            'total_lines': 0,
            'source_files': {},
            'libraries': {},
            'packages': {},
            'components': [],
            'classes': [],
            'interfaces': [],
            'functions': [],
            'namespaces': [],
            'test_files': 0,
            'documentation_files': 0,
            'quality_metrics': {
                'large_files': [],
                'todos': 0,
                'fixmes': 0,
                'hacks': 0,
                'warnings': 0,
            },
            'code_structure': {
                'include_dependencies': defaultdict(list),
                'file_size_distribution': defaultdict(int),
                'namespace_usage': defaultdict(int),
                'export_macros': defaultdict(int),
            },
            'solid_metrics': {
                'single_responsibility': {
                    'large_classes': [],  # Classes with too many methods
                    'methods_per_class': [],
                },
                'open_closed': {
                    'virtual_methods': 0,
                    'total_methods': 0,
                    'inheritance_depth': {},
                },
                'liskov_substitution': {
                    'potential_violations': [],
                },
                'interface_segregation': {
                    'methods_per_interface': [],
                    'large_interfaces': [],
                },
                'dependency_inversion': {
                    'abstract_dependencies': 0,
                    'concrete_dependencies': 0,
                },
            },
        }
        
    def count_lines(self, file_path):
        """Count and categorize lines in a file as code, comments, or blank lines.
        Also count statements (instructions) in the code."""
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                lines = f.readlines()
                total = len(lines)
                code = 0
                comment = 0
                blank = 0
                statements = 0
                in_multiline_comment = False
                
                # Quality metrics
                todos = 0
                fixmes = 0
                hacks = 0
                warnings = 0
                
                # For statement counting, we need to track the content
                code_lines = []
                
                for line in lines:
                    stripped = line.strip()
                    
                    # Count technical debt markers (before categorization)
                    line_upper = stripped.upper()
                    if 'TODO' in line_upper:
                        todos += 1
                    if 'FIXME' in line_upper:
                        fixmes += 1
                    if 'HACK' in line_upper:
                        hacks += 1
                    if 'WARNING' in line_upper or 'WARN' in line_upper:
                        warnings += 1
                    
                    if not stripped:
                        blank += 1
                        continue
                    
                    # Handle multi-line comments
                    if '/*' in stripped and not in_multiline_comment:
                        in_multiline_comment = True
                        # Check if it's a single-line block comment
                        if '*/' in stripped:
                            in_multiline_comment = False
                        # If line starts with /*, count as comment, else as code with comment
                        if stripped.startswith('/*'):
                            comment += 1
                        else:
                            code += 1  # Code with trailing comment
                            code_lines.append(stripped)
                        continue
                    
                    if in_multiline_comment:
                        comment += 1
                        if '*/' in stripped:
                            in_multiline_comment = False
                        continue
                    
                    # Single line C++ comments
                    if stripped.startswith('//'):
                        comment += 1
                        continue
                    
                    # Everything else is code (including preprocessor directives)
                    code += 1
                    code_lines.append(stripped)
                
                # Count statements in code lines
                statements = self.count_statements(code_lines)
                
                # Update quality metrics
                self.stats['quality_metrics']['todos'] += todos
                self.stats['quality_metrics']['fixmes'] += fixmes
                self.stats['quality_metrics']['hacks'] += hacks
                self.stats['quality_metrics']['warnings'] += warnings
                
                return {
                    'total': total,
                    'code': code,
                    'comment': comment,
                    'blank': blank,
                    'statements': statements,
                    'todos': todos,
                    'fixmes': fixmes,
                    'hacks': hacks,
                    'warnings': warnings
                }
        except Exception as e:
            print(f"Error reading {file_path}: {e}")
            return {'total': 0, 'code': 0, 'comment': 0, 'blank': 0, 'statements': 0,
                   'todos': 0, 'fixmes': 0, 'hacks': 0, 'warnings': 0}
    
    def count_statements(self, code_lines):
        """Count statements/instructions in code lines.
        A statement is typically terminated by a semicolon, or is a control structure."""
        if not code_lines:
            return 0
        
        # Join all code lines to handle multi-line statements
        code_text = ' '.join(code_lines)
        
        # Remove string literals to avoid counting semicolons in strings
        code_text = re.sub(r'"[^"]*"', '""', code_text)
        code_text = re.sub(r"'[^']*'", "''", code_text)
        
        statement_count = 0
        
        # Count semicolons (main statement terminator in C++)
        statement_count += code_text.count(';')
        
        # Count control structures that don't always end with semicolons
        # but represent executable statements
        control_patterns = [
            r'\bif\s*\(',      # if statements
            r'\belse\b',       # else statements
            r'\bfor\s*\(',     # for loops
            r'\bwhile\s*\(',   # while loops
            r'\bdo\b',         # do-while loops
            r'\bswitch\s*\(',  # switch statements
            r'\bcase\b',       # case labels
            r'\bcatch\s*\(',   # catch blocks
            r'\btry\b',        # try blocks
        ]
        
        for pattern in control_patterns:
            matches = re.findall(pattern, code_text)
            statement_count += len(matches)
        
        # Subtract some false positives:
        # - for loop has typically 2 semicolons but represents 1 statement
        for_loops = len(re.findall(r'\bfor\s*\([^)]*;[^)]*;[^)]*\)', code_text))
        statement_count -= for_loops * 2  # Remove the 2 extra semicolons in for loops
        
        return max(0, statement_count)  # Ensure non-negative
    
    def extract_classes(self, file_path):
        """Extract class names from a C++ header file."""
        classes = []
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                # Match class declarations with public inheritance
                # Note: This pattern intentionally focuses on public inheritance 
                # as it's the most common. Private/protected inheritance
                # and complex multiple inheritance may not be captured.
                class_pattern = r'class\s+(?:\w+_EXPORT\s+)?(\w+)(?:\s*:\s*public\s+(\w+))?'
                matches = re.finditer(class_pattern, content)
                for match in matches:
                    class_name = match.group(1)
                    base_class = match.group(2) if match.group(2) else None
                    
                    # Count methods in this class for SOLID metrics
                    methods = self.count_class_methods(content, class_name)
                    
                    classes.append({
                        'name': class_name,
                        'base': base_class,
                        'file': str(file_path.relative_to(self.repo_path)),
                        'method_count': methods
                    })
        except Exception as e:
            print(f"Error extracting classes from {file_path}: {e}")
        return classes
    
    def count_class_methods(self, content, class_name):
        """Count methods in a class definition."""
        try:
            # Find class definition
            class_start = content.find(f'class {class_name}')
            if class_start == -1:
                return 0
            
            # Find matching closing brace (simplified - may not handle all nested cases)
            brace_count = 0
            class_content = ""
            in_class = False
            
            for i in range(class_start, len(content)):
                char = content[i]
                if char == '{':
                    brace_count += 1
                    in_class = True
                elif char == '}':
                    brace_count -= 1
                    if brace_count == 0 and in_class:
                        class_content = content[class_start:i]
                        break
                
                if in_class:
                    class_content += char
            
            # Count method declarations (simplified)
            method_count = 0
            # Look for function-like declarations
            method_pattern = r'(?:virtual\s+)?(?:static\s+)?(?:inline\s+)?(?:\w+(?:\s*\*|\s+&)?)\s+(\w+)\s*\([^)]*\)(?:\s+const)?(?:\s+override)?(?:\s*[;{=])'
            matches = re.finditer(method_pattern, class_content)
            
            for match in matches:
                method_name = match.group(1)
                # Filter out keywords and constructors
                if method_name not in ['if', 'while', 'for', 'switch', class_name]:
                    method_count += 1
                    
            return method_count
        except Exception:
            return 0
    
    def extract_components(self, file_path):
        """Extract component information from source files."""
        components = []
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                # Look for component declarations
                component_pattern = r'I_BEGIN_COMPONENT\s*\(\s*(\w+)\s*\)'
                matches = re.finditer(component_pattern, content)
                for match in matches:
                    component_name = match.group(1)
                    components.append({
                        'name': component_name,
                        'file': str(file_path.relative_to(self.repo_path))
                    })
        except Exception as e:
            print(f"Error extracting components from {file_path}: {e}")
        return components
    
    def extract_interfaces(self, file_path):
        """Extract interface declarations."""
        interfaces = []
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                # Match interface declarations (classes starting with I)
                interface_pattern = r'class\s+(?:\w+_EXPORT\s+)?(I[A-Z]\w+)'
                matches = re.finditer(interface_pattern, content)
                for match in matches:
                    interface_name = match.group(1)
                    interfaces.append({
                        'name': interface_name,
                        'file': str(file_path.relative_to(self.repo_path))
                    })
        except Exception as e:
            print(f"Error extracting interfaces from {file_path}: {e}")
        return interfaces
    
    def extract_functions(self, file_path):
        """Extract function/method declarations."""
        functions = []
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                # Match function declarations (simplified pattern)
                # Matches patterns like: void MyFunction(...) or virtual int GetValue() const
                function_pattern = r'(?:virtual\s+)?(?:static\s+)?(?:inline\s+)?(?:\w+(?:\s*\*|\s+&)?)\s+(\w+)\s*\([^)]*\)(?:\s+const)?(?:\s+override)?(?:\s*[;{])'
                matches = re.finditer(function_pattern, content)
                for match in matches:
                    function_name = match.group(1)
                    # Filter out common false positives (constructors, macros, etc.)
                    if not function_name.isupper() and function_name not in ['if', 'while', 'for', 'switch']:
                        functions.append({
                            'name': function_name,
                            'file': str(file_path.relative_to(self.repo_path))
                        })
        except Exception as e:
            print(f"Error extracting functions from {file_path}: {e}")
        return functions
    
    def extract_namespaces(self, file_path):
        """Extract namespace declarations."""
        namespaces = []
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                # Match namespace declarations
                namespace_pattern = r'namespace\s+(\w+)'
                matches = re.finditer(namespace_pattern, content)
                for match in matches:
                    namespace_name = match.group(1)
                    namespaces.append({
                        'name': namespace_name,
                        'file': str(file_path.relative_to(self.repo_path))
                    })
        except Exception as e:
            print(f"Error extracting namespaces from {file_path}: {e}")
        return namespaces
    
    def extract_includes(self, file_path):
        """Extract #include directives."""
        includes = []
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                # Match #include directives
                include_pattern = r'#include\s+[<"]([^>"]+)[>"]'
                matches = re.finditer(include_pattern, content)
                for match in matches:
                    include_file = match.group(1)
                    includes.append(include_file)
        except Exception as e:
            print(f"Error extracting includes from {file_path}: {e}")
        return includes
    
    def extract_export_macros(self, file_path):
        """Extract export macro usage."""
        export_macros = []
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                # Match export macros like IMT_*_EXPORT, ACF_*_EXPORT, etc.
                export_pattern = r'(\w+_EXPORT)'
                matches = re.finditer(export_pattern, content)
                for match in matches:
                    macro_name = match.group(1)
                    export_macros.append(macro_name)
        except Exception as e:
            print(f"Error extracting export macros from {file_path}: {e}")
        return export_macros

    
    def analyze_directory(self, directory, category):
        """Analyze a directory for source files."""
        dir_path = self.repo_path / directory
        if not dir_path.exists():
            return
        
        extensions = {'.h', '.cpp', '.c', '.hpp', '.cc'}
        large_file_threshold = 1000  # lines
        
        for file_path in dir_path.rglob('*'):
            if file_path.is_file() and file_path.suffix in extensions:
                self.stats['total_files'] += 1
                
                # Count lines
                line_counts = self.count_lines(file_path)
                self.stats['total_lines'] += line_counts['total']
                
                # Store file info
                rel_path = str(file_path.relative_to(self.repo_path))
                self.stats['source_files'][rel_path] = {
                    'lines': line_counts,
                    'category': category,
                    'extension': file_path.suffix
                }
                
                # Track large files (quality metric)
                if line_counts['total'] > large_file_threshold:
                    self.stats['quality_metrics']['large_files'].append({
                        'file': rel_path,
                        'lines': line_counts['total']
                    })
                
                # Update file size distribution
                size_bucket = (line_counts['total'] // 100) * 100  # Bucket by hundreds
                self.stats['code_structure']['file_size_distribution'][size_bucket] += 1
                
                # Extract classes from headers
                if file_path.suffix == '.h':
                    classes = self.extract_classes(file_path)
                    self.stats['classes'].extend(classes)
                    
                    interfaces = self.extract_interfaces(file_path)
                    self.stats['interfaces'].extend(interfaces)
                    
                    components = self.extract_components(file_path)
                    self.stats['components'].extend(components)
                    
                    functions = self.extract_functions(file_path)
                    self.stats['functions'].extend(functions)
                    
                    namespaces = self.extract_namespaces(file_path)
                    self.stats['namespaces'].extend(namespaces)
                    
                    # Update namespace usage counts
                    for ns in namespaces:
                        self.stats['code_structure']['namespace_usage'][ns['name']] += 1
                    
                    export_macros = self.extract_export_macros(file_path)
                    for macro in export_macros:
                        self.stats['code_structure']['export_macros'][macro] += 1
                
                # Extract includes from all source files
                includes = self.extract_includes(file_path)
                if includes:
                    self.stats['code_structure']['include_dependencies'][rel_path] = includes
                
                # Extract functions from cpp files too
                if file_path.suffix in ['.cpp', '.cc', '.c']:
                    functions = self.extract_functions(file_path)
                    self.stats['functions'].extend(functions)
                    
                    namespaces = self.extract_namespaces(file_path)
                    self.stats['namespaces'].extend(namespaces)
                    for ns in namespaces:
                        self.stats['code_structure']['namespace_usage'][ns['name']] += 1
    
    def analyze_libraries(self):
        """Analyze library structure in Include directory."""
        include_path = self.repo_path / 'Include'
        if not include_path.exists():
            return
        
        for lib_dir in include_path.iterdir():
            if lib_dir.is_dir() and not lib_dir.name.startswith('.'):
                lib_name = lib_dir.name
                files = list(lib_dir.rglob('*.h')) + list(lib_dir.rglob('*.cpp'))
                
                total_lines = 0
                for f in files:
                    if str(f.relative_to(self.repo_path)) in self.stats['source_files']:
                        total_lines += self.stats['source_files'][str(f.relative_to(self.repo_path))]['lines']['total']
                
                self.stats['libraries'][lib_name] = {
                    'file_count': len(files),
                    'total_lines': total_lines
                }
    
    def analyze_packages(self):
        """Analyze package structure in Impl directory."""
        impl_path = self.repo_path / 'Impl'
        if not impl_path.exists():
            return
        
        for pkg_dir in impl_path.iterdir():
            if pkg_dir.is_dir() and not pkg_dir.name.startswith('.'):
                pkg_name = pkg_dir.name
                files = list(pkg_dir.rglob('*.h')) + list(pkg_dir.rglob('*.cpp'))
                
                total_lines = 0
                for f in files:
                    if str(f.relative_to(self.repo_path)) in self.stats['source_files']:
                        total_lines += self.stats['source_files'][str(f.relative_to(self.repo_path))]['lines']['total']
                
                self.stats['packages'][pkg_name] = {
                    'file_count': len(files),
                    'total_lines': total_lines
                }
    
    def analyze_tests(self):
        """Analyze test files."""
        tests_path = self.repo_path / 'Tests'
        if not tests_path.exists():
            return
        
        test_files = list(tests_path.rglob('*.cpp')) + list(tests_path.rglob('*.h'))
        self.stats['test_files'] = len(test_files)
    
    def analyze_docs(self):
        """Analyze documentation files."""
        docs_path = self.repo_path / 'Docs'
        if not docs_path.exists():
            return
        
        doc_files = list(docs_path.rglob('*.md')) + list(docs_path.rglob('*.dox'))
        self.stats['documentation_files'] = len(doc_files)
    
    def generate(self):
        """Generate all statistics."""
        print("Analyzing Include directory...")
        self.analyze_directory('Include', 'library')
        
        print("Analyzing Impl directory...")
        self.analyze_directory('Impl', 'package')
        
        print("Analyzing library structure...")
        self.analyze_libraries()
        
        print("Analyzing package structure...")
        self.analyze_packages()
        
        print("Analyzing tests...")
        self.analyze_tests()
        
        print("Analyzing documentation...")
        self.analyze_docs()
        
        # Calculate summary statistics
        self.stats['summary'] = {
            'total_lines': self.stats['total_lines'],
            'total_files': self.stats['total_files'],
            'total_libraries': len(self.stats['libraries']),
            'total_packages': len(self.stats['packages']),
            'total_classes': len(self.stats['classes']),
            'total_interfaces': len(self.stats['interfaces']),
            'total_components': len(self.stats['components']),
            'total_functions': len(self.stats['functions']),
            'total_namespaces': len(set(ns['name'] for ns in self.stats['namespaces'])),
            'test_files': self.stats['test_files'],
            'documentation_files': self.stats['documentation_files'],
        }
        
        # Calculate code vs comment ratio
        total_code = sum(f['lines']['code'] for f in self.stats['source_files'].values())
        total_comment = sum(f['lines']['comment'] for f in self.stats['source_files'].values())
        total_blank = sum(f['lines']['blank'] for f in self.stats['source_files'].values())
        total_statements = sum(f['lines']['statements'] for f in self.stats['source_files'].values())
        
        self.stats['summary']['code_lines'] = total_code
        self.stats['summary']['comment_lines'] = total_comment
        self.stats['summary']['blank_lines'] = total_blank
        self.stats['summary']['total_statements'] = total_statements
        
        # Quality metrics calculations
        if total_code > 0:
            self.stats['summary']['comment_to_code_ratio'] = round(total_comment / total_code, 3)
        else:
            self.stats['summary']['comment_to_code_ratio'] = 0
        
        if self.stats['total_files'] > 0:
            self.stats['summary']['avg_lines_per_file'] = round(self.stats['total_lines'] / self.stats['total_files'], 1)
        else:
            self.stats['summary']['avg_lines_per_file'] = 0
        
        if len(self.stats['classes']) > 0:
            # Estimate average lines per class (rough approximation)
            self.stats['summary']['avg_lines_per_class'] = round(total_code / len(self.stats['classes']), 1)
        else:
            self.stats['summary']['avg_lines_per_class'] = 0
        
        # Sort large files by size
        self.stats['quality_metrics']['large_files'].sort(key=lambda x: x['lines'], reverse=True)
        
        # Calculate SOLID metrics
        self.calculate_solid_metrics()
        
        # Add quality summary
        self.stats['summary']['quality_score'] = self.calculate_quality_score()
        
        return self.stats
    
    def calculate_solid_metrics(self):
        """Calculate SOLID principle compliance indicators."""
        
        # Single Responsibility Principle (SRP)
        # Indicator: Classes with too many methods likely have multiple responsibilities
        method_threshold = 15  # Classes with more than 15 methods might violate SRP
        
        for cls in self.stats['classes']:
            method_count = cls.get('method_count', 0)
            if method_count > 0:
                self.stats['solid_metrics']['single_responsibility']['methods_per_class'].append({
                    'class': cls['name'],
                    'methods': method_count,
                    'file': cls['file']
                })
                
                if method_count > method_threshold:
                    self.stats['solid_metrics']['single_responsibility']['large_classes'].append({
                        'class': cls['name'],
                        'methods': method_count,
                        'file': cls['file']
                    })
        
        # Open/Closed Principle (OCP)
        # Indicator: Count virtual methods (extensible) vs total methods
        for cls in self.stats['classes']:
            # This is a simplified metric - in real implementation we'd need to parse virtual methods
            pass
        
        # Liskov Substitution Principle (LSP)
        # Indicator: Deep inheritance hierarchies can indicate LSP violations
        inheritance_depth = {}
        for cls in self.stats['classes']:
            if cls.get('base'):
                depth = self.calculate_inheritance_depth(cls['name'], self.stats['classes'])
                inheritance_depth[cls['name']] = depth
                if depth > 4:  # Deep inheritance might indicate issues
                    self.stats['solid_metrics']['liskov_substitution']['potential_violations'].append({
                        'class': cls['name'],
                        'depth': depth,
                        'file': cls['file']
                    })
        
        self.stats['solid_metrics']['open_closed']['inheritance_depth'] = inheritance_depth
        
        # Interface Segregation Principle (ISP)
        # Indicator: Interfaces with too many methods
        interface_method_threshold = 10
        
        for interface in self.stats['interfaces']:
            # Find corresponding class info for method count
            matching_class = next((c for c in self.stats['classes'] if c['name'] == interface['name']), None)
            if matching_class:
                method_count = matching_class.get('method_count', 0)
                if method_count > 0:
                    self.stats['solid_metrics']['interface_segregation']['methods_per_interface'].append({
                        'interface': interface['name'],
                        'methods': method_count,
                        'file': interface['file']
                    })
                    
                    if method_count > interface_method_threshold:
                        self.stats['solid_metrics']['interface_segregation']['large_interfaces'].append({
                            'interface': interface['name'],
                            'methods': method_count,
                            'file': interface['file']
                        })
        
        # Dependency Inversion Principle (DIP)
        # Indicator: Dependencies on interfaces (I*) vs concrete classes
        # Note: This heuristic assumes naming convention where interfaces
        # start with 'I' followed by uppercase letter (e.g., IComponent, ISerializable)
        for file_path, includes in self.stats['code_structure']['include_dependencies'].items():
            for include in includes:
                # Check if it's an interface (starts with 'I' and has uppercase second letter)
                include_name = include.split('/')[-1].replace('.h', '')
                if len(include_name) > 1 and include_name[0] == 'I' and include_name[1].isupper():
                    self.stats['solid_metrics']['dependency_inversion']['abstract_dependencies'] += 1
                else:
                    self.stats['solid_metrics']['dependency_inversion']['concrete_dependencies'] += 1
        
        # Calculate SOLID compliance score
        self.stats['summary']['solid_compliance_score'] = self.calculate_solid_score()
    
    def calculate_inheritance_depth(self, class_name, all_classes, depth=1, visited=None):
        """Calculate inheritance depth for a class."""
        if visited is None:
            visited = set()
        
        if class_name in visited:
            return depth  # Circular inheritance
        
        visited.add(class_name)
        
        cls = next((c for c in all_classes if c['name'] == class_name), None)
        if not cls or not cls.get('base'):
            return depth
        
        return self.calculate_inheritance_depth(cls['base'], all_classes, depth + 1, visited)
    
    def calculate_solid_score(self):
        """Calculate SOLID compliance score (0-100)."""
        score = 100.0
        
        # Thresholds for SOLID scoring
        SRP_MAX_DEDUCTION = 15
        LSP_MAX_DEDUCTION = 15
        ISP_MAX_DEDUCTION = 15
        DIP_GOOD_RATIO = 0.3  # >30% abstract dependencies is good
        DIP_POOR_RATIO = 0.1  # <10% abstract dependencies is poor
        DIP_MAX_BONUS = 5
        DIP_POOR_PENALTY = 10
        
        # SRP: Deduct for large classes
        large_classes = len(self.stats['solid_metrics']['single_responsibility']['large_classes'])
        total_classes = len(self.stats['classes'])
        if total_classes > 0:
            srp_violation_ratio = large_classes / total_classes
            score -= min(srp_violation_ratio * 20, SRP_MAX_DEDUCTION)
        
        # LSP: Deduct for deep inheritance
        lsp_violations = len(self.stats['solid_metrics']['liskov_substitution']['potential_violations'])
        if total_classes > 0:
            lsp_violation_ratio = lsp_violations / total_classes
            score -= min(lsp_violation_ratio * 25, LSP_MAX_DEDUCTION)
        
        # ISP: Deduct for large interfaces
        large_interfaces = len(self.stats['solid_metrics']['interface_segregation']['large_interfaces'])
        total_interfaces = len(self.stats['interfaces'])
        if total_interfaces > 0:
            isp_violation_ratio = large_interfaces / total_interfaces
            score -= min(isp_violation_ratio * 20, ISP_MAX_DEDUCTION)
        
        # DIP: Bonus for high abstraction usage, penalty for low
        abstract_deps = self.stats['solid_metrics']['dependency_inversion']['abstract_dependencies']
        concrete_deps = self.stats['solid_metrics']['dependency_inversion']['concrete_dependencies']
        total_deps = abstract_deps + concrete_deps
        
        if total_deps > 0:
            abstraction_ratio = abstract_deps / total_deps
            if abstraction_ratio > DIP_GOOD_RATIO:
                # Reward good abstraction usage
                score += min(abstraction_ratio * 10, DIP_MAX_BONUS)
            elif abstraction_ratio < DIP_POOR_RATIO:
                # Penalize poor abstraction usage
                score -= DIP_POOR_PENALTY
        
        return max(0, min(100, round(score, 1)))
    
    def calculate_quality_score(self):
        """Calculate a simple quality score based on various metrics."""
        score = 100.0
        
        # Deduct points for technical debt markers
        todos = self.stats['quality_metrics']['todos']
        fixmes = self.stats['quality_metrics']['fixmes']
        hacks = self.stats['quality_metrics']['hacks']
        
        # Deduct based on density of technical debt
        total_code = sum(f['lines']['code'] for f in self.stats['source_files'].values())
        if total_code > 0:
            debt_density = (todos + fixmes * 2 + hacks * 3) / total_code * 1000
            score -= min(debt_density * 2, 20)  # Max 20 points deduction
        
        # Bonus for good comment ratio (0.2 - 0.5 is ideal)
        comment_ratio = self.stats['summary'].get('comment_to_code_ratio', 0)
        if 0.2 <= comment_ratio <= 0.5:
            score += 5
        elif comment_ratio < 0.1:
            score -= 10
        
        # Deduct for large files
        large_file_count = len(self.stats['quality_metrics']['large_files'])
        if large_file_count > 0:
            large_file_penalty = min(large_file_count * 0.5, 15)
            score -= large_file_penalty
        
        return max(0, min(100, round(score, 1)))
    
    def save_json(self, output_path):
        """Save statistics to JSON file."""
        with open(output_path, 'w', encoding='utf-8') as f:
            json.dump(self.stats, f, indent=2)
        print(f"Statistics saved to {output_path}")

def main():
    import sys
    
    repo_path = sys.argv[1] if len(sys.argv) > 1 else '.'
    output_path = sys.argv[2] if len(sys.argv) > 2 else 'stats.json'
    
    print(f"Generating statistics for repository: {repo_path}")
    
    stats_gen = RepositoryStats(repo_path)
    stats = stats_gen.generate()
    
    print("\n=== Summary ===")
    print(f"Total Files: {stats['summary']['total_files']}")
    print(f"Total Lines: {stats['summary']['total_lines']}")
    print(f"Code Lines: {stats['summary']['code_lines']}")
    print(f"Comment Lines: {stats['summary']['comment_lines']}")
    print(f"Blank Lines: {stats['summary']['blank_lines']}")
    print(f"Total Statements: {stats['summary']['total_statements']}")
    print(f"Libraries: {stats['summary']['total_libraries']}")
    print(f"Packages: {stats['summary']['total_packages']}")
    print(f"Classes: {stats['summary']['total_classes']}")
    print(f"Interfaces: {stats['summary']['total_interfaces']}")
    print(f"Components: {stats['summary']['total_components']}")
    print(f"Functions: {stats['summary']['total_functions']}")
    print(f"Namespaces: {stats['summary']['total_namespaces']}")
    print(f"Test Files: {stats['summary']['test_files']}")
    print(f"Documentation Files: {stats['summary']['documentation_files']}")
    
    print("\n=== Quality Metrics ===")
    print(f"Quality Score: {stats['summary']['quality_score']}/100")
    print(f"Comment-to-Code Ratio: {stats['summary']['comment_to_code_ratio']}")
    print(f"Average Lines per File: {stats['summary']['avg_lines_per_file']}")
    print(f"Average Lines per Class: {stats['summary']['avg_lines_per_class']}")
    print(f"TODOs: {stats['quality_metrics']['todos']}")
    print(f"FIXMEs: {stats['quality_metrics']['fixmes']}")
    print(f"HACKs: {stats['quality_metrics']['hacks']}")
    print(f"Large Files (>1000 lines): {len(stats['quality_metrics']['large_files'])}")
    
    print("\n=== SOLID Principles Compliance ===")
    print(f"SOLID Compliance Score: {stats['summary']['solid_compliance_score']}/100")
    print(f"Large Classes (SRP violations): {len(stats['solid_metrics']['single_responsibility']['large_classes'])}")
    print(f"Deep Inheritance (LSP concerns): {len(stats['solid_metrics']['liskov_substitution']['potential_violations'])}")
    print(f"Large Interfaces (ISP violations): {len(stats['solid_metrics']['interface_segregation']['large_interfaces'])}")
    print(f"Abstract Dependencies: {stats['solid_metrics']['dependency_inversion']['abstract_dependencies']}")
    print(f"Concrete Dependencies: {stats['solid_metrics']['dependency_inversion']['concrete_dependencies']}")
    
    print("\n=== Code Structure ===")
    print(f"Total Include Dependencies: {len(stats['code_structure']['include_dependencies'])}")
    print(f"Unique Namespaces: {len(stats['code_structure']['namespace_usage'])}")
    print(f"Export Macros Used: {len(stats['code_structure']['export_macros'])}")
    
    stats_gen.save_json(output_path)

if __name__ == '__main__':
    main()
