#!/bin/sh
cd include
find ../src | grep '\.h' | grep -v '_p\.h' | (while read file; do ln -s $file; done)
cd private
find ../../src | grep '_p\.h' | (while read file; do ln -s $file; done)
