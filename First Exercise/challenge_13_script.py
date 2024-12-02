import os
with open(".hello_there", 'r+b') as file:
    os.ftruncate(file.fileno(), 32768)
