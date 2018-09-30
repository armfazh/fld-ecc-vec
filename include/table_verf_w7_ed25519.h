/**
 * Copyright 2017 Armando Faz Hernández
 * This file is part of faz_ecc_avx2.
 *
 * faz_ecc_avx2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * faz_ecc_avx2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with faz_ecc_avx2.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TABLES_VERIF_ED255_W7_H
#define TABLES_VERIF_ED255_W7_H

#include <faz_fp_avx2.h>

#define OMEGA_STATIC 7

/**
 * Table for verification
 * Multiples of the generator point <G>=E[r]
 * Num Points: 2^(w-2)
 * w=7 w-NAF
 * Given a point P=(x,y,z=1) pre-compute:
 *      subYX = y-x
 *      addYX = x+y
 *      _2TYX = 2*x*y
 * Format for each pre-computed point:
 *     [x+y, y-x, 2*d*x*y]
 **/
static const ALIGN uint64_t TableVerif_static_w7_Ed25519[(1 << (OMEGA_STATIC - 2)) * 3 * NUM_DIGITS_FP25519_X64] = {
/* 1P  */
    0x2fbc93c6f58c3b85, 0xcf932dc6fb8c0e19, 0x270b4898643d42c2, 0x07cf9d3a33d4ba65,
    0x9d103905d740913e, 0xfd399f05d140beb3, 0xa5c18434688f8a09, 0x44fd2f9298f81267,
    0xabc91205877aaa68, 0x26d9e823ccaac49e, 0x5a1b7dcbdd43598c, 0x6f117b689f0c65a8,
/* 3P  */
    0xaf25b0a84cee9730, 0x025a8430e8864b8a, 0xc11b50029f016732, 0x7a164e1b9a80f8f4,
    0x56611fe8a4fcd265, 0x3bd353fde5c1ba7d, 0x8131f31a214bd6bd, 0x2ab91587555bda62,
    0x14ae933f0dd0d889, 0x589423221c35da62, 0xd170e5458cf2db4c, 0x5a2826af12b9b4c6,
/* 5P  */
    0xa212bc4408a5bb33, 0x8d5048c3c75eed02, 0xdd1beb0c5abfec44, 0x2945ccf146e206eb,
    0x7f9182c3a447d6ba, 0xd50014d14b2729b7, 0xe33cf11cb864a087, 0x154a7e73eb1b55f3,
    0xbcbbdbf1812a8285, 0x270e0807d0bdd1fc, 0xb41b670b1bbda72d, 0x43aabe696b3bb69a,
/* 7P  */
    0x6b1a5cd0944ea3bf, 0x7470353ab39dc0d2, 0x71b2528228542e49, 0x461bea69283c927e,
    0xba6f2c9aaa3221b1, 0x6ca021533bba23a7, 0x9dea764f92192c3a, 0x1d6edd5d2e5317e0,
    0xf1836dc801b8b3a2, 0xb3035f47053ea49a, 0x529c41ba5877adf3, 0x7a9fbb1c6a0f90a7,
/* 9P  */
    0x9b2e678aa6a8632f, 0xa6509e6f51bc46c5, 0xceb233c9c686f5b5, 0x34b9ed338add7f59,
    0xf36e217e039d8064, 0x98a081b6f520419b, 0x96cbc608e75eb044, 0x49c05a51fadc9c8f,
    0x06b4e8bf9045af1b, 0xe2ff83e8a719d22f, 0xaaf6fc2993d4cf16, 0x73c172021b008b06,
/* 11P  */
    0x2fbf00848a802ade, 0xe5d9fecf02302e27, 0x113e847117703406, 0x4275aae2546d8faf,
    0x315f5b0249864348, 0x3ed6b36977088381, 0xa3a075556a8deb95, 0x18ab598029d5c77f,
    0xd82b2cc5fd6089e9, 0x031eb4a13282e4a4, 0x44311199b51a8622, 0x3dc65522b53df948,
/* 13P  */
    0xbf70c222a2007f6d, 0xbf84b39ab5bcdedb, 0x537a0e12fb07ba07, 0x234fd7eec346f241,
    0x506f013b327fbf93, 0xaefcebc99b776f6b, 0x9d12b232aaad5968, 0x0267882d176024a7,
    0x5360a119732ea378, 0x2437e6b1df8dd471, 0xa2ef37f891a7e533, 0x497ba6fdaa097863,
/* 15P  */
    0x24cecc0313cfeaa0, 0x8648c28d189c246d, 0x2dbdbdfac1f2d4d0, 0x61e22917f12de72b,
    0x040bcd86468ccf0b, 0xd3829ba42a9910d6, 0x7508300807b25192, 0x43b5cd4218d05ebf,
    0x5d9a762f9bd0b516, 0xeb38af4e373fdeee, 0x032e5a7d93d64270, 0x511d61210ae4d842,
/* 17P  */
    0x92c676ef950e9d81, 0xa54620cdc0d7044f, 0xaa9b36646f8f1248, 0x6d325924ddb855e3,
    0x081386484420de87, 0x8a1cf016b592edb4, 0x39fa4e2729942d25, 0x71a7fe6fe2482810,
    0x6c7182b8a5c8c854, 0x33fd1479fe5f2a03, 0x72cf591883778d0c, 0x4746c4b6559eeaa9,
/* 19P  */
    0xd3777b3c6dc69a2b, 0xdefab2276f89f617, 0x45651cf7b53a16b5, 0x5c9a51de34fe9fb7,
    0x348546c864741147, 0x7d35aedd0efcc849, 0xff939a760672a332, 0x219663497db5e6d6,
    0xf510f1cf79f10e67, 0xffdddaa1e658515b, 0x09c3a71710142277, 0x4804503c608223bb,
/* 21P  */
    0xc4249ed02ca37fc7, 0xa059a0e3a615acab, 0x88a96ed7c96e0e23, 0x553398a51650696d,
    0x3b6821d23a36d175, 0xbbb40aa7e99b9e32, 0x5d9e5ce420838a47, 0x771e098858de4c5e,
    0x9a12f5d278451edf, 0x3ada5d7985899ccb, 0x477f4a2d9fa59508, 0x5a5ed1d68ff5a611,
/* 23P  */
    0x1195122afe150e83, 0xcf209a257e4b35d8, 0x7387f8291e711e20, 0x44acb897d8bf92f0,
    0xbae5e0c558527359, 0x392e5c19cadb9d7e, 0x28653c1eda1cabe9, 0x019b60135fefdc44,
    0x1e6068145e134b83, 0xc4f5e64f24304c16, 0x506e88a8fc1a3ed7, 0x150c49fde6ad2f92,
/* 25P  */
    0x8e7bf29509471138, 0x5d6fef394f75a651, 0x10af79c425a708ad, 0x6b2b5a075bb99922,
    0xb849863c9cdca868, 0xc83f44dbb8714ad0, 0xfe3ee3560c36168d, 0x78a6d7791e05fbc1,
    0x58bf704b47a0b976, 0xa601b355741748d5, 0xaa2b1fb1d542f590, 0x725c7ffc4ad55d00,
/* 27P  */
    0xe4426715d1cf99b2, 0x7352d51102a20d34, 0x23d1157b8b12109f, 0x794cc9277cb1f3a3,
    0x91802bf71cd098c0, 0xfe416ca4ed5e6366, 0xdf585d714902994c, 0x4cd54625f855fae7,
    0x4af6c426c2ac5053, 0xbc9aedad32f67258, 0x2ad032f10a311021, 0x7008357b6fcc8e85,
/* 29P  */
    0x0b88672738773f01, 0xb8ccc8fa95fbccfb, 0x8d2dd5a3b9ad29b6, 0x06ef7e9851ad0f6a,
    0xd01b9fbb82584a34, 0x47ab6463d2b4792b, 0xb631639c48536202, 0x13a92a3669d6d428,
    0xca93771cc0577de5, 0x7540e41e5035dc5c, 0x24680f01d802e071, 0x3c296ddf8a2af86a,
/* 31P  */
    0xaead15f9d914a713, 0xa92f7bf98c8ff912, 0xaff823179f53d730, 0x7a99d393490c77ba,
    0xfceb4d2ebb1f2541, 0xb89510c740adb91f, 0xfc71a37dd0a1ad05, 0x0a892c700747717b,
    0x8f52ed2436bda3e8, 0x77a8c84157e80794, 0xa5a96563262f9ce0, 0x286762d28302f7d2,
/* 33P  */
    0x4e7836093ce35b25, 0x82e1181db26baa97, 0x0cc192d3cbc7b83f, 0x32f1da046a9d9d3a,
    0x7c558e2bce2ef5bd, 0xe4986cb46747bc63, 0x154a179f3bbb89b8, 0x7686f2a3d6f1767a,
    0xaa8d12a66d597c6a, 0x8f11930304d3852b, 0x3f91dc73c209b022, 0x561305f8a9ad28a6,
/* 35P  */
    0x100c978dec92aed1, 0xca43d5434d6d73e5, 0x83131b22d847ba48, 0x00aaec53e35d4d2c,
    0x6722cc28e7b0c0d5, 0x709de9bbdb075c53, 0xcaf68da7d7010a61, 0x030a1aef2c57cc6c,
    0x7bb1f773003ad2aa, 0x0b3f29802b216608, 0x7821dc86520ed23e, 0x20be9c1c24065480,
/* 37P  */
    0xe15387d8249673a6, 0x5943bc2df546e493, 0x1c7f9a81c36f63b5, 0x750ab3361f0ac1de,
    0x20e0e44ae2025e60, 0xb03b3b2fcbdcb938, 0x105d639cf95a0d1c, 0x69764c545067e311,
    0x1e8a3283a2f81037, 0x6f2eda23bd7fcbf1, 0xb72fd15bac2e2563, 0x54f96b3fb7075040,
/* 39P  */
    0x0fadf20429669279, 0x3adda2047d7d724a, 0x6f3d94828c5760f1, 0x3d7fe9c52bb7539e,
    0x177dafc616b11ecd, 0x89764b9cfa576479, 0xb7a8a110e6ece785, 0x78e6839fbe85dbf0,
    0x70332df737b8856b, 0x75d05d43041a178a, 0x320ff74aa0e59e22, 0x70f268f350088242,
/* 41P  */
    0x66864583b1805f47, 0xf535c5d160dd7c19, 0xe9874eb71e4cb006, 0x7c0d345cfad889d9,
    0x2324112070dcf355, 0x380cc97ee7fce117, 0xb31ddeed3552b698, 0x404e56c039b8c4b9,
    0x591f1f4b8c78338a, 0xa0366ab167e0b5e1, 0x5cbc4152b45f3d44, 0x20d754762aaec777,
/* 43P  */
    0x5e8fc36fc73bb758, 0xace543a5363cbb9a, 0xa9934a7d903bc922, 0x2b8f1e46f3ceec62,
    0x9d74feb135b9f543, 0x84b37df1de8c956c, 0xe9322b0757138ba9, 0x38b8ada8790b4ce1,
    0xb5c04a9cdf51f95d, 0x2b3952aecb1fdeac, 0x1d106d8b328b66da, 0x049aeb32ceba1953,
/* 45P  */
    0xaa507d0b75fc7931, 0x0fef924b7a6725d3, 0x1d82542b396b3930, 0x795ee17530f674fc,
    0xd7767d3c63dcfe7e, 0x209c594897856e40, 0xb6676861e14f7c13, 0x51c665e0c8d625fc,
    0x254a5b0a52ecbd81, 0x5d411f6ee034afe7, 0xe6a24d0dcaee4a31, 0x6cd19bf49dc54477,
/* 47P  */
    0x1ffe612165afc386, 0x082a2a88b8d51b10, 0x76f6627e20990baa, 0x5e01b3a7429e43e7,
    0x7e87619052179ca3, 0x571d0a060b2c9f85, 0x80a2baa88499711e, 0x7520f3db40b2e638,
    0x3db50be3d39357a1, 0x967b6cdd599e94a5, 0x1a309a64df311e6e, 0x71092c9ccef3c986,
/* 49P  */
    0x856bd8ac74051dcf, 0x03f6a40855b7aa1e, 0x3a4ae7cbc9743ceb, 0x4173a5bb7137abde,
    0x53d8523f0364918c, 0xa2b404f43fab6b1c, 0x080b4a9e6681e5a4, 0x0ea15b03d0257ba7,
    0x17c56e31f0f9218a, 0x5a696e2b1afc4708, 0xf7931668f4b2f176, 0x5fc565614a4e3a67,
/* 51P  */
    0x4892e1e67790988e, 0x01d5950f1c5cd722, 0xe3b0819ae5923eed, 0x3214c7409d46651b,
    0x136e570dc46d7ae5, 0x0fd0aacc54f8dc8f, 0x59549f03310dad86, 0x62711c414c454aa1,
    0x1329827406651770, 0x3ba4a0668a279436, 0xd9b6b8ec185d223c, 0x5bea94073ecb833c,
/* 53P  */
    0xb470ce63f343d2f8, 0x0067ba8f0543e8f1, 0x35da51a1a2117b6f, 0x4ad0785944f1bd2f,
    0x641dbf0912c89be4, 0xacf38b317d6e579c, 0xabfe9e02f697b065, 0x3aacd5c148f61eec,
    0x858e3b34c3318301, 0xdc99c04707316826, 0x34085b2ed39da88c, 0x3aff0cb1d902853d,
/* 55P  */
    0x9226430bf4c53505, 0x68e49c13261f2283, 0x09ef33788fd327c6, 0x2ccf9f732bd99e7f,
    0x87c5c7eb3a20405e, 0x8ee311efedad56c9, 0x29252e48ad29d5f9, 0x110e7e86f4cd251d,
    0x57c0d89ed603f5e4, 0x12888628f0b0200c, 0x53172709a02e3bb7, 0x05c557e0b9693a37,
/* 57P  */
    0xf776bbb089c20eb0, 0x61f85bf6fa0fd85c, 0xb6b93f4e634421fb, 0x289fef0841861205,
    0xd8f9ce311fc97e6f, 0x7a3f263011f9fdae, 0xe15b7ea08bed25dd, 0x6e154c178fe9875a,
    0xcf616336fed69abf, 0x9b16e4e78335c94f, 0x13789765753a7fe7, 0x6afbf642a95ca319,
/* 59P  */
    0x5de55070f913a8cc, 0x7d1d167b2b0cf561, 0xda2956b690ead489, 0x12c093cedb801ed9,
    0x7da8de0c62f5d2c1, 0x98fc3da4b00e7b9a, 0x7deb6ada0dad70e0, 0x0db4b851b95038c4,
    0xfc147f9308b8190f, 0x06969da0a11ae310, 0xcee75572dac7d7fd, 0x33aa8799c6635ce6,
/* 61P  */
    0x8348f588fc156cb1, 0x6da2ba9b1a0a6d27, 0xe2262d5c87ca5ab6, 0x212cd0c1c8d589a6,
    0xaf0ff51ebd085cf2, 0x78f51a8967d33f1f, 0x6ec2bfe15060033c, 0x233c6f29e8e21a86,
    0xd2f4d5107f18c781, 0x122ecdf2527e9d28, 0xa70a862a3d3d3341, 0x1db7778911914ce3,
/* 63P  */
    0xb3394769dd701ab6, 0xe2b8ded419cf8da5, 0x15df4161fd2ac852, 0x7ae2ca8a017d24be,
    0xddf352397c6bc26f, 0x7a97e2cc53d50113, 0x7c74f43abf79a330, 0x31ad97ad26e2adfc,
    0xb7e817ed0920b962, 0x1e8518cc3f19da9d, 0xe491c14f25560a64, 0x1ed1fc53a6622c83
};

#endif  /* TABLES_VERIF_ED255_W7_H */
