
from jinja2 import Environment, FileSystemLoader
from pathlib import Path
from cmsis_svd import SVDParser
import os
import shutil

SVD_DATA_DIR = "/workspaces/repository"
TEMPLATE_FILE = "peripheral.jinja2"
TEMPLATE_FOLDER = "."
RESULTS_FOLDER = "results"
LIBRARY_HEADER_FOLDER = ".."
FILE_EXTENSION = ".hpp"
MCU = "rp2040"


# Prepare the Jinja2 environment and load the template
env = Environment(loader=FileSystemLoader(TEMPLATE_FOLDER), trim_blocks=True, lstrip_blocks=True, extensions=['jinja2.ext.loopcontrols'])
template = env.get_template(TEMPLATE_FILE)

parser = SVDParser.for_mcu(SVD_DATA_DIR, MCU)
device = parser.get_device()

for peripheral in device.peripherals:
    output = template.render(peripheral=peripheral)
    file = Path(f"{RESULTS_FOLDER}/{peripheral.name.lower()}{FILE_EXTENSION}")
    file.parent.mkdir(parents=True, exist_ok=True)
    file.write_text(output)


files=os.listdir(RESULTS_FOLDER)

for fname in files:
    shutil.copy2(os.path.join(RESULTS_FOLDER, fname), LIBRARY_HEADER_FOLDER)
