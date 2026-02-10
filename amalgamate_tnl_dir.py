import os
import re

# Configuration
target_dir = 'tnl'
output_file = 'tnl_amalgamation.cpp'
extensions = {'.h', '.cpp'}

system_includes = set()
headers_content = []
sources_content = []

# Regex for includes
# Handles: #include <header>, #include "header", #  include ...
inc_pattern = re.compile(r'^\s*#\s*include\s+(["<])([^">]+)([ জৈ])')

def process_file(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
            lines = f.readlines()
    except Exception as e:
        return f"\n// ERROR READING FILE: {filepath} ({e})\n"
    
    processed_lines = []
    # Use relative path for cleaner comments
    rel_path = os.path.relpath(filepath, '.').replace('\\', '/')
    processed_lines.append(f"\n// --- START OF FILE: {rel_path} ---\
")
    
    for line in lines:
        match = inc_pattern.match(line)
        if match:
            sep_start, name, sep_end = match.groups()
            if sep_start == '<':
                # System include
                system_includes.add(line.strip())
                processed_lines.append(f"// {line.strip()} // Moved to top\n")
            else:
                # Local include - assume it's part of the amalgamation and skip it
                processed_lines.append(f"// {line.strip()} // Skipped for amalgamation\n")
        else:
            processed_lines.append(line)
            
    processed_lines.append(f"\n// --- END OF FILE: {rel_path} ---\
")
    return "".join(processed_lines)

# Collect files
h_files = []
cpp_files = []

if not os.path.exists(target_dir):
    print(f"Error: Directory '{target_dir}' not found.")
    exit(1)

for dirpath, dirnames, filenames in os.walk(target_dir):
    # Skip hidden dirs
    dirnames[:] = [d for d in dirnames if not d.startswith('.')]
    
    for filename in filenames:
        ext = os.path.splitext(filename)[1].lower()
        if ext in extensions:
            path = os.path.join(dirpath, filename)
            if ext == '.h':
                h_files.append(path)
            else:
                cpp_files.append(path)

# Sort for deterministic output
h_files.sort()
cpp_files.sort()

# Process
print(f"Processing {len(h_files)} headers from '{target_dir}'...")
for f in h_files:
    headers_content.append(process_file(f))

print(f"Processing {len(cpp_files)} sources from '{target_dir}'...")
for f in cpp_files:
    sources_content.append(process_file(f))

# Write Output
with open(output_file, 'w', encoding='utf-8') as out:
    out.write(f"// TNL Amalgamated Source File\n")
    out.write(f"// Generated from directory: {target_dir}\n\n")
    
    out.write("// --- System Includes ---\
")
    for inc in sorted(list(system_includes)):
        out.write(inc + "\n")
    out.write("\n")
    
    out.write("// --- Headers ---\
")
    for content in headers_content:
        out.write(content)
        
    out.write("// --- Sources ---\
")
    for content in sources_content:
        out.write(content)

print(f"Created {output_file}")
