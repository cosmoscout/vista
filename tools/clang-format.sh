#!/bin/bash

# ------------------------------------------------------------------------------------------------ #
#                                    This file is part of ViSTA                                    #
# ------------------------------------------------------------------------------------------------ #

# The modules are assumed to be in the parent folder.
SRC_DIR="$( cd "$( dirname "$0" )" && pwd )"

# Execute clang format for all *.cpp, *.hpp *.h and *.inl files.
find "$SRC_DIR/../VistaCoreLibs" "$SRC_DIR/../VistaDemo" -type f \
     \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' -o -name '*.inl' \) -exec sh -c '
  for file do
    echo "Formatting $file..."
    clang-format -i "$file"
  done
' sh {} +
