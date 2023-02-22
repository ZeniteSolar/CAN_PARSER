#!/bin/bash
cd $(dirname $0)
TESTS=$(find . -type f -executable ! -name "*.sh")
echo ${TESTS}
for test in ${TESTS}
do
	$test
done
