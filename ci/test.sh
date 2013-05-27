#!/usr/bin/env bash
set -e
make CONFIG_SIMAVR=1 DEVICE=arduino_uno test | tee build/output.log
cat build/output.log | grep -q "TESTS PASSED"  
