import os
import sys

# Path to the /tmp/ directory
tmp_dir = '/tmp'

# Loop through all files in the /tmp directory
for filename in os.listdir(tmp_dir):
    # Check if the file starts with 'riddle-'
    if filename.startswith('riddle-'):
        full_path = os.path.join(tmp_dir, filename)

offset=0x6F 
letter = str(sys.argv[1])
try:
    with open(full_path, 'r+b') as file:
        file.seek(offset)
        file.write(letter.encode('utf-8'))
    print(f'Successfully wrote \'{letter}\' at offset 0x{offset:02X}')
except FileNotFoundError:
    print(f'Error: The file \'{filename}\' does not exist.')
except OSError as e:
    print(f'Error: {e}')