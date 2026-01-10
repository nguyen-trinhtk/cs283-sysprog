#!/bin/bash
# Test with stdin
echo "Hello world" | ./wordcount

# Test with a file
echo -e "Line 1\nLine 2\nLine 3" > test.txt
./wordcount test.txt

# Test with options
./wordcount -l test.txt
./wordcount -w test.txt
./wordcount -l -w test.txt

# Test with multiple files
echo "File A" > a.txt
echo "File B" > b.txt
./wordcount a.txt b.txt