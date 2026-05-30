#!/usr/bin/env bash
set -euo pipefail

usage() {
    echo "Usage: $0 [release-directory]" >&2
    exit 1
}

if [ $# -gt 1 ]; then
    usage
fi

release_dir="${1:-.}"
gpg_prog="${GPG:-gpg}"
gpg_args=(--digest-algo sha256)

if [ -n "${SIGNER:-}" ]; then
    gpg_args+=(--local-user "$SIGNER")
fi

cd "$release_dir"

rm -f SHA256SUMS SHA256SUMS.asc

find . -maxdepth 1 -type f \
    ! -name 'SHA256SUMS' \
    ! -name 'SHA256SUMS.asc' \
    ! -name '*.assert' \
    ! -name '*.assert.sig' \
    ! -name '*-debug.tar.gz' \
    ! -name '*-debug.zip' \
    -print0 \
    | sort -z \
    | xargs -r0 sha256sum > SHA256SUMS

if [ ! -s SHA256SUMS ]; then
    echo "No release artifacts found in $release_dir" >&2
    rm -f SHA256SUMS
    exit 1
fi

"$gpg_prog" "${gpg_args[@]}" --clearsign SHA256SUMS
rm SHA256SUMS

echo "Created $(pwd)/SHA256SUMS.asc"
