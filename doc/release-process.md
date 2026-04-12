Release Process
====================

## Branch updates

### Before every release candidate

* Update translations (ping wumpus on IRC) see [translation_process.md](https://github.com/bitcoin/bitcoin/blob/master/doc/translation_process.md#synchronising-translations).
* Update manpages, see [gen-manpages.sh](https://github.com/bitcoin/bitcoin/blob/master/contrib/devtools/README.md#gen-manpagessh).
* Update release candidate version in `configure.ac` (`CLIENT_VERSION_RC`).

### Before every major and minor release

* Update [bips.md](bips.md) to account for changes since the last release (don't forget to bump the version number on the first line).
* Update version in `configure.ac` (don't forget to set `CLIENT_VERSION_RC` to `0`).
* Write release notes (see "Write the release notes" below).

### Before every major release

* On both the master branch and the new release branch:
  - update `CLIENT_VERSION_MINOR` in [`configure.ac`](../configure.ac)
  - update `CLIENT_VERSION_MINOR`, `PACKAGE_VERSION`, and `PACKAGE_STRING` in [`build_msvc/bitcoin_config.h`](/build_msvc/bitcoin_config.h)
* On the new release branch in [`configure.ac`](../configure.ac) and [`build_msvc/bitcoin_config.h`](/build_msvc/bitcoin_config.h) (see [this commit](https://github.com/bitcoin/bitcoin/commit/742f7dd)):
  - set `CLIENT_VERSION_REVISION` to `0`
  - set `CLIENT_VERSION_IS_RELEASE` to `true`

#### Before branch-off

* Update hardcoded [seeds](/contrib/seeds/README.md), see [this pull request](https://github.com/bitcoin/bitcoin/pull/7415) for an example.
* Update [`src/chainparams.cpp`](/src/chainparams.cpp) m_assumed_blockchain_size and m_assumed_chain_state_size with the current size plus some overhead (see [this](#how-to-calculate-assumed-blockchain-and-chain-state-size) for information on how to calculate them).
* Update [`src/chainparams.cpp`](/src/chainparams.cpp) chainTxData with statistics about the transaction count and rate. Use the output of the `getchaintxstats` RPC, see
  [this pull request](https://github.com/bitcoin/bitcoin/pull/20263) for an example. Reviewers can verify the results by running `getchaintxstats <window_block_count> <window_final_block_hash>` with the `window_block_count` and `window_final_block_hash` from your output.
* Update `src/chainparams.cpp` nMinimumChainWork and defaultAssumeValid (and the block height comment) with information from the `getblockheader` (and `getblockhash`) RPCs.
  - The selected value must not be orphaned so it may be useful to set the value two blocks back from the tip.
  - Testnet should be set some tens of thousands back from the tip due to reorgs there.
  - This update should be reviewed with a reindex-chainstate with assumevalid=0 to catch any defect
     that causes rejection of blocks in the past history.
- Clear the release notes and move them to the wiki (see "Write the release notes" below).

#### After branch-off (on master)

- Update the version of `contrib/gitian-descriptors/*.yml`.

#### After branch-off (on the major release branch)

- Update the versions.
- Create a pinned meta-issue for testing the release candidate (see [this issue](https://github.com/bitcoin/bitcoin/issues/17079) for an example) and provide a link to it in the release announcements where useful.

#### Before final release

- Merge the release notes from the wiki into the branch.
- Ensure the "Needs release note" label is removed from all relevant pull requests and issues.


## Building

### First time / New builders

Feathercoin release builds on this branch use Gitian to create unsigned release artifacts.
We do not use a shared `gitian.sigs` repository and we do not use detached Apple/Microsoft
codesigning payloads. Instead:

- Gitian builder attestations are kept locally in an `attestations/` directory.
- Windows and macOS artifacts are published unsigned.
- Release integrity is provided by `SHA256SUMS.asc`, signed by the Feathercoin release key.

If you're using the automated script (found in [contrib/gitian-build.py](/contrib/gitian-build.py)),
run it with `--setup` the first time. The expected directory layout is:

    cd /path/to/your/toplevel/build
    git clone https://github.com/devrandom/gitian-builder.git
    git clone https://github.com/FeatherCoin/Feathercoin.git feathercoin

### Write the release notes

Write the release notes. `git shortlog` helps a lot, for example:

    git shortlog --no-merges v(current version, e.g. 0.19.2)..v(new version, e.g. 0.20.0)

Generate list of authors:

    git log --format='- %aN' v(current version, e.g. 0.20.0)..v(new version, e.g. 0.20.1) | sort -fiu

Tag the version (or release candidate) in git:

    git tag -s v(new version, e.g. 0.20.0)

### Setup and perform Gitian builds

Set the version and your GPG signer:

    export SIGNER="(your release signing key name or fingerprint)"
    export VERSION="(new version, e.g. 0.21.0)"

Ensure the Feathercoin checkout is at the exact tag or branch you want to build:

    pushd ./feathercoin
    git fetch
    git checkout v${VERSION}
    popd

Ensure `gitian-builder` is up-to-date:

    pushd ./gitian-builder
    git pull
    popd

### Fetch and create inputs: (first time, or when dependency versions change)

    pushd ./gitian-builder
    mkdir -p inputs
    wget -O inputs/osslsigncode-2.0.tar.gz https://github.com/mtrojnar/osslsigncode/archive/2.0.tar.gz
    echo '5a60e0a4b3e0b4d655317b2f12a810211c50242138322b16e7e01c6fbb89d92f inputs/osslsigncode-2.0.tar.gz' | sha256sum -c
    popd

Create the macOS SDK tarball, see the [macdeploy instructions](/contrib/macdeploy/README.md#deterministic-macos-dmg-notes) for details, and copy it into the inputs directory.

### Optional: Seed the Gitian sources cache

NOTE: Gitian is sometimes unable to download files. If you have errors, try the step below.

By default, Gitian will fetch source files as needed. To cache them ahead of time, make sure you have checked out the tag you want to build in feathercoin, then:

    pushd ./gitian-builder
    make -C ../feathercoin/depends download SOURCES_PATH=`pwd`/cache/common
    popd

Only missing files will be fetched, so this is safe to re-run for each build.

### Build unsigned Feathercoin artifacts

Use the wrapper script. Docker mode is the preferred host setup on modern systems:

    ./contrib/gitian-build.py -S -d
    ./contrib/gitian-build.py -b -d --signer "$SIGNER" "$VERSION"

Build output expected:

  1. source tarball (`feathercoin-${VERSION}.tar.gz`)
  2. linux dist tarballs (`feathercoin-${VERSION}-*-linux*.tar.gz`)
  3. windows unsigned installer and zip (`feathercoin-${VERSION}-win64-setup-unsigned.exe`, `feathercoin-${VERSION}-win64.zip`)
  4. macOS unsigned installer and dist tarball (`feathercoin-${VERSION}-osx-unsigned.dmg`, `feathercoin-${VERSION}-osx64.tar.gz`)
  5. local Gitian attestation files in `attestations/`

### Verify local or collected attestations

To verify against the attestation files in `attestations/`:

    ./contrib/gitian-build.py -v "$VERSION"

If you have collected attestations from other builders, place them in another directory
and point the verifier at it:

    ./contrib/gitian-build.py -v --attestations-dir /path/to/attestations "$VERSION"

### Create and sign SHA256SUMS

From the release artifact directory:

    cd feathercoin-binaries/${VERSION}
    ../../feathercoin/contrib/release/create-sha256sums.sh .

This creates `SHA256SUMS.asc` and excludes `*-debug*` archives and attestation files.

### Publish the release

Publish the unsigned artifacts to the GitHub release page together with:

- `SHA256SUMS.asc`
- the release notes
- optionally a tarball of the `attestations/` entries from builders who reproduced the release

The `*-debug*` files generated by Gitian contain debug symbols for developers.
Do not publish them as normal end-user release assets.

### Additional information

#### <a name="how-to-calculate-assumed-blockchain-and-chain-state-size"></a>How to calculate `m_assumed_blockchain_size` and `m_assumed_chain_state_size`

Both variables are used as a guideline for how much space the user needs on their drive in total, not just strictly for the blockchain.
Note that all values should be taken from a **fully synced** node and have an overhead of 5-10% added on top of its base value.

To calculate `m_assumed_blockchain_size`:
- For `mainnet` -> Take the size of the data directory, excluding `/regtest` and `/testnet3` directories.
- For `testnet` -> Take the size of the `/testnet3` directory.


To calculate `m_assumed_chain_state_size`:
- For `mainnet` -> Take the size of the `/chainstate` directory.
- For `testnet` -> Take the size of the `/testnet3/chainstate` directory.

Notes:
- When taking the size for `m_assumed_blockchain_size`, there's no need to exclude the `/chainstate` directory since it's a guideline value and an overhead will be added anyway.
- The expected overhead for growth may change over time, so it may not be the same value as last release; pay attention to that when changing the variables.
