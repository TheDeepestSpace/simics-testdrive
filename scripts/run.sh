#!/usr/bin/env bash
# Build the DML modules + firmware, run the simulation, fail unless firmware says PASS.
set -euo pipefail
cd "$(dirname "$0")/.."

[ -x ./simics ] || scripts/setup-project.sh
make -s toy-i2c-master toy-i2c-slave
make -s -C firmware

out="$(./simics --batch-mode --no-win targets/testdrive.simics 2>&1)"
echo "$out"
grep -q 'RESULT: PASS' <<<"$out"
