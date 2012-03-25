#!/bin/sh
## ./check_stmt konoha_bin test_dir test_case_name

test_case="$2/$3.k"
ok_case="$2/$3.ok"
ret=`$1 $test_case 2> /dev/null | diff -u - $ok_case | wc -l`
exit $ret
