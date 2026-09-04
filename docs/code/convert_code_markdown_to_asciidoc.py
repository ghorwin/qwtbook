#! /usr/bin/env python
# -*- coding: utf-8 -*-

import re
import argparse

def convert_code_blocks(markdown_text):
    # Regulärer Ausdruck für ```c++ ... ```
    pattern = re.compile(r'```c\+\+\n(.*?)```', re.DOTALL)

    # Ersetzen mit [source,c++] ---- ----
    converted_text = pattern.sub(r'[source,cpp]\n----\n\1----', markdown_text)

    return converted_text

# main
parser = argparse.ArgumentParser(description="Konvertiert Markdown ```c++ Codeblöcke zu [source,cpp]")
parser.add_argument('input_file', help="Pfad zur Eingabedatei (.md)")
parser.add_argument('output_file', help="Pfad zur Ausgabedatei (.adoc)")

args = parser.parse_args()

with open(args.input_file, 'r', encoding='utf-8') as infile:
	md_content = infile.read()

converted = convert_code_blocks(md_content)

with open(args.output_file, 'w', encoding='utf-8') as outfile:
	outfile.write(converted)

print(f"Konvertierung abgeschlossen: {args.output_file}")
