#!/usr/bin/python3

import os
import sys
import subprocess
from pathlib import Path

def process_c_files(directory_path):
    """
    处理指定目录下的所有.c文件，执行编译和运行测试
    
    Args:
        directory_path: 目录路径（相对或绝对路径）
    """
    # 转换为Path对象以便更好地处理路径
    directory = Path(directory_path)
    
    # 检查目录是否存在
    if not directory.exists() or not directory.is_dir():
        print(f"错误: 目录 '{directory_path}' 不存在或不是目录")
        return
    
    # 统计成功和失败的数量
    success_count = 0
    failed_count = 0
    
    # 遍历目录下的所有.c文件
    for c_file in directory.glob("*.c"):
        print(f"\n处理文件: {c_file}")
        
        # 检查对应的.out文件是否存在
        out_file = c_file.with_suffix('.out')
        if not out_file.exists():
            print(f"警告: 找不到对应的.out文件: {out_file}")
            continue
        
        # 步骤1: 执行 compiler asm 命令
        try:
            # 使用subprocess执行compiler asm命令，设置超时为30秒防止挂死
            with open('comp.log', 'w') as comp_log:
                result = subprocess.run(
                    ['compiler', 'asm', str(c_file), '-f'],
                    stdout=comp_log,
                    stderr=subprocess.STDOUT,  # 将stderr重定向到stdout
                    timeout=30,
                    check=True  # 如果返回非零状态码则抛出异常
                )
            
            # 如果编译成功，继续执行make run_asm
            print(f"编译成功: {c_file}")
            
            # 步骤2: 执行make run_asm命令
            try:
                with open('run.log', 'w') as run_log:
                    result = subprocess.run(
                        ['make', 'run_asm', '-f', 'Makefile'],
                        stdout=run_log,
                        stderr=subprocess.STDOUT,
                        timeout=30,
                        check=True
                    )
                
                # 步骤3: 处理run.log文件，截取第四行及以后的内容
                with open('run.log', 'r') as f:
                    lines = f.readlines()
                
                # 如果文件行数不足4行，使用空内容
                if len(lines) >= 4:
                    actual_content = ''.join(lines[3:])  # 从第4行开始（索引3）
                else:
                    actual_content = ""
                
                # 将截取的内容写入临时文件用于diff比较
                temp_actual_file = 'temp_actual.log'
                with open(temp_actual_file, 'w') as f:
                    f.write(actual_content)
                
                # 步骤4: 执行diff比较
                try:
                    result = subprocess.run(
                        ['diff', temp_actual_file, str(out_file)],
                        capture_output=True,
                        text=True
                    )
                    
                    # 根据diff返回码判断是否匹配
                    if result.returncode == 0:
                        print(f"✓ 测试通过: {c_file.name}")
                        success_count += 1
                    else:
                        print(f"✗ 测试失败: {c_file.name}")
                        print(f"Diff输出: {result.stdout}")
                        failed_count += 1
                
                except Exception as diff_error:
                    print(f"Diff执行错误: {diff_error}")
                    failed_count += 1
                finally:
                    # 清理临时文件
                    if os.path.exists(temp_actual_file):
                        os.remove(temp_actual_file)
                        
            except subprocess.TimeoutExpired:
                print(f"运行超时: make run_asm 命令执行时间过长")
                failed_count += 1
            except subprocess.CalledProcessError as make_error:
                print(f"运行失败: make run_asm 命令返回非零状态码: {make_error.returncode}")
                failed_count += 1
            except Exception as make_error:
                print(f"运行错误: {make_error}")
                failed_count += 1
                
        except subprocess.TimeoutExpired:
            print(f"编译超时: compiler asm 命令执行时间过长")
            print(f"{c_file} compile error")
            failed_count += 1
        except subprocess.CalledProcessError as compile_error:
            print(f"编译失败: compiler asm 命令返回非零状态码: {compile_error.returncode}")
            print(f"{c_file} compile error")
            failed_count += 1
        except FileNotFoundError:
            print(f"错误: 找不到 'compiler' 或 'make' 命令，请确保它们在PATH中")
            failed_count += 1
        except Exception as e:
            print(f"编译错误: {e}")
            print(f"{c_file} compile error")
            failed_count += 1
    
    # 输出最终统计结果
    print(f"\n{'='*50}")
    print(f"测试完成!")
    print(f"成功: {success_count}")
    print(f"失败: {failed_count}")
    print(f"总计: {success_count + failed_count}")
    print(f"{'='*50}")

def main():
    """
    主函数：处理命令行参数并执行测试
    """
    if len(sys.argv) != 2:
        print("用法: python script.py <目录路径>")
        print("示例: python script.py ./test_cases")
        sys.exit(1)
    
    directory_path = sys.argv[1]
    process_c_files(directory_path)

if __name__ == "__main__":
    main()

