"""
This file creates one C++ header file for each peripheral present in the given SVD file (svd_file).

The generated headers are put in the provided output folder (output_dir).

By default, the script clears the target folder of all '.hpp' files. This can be disabled using the --no-clear flag.

By default, generated code is minimised to safe some space. I deem this acceptable since it is not really meant to be
read by a person, but there is an option to 'prettify' the code using the '--pretty' flag.
"""
import os
import sys
from jinja2 import Environment, FileSystemLoader
from cmsis_svd import SVDParser
from argparse import ArgumentParser
from minifier import minify_source
import helpers

TEMPLATE_DIR = "templates"

### Parse command line arguments ###
arg_parser = ArgumentParser(description="Generate TSRI registers from microcontroller SVD file.")
arg_parser.add_argument("svd_file", help="Path to the SVD file.")
arg_parser.add_argument("output_dir", help="Directory to output the generated code.")
arg_parser.add_argument("-g", "--generate-only", nargs="*", default=[], help="Generate only the peripherals with the given LOWERCASE name.")
arg_parser.add_argument("-l", "--list-output-files", action="store_true", help="List the output header files, but do not generate them. Used during CMake configuration.")
arg_parser.add_argument("-n", "--no-clear", action="store_true", help="Do not clear the output directory header files.")
arg_parser.add_argument("-p", "--pretty", action="store_true", help="Keep the code layout somewhat pretty. By default, this is false: all whitespace is removed to reduce memory footprint.")
arg_parser.add_argument("--namespace", default="", help="C++ namespace to put the registers in")
args = arg_parser.parse_args()

def get_peripheral_file(peripheral):
    """
    Return the file name for the given peripheral.
    """
    return f"{args.output_dir}/{peripheral.name.lower()}.hpp"

### Prepare the Jinja2 environment ###
env = Environment(loader=FileSystemLoader(TEMPLATE_DIR), trim_blocks=True, lstrip_blocks=True, extensions=['jinja2.ext.loopcontrols'])

### Parse SVD file accordring to user-provided path ###
parser = SVDParser.for_xml_file(args.svd_file)
device = parser.get_device()

### Construct device representation that can be used in the templates ###
peripherals = []
if args.generate_only != []:
    for peripheral in device.peripherals:
        if peripheral.name.lower() in args.generate_only:
            peripherals.append(helpers.parse_peripheral(peripheral))
else:
    peripherals = helpers.parse_peripherals(device)

### If we only list output files, list them and then exit ###
if args.list_output_files:
    for i, peripheral in enumerate(peripherals):
        if i - 1 == len(peripherals):
            print(get_peripheral_file(peripheral), end="")
        else:
            print(get_peripheral_file(peripheral), end=";")
    sys.exit(0)

## Check if output directory exists, if not, create it ###
if not os.path.exists(args.output_dir):
    os.mkdir(args.output_dir)
elif not args.no_clear:
    for item in os.listdir(args.output_dir):
        if item.endswith(".hpp"):
            os.remove(os.path.join(args.output_dir, item))

### Generate code for each peripheral and move into output folder ###
for peripheral in peripherals:
    template = env.get_template("peripheral.jinja2")
    output = template.render(peripheral=peripheral, namespace=args.namespace)
    output = minify_source(output) if not args.pretty else output

    # This makes sure comments stay on their own line. This is done so the comments render correctly in the IDE.
    output = output.replace("/*", "\n/*").replace("*/", "*/\n") if not args.pretty else output

    with open(get_peripheral_file(peripheral), "w") as f:
        f.write(output)
