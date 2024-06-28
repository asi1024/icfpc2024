#!/bin/bash

set -eux

for FILE in `ls raw`; do
    echo $FILE
    ./icfp_parser/target/debug/run < raw/$FILE > tmp
    python trim.py < tmp > spaceship_data/$FILE
done
