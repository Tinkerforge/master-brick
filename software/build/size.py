import fnmatch
import os
import glob

size_str = "arm-none-eabi-size "
for root, dirnames, filenames in os.walk('.'):
    for filename in fnmatch.filter(filenames, '*.obj'):
        size_str += " " + os.path.join(root, filename)

size_str += " " + glob.glob("*.elf")[0]

os.system(size_str)
