#!/bin/bash

echo '*** Generating html ***' &&
make html &&

echo '*** Generating pdf ***' &&
make pdf &&

echo '*** Copying files to ../docs directory' &&
make deploy
