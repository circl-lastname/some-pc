#!/bin/sh
set -xe

some-as rom.asm rom
mkdir -p ../testing
cp rom ../testing
