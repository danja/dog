for file in *.ass; do ../../python/ass.py -s ../../src/dog-1/dog-1.ino -i "$file" -o "${file/%ass/dog}"; done
