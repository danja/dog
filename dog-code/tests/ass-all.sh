for file in *.ass; do ../../python/ass.py -s ../../src/dog-1/dog-1.ino  -d ../../docs/opcodes.md -i "$file" -o "${file/%ass/dog}"; done
