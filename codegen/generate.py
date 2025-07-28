
from jinja2 import Environment, FileSystemLoader

TEMPLATE_DIR = "templates"

# Prepare the Jinja2 environment
env = Environment(loader=FileSystemLoader(TEMPLATE_DIR), trim_blocks=True, lstrip_blocks=True, extensions=['jinja2.ext.loopcontrols'])

# TODO: parse SVD file accordring to user-provided path

# TODO: construct device representation that can be used in the templates

# TODO: move created files into output directory provided by user
