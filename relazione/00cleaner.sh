#!/bin/bash

SEARCH_PATTERN='.*\.\(out\|aux\|toc\|log\|blg\|bbl\|backup\|dvi\|.*~\)$'

find . -iregex "$SEARCH_PATTERN" -exec rm {} \;

# CURR_DIR="${PWD##*/}"
# tar cszf ../"$CURR_DIR".tar.gz "../$CURR_DIR"

