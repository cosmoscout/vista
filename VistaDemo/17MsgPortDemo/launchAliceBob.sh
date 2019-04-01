#!/bin/sh

echo "Launching Bob first!"

(cd Bob_ApplicationDemo && ./17BobDemoD)&

echo "Waiting approx. 10 seconds to launch Alice"

sleep 10

(cd Alice_shellDemo && ./17AliceDemoD)
