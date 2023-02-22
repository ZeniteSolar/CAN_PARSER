#!/bin/bash
TESTS=$(find . -type f -executable ! -name "*.sh")
for test in TESTS
do
	test
done
