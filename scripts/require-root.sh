#!/bin/bash

if ! sudo true; then
  >&2 echo "You must have sudo to run this script"
  exit 1
fi
