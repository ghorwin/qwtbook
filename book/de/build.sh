#!/bin/bash

mkdocs build

rm -rf ../../docs
mv site ../../docs

