// Copyright (c) 2018-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <vector>
#include <string>
#include <script/sign.h>
#include <script/standard.h>
#include <test/setup_common.h>
#include <boost/test/unit_test.hpp>
#include <script/descriptor.h>
#include <util/strencodings.h>

namespace {

void CheckUnparsable(const std::string& prv, const std::string& pub, const std::string& expected_error)
{
    FlatSigningProvider keys_priv, keys_pub;
    std::string error;
    auto parse_priv = Parse(prv, keys_priv, error);
    auto parse_pub = Parse(pub, keys_pub, error);
    BOOST_CHECK_MESSAGE(!parse_priv, prv);
    BOOST_CHECK_MESSAGE(!parse_pub, pub);
    BOOST_CHECK(error == expected_error);
}

constexpr int DEFAULT = 0;
constexpr int RANGE = 1; // Expected to be ranged descriptor
constexpr int HARDENED = 2; // Derivation needs access to private keys
constexpr int UNSOLVABLE = 4; // This descriptor is not expected to be solvable
constexpr int SIGNABLE = 8; // We can sign with this descriptor (this is not true when actual BIP32 derivation is used, as that's not integrated in our signing code)

/** Compare two descriptors. If only one of them has a checksum, the checksum is ignored. */
bool EqualDescriptor(std::string a, std::string b)
{
    bool a_check = (a.size() > 9 && a[a.size() - 9] == '#');
    bool b_check = (b.size() > 9 && b[b.size() - 9] == '#');
    if (a_check != b_check) {
        if (a_check) a = a.substr(0, a.size() - 9);
        if (b_check) b = b.substr(0, b.size() - 9);
    }
    return a == b;
}

std::string UseHInsteadOfApostrophe(const std::string& desc)
{
    std::string ret = desc;
    while (true) {
        auto it = ret.find('\'');
        if (it == std::string::npos) break;
        ret[it] = 'h';
    }

    // GetDescriptorChecksum returns "" if the checksum exists but is bad.
    // Switching apostrophes with 'h' breaks the checksum if it exists - recalculate it and replace the broken one.
    if (GetDescriptorChecksum(ret) == "") {
        ret = ret.substr(0, desc.size() - 9);
        ret += std::string("#") + GetDescriptorChecksum(ret);
    }
    return ret;
}

const std::set<std::vector<uint32_t>> ONLY_EMPTY{{}};

void DoCheck(const std::string& prv, const std::string& pub, int flags, const std::vector<std::vector<std::string>>& scripts, const std::set<std::vector<uint32_t>>& paths = ONLY_EMPTY,
    bool replace_apostrophe_with_h_in_prv=false, bool replace_apostrophe_with_h_in_pub=false)
{
    FlatSigningProvider keys_priv, keys_pub;
    std::set<std::vector<uint32_t>> left_paths = paths;
    std::string error;

    std::unique_ptr<Descriptor> parse_priv;
    std::unique_ptr<Descriptor> parse_pub;
    // Check that parsing succeeds.
    if (replace_apostrophe_with_h_in_prv) {
        parse_priv = Parse(UseHInsteadOfApostrophe(prv), keys_priv, error);
    } else {
        parse_priv = Parse(prv, keys_priv, error);
    }
    if (replace_apostrophe_with_h_in_pub) {
        parse_pub = Parse(UseHInsteadOfApostrophe(pub), keys_pub, error);
    } else {
        parse_pub = Parse(pub, keys_pub, error);
    }

    BOOST_CHECK(parse_priv);
    BOOST_CHECK(parse_pub);

    // Check private keys are extracted from the private version but not the public one.
    BOOST_CHECK(keys_priv.keys.size());
    BOOST_CHECK(!keys_pub.keys.size());

    // Check that both versions serialize back to the public version.
    std::string pub1 = parse_priv->ToString();
    std::string pub2 = parse_pub->ToString();
    BOOST_CHECK(EqualDescriptor(pub, pub1));
    BOOST_CHECK(EqualDescriptor(pub, pub2));

    // Check that both can be serialized with private key back to the private version, but not without private key.
    std::string prv1;
    BOOST_CHECK(parse_priv->ToPrivateString(keys_priv, prv1));
    BOOST_CHECK(EqualDescriptor(prv, prv1));
    BOOST_CHECK(!parse_priv->ToPrivateString(keys_pub, prv1));
    BOOST_CHECK(parse_pub->ToPrivateString(keys_priv, prv1));
    BOOST_CHECK(EqualDescriptor(prv, prv1));
    BOOST_CHECK(!parse_pub->ToPrivateString(keys_pub, prv1));

    // Check whether IsRange on both returns the expected result
    BOOST_CHECK_EQUAL(parse_pub->IsRange(), (flags & RANGE) != 0);
    BOOST_CHECK_EQUAL(parse_priv->IsRange(), (flags & RANGE) != 0);

    // * For ranged descriptors,  the `scripts` parameter is a list of expected result outputs, for subsequent
    //   positions to evaluate the descriptors on (so the first element of `scripts` is for evaluating the
    //   descriptor at 0; the second at 1; and so on). To verify this, we evaluate the descriptors once for
    //   each element in `scripts`.
    // * For non-ranged descriptors, we evaluate the descriptors at positions 0, 1, and 2, but expect the
    //   same result in each case, namely the first element of `scripts`. Because of that, the size of
    //   `scripts` must be one in that case.
    if (!(flags & RANGE)) assert(scripts.size() == 1);
    size_t max = (flags & RANGE) ? scripts.size() : 3;

    // Iterate over the position we'll evaluate the descriptors in.
    for (size_t i = 0; i < max; ++i) {
        // Call the expected result scripts `ref`.
        const auto& ref = scripts[(flags & RANGE) ? i : 0];
        // When t=0, evaluate the `prv` descriptor; when t=1, evaluate the `pub` descriptor.
        for (int t = 0; t < 2; ++t) {
            // When the descriptor is hardened, evaluate with access to the private keys inside.
            const FlatSigningProvider& key_provider = (flags & HARDENED) ? keys_priv : keys_pub;

            // Evaluate the descriptor selected by `t` in poisition `i`.
            FlatSigningProvider script_provider, script_provider_cached;
            std::vector<CScript> spks, spks_cached;
            std::vector<unsigned char> cache;
            BOOST_CHECK((t ? parse_priv : parse_pub)->Expand(i, key_provider, spks, script_provider, &cache));

            // Compare the output with the expected result.
            BOOST_CHECK_EQUAL(spks.size(), ref.size());

            // Try to expand again using cached data, and compare.
            BOOST_CHECK(parse_pub->ExpandFromCache(i, cache, spks_cached, script_provider_cached));
            BOOST_CHECK(spks == spks_cached);
            BOOST_CHECK(script_provider.pubkeys == script_provider_cached.pubkeys);
            BOOST_CHECK(script_provider.scripts == script_provider_cached.scripts);
            BOOST_CHECK(script_provider.origins == script_provider_cached.origins);

            // For each of the produced scripts, verify solvability, and when possible, try to sign a transaction spending it.
            for (size_t n = 0; n < spks.size(); ++n) {
                BOOST_CHECK_EQUAL(ref[n], HexStr(spks[n].begin(), spks[n].end()));
                BOOST_CHECK_EQUAL(IsSolvable(Merge(key_provider, script_provider), spks[n]), (flags & UNSOLVABLE) == 0);

                if (flags & SIGNABLE) {
                    CMutableTransaction spend;
                    spend.vin.resize(1);
                    spend.vout.resize(1);
                    BOOST_CHECK_MESSAGE(SignSignature(Merge(keys_priv, script_provider), spks[n], spend, 0, 1, SIGHASH_ALL), prv);
                }

                /* Infer a descriptor from the generated script, and verify its solvability and that it roundtrips. */
                auto inferred = InferDescriptor(spks[n], script_provider);
                BOOST_CHECK_EQUAL(inferred->IsSolvable(), !(flags & UNSOLVABLE));
                std::vector<CScript> spks_inferred;
                FlatSigningProvider provider_inferred;
                BOOST_CHECK(inferred->Expand(0, provider_inferred, spks_inferred, provider_inferred));
                BOOST_CHECK_EQUAL(spks_inferred.size(), 1);
                BOOST_CHECK(spks_inferred[0] == spks[n]);
                BOOST_CHECK_EQUAL(IsSolvable(provider_inferred, spks_inferred[0]), !(flags & UNSOLVABLE));
                BOOST_CHECK(provider_inferred.origins == script_provider.origins);
            }

            // Test whether the observed key path is present in the 'paths' variable (which contains expected, unobserved paths),
            // and then remove it from that set.
            for (const auto& origin : script_provider.origins) {
                BOOST_CHECK_MESSAGE(paths.count(origin.second.second.path), "Unexpected key path: " + prv);
                left_paths.erase(origin.second.second.path);
            }
        }
    }

    // Verify no expected paths remain that were not observed.
    BOOST_CHECK_MESSAGE(left_paths.empty(), "Not all expected key paths found: " + prv);
}

void Check(const std::string& prv, const std::string& pub, int flags, const std::vector<std::vector<std::string>>& scripts, const std::set<std::vector<uint32_t>>& paths = ONLY_EMPTY)
{
    bool found_apostrophes_in_prv = false;
    bool found_apostrophes_in_pub = false;

    // Do not replace apostrophes with 'h' in prv and pub
    DoCheck(prv, pub, flags, scripts, paths);

    // Replace apostrophes with 'h' in prv but not in pub, if apostrophes are found in prv
    if (prv.find('\'') != std::string::npos) {
        found_apostrophes_in_prv = true;
        DoCheck(prv, pub, flags, scripts, paths, /* replace_apostrophe_with_h_in_prv = */true, /*replace_apostrophe_with_h_in_pub = */false);
    }

    // Replace apostrophes with 'h' in pub but not in prv, if apostrophes are found in pub
    if (pub.find('\'') != std::string::npos) {
        found_apostrophes_in_pub = true;
        DoCheck(prv, pub, flags, scripts, paths, /* replace_apostrophe_with_h_in_prv = */false, /*replace_apostrophe_with_h_in_pub = */true);
    }

    // Replace apostrophes with 'h' both in prv and in pub, if apostrophes are found in both
    if (found_apostrophes_in_prv && found_apostrophes_in_pub) {
        DoCheck(prv, pub, flags, scripts, paths, /* replace_apostrophe_with_h_in_prv = */true, /*replace_apostrophe_with_h_in_pub = */true);
    }
}

}

BOOST_FIXTURE_TEST_SUITE(descriptor_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(descriptor_test)
{
    // Basic single-key compressed
    Check("combo(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq)", "combo(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd)", SIGNABLE, {{"2103a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bdac","76a9149a1c78a507689f6f54b847ad1cef1e614ee23f1e88ac","00149a1c78a507689f6f54b847ad1cef1e614ee23f1e","a91484ab21b1b2fd065d4504ff693d832434b6108d7b87"}});
    Check("pk(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq)", "pk(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd)", SIGNABLE, {{"2103a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bdac"}});
    Check("pkh([deadbeef/1/2'/3/4']N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq)", "pkh([deadbeef/1/2'/3/4']03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd)", SIGNABLE, {{"76a9149a1c78a507689f6f54b847ad1cef1e614ee23f1e88ac"}}, {{1,0x80000002UL,3,0x80000004UL}});
    Check("wpkh(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq)", "wpkh(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd)", SIGNABLE, {{"00149a1c78a507689f6f54b847ad1cef1e614ee23f1e"}});
    Check("sh(wpkh(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq))", "sh(wpkh(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd))", SIGNABLE, {{"a91484ab21b1b2fd065d4504ff693d832434b6108d7b87"}});
    CheckUnparsable("sh(wpkh(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laa))", "sh(wpkh(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5))", "Pubkey '03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5' is invalid"); // Invalid pubkey
    CheckUnparsable("pkh(deadbeef/1/2'/3/4']N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq)", "pkh(deadbeef/1/2'/3/4']03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd)", "Key origin start '[ character expected but not found, got 'd' instead"); // Missing start bracket in key origin
    CheckUnparsable("pkh([deadbeef]/1/2'/3/4']N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq)", "pkh([deadbeef]/1/2'/3/4']03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd)", "Multiple ']' characters found for a single pubkey"); // Multiple end brackets in key origin

    // Basic single-key uncompressed
    Check("combo(5nkz6iXEy7TD8szmbdiPx6L7by196KRNzvqXrHeGwKFJ5xegRPj)", "combo(04a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd5b8dec5235a0fa8722476c7709c02559e3aa73aa03918ba2d492eea75abea235)", SIGNABLE, {{"4104a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd5b8dec5235a0fa8722476c7709c02559e3aa73aa03918ba2d492eea75abea235ac","76a914b5bd079c4d57cc7fc28ecf8213a6b791625b818388ac"}});
    Check("pk(5nkz6iXEy7TD8szmbdiPx6L7by196KRNzvqXrHeGwKFJ5xegRPj)", "pk(04a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd5b8dec5235a0fa8722476c7709c02559e3aa73aa03918ba2d492eea75abea235)", SIGNABLE, {{"4104a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd5b8dec5235a0fa8722476c7709c02559e3aa73aa03918ba2d492eea75abea235ac"}});
    Check("pkh(5nkz6iXEy7TD8szmbdiPx6L7by196KRNzvqXrHeGwKFJ5xegRPj)", "pkh(04a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd5b8dec5235a0fa8722476c7709c02559e3aa73aa03918ba2d492eea75abea235)", SIGNABLE, {{"76a914b5bd079c4d57cc7fc28ecf8213a6b791625b818388ac"}});
    CheckUnparsable("wpkh(5nkz6iXEy7TD8szmbdiPx6L7by196KRNzvqXrHeGwKFJ5xegRPj)", "wpkh(04a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd5b8dec5235a0fa8722476c7709c02559e3aa73aa03918ba2d492eea75abea235)", "Uncompressed keys are not allowed"); // No uncompressed keys in witness
    CheckUnparsable("wsh(pk(5nkz6iXEy7TD8szmbdiPx6L7by196KRNzvqXrHeGwKFJ5xegRPj))", "wsh(pk(04a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd5b8dec5235a0fa8722476c7709c02559e3aa73aa03918ba2d492eea75abea235))", "Uncompressed keys are not allowed"); // No uncompressed keys in witness
    CheckUnparsable("sh(wpkh(5nkz6iXEy7TD8szmbdiPx6L7by196KRNzvqXrHeGwKFJ5xegRPj))", "sh(wpkh(04a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd5b8dec5235a0fa8722476c7709c02559e3aa73aa03918ba2d492eea75abea235))", "Uncompressed keys are not allowed"); // No uncompressed keys in witness

    // Some unconventional single-key constructions
    Check("sh(pk(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq))", "sh(pk(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd))", SIGNABLE, {{"a9141857af51a5e516552b3086430fd8ce55f7c1a52487"}});
    Check("sh(pkh(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq))", "sh(pkh(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd))", SIGNABLE, {{"a9141a31ad23bf49c247dd531a623c2ef57da3c400c587"}});
    Check("wsh(pk(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq))", "wsh(pk(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd))", SIGNABLE, {{"00202e271faa2325c199d25d22e1ead982e45b64eeb4f31e73dbdf41bd4b5fec23fa"}});
    Check("wsh(pkh(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq))", "wsh(pkh(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd))", SIGNABLE, {{"0020338e023079b91c58571b20e602d7805fb808c22473cbc391a41b1bd3a192e75b"}});
    Check("sh(wsh(pk(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq)))", "sh(wsh(pk(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd)))", SIGNABLE, {{"a91472d0c5a3bfad8c3e7bd5303a72b94240e80b6f1787"}});
    Check("sh(wsh(pkh(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq)))", "sh(wsh(pkh(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd)))", SIGNABLE, {{"a914b61b92e2ca21bac1e72a3ab859a742982bea960a87"}});

    // Versions with BIP32 derivations
    Check("combo([01234567]xqMPAtDNjzoK7HC2yahamBSEvXBna8dBtutCzzrG5KSUZ4pu5DtfCruKGrWCLQ3MjY53BwaHkYMtuPbyWLKbXHsJZTXAFw6nFVpjETTefZwCGiN)", "combo([01234567]xq1voveFYRDuJ1PcoiHCLNfcPfGVTbfscS6Khmfk4M2tCQmd8zgFntuyqeZ3tmDpi5ZyUiqK1isuhcqTgmV6neMEbtaBvREoYBDfYQ1HKdKzVZD)", SIGNABLE, {{"210268ba5160475770bcd7649bdf330075cd4cee4c8c41fce33ce927f324f60714fbac","76a914025566b882602393b581083c59663cbb7f94d02f88ac","0014025566b882602393b581083c59663cbb7f94d02f","a914e4a051e7c2bc7b624c0073a631ee87ed24cea95587"}});
    Check("pk(xqMPArqsSpdUEv3WEspgHYMfdPuDuUWAuafTKsX8W954j9P2Grxis7HYMMLm849vugZwZu3KH7vXjMFU3xPbmAR8ADCtzRTcH2ueWGcYjLYRKzy/0)", "pk(xq1vouGkFF44ReF651QHrjb36XyvnwYrd6sa2eLcVAfUNVKkLdkKT9JCv9PcgTeAz96dWyEjXP48MrtMSRGiuuaHaWqj3ZvGqPXygCqHh1JjBPW/0)", DEFAULT, {{"21036c56fa096e7c9ff022101d0f208245a259a6027207788c432da2e403089b55f9ac"}}, {{0}});
    Check("pkh(xqMPAsACVBFvsTa8NPzFmPZhWhWjZ4rB8hU6fNoeXFKj4QeRnEGJAsf1yS7z4FKkWJxa2RDDBWBEBfjY3NxwAabw2u3jUZqcrEkJriGyBMHjvL1/2147483647'/0)", "pkh(xq1voub5HbgX4BmiCXZsLao4yqbSSXtrrDgDN9d8WGv8hkb9r13tkufgYEAqcdpuR9P7GVcsQmAjHg9FUhpvAzfxE9JrkYu8dNNF8qhMjy63Ju1/2147483647'/0)", HARDENED, {{"76a914b0396faa5bfa180ba8730cfd0d915bf7da03acd488ac"}}, {{0xFFFFFFFFUL,0}});
    Check("wpkh([ffffffff/13']xqMPAsrcsnkzeLAagdivjzYCHzXUuVfxXyAXGvEfRSoxxCctr8isv48ayx9b846tbKGEfTxTNSaqLGa11gQwt4VpNZXkXXL61B8Mo1NnRidkxaj/1/2/*)", "wpkh([ffffffff/13']xq1vovHVgDBaq4NAWmJYKBmZm8cBnxieFVNdyh49QUQNbYZcuuWUW69FYkCSgTScUm37LBinuEEiQEinwaWvfog1L81Jz2Xjj1uQhVGJb1g8P89/1/2/*)", RANGE, {{"0014901e9616593a126120e2573e4605541ed70595f2"},{"0014bf36be95ec0e1f45b6629d2d6aeb107015a42c22"},{"0014ebc82c6b0bdd466fdcb3a8730631997bbccfeab6"}}, {{0x8000000DUL, 1, 2, 0}, {0x8000000DUL, 1, 2, 1}, {0x8000000DUL, 1, 2, 2}});
    Check("sh(wpkh(xqMPArwgwe3duahTWQ8Gp6snr4VCS9aia9njhPDxaW2Hzta9aZiBURV5TdbTvzpUS7SaKPxnntHvgb7yi6u3R2AERtiaJtFdW6KNRShpZ5yFEJe/10/20/30/40/*'))", "sh(wpkh(xq1vouNZk4UE6Ju3LXhtPJ7AKCZuKcdQHfzrQA3SZXcheEWseLVn4TVk2ReKVN1scK2GZfD9rc9SCybqbR1t223h4yKkbqArc7c8ToZMoQ3sdU2/10/20/30/40/*'))", RANGE | HARDENED, {{"a914a6fa935b1dbcb2b491a5f6902aa9be3d91faa72887"},{"a9142c06e2fb7feef6139d4b6bacff87d5041d5d03ef87"},{"a9144ef70d919ffc465185af1d6922dfc075b5e219c387"}}, {{10, 20, 30, 40, 0x80000000UL}, {10, 20, 30, 40, 0x80000001UL}, {10, 20, 30, 40, 0x80000002UL}});
    Check("combo(xqMPAtbAo8McgNvBoywTDxsbbvWZe6LVf5SS4zweDA9WdZBtqYUXr3z4U2bvRpuAUXZRJuv9AzU88CCSFPc4tvEuumfSekmTJGYTMjMeCW4uNM1/*)", "combo(xq1vow23bYnCs77me7X4oA6y54bGXZPBNbeYmmm8CBjvGu8cuKG8S5zj2pemzFZXzJmBAmVBTBisWxrSXbyvkZ2q1RPUCR38CQKvxat8qq11u7d/*)", RANGE, {{"210278966575fa9662450d5d16bd4df95d14a1a3b655c5eca988b8bceff5bcbe8bdeac","76a9140b225b6b204eaf959649334d190277553b81211388ac","00140b225b6b204eaf959649334d190277553b812113","a9148064d90d9f663e9d580243f4efa57db7ab91a8ce87"},{"2103073a07d7c9c8b7600bb5f6741642777ca6fb9e92b00c0fc91c464a403a3d7c9dac","76a914eb6d00ad48e359d08872c716a553bc562b78ff0688ac","0014eb6d00ad48e359d08872c716a553bc562b78ff06","a9144b79a59e0bb2f231cba4b3b619d9f9702272616f87"}}, {{0}, {1}});
    CheckUnparsable("combo([012345678]xqMPAtDNjzoK7HC2yahamBSEvXBna8dBtutCzzrG5KSUZ4pu5DtfCruKGrWCLQ3MjY53BwaHkYMtuPbyWLKbXHsJZTXAFw6nFVpjETTefZwCGiN)", "combo([012345678]xq1voveFYRDuJ1PcoiHCLNfcPfGVTbfscS6Khmfk4M2tCQmd8zgFntuyqeZ3tmDpi5ZyUiqK1isuhcqTgmV6neMEbtaBvREoYBDfYQ1HKdKzVZD)", "Fingerprint is not 4 bytes (9 characters instead of 8 characters)"); // Too long key fingerprint
    CheckUnparsable("pkh(xqMPAsBdDK9WoPJgwbH7XjVaNk3F34MapYLZjwgynePo6PhcNPMQnhLrjirn2XbE3vJcYCgKb8FabVSF2eYqB6xLNR6j2mh9cMFvmhhRze6pzEW/2147483648)", "pkh(xq1voucW1ja6z7WGmirj6viwqt7wvXQGY4YgSiWTmfzCjjeLSA91NjMXJWudaunmKaJhPKJ9niQz21ag45eCw9mmcqZHcjSgF8znEMv8jdEZa4C/2147483648)", "Key path value 2147483648 is out of range"); // BIP 32 path element overflow
    CheckUnparsable("pkh(xqMPAsBdDK9WoPJgwbH7XjVaNk3F34MapYLZjwgynePo6PhcNPMQnhLrjirn2XbE3vJcYCgKb8FabVSF2eYqB6xLNR6j2mh9cMFvmhhRze6pzEW/1aa)", "pkh(xq1voucW1ja6z7WGmirj6viwqt7wvXQGY4YgSiWTmfzCjjeLSA91NjMXJWudaunmKaJhPKJ9niQz21ag45eCw9mmcqZHcjSgF8znEMv8jdEZa4C/1aa)", "Key path value '1aa' is not a valid uint32"); // Path is not valid uint

    // Multisig constructions
    Check("multi(1,N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq,5nkz6iXEy7TD8szmbdiPx6L7by196KRNzvqXrHeGwKFJ5xegRPj)", "multi(1,03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd,04a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd5b8dec5235a0fa8722476c7709c02559e3aa73aa03918ba2d492eea75abea235)", SIGNABLE, {{"512103a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd4104a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd5b8dec5235a0fa8722476c7709c02559e3aa73aa03918ba2d492eea75abea23552ae"}});
    Check("sh(multi(2,[00000000/111'/222]xqMPAtDNjzoK7HC2yahamBSEvXBna8dBtutCzzrG5KSUZ4pu5DtfCruKGrWCLQ3MjY53BwaHkYMtuPbyWLKbXHsJZTXAFw6nFVpjETTefZwCGiN,xqMPAtPYpxdhaNTnWbe1KYDXy4rktUDtydidKEk5F1qGigUZcgtCjgfPn3bN9N4V6oRgNDMNNT8pp5pznQLGL4JchCVAFpqYWMsziRQqpzH8EwY/0))", "sh(multi(2,[00000000/111'/222]xq1voveFYRDuJ1PcoiHCLNfcPfGVTbfscS6Khmfk4M2tCQmd8zgFntuyqeZ3tmDpi5ZyUiqK1isuhcqTgmV6neMEbtaBvREoYBDfYQ1HKdKzVZD,xq1vovpRdP4Hm6fNLjDctjSuSCwTmwGah9vk21ZZE3RgN2RHgTfoKig4LqeDhkxx6uxS4mBhqrUmxHwH2P7okjVPuYus7SXGRLDCVF7AZs6FbdJ/0))", DEFAULT, {{"a9140654ee5a7af702bd68848ecd081ac0eb22ab1ab487"}}, {{0x8000006FUL,222},{0}});
    Check("wsh(multi(2,xqMPAsBdDK9WoPJgwbH7XjVaNk3F34MapYLZjwgynePo6PhcNPMQnhLrjirn2XbE3vJcYCgKb8FabVSF2eYqB6xLNR6j2mh9cMFvmhhRze6pzEW/2147483647'/0,xqMPAtFcRyqywUNxSVsGCMqRH2iHGuhqEhSi59M8dEaupgBwSjXLL46qE6bLz3CmL5whicnbrZax1xkHVPiX8w6kHjVScfcqHtRo9i2ood93pUW/1/2/*,xqMPAtC7ZJk8FQx8UTLFkVKWW1m41tvFMDJooFKnFyMPS6uXpthqBrtU2MJvRhyLpF6yZYfE4YE5RJ9rkteGHNAz9MaA7PJzRrtqB8JEgWWFUa6/10/20/30/40/*'))", "wsh(multi(2,xq1voucW1ja6z7WGmirj6viwqt7wvXQGY4YgSiWTmfzCjjeLSA91NjMXJWudaunmKaJhPKJ9niQz21ag45eCw9mmcqZHcjSgF8znEMv8jdEZa4C/2147483647'/0,xq1vovgVEQGa8CaYGdSsmZ4nkAnzANkWxDepmvAccGBKU28fWWJvv67VnteCYQ7WqPht37KySMY7PU1VdcDzqc1yYboGkzmd5tT6TVgWTPNyjpc/1/2/*,xq1vovczMjAiS99iJausKgYsy9qkuMxw4jWvW29GEzwo5SrFtfVRmtu8b9Mmz6y6nhz5eSMKcgyg38szSeCZEsniqLZ3kxYWFkepoBE6sqSkJzq/10/20/30/40/*'))", HARDENED | RANGE, {{"0020af162a179c064447334c1cf5886f8b3e87d7e09696d34f1107d9b2837e3f3d03"},{"00209965a0040bc7e7879e6d8e053da917415b9052d3d45dc7ecbab6ef2ff9139105"},{"0020fad7b7b75f74b407abb217eef086abdc0f0fe1acc8225fa74add75fc98eb7a13"}}, {{0xFFFFFFFFUL,0}, {1,2,0}, {1,2,1}, {1,2,2}, {10, 20, 30, 40, 0x80000000UL}, {10, 20, 30, 40, 0x80000001UL}, {10, 20, 30, 40, 0x80000002UL}});
    Check("sh(wsh(multi(16,N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq,N4wZAT3Betdtcx9kghjVQhNVKu8pHY719VNHfdLMzMaHJkEF76qo,N41d9teHeBFEeoTUAGFiuyXzxETSx4QuKkZ1Bvv5DZTips7NtWKM,N9cSFFHKrPR6Qj41vjPLu4ZLWt9uwRdsa11dnCWWCdXU17FiyHjP,N1sdGKZHXjeBa67DkCS7fDCjGYR9eLjr4jQf4ZhtyPa8o7iPvN15,N8MmmchY3RgLj2b7GdNdywQnafN4AgJWrcKuiwsHq1Wrappfxg4o,N7fLFqgTAsLv17FaJtqutK298MgHQijuao1DnFSBiSE5865ZHSGw,N2b8maeLugbNKUP9tqLHQMzy9ZwppwEDPeMSErZcfFchnK5QAEpa,N9L6uo5CHv98icbr5s9NVARLz5L7sBQauSC2KZkoT7R2ABbppCcZ,N2DiJUBTm9bPRdcQqCsxL49PDyJHstMkqerNaETYSwcteCU5NLtS,N8QLChYdNBMsX126ix1uuz3oS6TyAivPRsqqZYevJjV4U8KMLLAb,N48ShEGuPb2yv5NdRaHTqQVssYRK8fj6a7ZAydsXNcyjRyeWscfi,N6kZ9zTXDqrMBX41VsNrubCVm4suBFiCQ6AfMh4SB5N6jCsZ6fqB,N9WFq818G5H1noZrH3CTKRdxdNuemwYb7tbi91GcH4zHkwfBZSm7,N5RdJAqqBs45oDbvXiEwZCFfYrNkvcPnunZNVuchbgEPwuJwXDnn,N8RDQv3aS4Km37LZiu2W6dwCWW37p4e5uNEsQENLvRcGXswoKYwt)))","sh(wsh(multi(16,03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd,03fdf4907810a9f5d9462a1ae09feee5ab205d32798b0ffcc379442021f84c5bbf,03b96eebdc42daea1ddac8999ac44f5ec1643ac6dd23df86ffea4d5cc00e06ed3c,03e297b18b09a49bc2c41eb4dcea96879152ec337af1ad8c808cd076a5b36ae112,03b8109d94db606e7f0027d45c13bfc2ea0a081aae21325f1a65611759f8db8b2f,02970e4793324d3bc0e24f785ad51343ba32dd8bb7da5a89d0b40fecd61e2327f2,025a103f75ce98386931d052e391ffe2ebe63129c860f8b4cc9a568d10e3ed45d0,02c8f0398a5ebcaee6e70f9c8a8f06d7dd5ac1ff16b7d77733617201beef5bb773,024bb0466c8ea80719dc54f3153f92aa4692b4a4f74bbf3fbbce5e6aa13aa5e004,02826c863e918747d06e1c43da7ef93e7fb0e4eb22b045a6a03e5416ffb429e686,020830ab799a6d4995519a0c96c8548c58dcad443d7a3886213741ba4ccf75235a,02b4532e3096cd9ff2a6eab14ddd376f8358bc9ec66d61d5f985a1a2afe648e05a,0290d0152ab9f0a02e918cacc108d16d668799f572b28304e064e33926e9f48c25,03b9a1c7ab6e47dfc990c249c56f849a472bbfa5395007e1262d161f8ca74580e1,020166db5d46e42e1cfe404570aecc46f14637b448832af64feba34b612dbba906,03bde46fab804dcd8d299ece597a1db723d3e6e1171e490ae2b7c311aba75e98a1)))", SIGNABLE, {{"a9146ebd5f1a4db5fa17a409e3753dbeac8acad9981387"}});
    CheckUnparsable("sh(multi(16,N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq,N4wZAT3Betdtcx9kghjVQhNVKu8pHY719VNHfdLMzMaHJkEF76qo,N41d9teHeBFEeoTUAGFiuyXzxETSx4QuKkZ1Bvv5DZTips7NtWKM,N9cSFFHKrPR6Qj41vjPLu4ZLWt9uwRdsa11dnCWWCdXU17FiyHjP,N1sdGKZHXjeBa67DkCS7fDCjGYR9eLjr4jQf4ZhtyPa8o7iPvN15,N8MmmchY3RgLj2b7GdNdywQnafN4AgJWrcKuiwsHq1Wrappfxg4o,N7fLFqgTAsLv17FaJtqutK298MgHQijuao1DnFSBiSE5865ZHSGw,N2b8maeLugbNKUP9tqLHQMzy9ZwppwEDPeMSErZcfFchnK5QAEpa,N9L6uo5CHv98icbr5s9NVARLz5L7sBQauSC2KZkoT7R2ABbppCcZ,N2DiJUBTm9bPRdcQqCsxL49PDyJHstMkqerNaETYSwcteCU5NLtS,N8QLChYdNBMsX126ix1uuz3oS6TyAivPRsqqZYevJjV4U8KMLLAb,N48ShEGuPb2yv5NdRaHTqQVssYRK8fj6a7ZAydsXNcyjRyeWscfi,N6kZ9zTXDqrMBX41VsNrubCVm4suBFiCQ6AfMh4SB5N6jCsZ6fqB,N9WFq818G5H1noZrH3CTKRdxdNuemwYb7tbi91GcH4zHkwfBZSm7,N5RdJAqqBs45oDbvXiEwZCFfYrNkvcPnunZNVuchbgEPwuJwXDnn,N8RDQv3aS4Km37LZiu2W6dwCWW37p4e5uNEsQENLvRcGXswoKYwt))","sh(multi(16,03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd,03fdf4907810a9f5d9462a1ae09feee5ab205d32798b0ffcc379442021f84c5bbf,03b96eebdc42daea1ddac8999ac44f5ec1643ac6dd23df86ffea4d5cc00e06ed3c,03e297b18b09a49bc2c41eb4dcea96879152ec337af1ad8c808cd076a5b36ae112,03b8109d94db606e7f0027d45c13bfc2ea0a081aae21325f1a65611759f8db8b2f,02970e4793324d3bc0e24f785ad51343ba32dd8bb7da5a89d0b40fecd61e2327f2,025a103f75ce98386931d052e391ffe2ebe63129c860f8b4cc9a568d10e3ed45d0,02c8f0398a5ebcaee6e70f9c8a8f06d7dd5ac1ff16b7d77733617201beef5bb773,024bb0466c8ea80719dc54f3153f92aa4692b4a4f74bbf3fbbce5e6aa13aa5e004,02826c863e918747d06e1c43da7ef93e7fb0e4eb22b045a6a03e5416ffb429e686,020830ab799a6d4995519a0c96c8548c58dcad443d7a3886213741ba4ccf75235a,02b4532e3096cd9ff2a6eab14ddd376f8358bc9ec66d61d5f985a1a2afe648e05a,0290d0152ab9f0a02e918cacc108d16d668799f572b28304e064e33926e9f48c25,03b9a1c7ab6e47dfc990c249c56f849a472bbfa5395007e1262d161f8ca74580e1,020166db5d46e42e1cfe404570aecc46f14637b448832af64feba34b612dbba906,03bde46fab804dcd8d299ece597a1db723d3e6e1171e490ae2b7c311aba75e98a1))", "P2SH script is too large, 547 bytes is larger than 520 bytes"); // P2SH does not fit 16 compressed pubkeys in a redeemscript
    CheckUnparsable("wsh(multi(2,[aaaaaaaa][aaaaaaaa]xqMPAsBdDK9WoPJgwbH7XjVaNk3F34MapYLZjwgynePo6PhcNPMQnhLrjirn2XbE3vJcYCgKb8FabVSF2eYqB6xLNR6j2mh9cMFvmhhRze6pzEW/2147483647'/0,xqMPAtFcRyqywUNxSVsGCMqRH2iHGuhqEhSi59M8dEaupgBwSjXLL46qE6bLz3CmL5whicnbrZax1xkHVPiX8w6kHjVScfcqHtRo9i2ood93pUW/1/2/*,xqMPAtC7ZJk8FQx8UTLFkVKWW1m41tvFMDJooFKnFyMPS6uXpthqBrtU2MJvRhyLpF6yZYfE4YE5RJ9rkteGHNAz9MaA7PJzRrtqB8JEgWWFUa6/10/20/30/40/*'))", "wsh(multi(2,[aaaaaaaa][aaaaaaaa]xq1voucW1ja6z7WGmirj6viwqt7wvXQGY4YgSiWTmfzCjjeLSA91NjMXJWudaunmKaJhPKJ9niQz21ag45eCw9mmcqZHcjSgF8znEMv8jdEZa4C/2147483647'/0,xq1vovgVEQGa8CaYGdSsmZ4nkAnzANkWxDepmvAccGBKU28fWWJvv67VnteCYQ7WqPht37KySMY7PU1VdcDzqc1yYboGkzmd5tT6TVgWTPNyjpc/1/2/*,xq1vovczMjAiS99iJausKgYsy9qkuMxw4jWvW29GEzwo5SrFtfVRmtu8b9Mmz6y6nhz5eSMKcgyg38szSeCZEsniqLZ3kxYWFkepoBE6sqSkJzq/10/20/30/40/*'))", "Multiple ']' characters found for a single pubkey"); // Double key origin descriptor
    CheckUnparsable("wsh(multi(2,[aaaagaaa]xqMPAsBdDK9WoPJgwbH7XjVaNk3F34MapYLZjwgynePo6PhcNPMQnhLrjirn2XbE3vJcYCgKb8FabVSF2eYqB6xLNR6j2mh9cMFvmhhRze6pzEW/2147483647'/0,xqMPAtFcRyqywUNxSVsGCMqRH2iHGuhqEhSi59M8dEaupgBwSjXLL46qE6bLz3CmL5whicnbrZax1xkHVPiX8w6kHjVScfcqHtRo9i2ood93pUW/1/2/*,xqMPAtC7ZJk8FQx8UTLFkVKWW1m41tvFMDJooFKnFyMPS6uXpthqBrtU2MJvRhyLpF6yZYfE4YE5RJ9rkteGHNAz9MaA7PJzRrtqB8JEgWWFUa6/10/20/30/40/*'))", "wsh(multi(2,[aaagaaaa]xq1voucW1ja6z7WGmirj6viwqt7wvXQGY4YgSiWTmfzCjjeLSA91NjMXJWudaunmKaJhPKJ9niQz21ag45eCw9mmcqZHcjSgF8znEMv8jdEZa4C/2147483647'/0,xq1vovgVEQGa8CaYGdSsmZ4nkAnzANkWxDepmvAccGBKU28fWWJvv67VnteCYQ7WqPht37KySMY7PU1VdcDzqc1yYboGkzmd5tT6TVgWTPNyjpc/1/2/*,xq1vovczMjAiS99iJausKgYsy9qkuMxw4jWvW29GEzwo5SrFtfVRmtu8b9Mmz6y6nhz5eSMKcgyg38szSeCZEsniqLZ3kxYWFkepoBE6sqSkJzq/10/20/30/40/*'))", "Fingerprint 'aaagaaaa' is not hex"); // Non hex fingerprint
    CheckUnparsable("wsh(multi(2,[aaaaaaaa],xqMPAtFcRyqywUNxSVsGCMqRH2iHGuhqEhSi59M8dEaupgBwSjXLL46qE6bLz3CmL5whicnbrZax1xkHVPiX8w6kHjVScfcqHtRo9i2ood93pUW/1/2/*,xqMPAtC7ZJk8FQx8UTLFkVKWW1m41tvFMDJooFKnFyMPS6uXpthqBrtU2MJvRhyLpF6yZYfE4YE5RJ9rkteGHNAz9MaA7PJzRrtqB8JEgWWFUa6/10/20/30/40/*'))", "wsh(multi(2,[aaaaaaaa],xq1vovgVEQGa8CaYGdSsmZ4nkAnzANkWxDepmvAccGBKU28fWWJvv67VnteCYQ7WqPht37KySMY7PU1VdcDzqc1yYboGkzmd5tT6TVgWTPNyjpc/1/2/*,xq1vovczMjAiS99iJausKgYsy9qkuMxw4jWvW29GEzwo5SrFtfVRmtu8b9Mmz6y6nhz5eSMKcgyg38szSeCZEsniqLZ3kxYWFkepoBE6sqSkJzq/10/20/30/40/*'))", "No key provided"); // No public key with origin
    CheckUnparsable("wsh(multi(2,[aaaaaaa]xqMPAsBdDK9WoPJgwbH7XjVaNk3F34MapYLZjwgynePo6PhcNPMQnhLrjirn2XbE3vJcYCgKb8FabVSF2eYqB6xLNR6j2mh9cMFvmhhRze6pzEW/2147483647'/0,xqMPAtFcRyqywUNxSVsGCMqRH2iHGuhqEhSi59M8dEaupgBwSjXLL46qE6bLz3CmL5whicnbrZax1xkHVPiX8w6kHjVScfcqHtRo9i2ood93pUW/1/2/*,xqMPAtC7ZJk8FQx8UTLFkVKWW1m41tvFMDJooFKnFyMPS6uXpthqBrtU2MJvRhyLpF6yZYfE4YE5RJ9rkteGHNAz9MaA7PJzRrtqB8JEgWWFUa6/10/20/30/40/*'))", "wsh(multi(2,[aaaaaaa]xq1voucW1ja6z7WGmirj6viwqt7wvXQGY4YgSiWTmfzCjjeLSA91NjMXJWudaunmKaJhPKJ9niQz21ag45eCw9mmcqZHcjSgF8znEMv8jdEZa4C/2147483647'/0,xq1vovgVEQGa8CaYGdSsmZ4nkAnzANkWxDepmvAccGBKU28fWWJvv67VnteCYQ7WqPht37KySMY7PU1VdcDzqc1yYboGkzmd5tT6TVgWTPNyjpc/1/2/*,xq1vovczMjAiS99iJausKgYsy9qkuMxw4jWvW29GEzwo5SrFtfVRmtu8b9Mmz6y6nhz5eSMKcgyg38szSeCZEsniqLZ3kxYWFkepoBE6sqSkJzq/10/20/30/40/*'))", "Fingerprint is not 4 bytes (7 characters instead of 8 characters)"); // Too short fingerprint
    CheckUnparsable("wsh(multi(2,[aaaaaaaaa]xqMPAsBdDK9WoPJgwbH7XjVaNk3F34MapYLZjwgynePo6PhcNPMQnhLrjirn2XbE3vJcYCgKb8FabVSF2eYqB6xLNR6j2mh9cMFvmhhRze6pzEW/2147483647'/0,xqMPAtFcRyqywUNxSVsGCMqRH2iHGuhqEhSi59M8dEaupgBwSjXLL46qE6bLz3CmL5whicnbrZax1xkHVPiX8w6kHjVScfcqHtRo9i2ood93pUW/1/2/*,xqMPAtC7ZJk8FQx8UTLFkVKWW1m41tvFMDJooFKnFyMPS6uXpthqBrtU2MJvRhyLpF6yZYfE4YE5RJ9rkteGHNAz9MaA7PJzRrtqB8JEgWWFUa6/10/20/30/40/*'))", "wsh(multi(2,[aaaaaaaaa]xq1voucW1ja6z7WGmirj6viwqt7wvXQGY4YgSiWTmfzCjjeLSA91NjMXJWudaunmKaJhPKJ9niQz21ag45eCw9mmcqZHcjSgF8znEMv8jdEZa4C/2147483647'/0,xq1vovgVEQGa8CaYGdSsmZ4nkAnzANkWxDepmvAccGBKU28fWWJvv67VnteCYQ7WqPht37KySMY7PU1VdcDzqc1yYboGkzmd5tT6TVgWTPNyjpc/1/2/*,xq1vovczMjAiS99iJausKgYsy9qkuMxw4jWvW29GEzwo5SrFtfVRmtu8b9Mmz6y6nhz5eSMKcgyg38szSeCZEsniqLZ3kxYWFkepoBE6sqSkJzq/10/20/30/40/*'))", "Fingerprint is not 4 bytes (9 characters instead of 8 characters)"); // Too long fingerprint
    CheckUnparsable("multi(a,N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq,5nkz6iXEy7TD8szmbdiPx6L7by196KRNzvqXrHeGwKFJ5xegRPj)", "multi(a,03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd,04a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd5b8dec5235a0fa8722476c7709c02559e3aa73aa03918ba2d492eea75abea235)", "Multi threshold 'a' is not valid"); // Invalid threshold
    CheckUnparsable("multi(0,N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq,5nkz6iXEy7TD8szmbdiPx6L7by196KRNzvqXrHeGwKFJ5xegRPj)", "multi(0,03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd,04a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd5b8dec5235a0fa8722476c7709c02559e3aa73aa03918ba2d492eea75abea235)", "Multisig threshold cannot be 0, must be at least 1"); // Threshold of 0
    CheckUnparsable("multi(3,N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq,5nkz6iXEy7TD8szmbdiPx6L7by196KRNzvqXrHeGwKFJ5xegRPj)", "multi(3,03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd,04a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd5b8dec5235a0fa8722476c7709c02559e3aa73aa03918ba2d492eea75abea235)", "Multisig threshold cannot be larger than the number of keys; threshold is 3 but only 2 keys specified"); // Threshold larger than number of keys
    CheckUnparsable("multi(3,KzoAz5CanayRKex3fSLQ2BwJpN7U52gZvxMyk78nDMHuqrUxuSJy,KwGNz6YCCQtYvFzMtrC6D3tKTKdBBboMrLTsjr2NYVBwapCkn7Mr,KxogYhiNfwxuswvXV66eFyKcCpm7dZ7TqHVqujHAVUjJxyivxQ9X,L2BUNduTSyZwZjwNHynQTF14mv2uz2NRq5n5sYWTb4FkkmqgEE9f)", "multi(3,03669b8afcec803a0d323e9a17f3ea8e68e8abe5a278020a929adbec52421adbd0,0260b2003c386519fc9eadf2b5cf124dd8eea4c4e68d5e154050a9346ea98ce600,0362a74e399c39ed5593852a30147f2959b56bb827dfa3e60e464b02ccf87dc5e8,0261345b53de74a4d721ef877c255429961b7e43714171ac06168d7e08c542a8b8)", "Cannot have 4 pubkeys in bare multisig; only at most 3 pubkeys"); // Threshold larger than number of keys
    CheckUnparsable("sh(multi(16,KzoAz5CanayRKex3fSLQ2BwJpN7U52gZvxMyk78nDMHuqrUxuSJy,KwGNz6YCCQtYvFzMtrC6D3tKTKdBBboMrLTsjr2NYVBwapCkn7Mr,KxogYhiNfwxuswvXV66eFyKcCpm7dZ7TqHVqujHAVUjJxyivxQ9X,L2BUNduTSyZwZjwNHynQTF14mv2uz2NRq5n5sYWTb4FkkmqgEE9f,L1okJGHGn1kFjdXHKxXjwVVtmCMR2JA5QsbKCSpSb7ReQjezKeoD,KxDCNSST75HFPaW5QKpzHtAyaCQC7p9Vo3FYfi2u4dXD1vgMiboK,L5edQjFtnkcf5UWURn6UuuoFrabgDQUHdheKCziwN42aLwS3KizU,KzF8UWFcEC7BYTq8Go1xVimMkDmyNYVmXV5PV7RuDicvAocoPB8i,L3nHUboKG2w4VSJ5jYZ5CBM97oeK6YuKvfZxrefdShECcjEYKMWZ,KyjHo36dWkYhimKmVVmQTq3gERv3pnqA4xFCpvUgbGDJad7eS8WE,KwsfyHKRUTZPQtysN7M3tZ4GXTnuov5XRgjdF2XCG8faAPmFruRF,KzCUbGhN9LJhdeFfL9zQgTJMjqxdBKEekRGZX24hXdgCNCijkkap,KzgpMBwwsDLwkaC5UrmBgCYaBD2WgZ7PBoGYXR8KT7gCA9UTN5a3,KyBXTPy4T7YG4q9tcAM3LkvfRpD1ybHMvcJ2ehaWXaSqeGUxEdkP,KzJDe9iwJRPtKP2F2AoN6zBgzS7uiuAwhWCfGdNeYJ3PC1HNJ8M8,L1xbHrxynrqLKkoYc4qtoQPx6uy5qYXR5ZDYVYBSRmCV5piU3JG9,N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq))","sh(multi(16,03669b8afcec803a0d323e9a17f3ea8e68e8abe5a278020a929adbec52421adbd0,0260b2003c386519fc9eadf2b5cf124dd8eea4c4e68d5e154050a9346ea98ce600,0362a74e399c39ed5593852a30147f2959b56bb827dfa3e60e464b02ccf87dc5e8,0261345b53de74a4d721ef877c255429961b7e43714171ac06168d7e08c542a8b8,02da72e8b46901a65d4374fe6315538d8f368557dda3a1dcf9ea903f3afe7314c8,0318c82dd0b53fd3a932d16e0ba9e278fcc937c582d5781be626ff16e201f72286,0297ccef1ef99f9d73dec9ad37476ddb232f1238aff877af19e72ba04493361009,02e502cfd5c3f972fe9a3e2a18827820638f96b6f347e54d63deb839011fd5765d,03e687710f0e3ebe81c1037074da939d409c0025f17eb86adb9427d28f0f7ae0e9,02c04d3a5274952acdbc76987f3184b346a483d43be40874624b29e3692c1df5af,02ed06e0f418b5b43a7ec01d1d7d27290fa15f75771cb69b642a51471c29c84acd,036d46073cbb9ffee90473f3da429abc8de7f8751199da44485682a989a4bebb24,02f5d1ff7c9029a80a4e36b9a5497027ef7f3e73384a4a94fbfe7c4e9164eec8bc,02e41deffd1b7cce11cde209a781adcffdabd1b91c0ba0375857a2bfd9302419f3,02d76625f7956a7fc505ab02556c23ee72d832f1bac391bcd2d3abce5710a13d06,0399eb0a5487515802dc14544cf10b3666623762fbed2ec38a3975716e2c29c232,03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd))", "Cannot have 17 keys in multisig; must have between 1 and 16 keys, inclusive"); // Cannot have more than 16 keys in a multisig

    // Check for invalid nesting of structures
    CheckUnparsable("sh(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq)", "sh(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd)", "A function is needed within P2SH"); // P2SH needs a script, not a key
    CheckUnparsable("sh(combo(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq))", "sh(combo(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd))", "Cannot have combo in non-top level"); // Old must be top level
    CheckUnparsable("wsh(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq)", "wsh(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd)", "A function is needed within P2WSH"); // P2WSH needs a script, not a key
    CheckUnparsable("wsh(wpkh(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq))", "wsh(wpkh(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd))", "Cannot have wpkh within wsh"); // Cannot embed witness inside witness
    CheckUnparsable("wsh(sh(pk(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq)))", "wsh(sh(pk(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd)))", "Cannot have sh in non-top level"); // Cannot embed P2SH inside P2WSH
    CheckUnparsable("sh(sh(pk(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq)))", "sh(sh(pk(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd)))", "Cannot have sh in non-top level"); // Cannot embed P2SH inside P2SH
    CheckUnparsable("wsh(wsh(pk(N8y92EGUefUgZn5yqs3iJAXN9LBciUJc6gdDAYTXLjY6QVbn6Laq)))", "wsh(wsh(pk(03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd)))", "Cannot have wsh within wsh"); // Cannot embed P2WSH inside P2WSH

    // Checksums
    Check("sh(multi(2,[00000000/111'/222]xqMPAtDNjzoK7HC2yahamBSEvXBna8dBtutCzzrG5KSUZ4pu5DtfCruKGrWCLQ3MjY53BwaHkYMtuPbyWLKbXHsJZTXAFw6nFVpjETTefZwCGiN,xqMPAtPYpxdhaNTnWbe1KYDXy4rktUDtydidKEk5F1qGigUZcgtCjgfPn3bN9N4V6oRgNDMNNT8pp5pznQLGL4JchCVAFpqYWMsziRQqpzH8EwY/0))#upnc5r3m", "sh(multi(2,[00000000/111'/222]xq1voveFYRDuJ1PcoiHCLNfcPfGVTbfscS6Khmfk4M2tCQmd8zgFntuyqeZ3tmDpi5ZyUiqK1isuhcqTgmV6neMEbtaBvREoYBDfYQ1HKdKzVZD,xq1vovpRdP4Hm6fNLjDctjSuSCwTmwGah9vk21ZZE3RgN2RHgTfoKig4LqeDhkxx6uxS4mBhqrUmxHwH2P7okjVPuYus7SXGRLDCVF7AZs6FbdJ/0))#razuzyqt", DEFAULT, {{"a9140654ee5a7af702bd68848ecd081ac0eb22ab1ab487"}}, {{0x8000006FUL,222},{0}});
    Check("sh(multi(2,[00000000/111'/222]xqMPAtDNjzoK7HC2yahamBSEvXBna8dBtutCzzrG5KSUZ4pu5DtfCruKGrWCLQ3MjY53BwaHkYMtuPbyWLKbXHsJZTXAFw6nFVpjETTefZwCGiN,xqMPAtPYpxdhaNTnWbe1KYDXy4rktUDtydidKEk5F1qGigUZcgtCjgfPn3bN9N4V6oRgNDMNNT8pp5pznQLGL4JchCVAFpqYWMsziRQqpzH8EwY/0))", "sh(multi(2,[00000000/111'/222]xq1voveFYRDuJ1PcoiHCLNfcPfGVTbfscS6Khmfk4M2tCQmd8zgFntuyqeZ3tmDpi5ZyUiqK1isuhcqTgmV6neMEbtaBvREoYBDfYQ1HKdKzVZD,xq1vovpRdP4Hm6fNLjDctjSuSCwTmwGah9vk21ZZE3RgN2RHgTfoKig4LqeDhkxx6uxS4mBhqrUmxHwH2P7okjVPuYus7SXGRLDCVF7AZs6FbdJ/0))", DEFAULT, {{"a9140654ee5a7af702bd68848ecd081ac0eb22ab1ab487"}}, {{0x8000006FUL,222},{0}});
    CheckUnparsable("sh(multi(2,[00000000/111'/222]xqMPAtDNjzoK7HC2yahamBSEvXBna8dBtutCzzrG5KSUZ4pu5DtfCruKGrWCLQ3MjY53BwaHkYMtuPbyWLKbXHsJZTXAFw6nFVpjETTefZwCGiN,xqMPAtPYpxdhaNTnWbe1KYDXy4rktUDtydidKEk5F1qGigUZcgtCjgfPn3bN9N4V6oRgNDMNNT8pp5pznQLGL4JchCVAFpqYWMsziRQqpzH8EwY/0))#", "sh(multi(2,[00000000/111'/222]xq1voveFYRDuJ1PcoiHCLNfcPfGVTbfscS6Khmfk4M2tCQmd8zgFntuyqeZ3tmDpi5ZyUiqK1isuhcqTgmV6neMEbtaBvREoYBDfYQ1HKdKzVZD,xq1vovpRdP4Hm6fNLjDctjSuSCwTmwGah9vk21ZZE3RgN2RHgTfoKig4LqeDhkxx6uxS4mBhqrUmxHwH2P7okjVPuYus7SXGRLDCVF7AZs6FbdJ/0))#", "Expected 8 character checksum, not 0 characters"); // Empty checksum
    CheckUnparsable("sh(multi(2,[00000000/111'/222]xqMPAtDNjzoK7HC2yahamBSEvXBna8dBtutCzzrG5KSUZ4pu5DtfCruKGrWCLQ3MjY53BwaHkYMtuPbyWLKbXHsJZTXAFw6nFVpjETTefZwCGiN,xqMPAtPYpxdhaNTnWbe1KYDXy4rktUDtydidKEk5F1qGigUZcgtCjgfPn3bN9N4V6oRgNDMNNT8pp5pznQLGL4JchCVAFpqYWMsziRQqpzH8EwY/0))#upnc5r3mq", "sh(multi(2,[00000000/111'/222]xq1voveFYRDuJ1PcoiHCLNfcPfGVTbfscS6Khmfk4M2tCQmd8zgFntuyqeZ3tmDpi5ZyUiqK1isuhcqTgmV6neMEbtaBvREoYBDfYQ1HKdKzVZD,xq1vovpRdP4Hm6fNLjDctjSuSCwTmwGah9vk21ZZE3RgN2RHgTfoKig4LqeDhkxx6uxS4mBhqrUmxHwH2P7okjVPuYus7SXGRLDCVF7AZs6FbdJ/0))#razuzyqtq", "Expected 8 character checksum, not 9 characters"); // Too long checksum
    CheckUnparsable("sh(multi(2,[00000000/111'/222]xqMPAtDNjzoK7HC2yahamBSEvXBna8dBtutCzzrG5KSUZ4pu5DtfCruKGrWCLQ3MjY53BwaHkYMtuPbyWLKbXHsJZTXAFw6nFVpjETTefZwCGiN,xqMPAtPYpxdhaNTnWbe1KYDXy4rktUDtydidKEk5F1qGigUZcgtCjgfPn3bN9N4V6oRgNDMNNT8pp5pznQLGL4JchCVAFpqYWMsziRQqpzH8EwY/0))#upnc5r3", "sh(multi(2,[00000000/111'/222]xq1voveFYRDuJ1PcoiHCLNfcPfGVTbfscS6Khmfk4M2tCQmd8zgFntuyqeZ3tmDpi5ZyUiqK1isuhcqTgmV6neMEbtaBvREoYBDfYQ1HKdKzVZD,xq1vovpRdP4Hm6fNLjDctjSuSCwTmwGah9vk21ZZE3RgN2RHgTfoKig4LqeDhkxx6uxS4mBhqrUmxHwH2P7okjVPuYus7SXGRLDCVF7AZs6FbdJ/0))#razuzyq", "Expected 8 character checksum, not 7 characters"); // Too short checksum
    CheckUnparsable("sh(multi(3,[00000000/111'/222]xqMPAtDNjzoK7HC2yahamBSEvXBna8dBtutCzzrG5KSUZ4pu5DtfCruKGrWCLQ3MjY53BwaHkYMtuPbyWLKbXHsJZTXAFw6nFVpjETTefZwCGiN,xqMPAtPYpxdhaNTnWbe1KYDXy4rktUDtydidKEk5F1qGigUZcgtCjgfPn3bN9N4V6oRgNDMNNT8pp5pznQLGL4JchCVAFpqYWMsziRQqpzH8EwY/0))#upnc5r3m", "sh(multi(3,[00000000/111'/222]xq1voveFYRDuJ1PcoiHCLNfcPfGVTbfscS6Khmfk4M2tCQmd8zgFntuyqeZ3tmDpi5ZyUiqK1isuhcqTgmV6neMEbtaBvREoYBDfYQ1HKdKzVZD,xq1vovpRdP4Hm6fNLjDctjSuSCwTmwGah9vk21ZZE3RgN2RHgTfoKig4LqeDhkxx6uxS4mBhqrUmxHwH2P7okjVPuYus7SXGRLDCVF7AZs6FbdJ/0))#razuzyqt", "Provided checksum 'razuzyqt' does not match computed checksum '96vumysv'"); // Error in payload
    CheckUnparsable("sh(multi(2,[00000000/111'/222]xqMPAtDNjzoK7HC2yahamBSEvXBna8dBtutCzzrG5KSUZ4pu5DtfCruKGrWCLQ3MjY53BwaHkYMtuPbyWLKbXHsJZTXAFw6nFVpjETTefZwCGiN,xqMPAtPYpxdhaNTnWbe1KYDXy4rktUDtydidKEk5F1qGigUZcgtCjgfPn3bN9N4V6oRgNDMNNT8pp5pznQLGL4JchCVAFpqYWMsziRQqpzH8EwY/0))#upnc4r3m", "sh(multi(2,[00000000/111'/222]xq1voveFYRDuJ1PcoiHCLNfcPfGVTbfscS6Khmfk4M2tCQmd8zgFntuyqeZ3tmDpi5ZyUiqK1isuhcqTgmV6neMEbtaBvREoYBDfYQ1HKdKzVZD,xq1vovpRdP4Hm6fNLjDctjSuSCwTmwGah9vk21ZZE3RgN2RHgTfoKig4LqeDhkxx6uxS4mBhqrUmxHwH2P7okjVPuYus7SXGRLDCVF7AZs6FbdJ/0))#razuzyrt", "Provided checksum 'razuzyrt' does not match computed checksum 'razuzyqt'"); // Error in checksum
    CheckUnparsable("sh(multi(2,[00000000/111'/222]xqMPAtDNjzoK7HC2yahamBSEvXBna8dBtutCzzrG5KSUZ4pu5DtfCruKGrWCLQ3MjY53BwaHkYMtuPbyWLKbXHsJZTXAFw6nFVpjETTefZwCGiN,xqMPAtPYpxdhaNTnWbe1KYDXy4rktUDtydidKEk5F1qGigUZcgtCjgfPn3bN9N4V6oRgNDMNNT8pp5pznQLGL4JchCVAFpqYWMsziRQqpzH8EwY/0))##upnc5r3m", "sh(multi(2,[00000000/111'/222]xq1voveFYRDuJ1PcoiHCLNfcPfGVTbfscS6Khmfk4M2tCQmd8zgFntuyqeZ3tmDpi5ZyUiqK1isuhcqTgmV6neMEbtaBvREoYBDfYQ1HKdKzVZD,xq1vovpRdP4Hm6fNLjDctjSuSCwTmwGah9vk21ZZE3RgN2RHgTfoKig4LqeDhkxx6uxS4mBhqrUmxHwH2P7okjVPuYus7SXGRLDCVF7AZs6FbdJ/0))##razuzyqt", "Multiple '#' symbols"); // Error in checksum

    // Addr and raw tests
    CheckUnparsable("", "addr(asdf)", "Address is not valid"); // Invalid address
    CheckUnparsable("", "raw(asdf)", "Raw script is not hex"); // Invalid script
    CheckUnparsable("", "raw(Ü)#00000000", "Invalid characters in payload"); // Invalid chars
}

BOOST_AUTO_TEST_SUITE_END()
