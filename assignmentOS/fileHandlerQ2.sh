#!/usr/bin/env bash

set -euo pipefail   # stop on error, treat unset vars as error, fail on pipeline errors

DIR="${1:-}"        # take first arg or empty like dir = sys.argv[1] if len(sys.argv) > 1 else ''

#if not dir: dir = input("Directory: ")
if [ -z "$DIR" ]; then
  read -rp "Directory: " DIR
fi

#if not os.path.isdir(dir): sys.exit("Error"), -d is to check path
if [ ! -d "$DIR" ]; then
  echo "Error: '$DIR' is not a directory." >&2
  exit 1
fi

# 1) count files and subdirectories (recursive)
files_count=$(find "$DIR" -type f | wc -l)
dirs_count=$(find "$DIR" -type d -mindepth 1 | wc -l)

# 2) largest file by size (GNU find -printf)
largest_line=$(find "$DIR" -type f -printf '%s\t%p\n' | sort -n | tail -n1 || true)

if [ -n "$largest_line" ]; then
  largest_size=${largest_line%%$'\t'*}
  largest_file=${largest_line#*$'\t'}
else
  largest_size=0
  largest_file="(none)"
fi

# 3) document files changed in last 24h -> print first 10 lines
echo "Directory report for: $DIR"
echo "Files: $files_count"
echo "Subdirs: $dirs_count"
echo "Largest: $largest_file ($largest_size bytes)"
echo
echo "Documents modified in last 24 hours (first 10 lines each):"

find "$DIR" -type f -mtime -1 -print0 | while IFS= read -r -d '' f; do

  if file -b --mime-type "$f" 2>/dev/null | grep -qi text; then
    printf '\n---- %s ----\n' "$f"
    head -n 10 -- "$f" || true
  fi

done

echo
echo "Done."
