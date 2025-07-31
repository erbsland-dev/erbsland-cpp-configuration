#  Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
#  SPDX-License-Identifier: Apache-2.0


import re
from pathlib import Path

ref_path = Path(__file__).parent / "chapters" / "reference"
src_path = Path(__file__).parent.parent / "src" / "erbsland" / "conf"

re_entry = re.compile(r".. doxygen\w+:: erbsland::conf::(\w+)")
re_brief = re.compile(r"(?s)/// ([^\n]+)\n(?:///[^\n]*\n)*(?:class|struct|enum|enum class|using) (\w+)")

entry_map = {}
for file in ref_path.glob("*.rst"):
    content = file.read_text()
    for match in re_entry.finditer(content):
        entry_map[match.group(1)] = file.name.replace(".rst", "")

brief_map = {}
for file in src_path.glob("*.hpp"):
    print(file)
    content = file.read_text()
    for match in re_brief.finditer(content):
        brief_map[match.group(2)] = match.group(1).strip().replace("`", "")

print(".. list-table::")
print("    :header-rows: 1")
print()
print("    *   - Name")
print("        - Description")
for name in sorted(entry_map.keys()):
    page = entry_map[name]
    brief = brief_map.get(name, "")
    print(f"    *   - :doc:`{name}<{page}>`")
    print(f"        - {brief}")
