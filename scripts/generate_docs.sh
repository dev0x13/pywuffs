#!/bin/bash

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

python3 -m pip install sphinx sphinx_epytext sphinx-rtd-theme

python3 -m sphinx -b html -E -a "$script_dir/../docs/src/" "$script_dir/../docs/"
