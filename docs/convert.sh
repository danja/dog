#!/usr/bin/env bash

marked -i opcodes.md -o opcodes.html
marked -i manual.md -o manual.html
gimli -f -s gimli.css opcodes.md
gimli -f -s gimli.css manual.md
