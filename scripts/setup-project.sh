#!/usr/bin/env bash
# (Re)generate the Simics project scaffolding in the repo root and point it at
# the packages installed under $SIMICS_HOME (default /opt/simics).
set -euo pipefail
cd "$(dirname "$0")/.."

SIMICS_HOME="${SIMICS_HOME:-/opt/simics}"
BASE="$(ls -d "$SIMICS_HOME"/simics-7.* | sort -V | tail -1)"

# .package-list lists package directories (absolute paths).
{
  for pkg in "$SIMICS_HOME"/simics-risc-v-simple-* "$SIMICS_HOME"/simics-risc-v-cpu-*; do
    realpath "$pkg"
  done
} > .package-list

"$BASE/bin/project-setup" --ignore-existing-files . >/dev/null
echo "Simics project ready (base: $BASE)"
