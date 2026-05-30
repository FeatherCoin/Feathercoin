Feathercoin Core 0.21.0 Release Notes
=====================================

Feathercoin Core 0.21.0 is a major release that rebases Feathercoin from the
Bitcoin Core 0.19 codebase to the Bitcoin Core 0.21.1 codebase while restoring
Feathercoin consensus, network, wallet, fee, branding, and checkpoint behavior.

This release includes the upstream Bitcoin Core changes from 0.20.0, 0.20.1,
0.21.0, and 0.21.1, adapted for the Feathercoin network.

Release binaries and signed hashes are available from:

  <https://github.com/FeatherCoin/Feathercoin/releases>

Please report issues using the GitHub issue tracker:

  <https://github.com/FeatherCoin/Feathercoin/issues>

How to Upgrade
==============

If you are running an older version, shut it down cleanly and wait until it has
fully exited. Then install the new release:

- On Linux, replace the existing `feathercoind`, `feathercoin-qt`,
  `feathercoin-cli`, `feathercoin-tx`, and `feathercoin-wallet` binaries.
- On Windows, run the installer or replace the unpacked binaries.
- On macOS, replace the application bundle or unpacked binaries.

Always back up your wallet before upgrading.

The default data directory remains the Feathercoin data directory. Existing
wallets and chainstate are expected to be used in place.

Compatibility
=============

This release is built for Linux, Windows, and macOS. The macOS release targets
macOS 10.12 and newer.

The Feathercoin Core 0.21.0 macOS release package is x86_64-only. Apple Silicon
users need Rosetta to run the published macOS package. Users who want native
arm64 command-line binaries can build current `master` on Apple Silicon; that
path has been tested for `feathercoind` and `feathercoin-cli` with asm enabled.

The Windows and macOS packages are unsigned. Users should expect operating
system warnings when opening or installing them.

The `peers.dat` format changed upstream in Bitcoin Core 0.21 to support BIP155
addresses, including Tor v3 addresses. If a 0.21 node writes `peers.dat`, older
0.19-based nodes may be unable to read that file after a downgrade. If needed,
remove `peers.dat` and the older node will rebuild its peer database.

Notable Feathercoin Changes
===========================

Bitcoin Core 0.21.1 Codebase
----------------------------

Feathercoin has been ported onto the Bitcoin Core 0.21.1 codebase. This brings
two major upstream release lines worth of node, wallet, RPC, P2P, build-system,
test, and GUI changes while retaining Feathercoin network rules.

Consensus and Proof of Work
---------------------------

- Restored Feathercoin's NeoScrypt proof-of-work path and historical PoW fork
  handling.
- Restored Feathercoin network parameters, genesis blocks, message starts,
  ports, address prefixes, seeds, checkpoints, minimum chain work, and assumed
  valid blocks.
- Restored Feathercoin supply limits, subsidy schedule, retarget behavior,
  fork activation heights, and time-warp protection rules.
- Updated stale-tip and direct-fetch network heuristics for Feathercoin's block
  cadence.

Automatic Checkpointing
-----------------------

ACP synchronized checkpointing has been restored for mainnet, testnet, and
regtest.

ACP is enabled by default. During initial block download, checkpoint messages
are still received and tracked, but enforcement is deferred until the node exits
IBD. This keeps initial sync practical while preserving ACP enforcement once the
node reaches the chain tip.

New and restored ACP-related behavior includes:

- `getcheckpoint` RPC to inspect the current synchronized checkpoint.
- `sendcheckpoint` RPC for checkpoint master nodes.
- `-checkpointenabled` startup option, defaulting to `true`.
- `-checkpointdepth` and `-checkpointkey` startup options.
- ACP service-bit relay between compatible peers.

Disabling ACP with `-checkpointenabled=false` should be treated as a diagnostic
or recovery option. It removes the synchronized checkpoint protection that was
added to defend the chain against deep reorganization attacks.

Fees and Wallet Policy
----------------------

Feathercoin wallet fee defaults have been restored on top of the newer Bitcoin
Core wallet code:

- Default fallback fee: `0.02 FTC/kB`.
- Default wallet minimum transaction fee: `0.001 FTC/kB`.
- Default maximum total wallet transaction fee: `20 FTC`.
- High-fee warning thresholds have been adjusted for Feathercoin values.

These defaults are intended to avoid creating transactions that are likely to be
rejected by existing Feathercoin miners and nodes using the 0.19 policy.

Taproot and Bech32m
-------------------

This release includes the Taproot, Schnorr signature, Tapscript, and Bech32m
support introduced upstream in Bitcoin Core 0.21.1.

For Feathercoin mainnet, Taproot signaling is configured for:

- Start time: 1 June 2026.
- Timeout: 1 June 2027.
- Version bit: 2.

Taproot is not active on mainnet at release time. Do not treat mainnet Taproot
outputs as secure until the deployment has locked in and activated on the
Feathercoin network.

Testnet has its own activation window. Regtest and the new Feathercoin signet
activate the latest rules from genesis for testing.

Signet
------

Feathercoin now includes signet support for controlled test networks. The
default signet parameters have been changed from Bitcoin's defaults to
Feathercoin-specific values, including a Feathercoin signet genesis block,
challenge, ports, message start, and `tf` address prefix.

Upstream Bitcoin Core 0.20 and 0.21 Highlights
==============================================

P2P and Network
---------------

- BIP61 reject messages were removed.
- BIP155 address relay was added, including Tor v3 address support.
- Automatically created Tor onion services now use Tor v3 keys.
- `anchors.dat` is used to reconnect block-relay-only peers after restart.
- BIP157 compact block filters can be served to peers when enabled.
- BIP339 wtxid relay support was added.
- The mempool tracks locally submitted transactions that have not yet been
  acknowledged by peers.
- Peer discouragement replaced parts of the older ban-score behavior.
- `-asmap` support was added for experimental peer diversity by autonomous
  system.

Wallet, GUI, and RPC
--------------------

- Descriptor wallet support was added as an experimental wallet type.
- Descriptor wallets use SQLite instead of Berkeley DB.
- Existing legacy wallets remain supported and remain the default wallet type.
- Automatic wallet creation on startup was removed. Create wallets explicitly
  with the GUI, `createwallet`, or `feathercoin-wallet create`.
- The old `-salvagewallet` startup option was removed. Use
  `feathercoin-wallet salvage` instead.
- `-zapwallettxes` was removed.
- The `send` RPC was added as an experimental transaction creation RPC.
- Wallet RPCs gained improved fee-rate options, including sat/vB fee-rate
  parameters for several calls.
- `getindexinfo` RPC was added.
- `bitcoin-cli -netinfo` and `bitcoin-cli -generate` equivalents are now
  available as `feathercoin-cli -netinfo` and `feathercoin-cli -generate`.
- GUI wallet loading behavior now records GUI-created and GUI-loaded wallets in
  `settings.json` so they can be reloaded on next startup.

Build System and Dependencies
-----------------------------

- OpenSSL is no longer used by the node.
- BIP70 payment request support has been removed.
- Release binaries require glibc 2.17 or newer on Linux.
- Source archives no longer include generated autotools files. When building
  from source archives, run `./autogen.sh` before `./configure`.
- Gitian deterministic build support has been refreshed for Feathercoin,
  including local attestations and signed `SHA256SUMS.asc` release hashes.

Notable RPC and Configuration Changes
=====================================

Some upstream RPC behavior changed between 0.19 and 0.21:

- `getpeerinfo` gained `network`, `last_block`, `last_transaction`, and
  `connection_type` fields.
- `getpeerinfo` no longer returns some deprecated fields by default, including
  `banscore`, `addnode`, and `whitelisted`.
- `getnetworkinfo` now returns separate inbound and outbound connection counts.
- `testmempoolaccept` returns additional size and fee information.
- `sendrawtransaction` and other fee-sensitive RPC paths use newer max-feerate
  handling.
- `rpcwhitelist` and `rpcwhitelistdefault` can restrict RPC users to selected
  calls.
- `startupnotify` can run a command after node startup completes.
- The deprecated `-debug=db` category was removed; use `-debug=walletdb`.
- The `-banscore` option was removed.
- The `-whitelistforcerelay` option was removed.
- `bootstrap.dat` is no longer imported automatically. Use `-loadblock=<file>`.

Release Verification
====================

Release artifacts are accompanied by a clearsigned `SHA256SUMS.asc` file.

The release hashes were signed with:

```text
AD87 DFB2 CBEA 952F 40DC  A637 21E0 9A54 A361 5F99
Bushstar <bushsolo@gmail.com>
```

The generated release assets include:

- Linux binary archives for x86_64, aarch64, armhf, and riscv64.
- Windows 64-bit zip archive and unsigned installer.
- macOS unsigned x86_64 DMG and macOS x86_64 tar archive.
- Source archive.

Known Notes
===========

- Windows and macOS binaries are not code signed.
- The macOS 0.21.0 release package is x86_64-only and requires Rosetta on
  Apple Silicon.
- Taproot is included but is not active on Feathercoin mainnet at release time.
- ACP enforcement is deferred during initial block download and enabled after
  the node reaches the chain tip.
- Descriptor wallets are experimental. Existing legacy wallets remain supported.

Credits
=======

Thanks to the Bitcoin Core contributors whose 0.20 and 0.21 work forms the
base of this release, and to the Feathercoin contributors and testers who
ported and validated the Feathercoin-specific consensus, network, wallet, and
release-build changes.
