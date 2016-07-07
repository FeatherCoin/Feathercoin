#!/bin/sh
set -e
echo running autoreconf .....
autoreconf  -if --warnings=all
autoupdate
echo
echo running autoupdate completed .....
