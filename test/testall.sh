#!/bin/bash

find ./test -maxdepth 2 -name '*.k' | while read utest; do
	#echo "testing $utest .."
	konoha2 --test-with $utest || echo "[FAILED] $utest"
done

