#!/bin/bash

find Assets -type f ! -name "*.o" -exec \
    ld -r -b binary -o "{}.o" "{}" \
\;
