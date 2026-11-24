#!/usr/bin/env bash
# Prints the CHANGELOG.md section for a release tag, without its "## vX.Y.Z"
# heading, for use as the GitHub release notes.
#
#   .github/scripts/release-notes.sh v1.1.0         # section "## v1.1.0"
#   .github/scripts/release-notes.sh v1.1.0-rc1     # also section "## v1.1.0"
#
# Exits with an error if the section is missing or empty.

set -euo pipefail

tag="${1:?usage: release-notes.sh <tag>}"
changelog="${2:-CHANGELOG.md}"

# Pre-release tags use the section of the version they lead up to
version="${tag%%-*}"

notes=$(awk -v heading="## $version" '
    $0 == heading { found = 1; next }
    found && /^## / { exit }
    found { print }
' "$changelog")

# Trim blank lines at the start and end
notes=$(printf '%s\n' "$notes" | sed -e '/./,$!d' | sed -e ':a' -e '/^\n*$/{$d;N;ba' -e '}')

if [ -z "$notes" ]; then
    echo "No \"## $version\" section with content in $changelog" >&2
    exit 1
fi

printf '%s\n' "$notes"
