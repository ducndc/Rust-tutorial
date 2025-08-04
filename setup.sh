#!/bin/bash

echo "Create environment"

python3 -m venv myenv
source myenv/bin/activate

pip install PySide6

pip install -r ./log-parser/requirements.txt