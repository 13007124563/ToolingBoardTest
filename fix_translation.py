#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys

file_path = r"d:\My_work\QT_pro\ToolingBoardTest\ToolingBoardTest_cn.ts"

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# 替换中文翻译
old_text = '脚本将持续运行进行网络监控。您可以点击"保存"按钮保存测试记录。'
new_text = '脚本将持续运行进行网络监控。'

content = content.replace(old_text, new_text)

with open(file_path, 'w', encoding='utf-8') as f:
    f.write(content)

print(f"Successfully updated {file_path}")
