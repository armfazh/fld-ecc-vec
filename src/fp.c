#include "fp.h"
#include "random.h"
#include <stdio.h>
#include <stdlib.h>

#include "avx2.h"
#include "element_1w_h0h8.c"
#include "element_2w_h0h8.c"
#include "element_4w_h0h8.c"

const struct _struct_Fp Fp = {
//		.fp25519 = {
//				._1way = {
//						.add   = add_Element_1w_h0h8,
//						.sub   = sub_Element_1w_h0h8,
//						.mul   = mul_Element_1w_h0h8,
//						.sqr   = sqr_Element_1w_h0h8,
//						.inv   = inv_Element_1w_h0h8,
//						.sqrt  = sqrt_Element_1w_h0h8,
//						.cred  = compress_Element_1w_h0h8,
//						.cmp   = compare_Element_1w_h0h8,
//						.rand  = random_Element_1w_h0h8,
//						.print = print_Element_1w_h0h8
//				},
//				._2way = {
//						.add = add_Element_2w_h0h8,
//						.sub = sub_Element_2w_h0h8,
//						.mul = mul_Element_2w_h0h8,
//						.sqr = sqr_Element_2w_h0h8,
//						.cred = compress_Element_2w_h0h8
//				},
//				._4way = {
//						.add = add_Element_4w_h0h8,
//						.sub = sub_Element_4w_h0h8,
//						.mul = mul_Element_4w_h0h8,
//						.sqr = sqr_Element_4w_h0h8,
//						.cred = compress_Element_4w_h0h8
//				}
//		},
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
						.new = new_Element_1w_h0h8,
						.clean = clean_Element_1w_h0h8
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
						.new = new_Element_2w_h0h8,
						.clean = clean_Element_2w_h0h8
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
						.new = new_Element_4w_h0h8,
						.clean = clean_Element_4w_h0h8
				}
		}
};

