#!/usr/bin/bash

python generate_doxygen.py \
  --model qwen2.5-coder:7b-instruct \
  --folder ../../app/ \
  --sleep 2 \
  --exclude "*plugin_view.h" \
  --exclude "*save_handler.h" \
  --exclude "*behaviour_canvas.h" \
  --exclude "*canvas_view.h" \
  --exclude "*canvas.h" 
