#!/usr/bin/env python3
# Copyright (c) 2026 The Feathercoin developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test Feathercoin ACP synchronized checkpoint behavior."""

import time

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import assert_equal, assert_raises_rpc_error


CHECKPOINT_KEY = "92y5e757QRFCpbMsBp3UrgE7zsvBrnHtyog5Zt1ELgFRaXEFQKZ"
CHECKPOINT_DEPTH = 5
MAIN_CHAIN_BLOCKS = 20
AUTO_CHECKPOINT_HEIGHT = MAIN_CHAIN_BLOCKS - CHECKPOINT_DEPTH
MANUAL_CHECKPOINT_HEIGHT = MAIN_CHAIN_BLOCKS - 2
ALT_CHAIN_BLOCKS = 30


class CheckpointSyncTest(BitcoinTestFramework):
    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 5
        self.extra_args = [
            [f"-checkpointkey={CHECKPOINT_KEY}", f"-checkpointdepth={CHECKPOINT_DEPTH}"],
            [],
            [],
            ["-checkpointenabled=false"],
            [],
        ]

    def setup_network(self):
        self.setup_nodes()
        self.connect_nodes(0, 1)
        self.sync_blocks(self.nodes[0:2])

    def wait_for_checkpoint(self, node_indexes, checkpoint_hash, height):
        def have_checkpoint():
            for node_index in node_indexes:
                info = self.nodes[node_index].getcheckpoint()
                if info["synccheckpoint"] != checkpoint_hash:
                    return False
                if info.get("height") != height:
                    return False
            return True

        self.wait_until(have_checkpoint)

    def assert_tip_stable(self, node_indexes, expected_hash, expected_height, duration=3):
        stop_time = time.time() + duration * self.options.timeout_factor
        while time.time() < stop_time:
            for node_index in node_indexes:
                assert_equal(self.nodes[node_index].getbestblockhash(), expected_hash)
                assert_equal(self.nodes[node_index].getblockcount(), expected_height)
            time.sleep(0.1)

    def submit_chain(self, source_node, dest_node, height):
        for block_height in range(1, height + 1):
            block_hash = source_node.getblockhash(block_height)
            block_hex = source_node.getblock(block_hash, 0)
            result = dest_node.submitblock(block_hex)
            assert result in (None, "inconclusive")

    def run_test(self):
        self.log.info("build a checkpointed chain and verify ACP RPC state")
        self.nodes[0].generate(MAIN_CHAIN_BLOCKS)
        self.sync_blocks(self.nodes[0:2])

        auto_checkpoint_hash = self.nodes[0].getblockhash(AUTO_CHECKPOINT_HEIGHT)
        self.wait_for_checkpoint([0, 1], auto_checkpoint_hash, AUTO_CHECKPOINT_HEIGHT)

        checkpoint_master_info = self.nodes[0].getcheckpoint()
        assert_equal(checkpoint_master_info["checkpointmaster"], True)
        assert "checkpointmaster" not in self.nodes[1].getcheckpoint()

        assert_raises_rpc_error(
            -1,
            "Not a checkpoint master node",
            self.nodes[1].sendcheckpoint,
            auto_checkpoint_hash,
        )

        self.log.info("manual sendcheckpoint updates the synchronized checkpoint")
        manual_checkpoint_hash = self.nodes[0].getblockhash(MANUAL_CHECKPOINT_HEIGHT)
        manual_result = self.nodes[0].sendcheckpoint(manual_checkpoint_hash)
        assert_equal(manual_result["synccheckpoint"], manual_checkpoint_hash)
        assert_equal(manual_result["height"], MANUAL_CHECKPOINT_HEIGHT)
        self.wait_for_checkpoint([0, 1], manual_checkpoint_hash, MANUAL_CHECKPOINT_HEIGHT)

        main_tip = self.nodes[0].getbestblockhash()

        self.log.info("late joiner receives a pending checkpoint and promotes it after syncing")
        with self.nodes[4].assert_debug_log(["ProcessSyncCheckpoint: pending for sync-checkpoint"]):
            self.connect_nodes(0, 4)
        self.sync_blocks([self.nodes[0], self.nodes[4]])
        self.wait_for_checkpoint([4], manual_checkpoint_hash, MANUAL_CHECKPOINT_HEIGHT)
        assert_equal(self.nodes[4].getbestblockhash(), main_tip)

        self.log.info("ACP-enforced node switches back when the checkpointed chain is available")
        self.nodes[2].generate(ALT_CHAIN_BLOCKS)
        assert_equal(self.nodes[2].getblockcount(), ALT_CHAIN_BLOCKS)
        self.submit_chain(self.nodes[0], self.nodes[2], MAIN_CHAIN_BLOCKS)
        self.connect_nodes(0, 2)
        self.wait_until(lambda: self.nodes[2].getbestblockhash() == main_tip, timeout=30)
        self.wait_for_checkpoint([2], manual_checkpoint_hash, MANUAL_CHECKPOINT_HEIGHT)
        assert_equal(self.nodes[2].getblockcount(), MAIN_CHAIN_BLOCKS)
        assert_equal(self.nodes[2].getbestblockhash(), main_tip)

        self.log.info("checkpointenabled=false keeps the longer chain")
        self.nodes[3].generate(ALT_CHAIN_BLOCKS)
        disabled_tip = self.nodes[3].getbestblockhash()
        self.connect_nodes(0, 3)
        self.connect_nodes(3, 1)
        self.assert_tip_stable([0, 1, 2, 4], main_tip, MAIN_CHAIN_BLOCKS)
        self.assert_tip_stable([3], disabled_tip, ALT_CHAIN_BLOCKS)


if __name__ == '__main__':
    CheckpointSyncTest().main()
