import re
import shutil
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))

# -- Project information -----------------------------------------------------
project = "Erbsland Configuration Language Parser for C++"
copyright = "2024-2025, Tobias Erbsland - Erbsland DEV"
author = "Tobias Erbsland - Erbsland DEV"
release = "1.0"

# -- General configuration ---------------------------------------------------
extensions = ["sphinx_rtd_theme", "sphinx_design", "sphinx_copybutton", "breathe", "_ext.ansi"]
templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

# -- Options for HTML output -------------------------------------------------
html_theme = "sphinx_rtd_theme"
html_static_path = ["_static"]
html_css_files = [
    "custom.css",
    "ansi.css",
    "https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.7.2/css/all.min.css"
]

# -- Options for Breathe -----------------------------------------------------
_project_dir = Path(__file__).parent.parent
_original_src_dir = _project_dir / "src"
_processed_src_dir = _project_dir / "_doxygen_input"
breathe_projects = {"erbsland-conf": _project_dir / "_build/breathe/doxygen/erbsland-conf/xml"}
breathe_default_project = "erbsland-conf"
breathe_projects_source = {"erbsland-conf": (_processed_src_dir, ["erbsland/conf"])}
breathe_doxygen_config_options = {
    "STRIP_FROM_PATH": _processed_src_dir,
    "STRIP_FROM_INC_PATH": _processed_src_dir,
    "ALIASES": "\"tested=@par Tested:^^\", \"notest=@par Not Tested:^^\", \"needtest=@par Needs Testing:^^\", \"wip=@par Work in Progress:^^\"",
    "ENABLE_PREPROCESSING": "yes",
    "MACRO_EXPANSION": "no",
}

def _swap_tr(match):
    return match.group(2) + " " + match.group(1) + " " + match.group(3)

def preprocess_sources():
    print("Preprocessing sources...")

    re_swap_tr = re.compile(r"auto\s+(.*?)\s+->\s*(.*?)\s*(override\s*[;{]|=\s*(?:0|default)\s*;|[;{])")
    re_std_helpers = re.compile(r"(?s)template\s*<>\s*\nstruct.*\{\n.*\n\};\n")
    re_macros = re.compile(r"""(?x)
        ERBSLAND_CONF_(?:
            COMPARE_MEMBER|
            COMPARE_FRIEND|
            CONTAINER_ACCESS_METHODS|
            CONTAINER_PUSH_BACK|
            CONTAINER_DEFINITIONS)
        .*\n
    """)
    re_requires = re.compile(r"(?s)(template.*?)\s+requires\s*\(.*?\)\s*\n")
    re_one_line_specialization = re.compile(r"(?s)template<>\s+\[\[nodiscard\]\]\s+inline.*?\}\s*\n")

    # Clean output dir
    if _processed_src_dir.exists():
        shutil.rmtree(_processed_src_dir)
    _processed_src_dir.mkdir(parents=True, exist_ok=True)

    # Copy and patch files
    for src_path in _original_src_dir.rglob("*.hpp"):
        dst_path = _processed_src_dir / src_path.relative_to(_original_src_dir)
        content = src_path.read_text(encoding="utf-8")
        content = re_swap_tr.sub(_swap_tr, content)
        content = re_std_helpers.sub("", content)
        content = re_macros.sub("// macro removed\n", content)
        content = re_requires.sub("\\1/* requires removed */\n", content)
        content = re_one_line_specialization.sub("", content)
        dst_path.parent.mkdir(parents=True, exist_ok=True)
        dst_path.write_text(content, encoding="utf-8")

# -- Add the syntax highlighter ---------------------------------------------
def setup(app):
    from _ext.pygments_elcl import ErbslandConfigurationLanguage

    app.add_lexer("erbsland-conf", ErbslandConfigurationLanguage)
    preprocess_sources()
