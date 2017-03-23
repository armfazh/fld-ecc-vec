
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "faz_fp_avx2.h"

#include "util.c"
#include "element_1w_x64.c"
#include "element_1w_h0h5.c"
#include "element_2w_h0h5.c"
#include "element_4w_h0h5.c"
#include "element_1w_h0h8.c"
#include "element_2w_h0h8.c"
#include "element_4w_h0h8.c"

const struct _struct_Fp Fp = {
		.fp25519 = {
				._1way_x64 = {
						.add   = add_Element_1w_x64,
						.sub   = sub_Element_1w_x64,
						.mul   = mul_Element_1w_x64,
						.sqr   = sqr_Element_1w_x64,
						.inv   = inv_Element_1w_x64,
						.sqrt  = sqrt_Element_1w_x64,
						.cred  = fred_Element_1w_x64,
						.cmp   = compare_Element_1w_x64,
						.rand  = random_Element_1w_x64,
						.print = print_Element_1w_x64,
						.init = init_Element_1w_x64,
						.clear = deallocate_bytes
				},
				._1way = {
						.add   = add_Element_1w_h0h5,
						.sub   = sub_Element_1w_h0h5,
						.mul   = mul_Element_1w_h0h5,
						.sqr   = sqr_Element_1w_h0h5,
						.inv   = inv_Element_1w_h0h5,
						.sqrt  = sqrt_Element_1w_h0h5,
						.cred  = compress_Element_1w_h0h5,
						.cmp   = compare_Element_1w_h0h5,
						.rand  = random_Element_1w_h0h5,
						.print = print_Element_1w_h0h5,
						.init = init_Element_1w_h0h5,
						.clear = deallocate_bytes
				},
				._2way = {
						.add = add_Element_2w_h0h5,
						.sub = sub_Element_2w_h0h5,
						.mul = mul_Element_2w_h0h5,
						.sqr = sqr_Element_2w_h0h5,
						.cred = compress_Element_2w_h0h5,
						.cmp = compare_Element_2w_h0h5,
						.rand = random_Element_2w_h0h5,
						.print = print_Element_2w_h0h5,
						.init = init_Element_2w_h0h5,
						.clear = deallocate_bytes
				},
				._4way = {
						.add = add_Element_4w_h0h5,
						.sub = sub_Element_4w_h0h5,
						.mul = mul_Element_4w_h0h5,
						.sqr = sqr_Element_4w_h0h5,
						.cred = compress_Element_4w_h0h5,
						.cmp = compare_Element_4w_h0h5,
						.rand = random_Element_4w_h0h5,
						.print = print_Element_4w_h0h5,
						.init = init_Element_4w_h0h5,
						.clear = deallocate_bytes
				}
		},
		.fp448 = {
				._1way = {
						.add = add_Element_1w_h0h8,
						.sub = sub_Element_1w_h0h8,
						.mul = mul_Element_1w_h0h8,
						.sqr = sqr_Element_1w_h0h8,
						.inv = inv_Element_1w_h0h8,
						.sqrt = sqrt_Element_1w_h0h8,
						.cred = compress_Element_1w_h0h8,
						.cmp = compare_Element_1w_h0h8,
						.rand = random_Element_1w_h0h8,
						.print = print_Element_1w_h0h8,
						.init = init_Element_1w_h0h8,
						.clear = deallocate_bytes
				},
				._2way = {
						.add = add_Element_2w_h0h8,
						.sub = sub_Element_2w_h0h8,
						.mul = mul_Element_2w_h0h8,
						.sqr = sqr_Element_2w_h0h8,
						.cred = compress_Element_2w_h0h8,
						.cmp = compare_Element_2w_h0h8,
						.rand = random_Element_2w_h0h8,
						.print = print_Element_2w_h0h8,
						.init = init_Element_2w_h0h8,
						.clear = deallocate_bytes
				},
				._4way = {
						.add = add_Element_4w_h0h8,
						.sub = sub_Element_4w_h0h8,
						.mul = mul_Element_4w_h0h8,
						.sqr = sqr_Element_4w_h0h8,
						.cred = compress_Element_4w_h0h8,
						.cmp = compare_Element_4w_h0h8,
						.rand = random_Element_4w_h0h8,
						.print = print_Element_4w_h0h8,
						.init = init_Element_4w_h0h8,
						.clear = deallocate_bytes
				}
		}
};


#ifdef __cplusplus
};
#endif /* __cplusplus */
