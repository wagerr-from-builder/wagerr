// Copyright (c) 2018-2020 The Dash Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test/test_wagerr.h>

#include <bls/bls.h>
#include <evo/simplifiedmns.h>
#include <netbase.h>

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(evo_simplifiedmns_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(simplifiedmns_merkleroots)
{
    std::vector<CSimplifiedMNListEntry> entries;
    for (size_t i = 0; i < 15; i++) {
        CSimplifiedMNListEntry smle;
        smle.proRegTxHash.SetHex(strprintf("%064x", i));
        smle.confirmedHash.SetHex(strprintf("%064x", i));

        std::string ip = strprintf("%d.%d.%d.%d", 0, 0, 0, i);
        Lookup(ip.c_str(), smle.service, i, false);

        std::vector<unsigned char> vecBytes{static_cast<unsigned char>(i)};
        vecBytes.resize(CBLSSecretKey::SerSize);

        smle.pubKeyOperator.Set(CBLSSecretKey(vecBytes).GetPublicKey());
        smle.keyIDVoting.SetHex(strprintf("%040x", i));
        smle.isValid = true;

        entries.emplace_back(smle);
    }

    std::vector<std::string> expectedHashes = {
        "a0f70cb5fa27737b9dc0d7fc1cfd86e8ac952e873e2b3e55b08089b3fd745107",
        "f9797ba0dc0867a161932d55047619e78026d22224840855a2058d7f80d54ff4",
        "d7a0547ca9c93a36f2156a26aa91d6d8399417d8ddabd4501340396bb62aa232",
        "32a958b2c45b75288d5992d411de9ff43a305a938d38492c5a084ccae391b737",
        "01cfb04ca705aa942e465225376df0cdaf72d3ebf36ad3db2f04073d06de2370",
        "39245f8c7e54419be0ca7761cd568cac106f0f934656227704b0ce03199d4d50",
        "f41153a8ba8b2bf16953a1214637fe10389b96d2e3df3e7932c8c98db9ccf074",
        "fa3c31f45a2457b44d44d1923a31a9ec64419172a0da5736d71d8e85f147e9c4",
        "c0cfefdb9a15c602be1359e5b5725b231cc3c85cbfbaa9b6bcd1d293747df8e9",
        "ab8e1d9c2d7caa0aee429cc1dd13a21e7336a0ceca07b6342e4b16f576b4fe4d",
        "0e0a096447128829bb09d771070f8b905d1e139658a08046125010d17b253847",
        "e376f36071e9d20b8600414b98c6082be96d4459d50afd10de0194af499f0561",
        "7ff9b2c89179116b2922772821f6442b947d805f7a81b925e36583b459a0706c",
        "ca68cd54e190343e8e230cab4813a41763582810a9df3865ddacc789c2e6a463",
        "1a6e74a4a4d763b88fe00a2d25d446db8bc2d19131a4e2434cc69aeaae2cefaa",
    };
    std::vector<std::string> calculatedHashes;

    for (auto& smle : entries) {
        calculatedHashes.emplace_back(smle.CalcHash().ToString());
        //printf("\"%s\",\n", calculatedHashes.back().c_str());
    }

    BOOST_CHECK(expectedHashes == calculatedHashes);

    CSimplifiedMNList sml(entries);

    std::string expectedMerkleRoot = "cd981c5eb6f31d5670ecc6f3648e765473111d87ead8861bbf15d4a609ddf584";
    std::string calculatedMerkleRoot = sml.CalcMerkleRoot(nullptr).ToString();
    //printf("merkleRoot=\"%s\",\n", calculatedMerkleRoot.c_str());

    BOOST_CHECK(expectedMerkleRoot == calculatedMerkleRoot);
}
BOOST_AUTO_TEST_SUITE_END()
