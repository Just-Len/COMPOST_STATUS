#!/bin/sh

# Install 'uglify-js' using npm
uglifyjs -o main.min.js --compress --mangle main.js
