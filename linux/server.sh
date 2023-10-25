#!/usr/bin/env bash
while true
do
   if pgrep server[y] 1>/dev/null; then 
      sleep 1
   else
      ./server_x64
   fi
done


