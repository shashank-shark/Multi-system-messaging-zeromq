#! /bin/bash

echo "Starting subscribers ..."
for i in {0..100}
 do
  ./sync_sub &
 done

echo "Starting publisher ..."
