#!/usr/bin/env bash

marked -i opcodes.md -o opcodes.html
gimli -f opcodes.md -s gimli.css

marked -i manual.md -o manual.html
gimli -f manual.md -s gimli.css

marked -i cheatsheet.md -o cheatsheet.html
gimli -f cheatsheet.md -s gimli.css

marked -i todo.md -o todo.html
gimli -f todo.md -s todo.css
