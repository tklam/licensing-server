#!/bin/sh
cur_dir="$(dirname "$0")"
. $cur_dir/export-env.sh

sqlite3 $dbname "insert into User values (\
        'TESTING_CLIENT', 'Group', strftime('%s', '2018-01-0100:00:01'));"

sqlite3 $dbname "insert into UserValidIP values (\
        'TESTING_CLIENT', '127.0.0.1');"
