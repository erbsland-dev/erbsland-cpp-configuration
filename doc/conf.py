# -- Project information -----------------------------------------------------
import sys
from pathlib import Path

project = "Erbsland Configuration Language Parser"
copyright = "2024-2025, Tobias Erbsland - Erbsland DEV"
author = "Tobias Erbsland - Erbsland DEV"
release = "1.0"

# -- General configuration ---------------------------------------------------
extensions = ["sphinx_rtd_theme", "sphinx_design", "breathe"]
templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

# -- Options for HTML output -------------------------------------------------
html_theme = "sphinx_rtd_theme"
html_static_path = ["_static"]
html_css_files = ["custom.css"]
html_context = {
    "banner": "Heads up! This documentation is still in progress — final updates coming soon."
}

# -- Options for Breathe -----------------------------------------------------
_project_dir = Path(__file__).parent.parent
_src_dir = _project_dir / "src"
breathe_projects = {"erbsland-conf": _project_dir / "_build/breathe/doxygen/erbsland-conf/xml"}
breathe_default_project = "erbsland-conf"
breathe_projects_source = {"erbsland-conf": (_src_dir, ["erbsland/conf"])}
breathe_doxygen_config_options = {
    "STRIP_FROM_PATH": _src_dir,
    "STRIP_FROM_INC_PATH": _src_dir,
}
