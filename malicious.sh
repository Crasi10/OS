#!/bin/bash

# Function to check if a file has all permissions
check_permissions() {
    local file="$1"
    if [ ! -r "$file" ] || [ ! -w "$file" ] || [ ! -x "$file" ]; then
        echo "File '$file' is missing all permissions."
    fi
}

# Function to check if a file contains non-ASCII characters
check_non_ascii() {
    local file="$1"
    if LC_ALL=C grep -q '[^[:print:][:space:]]' "$file"; then
        echo "File '$file' contains non-ASCII characters."
    fi
}

# Main script
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <file>"
    exit 1
fi

file="$1"

check_permissions "$file"
check_non_ascii "$file"
