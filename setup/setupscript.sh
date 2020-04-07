#!/bin/bash
# TODO 1:  change this, so we can take a parameter for a dir to cd in to...
cd /home/cx/dev/packages
# /TODO

check_for_cargo() {
    if hash cargo 2>/dev/null; then
        echo "cargo is installed. nothing needs to be done."
    else
        echo "Cargo and (possibly) the rust toolchain not installed... Downloading..."
        curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
    fi
}

check_for_cargo

cd hockeyscraper
cargo build
cargo run -- .