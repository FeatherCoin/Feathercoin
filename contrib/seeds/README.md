# Seeds

Utility to generate the seeds.txt list that is compiled into the client
(see [src/chainparamsseeds.h](/src/chainparamsseeds.h) and other utilities in [contrib/seeds](/contrib/seeds)).

Be sure to update `PATTERN_AGENT` in `makeseeds.py` to include the current version,
and remove old versions as necessary (at a minimum when GetDesirableServiceFlags
changes its default return value, as those are the services which seeds are added
to addrman with).

The checked-in `nodes_main.txt` and `nodes_test.txt` files are the source for
the fixed seeds compiled into the release. After updating those files from
Feathercoin peer data, regenerate `src/chainparamsseeds.h` like this:

    python3 generate-seeds.py . > ../../src/chainparamsseeds.h

If refreshing `nodes_main.txt` from crawler output, use Feathercoin DNS seed or
peer data in `makeseeds.py` input format, then filter it first:

    python3 makeseeds.py < seeds_main.txt > nodes_main.txt

Do not use Bitcoin seed data here; fixed seeds are a bootstrap fallback for the
Feathercoin network and should use Feathercoin peers and ports.

## Dependencies

Ubuntu:

    sudo apt-get install python3-dnspython
