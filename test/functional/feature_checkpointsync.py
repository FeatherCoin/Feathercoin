#!/usr/bin/env python3
# Copyright (c) 2017-2018 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test checkpoint sync."""

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (assert_equal, connect_nodes)


class CheckPointsyncTest(BitcoinTestFramework):
    def setup_network(self):
        #Setup the test network topology

        self.setup_nodes()
        connect_nodes(self.nodes[0], 1)
        self.sync_all(self.nodes[0:2])

    def set_test_params(self):
        self.num_nodes = 3
        self.setup_clean_chain = True
        self.extra_args = [["-checkpointkey=92y5e757QRFCpbMsBp3UrgE7zsvBrnHtyog5Zt1ELgFRaXEFQKZ","-checkpointdepth=5"],[],[]]

    def run_test(self):
        self.log.info("basic checkpoint sync")

        self.nodes[1].generate(200)
        self.sync_all(self.nodes[0:2])

        checkpoint = self.nodes[1].getcheckpoint()["synccheckpoint"]

        assert_equal(self.nodes[1].getblockhash(195), checkpoint)

        self.log.info("ignore uncheckpointed chain")

        self.nodes[2].generate(300)

        assert_equal(self.nodes[2].getblockcount(), 300)

        connect_nodes(self.nodes[1], 0)
        connect_nodes(self.nodes[0], 2)
        connect_nodes(self.nodes[2], 1)

        self.sync_all()

        assert_equal(self.nodes[0].getblockcount(), 200)
        assert_equal(self.nodes[1].getblockcount(), 200)

        self.log.info("uncheckpointed chain switched")

        assert_equal(self.nodes[2].getblockcount(), 200)

if __name__ == '__main__':
    CheckPointsyncTest().main()
