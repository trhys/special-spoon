#!/usr/bin/env bash
set -euo pipefail

repo="${1:?Usage: $0 <owner/repo>}"

engine_root="$(git rev-parse --show-toplevel)"
cd ./Sandbox/src

if [[ -e ./client-new || -L ./client-new ]]; then
    echo "client-new already exists; remove or rename it first." >&2
    exit 1
fi

if [[ -n "${CLIENT_REPO_TOKEN:-}" ]]; then
    GH_TOKEN="$CLIENT_REPO_TOKEN" gh repo clone "$repo" ./client-new
else
    git clone -- "https://github.com/${repo}.git" ./client-new
fi

rm -rf -- ./client
mv -T -- ./client-new ./client

git -C "$engine_root" ls-files -z -- Sandbox/src/client |
    git -C "$engine_root" update-index --skip-worktree -z --stdin

# Ignore additional client files locally.
exclude="$(git -C "$engine_root" rev-parse --absolute-git-dir)/info/exclude"
pattern="/Sandbox/src/client/"

if ! grep -Fxq -- "$pattern" "$exclude"; then
    printf '\n%s\n' "$pattern" >> "$exclude"
fi