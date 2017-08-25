#!/bin/sh
cur_dir="$(dirname "$0")"
. $cur_dir/export-env.sh

sqlite3 $dbname < create-user.sql
sqlite3 $dbname < create-login.sql
sqlite3 $dbname < create-valid-ip.sql
