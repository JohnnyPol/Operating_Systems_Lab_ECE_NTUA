#!/bin/bash

# Create the bf00 file
dd if=/dev/zero of=bf00 bs=1 count=16 seek=1073741824

# Write to bf00 using Python
python -c "with open('bf00', 'r+b') as f: f.seek(1073741824); f.write(b'Hello, this is some data!')"

# Loop to create symbolic links from bf01 to bf09 pointing to bf00
for i in $(seq -w 1 9); do
    ln bf00 bf0$i
    echo "Created hard link: bf0$i -> bf00"
done

./riddle
