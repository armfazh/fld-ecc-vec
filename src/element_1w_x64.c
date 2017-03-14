
#define copy_Element_1w_x64(C,A)\
	C[0] = A[0];\
	C[1] = A[1];\
	C[2] = A[2];\
	C[3] = A[3];


#define mul_Element_2w_x64(c,a,b)  \
	mul2_256x256_integer_x64(buffer_2w,a,b);\
    red_Element_2w_x64(c,buffer_2w);

#define sqr_Element_2w_x64(a)\
    sqr2_256x256_integer_x64(buffer_2w,a);\
    red_Element_2w_x64(a,buffer_2w);


/**
 *
 * @param c Two 512-bit products: c[0:7]=a[0:3]*b[0:3] and c[8:15]=a[4:7]*b[4:7]
 * @param a Two 256-bit integers: a[0:3] and a[4:7]
 * @param b Two 256-bit integers: b[0:3] and b[4:7]
 */
static void mul2_256x256_integer_x64(uint64_t *const c, uint64_t *const a, uint64_t *const b)
{
#ifdef __BMI2__
#ifdef __ADX__
	__asm__ __volatile__(
	    "movq    (%1), %%rdx         # A[0]  \n\t"
		"mulx	  (%2),  %%r8,  %%r9  # A[0]*B[0] \n\t"    "xorl   %%r10d, %%r10d \n\t"                                      "   movq	%%r8,  (%0)      \n\t"
		"mulx	 8(%2), %%r10, %%r11  # A[0]*B[1] \n\t"    "adox	 %%r9, %%r10  \n\t"                                      "   movq   %%r10, 8(%0)      \n\t"
		"mulx	16(%2), %%r12, %%r13  # A[0]*B[2] \n\t"    "adox	%%r11, %%r12  \n\t"
		"mulx	24(%2), %%r14, %%rdx  # A[0]*B[3] \n\t"    "adox	%%r13, %%r14  \n\t"                                                                          "   movq    $0, %%rax  \n\t"
														   "adox	%%rdx, %%rax  \n\t"

		"movq   8(%1), %%rdx         # A[1]  \n\t"
		"mulx	  (%2),  %%r8,  %%r9  # A[1]*B[0] \n\t"    "xorl   %%r10d, %%r10d \n\t"  "adcx	8(%0),  %%r8  \n\t"    "   movq	 %%r8,  8(%0)    \n\t"
		"mulx	 8(%2), %%r10, %%r11  # A[1]*B[1] \n\t"    "adox 	 %%r9, %%r10  \n\t"  "adcx	%%r12, %%r10  \n\t"    "   movq	%%r10, 16(%0)    \n\t"
		"mulx	16(%2), %%r12, %%r13  # A[1]*B[2] \n\t"    "adox	%%r11, %%r12  \n\t"  "adcx	%%r14, %%r12  \n\t"                                        "   movq    $0, %%r8   \n\t"
		"mulx	24(%2), %%r14, %%rdx  # A[1]*B[3] \n\t"    "adox	%%r13, %%r14  \n\t"  "adcx	%%rax, %%r14  \n\t"                                        "   movq    $0, %%rax  \n\t"
														   "adox 	%%rdx, %%rax  \n\t"  "adcx	 %%r8, %%rax  \n\t"

		"movq  16(%1), %%rdx         # A[2]  \n\t"
		"mulx	  (%2),  %%r8,  %%r9  # A[2]*B[0] \n\t"    "xorl   %%r10d, %%r10d \n\t"  "adcx 16(%0),  %%r8  \n\t"    "   movq	 %%r8, 16(%0)    \n\t"
		"mulx	 8(%2), %%r10, %%r11  # A[2]*B[1] \n\t"    "adox	 %%r9, %%r10  \n\t"  "adcx	%%r12, %%r10  \n\t"    "   movq	%%r10, 24(%0)    \n\t"
		"mulx	16(%2), %%r12, %%r13  # A[2]*B[2] \n\t"    "adox	%%r11, %%r12  \n\t"  "adcx	%%r14, %%r12  \n\t"                                        "   movq    $0, %%r8   \n\t"
		"mulx	24(%2), %%r14, %%rdx  # A[2]*B[3] \n\t"    "adox	%%r13, %%r14  \n\t"  "adcx	%%rax, %%r14  \n\t"                                        "   movq    $0, %%rax  \n\t"
														   "adox	%%rdx, %%rax  \n\t"  "adcx	 %%r8, %%rax  \n\t"

		"movq	24(%1), %%rdx         # A[3]  \n\t"
		"mulx	  (%2),  %%r8,  %%r9  # A[3]*B[0] \n\t"    "xorl   %%r10d, %%r10d \n\t"  "adcx 24(%0),  %%r8  \n\t"    "   movq	 %%r8, 24(%0)    \n\t"
		"mulx	 8(%2), %%r10, %%r11  # A[3]*B[1] \n\t"    "adox	 %%r9, %%r10  \n\t"  "adcx	%%r12, %%r10  \n\t"    "   movq	%%r10, 32(%0)    \n\t"
		"mulx	16(%2), %%r12, %%r13  # A[3]*B[2] \n\t"    "adox	%%r11, %%r12  \n\t"  "adcx	%%r14, %%r12  \n\t"    "   movq	%%r12, 40(%0)    \n\t"  "   movq    $0, %%r8   \n\t"
		"mulx	24(%2), %%r14, %%rdx  # A[3]*B[3] \n\t"    "adox	%%r13, %%r14  \n\t"  "adcx	%%rax, %%r14  \n\t"    "   movq	%%r14, 48(%0)    \n\t"  "   movq    $0, %%rax  \n\t"
														   "adox	%%rdx, %%rax  \n\t"  "adcx	 %%r8, %%rax  \n\t"    "   movq	%%rax, 56(%0)    \n\t"

		"movq  32(%1), %%rdx         # A[0]  \n\t"
		"mulx	32(%2),  %%r8,  %%r9  # A[0]*B[0] \n\t"    "xorl   %%r10d, %%r10d \n\t"                                      "   movq	%%r8, 64(%0)      \n\t"
		"mulx	40(%2), %%r10, %%r11  # A[0]*B[1] \n\t"    "adox	 %%r9, %%r10  \n\t"                                      "   movq   %%r10, 72(%0)      \n\t"
		"mulx	48(%2), %%r12, %%r13  # A[0]*B[2] \n\t"    "adox	%%r11, %%r12  \n\t"
		"mulx	56(%2), %%r14, %%rdx  # A[0]*B[3] \n\t"    "adox	%%r13, %%r14  \n\t"                                                                          "   movq    $0, %%rax  \n\t"
														   "adox	%%rdx, %%rax  \n\t"

		"movq  40(%1), %%rdx         # A[1]  \n\t"
		"mulx	32(%2),  %%r8,  %%r9  # A[1]*B[0] \n\t"    "xorl   %%r10d, %%r10d \n\t"  "adcx 72(%0),  %%r8  \n\t"    "   movq	 %%r8, 72(%0)    \n\t"
		"mulx	40(%2), %%r10, %%r11  # A[1]*B[1] \n\t"    "adox 	 %%r9, %%r10  \n\t"  "adcx	%%r12, %%r10  \n\t"    "   movq	%%r10, 80(%0)    \n\t"
		"mulx	48(%2), %%r12, %%r13  # A[1]*B[2] \n\t"    "adox	%%r11, %%r12  \n\t"  "adcx	%%r14, %%r12  \n\t"                                        "   movq    $0, %%r8   \n\t"
		"mulx	56(%2), %%r14, %%rdx  # A[1]*B[3] \n\t"    "adox	%%r13, %%r14  \n\t"  "adcx	%%rax, %%r14  \n\t"                                        "   movq    $0, %%rax  \n\t"
														   "adox 	%%rdx, %%rax  \n\t"  "adcx	 %%r8, %%rax  \n\t"

		"movq  48(%1), %%rdx         # A[2]  \n\t"
		"mulx	32(%2),  %%r8,  %%r9  # A[2]*B[0] \n\t"    "xorl   %%r10d, %%r10d \n\t"  "adcx 80(%0),  %%r8  \n\t"    "   movq	 %%r8, 80(%0)    \n\t"
		"mulx	40(%2), %%r10, %%r11  # A[2]*B[1] \n\t"    "adox	 %%r9, %%r10  \n\t"  "adcx	%%r12, %%r10  \n\t"    "   movq	%%r10, 88(%0)    \n\t"
		"mulx	48(%2), %%r12, %%r13  # A[2]*B[2] \n\t"    "adox	%%r11, %%r12  \n\t"  "adcx	%%r14, %%r12  \n\t"                                        "   movq    $0, %%r8   \n\t"
		"mulx	56(%2), %%r14, %%rdx  # A[2]*B[3] \n\t"    "adox	%%r13, %%r14  \n\t"  "adcx	%%rax, %%r14  \n\t"                                        "   movq    $0, %%rax  \n\t"
														   "adox	%%rdx, %%rax  \n\t"  "adcx	 %%r8, %%rax  \n\t"

		"movq	56(%1), %%rdx         # A[3]  \n\t"
		"mulx	32(%2),  %%r8,  %%r9  # A[3]*B[0] \n\t"    "xorl   %%r10d, %%r10d \n\t"  "adcx 88(%0),  %%r8  \n\t"    "   movq	 %%r8,  88(%0)    \n\t"
		"mulx	40(%2), %%r10, %%r11  # A[3]*B[1] \n\t"    "adox	 %%r9, %%r10  \n\t"  "adcx	%%r12, %%r10  \n\t"    "   movq	%%r10,  96(%0)    \n\t"
		"mulx	48(%2), %%r12, %%r13  # A[3]*B[2] \n\t"    "adox	%%r11, %%r12  \n\t"  "adcx	%%r14, %%r12  \n\t"    "   movq	%%r12, 104(%0)    \n\t"  "   movq    $0, %%r8   \n\t"
		"mulx	56(%2), %%r14, %%rdx  # A[3]*B[3] \n\t"    "adox	%%r13, %%r14  \n\t"  "adcx	%%rax, %%r14  \n\t"    "   movq	%%r14, 112(%0)    \n\t"  "   movq    $0, %%rax  \n\t"
														   "adox	%%rdx, %%rax  \n\t"  "adcx	 %%r8, %%rax  \n\t"    "   movq	%%rax, 120(%0)    \n\t"
	:
	: "r"  (c), "r" (a), "r"  (b)
	: "memory", "cc", "%rax", "%rdx",
		"%r8",  "%r9", "%r10", "%r11",
		"%r12", "%r13", "%r14"
	);
#else
	__asm__ __volatile__(
		"   movq     (%1), %%rdx  # A[0]                \n\t"
		"   mulx	  (%2),  %%r8,  %%r9  # A[0]*B[0]   \n\t"                                       "   movq	 %%r8,  (%0)  \n\t"
		"   mulx	 8(%2), %%r10, %%rax  # A[0]*B[1]   \n\t"    "   addq	%%r10,  %%r9     \n\t"  "   movq	 %%r9, 8(%0)  \n\t"
		"   mulx	16(%2), %%r12, %%rbx  # A[0]*B[2]   \n\t"    "   adcq	%%r12, %%rax     \n\t"
		"   mulx	24(%2), %%r14, %%rcx  # A[0]*B[3]   \n\t"    "   adcq	%%r14, %%rbx     \n\t"
		"   adcq	   $0, %%rcx                        \n\t"

		"   movq    8(%1), %%rdx  # A[1]                \n\t"
		"   mulx	  (%2),  %%r8,  %%r9  # A[1]*B[0]   \n\t"
		"   mulx	 8(%2), %%r10, %%r11  # A[1]*B[1]   \n\t"    "   addq	%%r10, %%r9      \n\t"
		"   mulx	16(%2), %%r12, %%r13  # A[1]*B[2]   \n\t"    "   adcq	%%r12, %%r11     \n\t"
		"   mulx	24(%2), %%r14, %%rdx  # A[1]*B[3]   \n\t"    "   adcq	%%r14, %%r13     \n\t"
		"   adcq	   $0, %%rdx                        \n\t"

		"   addq	 %%r8,  8(%0)                       \n\t"
		"   adcq	%%rax,  %%r9                        \n\t"    "    movq	 %%r9, 16(%0)    \n\t"      "   movq	 $0, %%rax    \n\t"
		"   adcq	%%r11, %%rbx                        \n\t"
		"   adcq	%%r13, %%rcx                        \n\t"
		"   adcq	%%rdx, %%rax                        \n\t"

		"   movq   16(%1), %%rdx  # A[2]                \n\t"
		"   mulx	  (%2),  %%r8,  %%r9  # A[2]*B[0]   \n\t"
		"   mulx	 8(%2), %%r10, %%r11  # A[2]*B[1]   \n\t"    "    addq	%%r10,  %%r9     \n\t"
		"   mulx	16(%2), %%r12, %%r13  # A[2]*B[2]   \n\t"    "    adcq	%%r12, %%r11     \n\t"
		"   mulx	24(%2), %%r14, %%rdx  # A[2]*B[3]   \n\t"    "    adcq	%%r14, %%r13     \n\t"
		"   adcq	   $0, %%rdx                        \n\t"

		"   addq	 %%r8, 16(%0)                       \n\t"
		"   adcq	%%rbx,  %%r9                        \n\t"    "    movq	 %%r9, 24(%0)    \n\t"      "   movq	 $0, %%rbx    \n\t"
		"   adcq	%%r11, %%rcx                        \n\t"
		"   adcq	%%r13, %%rax                        \n\t"
		"   adcq	%%rdx, %%rbx                        \n\t"

		"   movq	24(%1), %%rdx  # A[3]               \n\t"
		"   mulx	  (%2),  %%r8,  %%r9  # A[3]*B[0]   \n\t"
		"   mulx	 8(%2), %%r10, %%r11  # A[3]*B[1]   \n\t"    "    addq	%%r10,  %%r9     \n\t"
		"   mulx	16(%2), %%r12, %%r13  # A[3]*B[2]   \n\t"    "    adcq	%%r12, %%r11     \n\t"
		"   mulx	24(%2), %%r14, %%rdx  # A[3]*B[3]   \n\t"    "    adcq	%%r14, %%r13     \n\t"
		"   adcq     $0, %%rdx                          \n\t"

		"   addq    %%r8, 24(%0)                        \n\t"
		"   adcq	%%rcx,  %%r9                        \n\t"    "    movq	%%r9,  32(%0)    \n\t"   "   movq	 $0, %%rcx    \n\t"
		"   adcq	%%r11, %%rax                        \n\t"    "    movq	%%rax, 40(%0)    \n\t"
		"   adcq	%%r13, %%rbx                        \n\t"    "    movq	%%rbx, 48(%0)    \n\t"
		"   adcq	%%rdx, %%rcx                        \n\t"    "    movq	%%rcx, 56(%0)    \n\t"

		"   movq    32(%1), %%rdx  # A[0]               \n\t"
		"   mulx	32(%2),  %%r8,  %%r9  # A[0]*B[0]   \n\t"                                       "   movq	 %%r8, 64(%0)  \n\t"
		"   mulx	40(%2), %%r10, %%rax  # A[0]*B[1]   \n\t"    "   addq	%%r10,  %%r9     \n\t"  "   movq	 %%r9, 72(%0)  \n\t"
		"   mulx	48(%2), %%r12, %%rbx  # A[0]*B[2]   \n\t"    "   adcq	%%r12, %%rax     \n\t"
		"   mulx	56(%2), %%r14, %%rcx  # A[0]*B[3]   \n\t"    "   adcq	%%r14, %%rbx     \n\t"
		"   adcq	   $0, %%rcx                        \n\t"

		"   movq    40(%1), %%rdx  # A[1]               \n\t"
		"   mulx	32(%2),  %%r8,  %%r9  # A[1]*B[0]   \n\t"
		"   mulx	40(%2), %%r10, %%r11  # A[1]*B[1]   \n\t"    "   addq	%%r10, %%r9      \n\t"
		"   mulx	48(%2), %%r12, %%r13  # A[1]*B[2]   \n\t"    "   adcq	%%r12, %%r11     \n\t"
		"   mulx	56(%2), %%r14, %%rdx  # A[1]*B[3]   \n\t"    "   adcq	%%r14, %%r13     \n\t"
		"   adcq	   $0, %%rdx                        \n\t"

		"   addq	 %%r8,  72(%0)                      \n\t"
		"   adcq	%%rax,  %%r9                        \n\t"    "    movq	 %%r9, 80(%0)    \n\t"      "   movq	 $0, %%rax    \n\t"
		"   adcq	%%r11, %%rbx                        \n\t"
		"   adcq	%%r13, %%rcx                        \n\t"
		"   adcq	%%rdx, %%rax                        \n\t"

		"   movq   48(%1), %%rdx  # A[2]                \n\t"
		"   mulx	32(%2),  %%r8,  %%r9  # A[2]*B[0]   \n\t"
		"   mulx	40(%2), %%r10, %%r11  # A[2]*B[1]   \n\t"    "    addq	%%r10,  %%r9     \n\t"
		"   mulx	48(%2), %%r12, %%r13  # A[2]*B[2]   \n\t"    "    adcq	%%r12, %%r11     \n\t"
		"   mulx	56(%2), %%r14, %%rdx  # A[2]*B[3]   \n\t"    "    adcq	%%r14, %%r13     \n\t"
		"   adcq	   $0, %%rdx                        \n\t"

		"   addq	 %%r8, 80(%0)                       \n\t"
		"   adcq	%%rbx,  %%r9                        \n\t"    "    movq	 %%r9, 88(%0)    \n\t"      "   movq	 $0, %%rbx    \n\t"
		"   adcq	%%r11, %%rcx                        \n\t"
		"   adcq	%%r13, %%rax                        \n\t"
		"   adcq	%%rdx, %%rbx                        \n\t"

		"   movq	56(%1), %%rdx  # A[3]               \n\t"
		"   mulx	32(%2),  %%r8,  %%r9  # A[3]*B[0]   \n\t"
		"   mulx	40(%2), %%r10, %%r11  # A[3]*B[1]   \n\t"    "    addq	%%r10,  %%r9     \n\t"
		"   mulx	48(%2), %%r12, %%r13  # A[3]*B[2]   \n\t"    "    adcq	%%r12, %%r11     \n\t"
		"   mulx	56(%2), %%r14, %%rdx  # A[3]*B[3]   \n\t"    "    adcq	%%r14, %%r13     \n\t"
		"   adcq     $0, %%rdx                          \n\t"

		"   addq    %%r8, 88(%0)                        \n\t"
		"   adcq	%%rcx,  %%r9                        \n\t"    "    movq	%%r9,   96(%0)    \n\t"   "   movq	 $0, %%rcx    \n\t"
		"   adcq	%%r11, %%rax                        \n\t"    "    movq	%%rax, 104(%0)    \n\t"
		"   adcq	%%r13, %%rbx                        \n\t"    "    movq	%%rbx, 112(%0)    \n\t"
		"   adcq	%%rdx, %%rcx                        \n\t"    "    movq	%%rcx, 120(%0)    \n\t"
	:
	: "r"  (c), "r" (a), "r"  (b)
	: "memory", "cc", "%rax", "%rbx", "%rcx", "%rdx", "%r8",
	   "%r9", "%r10", "%r11", "%r12", "%r13", "%r14"
	);
#endif
#else   /* Without BMI2 */
	/**
	* TODO: Multiplications using MULQ instruction.
	**/
#endif
}

/**
 *
 * @param c
 * @param a
 */
static void sqr2_256x256_integer_x64(uint64_t *const c, uint64_t *const a)
{
#ifdef __BMI2__
	__asm__ __volatile__(
		"   movq	 (%1), %%rdx         # A[0]         \n\t"
		"   mulx	%%rdx,  %%r8, %%r9   # A[0]^2       \n\t"
		"   movq	8(%1), %%rdx         # A[1]         \n\t"
		"   mulx	%%rdx, %%r10, %%r11  # A[1]^2       \n\t"
		"   movq	 %%r8,   (%0)                       \n\t"
		"   movq	 %%r9,  8(%0)                       \n\t"
		"   movq	%%r10, 16(%0)                       \n\t"
		"   movq	%%r11, 24(%0)                       \n\t"

		"   movq	16(%1), %%rdx        # A[2]         \n\t"
		"   mulx	%%rdx,  %%r8, %%r9   # A[2]^2       \n\t"
		"   movq	24(%1), %%rdx        # A[3]         \n\t"
		"   mulx	%%rdx, %%r10, %%r11  # A[3]^2       \n\t"
		"   movq	 %%r8, 32(%0)                       \n\t"
		"   movq	 %%r9, 40(%0)                       \n\t"
		"   movq	%%r10, 48(%0)                       \n\t"
		"   movq	%%r11, 56(%0)                       \n\t"

		"   movq	 8(%1), %%rdx         # A[1]        \n\t"
		"   mulx	  (%1), %%r8, %%r9    # A[0]*A[1]   \n\t"
		"   mulx	16(%1), %%r10, %%r11  # A[2]*A[1]   \n\t"
		"   mulx	24(%1), %%rcx, %%r14  # A[3]*A[1]   \n\t"

		"   movq	16(%1), %%rdx         # A[2]        \n\t"
		"   mulx	24(%1), %%r12, %%r13  # A[3]*A[2]   \n\t"
		"   mulx	  (%1), %%rax, %%rdx  # A[0]*A[2]   \n\t"

		" 	addq	%%rax, %%r9       \n\t"
		" 	adcq	%%rdx, %%r10      \n\t"
		" 	adcq	%%rcx, %%r11      \n\t"
		" 	adcq	%%r14, %%r12      \n\t"
		" 	adcq	   $0, %%r13      \n\t"
		" 	movq	   $0, %%r14      \n\t"
		" 	adcq	   $0, %%r14      \n\t"

		" 	movq	  (%1), %%rdx        # A[0]         \n\t"
		" 	mulx	24(%1), %%rax, %%rdx # A[0]*A[3]    \n\t"

		" 	addq	%%rax, %%r10      \n\t"
		" 	adcq	%%rdx, %%r11      \n\t"
		" 	adcq	   $0, %%r12      \n\t"
		" 	adcq	   $0, %%r13      \n\t"
		" 	adcq	   $0, %%r14      \n\t"

		" 	shldq	$1, %%r13, %%r14  \n\t"
		" 	shldq	$1, %%r12, %%r13  \n\t"
		" 	shldq	$1, %%r11, %%r12  \n\t"
		" 	shldq	$1, %%r10, %%r11  \n\t"
		" 	shldq	$1,  %%r9, %%r10  \n\t"
		" 	shldq	$1,  %%r8,  %%r9  \n\t"
		" 	shlq	$1,  %%r8         \n\t"

		" 	addq	 8(%0),  %%r8     \n\t"  " 	movq	 %%r8,  8(%0)  \n\t"
		" 	adcq	16(%0),  %%r9     \n\t"  " 	movq	 %%r9, 16(%0)  \n\t"
		" 	adcq	24(%0), %%r10     \n\t"  " 	movq	%%r10, 24(%0)  \n\t"
		" 	adcq	32(%0), %%r11     \n\t"  " 	movq	%%r11, 32(%0)  \n\t"
		" 	adcq	40(%0), %%r12     \n\t"  " 	movq	%%r12, 40(%0)  \n\t"
		" 	adcq	48(%0), %%r13     \n\t"  " 	movq	%%r13, 48(%0)  \n\t"
		" 	adcq	56(%0), %%r14     \n\t"  " 	movq	%%r14, 56(%0)  \n\t"


		"   movq   32(%1), %%rdx         # A[0]         \n\t"
		"   mulx	%%rdx,  %%r8, %%r9   # A[0]^2       \n\t"
		"   movq   40(%1), %%rdx         # A[1]         \n\t"
		"   mulx	%%rdx, %%r10, %%r11  # A[1]^2       \n\t"
		"   movq	 %%r8, 64(%0)                       \n\t"
		"   movq	 %%r9, 72(%0)                       \n\t"
		"   movq	%%r10, 80(%0)                       \n\t"
		"   movq	%%r11, 88(%0)                       \n\t"

		"   movq	48(%1), %%rdx        # A[2]         \n\t"
		"   mulx	%%rdx,  %%r8, %%r9   # A[2]^2       \n\t"
		"   movq	56(%1), %%rdx        # A[3]         \n\t"
		"   mulx	%%rdx, %%r10, %%r11  # A[3]^2       \n\t"
		"   movq	 %%r8,  96(%0)                      \n\t"
		"   movq	 %%r9, 104(%0)                      \n\t"
		"   movq	%%r10, 112(%0)                      \n\t"
		"   movq	%%r11, 120(%0)                      \n\t"

		"   movq	40(%1), %%rdx         # A[1]        \n\t"
		"   mulx	32(%1), %%r8, %%r9    # A[0]*A[1]   \n\t"
		"   mulx	48(%1), %%r10, %%r11  # A[2]*A[1]   \n\t"
		"   mulx	56(%1), %%rcx, %%r14  # A[3]*A[1]   \n\t"

		"   movq	48(%1), %%rdx         # A[2]        \n\t"
		"   mulx	56(%1), %%r12, %%r13  # A[3]*A[2]   \n\t"
		"   mulx	32(%1), %%rax, %%rdx  # A[0]*A[2]   \n\t"

		" 	addq	%%rax, %%r9       \n\t"
		" 	adcq	%%rdx, %%r10      \n\t"
		" 	adcq	%%rcx, %%r11      \n\t"
		" 	adcq	%%r14, %%r12      \n\t"
		" 	adcq	   $0, %%r13      \n\t"
		" 	movq	   $0, %%r14      \n\t"
		" 	adcq	   $0, %%r14      \n\t"

		" 	movq	32(%1), %%rdx        # A[0]         \n\t"
		" 	mulx	56(%1), %%rax, %%rdx # A[0]*A[3]    \n\t"

		" 	addq	%%rax, %%r10      \n\t"
		" 	adcq	%%rdx, %%r11      \n\t"
		" 	adcq	   $0, %%r12      \n\t"
		" 	adcq	   $0, %%r13      \n\t"
		" 	adcq	   $0, %%r14      \n\t"

		" 	shldq	$1, %%r13, %%r14  \n\t"
		" 	shldq	$1, %%r12, %%r13  \n\t"
		" 	shldq	$1, %%r11, %%r12  \n\t"
		" 	shldq	$1, %%r10, %%r11  \n\t"
		" 	shldq	$1,  %%r9, %%r10  \n\t"
		" 	shldq	$1,  %%r8,  %%r9  \n\t"
		" 	shlq	$1,  %%r8         \n\t"

		" 	addq	 72(%0),  %%r8     \n\t"  " 	movq	 %%r8,  72(%0)  \n\t"
		" 	adcq	 80(%0),  %%r9     \n\t"  " 	movq	 %%r9,  80(%0)  \n\t"
		" 	adcq	 88(%0), %%r10     \n\t"  " 	movq	%%r10,  88(%0)  \n\t"
		" 	adcq	 96(%0), %%r11     \n\t"  " 	movq	%%r11,  96(%0)  \n\t"
		" 	adcq	104(%0), %%r12     \n\t"  " 	movq	%%r12, 104(%0)  \n\t"
		" 	adcq	112(%0), %%r13     \n\t"  " 	movq	%%r13, 112(%0)  \n\t"
		" 	adcq	120(%0), %%r14     \n\t"  " 	movq	%%r14, 120(%0)  \n\t"
	:
	: "r"  (c), "r" (a)
	:  "cc", "%rax", "%rcx", "%rdx",
	"%r8",  "%r9", "%r10", "%r11",
	"%r12", "%r13", "%r14"
	);
#else    /* Without BMI2 */
	/**
	* TODO: Multiplications using MULQ instruction.
	**/
#endif
}

/**
 *
 * @param c
 * @param a
 */
static void red_Element_2w_x64(uint64_t *const c, uint64_t *const a)
{
#ifdef __BMI2__
#ifdef __ADX__
	__asm__ __volatile__(
		"   movl	$38, %%edx     # 2*c = 38 = 2^256  \n\t"
		"   mulx	32(%1),  %%r8, %%r10 # c*C[4]      \n\t"   "  xorl  %%ebx, %%ebx  \n\t"  "  adox      (%1),  %%r8  \n\t"
		"   mulx	40(%1),  %%r9, %%r11 # c*C[5]      \n\t"   "  adcx	%%r10,  %%r9  \n\t"  "  adox     8(%1),  %%r9  \n\t"
		"   mulx	48(%1), %%r10, %%rax # c*C[6]      \n\t"   "  adcx	%%r11, %%r10  \n\t"  "  adox    16(%1), %%r10  \n\t"    "  movq  %%r10, 16(%0) \n\t"
		"   mulx	56(%1), %%r11, %%rcx # c*C[7]      \n\t"   "  adcx	%%rax, %%r11  \n\t"  "  adox    24(%1), %%r11  \n\t"    "  movq  %%r11, 24(%0) \n\t"
		                                                       "  adcx	%%rbx, %%rcx  \n\t"  "  adox     %%rbx, %%rcx  \n\t"
		"   xorl    %%ebx, %%ebx                       \n\t"
		"   mulx	%%rcx, %%rax, %%rcx                \n\t"   "  adcx  %%rax,  %%r8  \n\t"  "  movq     %%r8,   (%0)  \n\t"
		                                                       "  adcx  %%rcx,  %%r9  \n\t"  "  movq     %%r9,  8(%0)  \n\t"

		"   mulx	 96(%1),  %%r8, %%r10 # c*C[4]     \n\t"   "  xorl  %%ebx, %%ebx  \n\t"  "  adox    64(%1),  %%r8  \n\t"
		"   mulx	104(%1),  %%r9, %%r11 # c*C[5]     \n\t"   "  adcx	%%r10,  %%r9  \n\t"  "  adox    72(%1),  %%r9  \n\t"
		"   mulx	112(%1), %%r10, %%rax # c*C[6]     \n\t"   "  adcx	%%r11, %%r10  \n\t"  "  adox    80(%1), %%r10  \n\t"    "  movq  %%r10, 48(%0) \n\t"
		"   mulx	120(%1), %%r11, %%rcx # c*C[7]     \n\t"   "  adcx	%%rax, %%r11  \n\t"  "  adox    88(%1), %%r11  \n\t"    "  movq  %%r11, 56(%0) \n\t"
		                                                       "  adcx	%%rbx, %%rcx  \n\t"  "  adox     %%rbx, %%rcx  \n\t"
		"   xorl    %%ebx, %%ebx                       \n\t"
		"   mulx	%%rcx, %%rax, %%rcx                \n\t"   "  adcx  %%rax,  %%r8  \n\t"  "  movq     %%r8, 32(%0)  \n\t"
		                                                       "  adcx  %%rcx,  %%r9  \n\t"  "  movq     %%r9, 40(%0)  \n\t"
	:
	: "r"  (c), "r" (a)
	: "cc", "%rax", "%rbx", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11"
	);
#else
	__asm__ __volatile__(
	"   movl	$38, %%edx     # 2*c = 38 = 2^256      \n\t"
			"   mulx	32(%1), %%r8,  %%r9  # c*C[4]  \n\t"
			"   mulx	40(%1), %%r10, %%r11 # c*C[5]  \n\t"   "  addq	%%r9, %%r10   \n\t"
			"   mulx	48(%1), %%r12, %%r13 # c*C[6]  \n\t"   "  adcq	%%r11, %%r12  \n\t"
			"   mulx	56(%1), %%rax, %%rcx # c*C[7]  \n\t"   "  adcq	%%r13, %%rax  \n\t"
			"  adcq	   $0, %%rcx  \n\t"

			"  addq      (%1),  %%r8  \n\t"
			"  adcq     8(%1), %%r10  \n\t"
			"  adcq    16(%1), %%r12  \n\t"     "   movq	%%r12, 16(%0)  \n\t"
			"  adcq    24(%1), %%rax  \n\t"     "   movq	%%rax, 24(%0)  \n\t"
			"  adcq        $0, %%rcx  \n\t"

			"  mulx	%%rcx, %%rax, %%rcx   \n\t"
			"  addq     %%rax, %%r8       \n\t" "   movq    %%r8,    (%0)  \n\t"
			"  adcq     %%rcx, %%r10      \n\t" "   movq    %%r10,  8(%0)  \n\t"

			"   mulx	 96(%1), %%r8,  %%r9  # c*C[4]  \n\t"
			"   mulx	104(%1), %%r10, %%r11 # c*C[5]  \n\t"   "  addq	%%r9, %%r10   \n\t"
			"   mulx	112(%1), %%r12, %%r13 # c*C[6]  \n\t"   "  adcq	%%r11, %%r12  \n\t"
			"   mulx	120(%1), %%rax, %%rcx # c*C[7]  \n\t"   "  adcq	%%r13, %%rax  \n\t"
			"  adcq	   $0, %%rcx  \n\t"

			"  addq    64(%1),  %%r8  \n\t"
			"  adcq    72(%1), %%r10  \n\t"
			"  adcq    80(%1), %%r12  \n\t"     "   movq	%%r12, 48(%0)  \n\t"
			"  adcq    88(%1), %%rax  \n\t"     "   movq	%%rax, 56(%0)  \n\t"
			"  adcq        $0, %%rcx  \n\t"

			"  mulx	%%rcx, %%rax, %%rcx   \n\t"
			"  addq     %%rax, %%r8       \n\t" "   movq    %%r8,  32(%0)  \n\t"
			"  adcq     %%rcx, %%r10      \n\t" "   movq    %%r10, 40(%0)  \n\t"

	:
	: "r"  (c), "r" (a)
	: "cc", "%rax", "%rbx", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13"
	);
#endif
#else    /* Without BMI2 */
	/* [TODO] */
#endif
}


static void mul_256x256_integer_x64(uint64_t *const c, uint64_t *const a, uint64_t *const b)
{
#ifdef __BMI2__
#ifdef __ADX__
	__asm__ __volatile__(
		"   movq      (%1), %%rdx  # A[0]               \n\t"
		"   mulx	  (%2),  %%r8,  %%r9  # A[0]*B[0]   \n\t"    "  xorl   %%r10d, %%r10d    \n\t"                                      "   movq	%%r8,  (%0)      \n\t"
		"   mulx	 8(%2), %%r10, %%r11  # A[0]*B[1]   \n\t"    "  adox	 %%r9, %%r10     \n\t"                                      "   movq   %%r10, 8(%0)      \n\t"
		"   mulx	16(%2), %%r12, %%r13  # A[0]*B[2]   \n\t"    "  adox	%%r11, %%r12     \n\t"
		"   mulx	24(%2), %%r14, %%rdx  # A[0]*B[3]   \n\t"    "  adox	%%r13, %%r14     \n\t"                                                                          "   movq    $0, %%rax  \n\t"
																 "  adox	%%rdx, %%rax     \n\t"

		"   movq     8(%1), %%rdx  # A[1]               \n\t"
		"   mulx	  (%2),  %%r8,  %%r9  # A[1]*B[0]   \n\t"    "  xorl   %%r10d, %%r10d    \n\t"  "   adcx	8(%0),  %%r8   \n\t"    "   movq	 %%r8,  8(%0)    \n\t"
		"   mulx	 8(%2), %%r10, %%r11  # A[1]*B[1]   \n\t"    "  adox 	 %%r9, %%r10     \n\t"  "   adcx	%%r12, %%r10   \n\t"    "   movq	%%r10, 16(%0)    \n\t"
		"   mulx	16(%2), %%r12, %%r13  # A[1]*B[2]   \n\t"    "  adox	%%r11, %%r12     \n\t"  "   adcx	%%r14, %%r12   \n\t"                                        "   movq    $0, %%r8   \n\t"
		"   mulx	24(%2), %%r14, %%rdx  # A[1]*B[3]   \n\t"    "  adox	%%r13, %%r14     \n\t"  "   adcx	%%rax, %%r14   \n\t"                                        "   movq    $0, %%rax  \n\t"
																 "  adox 	%%rdx, %%rax     \n\t"  "   adcx	 %%r8, %%rax   \n\t"

		"   movq    16(%1), %%rdx  # A[2]               \n\t"
		"   mulx	  (%2),  %%r8,  %%r9  # A[2]*B[0]   \n\t"    "  xorl   %%r10d, %%r10d    \n\t"  "   adcx   16(%0),  %%r8   \n\t"    "   movq	 %%r8, 16(%0)    \n\t"
		"   mulx	 8(%2), %%r10, %%r11  # A[2]*B[1]   \n\t"    "  adox	 %%r9, %%r10     \n\t"  "   adcx	%%r12, %%r10   \n\t"    "   movq	%%r10, 24(%0)    \n\t"
		"   mulx	16(%2), %%r12, %%r13  # A[2]*B[2]   \n\t"    "  adox	%%r11, %%r12     \n\t"  "   adcx	%%r14, %%r12   \n\t"                                        "   movq    $0, %%r8   \n\t"
		"   mulx	24(%2), %%r14, %%rdx  # A[2]*B[3]   \n\t"    "  adox	%%r13, %%r14     \n\t"  "   adcx	%%rax, %%r14   \n\t"                                        "   movq    $0, %%rax  \n\t"
																 "  adox	%%rdx, %%rax     \n\t"  "   adcx	 %%r8, %%rax   \n\t"

		"   movq	24(%1), %%rdx  # A[3]               \n\t"
		"   mulx	  (%2),  %%r8,  %%r9  # A[3]*B[0]   \n\t"    "  xorl   %%r10d, %%r10d    \n\t"  "   adcx   24(%0),  %%r8   \n\t"    "   movq	 %%r8, 24(%0)    \n\t"
		"   mulx	 8(%2), %%r10, %%r11  # A[3]*B[1]   \n\t"    "  adox	 %%r9, %%r10     \n\t"  "   adcx	%%r12, %%r10   \n\t"    "   movq	%%r10, 32(%0)    \n\t"
		"   mulx	16(%2), %%r12, %%r13  # A[3]*B[2]   \n\t"    "  adox	%%r11, %%r12     \n\t"  "   adcx	%%r14, %%r12   \n\t"    "   movq	%%r12, 40(%0)    \n\t"  "   movq    $0, %%r8   \n\t"
		"   mulx	24(%2), %%r14, %%rdx  # A[3]*B[3]   \n\t"    "  adox	%%r13, %%r14     \n\t"  "   adcx	%%rax, %%r14   \n\t"    "   movq	%%r14, 48(%0)    \n\t"  "   movq    $0, %%rax  \n\t"
																 "  adox	%%rdx, %%rax     \n\t"  "   adcx	 %%r8, %%rax   \n\t"    "   movq	%%rax, 56(%0)    \n\t"
	:
	: "r"  (c), "r" (a), "r"  (b)
	: "memory", "cc", "%rax", "%rdx",
	"%r8",  "%r9", "%r10", "%r11",
	"%r12", "%r13", "%r14"
	);
#else
	__asm__ __volatile__(
		"   movq     (%1), %%rdx  # A[0]                \n\t"
		"   mulx	  (%2),  %%r8,  %%r9  # A[0]*B[0]   \n\t"                                       "   movq	 %%r8,  (%0)  \n\t"
		"   mulx	 8(%2), %%r10, %%rax  # A[0]*B[1]   \n\t"    "   addq	%%r10,  %%r9     \n\t"  "   movq	 %%r9, 8(%0)  \n\t"
		"   mulx	16(%2), %%r12, %%rbx  # A[0]*B[2]   \n\t"    "   adcq	%%r12, %%rax     \n\t"
		"   mulx	24(%2), %%r14, %%rcx  # A[0]*B[3]   \n\t"    "   adcq	%%r14, %%rbx     \n\t"
		"   adcq	   $0, %%rcx                        \n\t"

		"   movq    8(%1), %%rdx  # A[1]                \n\t"
		"   mulx	  (%2),  %%r8,  %%r9  # A[1]*B[0]   \n\t"
		"   mulx	 8(%2), %%r10, %%r11  # A[1]*B[1]   \n\t"    "   addq	%%r10, %%r9      \n\t"
		"   mulx	16(%2), %%r12, %%r13  # A[1]*B[2]   \n\t"    "   adcq	%%r12, %%r11     \n\t"
		"   mulx	24(%2), %%r14, %%rdx  # A[1]*B[3]   \n\t"    "   adcq	%%r14, %%r13     \n\t"
		"   adcq	   $0, %%rdx                        \n\t"

		"   addq	 %%r8,  8(%0)                       \n\t"
		"   adcq	%%rax,  %%r9                        \n\t"    "    movq	 %%r9, 16(%0)    \n\t"      "   movq	 $0, %%rax    \n\t"
		"   adcq	%%r11, %%rbx                        \n\t"
		"   adcq	%%r13, %%rcx                        \n\t"
		"   adcq	%%rdx, %%rax                        \n\t"

		"   movq   16(%1), %%rdx  # A[2]                \n\t"
		"   mulx	  (%2),  %%r8,  %%r9  # A[2]*B[0]   \n\t"
		"   mulx	 8(%2), %%r10, %%r11  # A[2]*B[1]   \n\t"    "    addq	%%r10,  %%r9     \n\t"
		"   mulx	16(%2), %%r12, %%r13  # A[2]*B[2]   \n\t"    "    adcq	%%r12, %%r11     \n\t"
		"   mulx	24(%2), %%r14, %%rdx  # A[2]*B[3]   \n\t"    "    adcq	%%r14, %%r13     \n\t"
		"   adcq	   $0, %%rdx                        \n\t"

		"   addq	 %%r8, 16(%0)                       \n\t"
		"   adcq	%%rbx,  %%r9                        \n\t"    "    movq	 %%r9, 24(%0)    \n\t"      "   movq	 $0, %%rbx    \n\t"
		"   adcq	%%r11, %%rcx                        \n\t"
		"   adcq	%%r13, %%rax                        \n\t"
		"   adcq	%%rdx, %%rbx                        \n\t"

		"   movq	24(%1), %%rdx  # A[3]               \n\t"
		"   mulx	  (%2),  %%r8,  %%r9  # A[3]*B[0]   \n\t"
		"   mulx	 8(%2), %%r10, %%r11  # A[3]*B[1]   \n\t"    "    addq	%%r10,  %%r9     \n\t"
		"   mulx	16(%2), %%r12, %%r13  # A[3]*B[2]   \n\t"    "    adcq	%%r12, %%r11     \n\t"
		"   mulx	24(%2), %%r14, %%rdx  # A[3]*B[3]   \n\t"    "    adcq	%%r14, %%r13     \n\t"
		"   adcq     $0, %%rdx                          \n\t"

		"   addq    %%r8, 24(%0)                        \n\t"
		"   adcq	%%rcx,  %%r9                        \n\t"    "    movq	%%r9,  32(%0)    \n\t"   "   movq	 $0, %%rcx    \n\t"
		"   adcq	%%r11, %%rax                        \n\t"    "    movq	%%rax, 40(%0)    \n\t"
		"   adcq	%%r13, %%rbx                        \n\t"    "    movq	%%rbx, 48(%0)    \n\t"
		"   adcq	%%rdx, %%rcx                        \n\t"    "    movq	%%rcx, 56(%0)    \n\t"
	:
	: "r"  (c), "r" (a), "r"  (b)
	: "memory", "cc", "%rax", "%rbx", "%rcx", "%rdx", "%r8",
	"%r9", "%r10", "%r11", "%r12", "%r13", "%r14"
	);
#endif
#else    /* Without BMI2 */
	/**
	* TODO: Multiplications using MULQ instruction.
	**/
#endif
}

static void sqr_256x256_integer_x64(uint64_t *const c, uint64_t *const a)
{
#ifdef __BMI2__
	__asm__ __volatile__(
		"   movq	 (%1), %%rdx         # A[0]         \n\t"
		"   mulx	%%rdx,  %%r8, %%r9   # A[0]^2       \n\t"
		"   movq	8(%1), %%rdx         # A[1]         \n\t"
		"   mulx	%%rdx, %%r10, %%r11  # A[1]^2       \n\t"
		"   movq	 %%r8,   (%0)                       \n\t"
		"   movq	 %%r9,  8(%0)                       \n\t"
		"   movq	%%r10, 16(%0)                       \n\t"
		"   movq	%%r11, 24(%0)                       \n\t"

		"   movq	16(%1), %%rdx        # A[2]         \n\t"
		"   mulx	%%rdx,  %%r8, %%r9   # A[2]^2       \n\t"
		"   movq	24(%1), %%rdx        # A[3]         \n\t"
		"   mulx	%%rdx, %%r10, %%r11  # A[3]^2       \n\t"
		"   movq	 %%r8, 32(%0)                       \n\t"
		"   movq	 %%r9, 40(%0)                       \n\t"
		"   movq	%%r10, 48(%0)                       \n\t"
		"   movq	%%r11, 56(%0)                       \n\t"

		"   movq	 8(%1), %%rdx         # A[1]        \n\t"
		"   mulx	  (%1), %%r8, %%r9    # A[0]*A[1]   \n\t"
		"   mulx	16(%1), %%r10, %%r11  # A[2]*A[1]   \n\t"
		"   mulx	24(%1), %%rcx, %%r14  # A[3]*A[1]   \n\t"

		"   movq	16(%1), %%rdx         # A[2]        \n\t"
		"   mulx	24(%1), %%r12, %%r13  # A[3]*A[2]   \n\t"
		"   mulx	  (%1), %%rax, %%rdx  # A[0]*A[2]   \n\t"

		" 	addq	%%rax, %%r9       \n\t"
		" 	adcq	%%rdx, %%r10      \n\t"
		" 	adcq	%%rcx, %%r11      \n\t"
		" 	adcq	%%r14, %%r12      \n\t"
		" 	adcq	   $0, %%r13      \n\t"
		" 	movq	   $0, %%r14      \n\t"
		" 	adcq	   $0, %%r14      \n\t"

		" 	movq	  (%1), %%rdx        # A[0]         \n\t"
		" 	mulx	24(%1), %%rax, %%rdx # A[0]*A[3]    \n\t"

		" 	addq	%%rax, %%r10      \n\t"
		" 	adcq	%%rdx, %%r11      \n\t"
		" 	adcq	   $0, %%r12      \n\t"
		" 	adcq	   $0, %%r13      \n\t"
		" 	adcq	   $0, %%r14      \n\t"

		" 	shldq	$1, %%r13, %%r14  \n\t"
		" 	shldq	$1, %%r12, %%r13  \n\t"
		" 	shldq	$1, %%r11, %%r12  \n\t"
		" 	shldq	$1, %%r10, %%r11  \n\t"
		" 	shldq	$1,  %%r9, %%r10  \n\t"
		" 	shldq	$1,  %%r8,  %%r9  \n\t"
		" 	shlq	$1,  %%r8         \n\t"

		" 	addq	 8(%0),  %%r8     \n\t"  " 	movq	 %%r8,  8(%0)  \n\t"
		" 	adcq	16(%0),  %%r9     \n\t"  " 	movq	 %%r9, 16(%0)  \n\t"
		" 	adcq	24(%0), %%r10     \n\t"  " 	movq	%%r10, 24(%0)  \n\t"
		" 	adcq	32(%0), %%r11     \n\t"  " 	movq	%%r11, 32(%0)  \n\t"
		" 	adcq	40(%0), %%r12     \n\t"  " 	movq	%%r12, 40(%0)  \n\t"
		" 	adcq	48(%0), %%r13     \n\t"  " 	movq	%%r13, 48(%0)  \n\t"
		" 	adcq	56(%0), %%r14     \n\t"  " 	movq	%%r14, 56(%0)  \n\t"
	:
	: "r"  (c), "r" (a)
	:  "memory", "cc", "%rax", "%rcx", "%rdx",
	"%r8",  "%r9", "%r10", "%r11",
	"%r12", "%r13", "%r14"
	);
#else    /* Without BMI2 */
	/**
	* TODO: Multiplications using MULQ instruction.
	**/
#endif
}

static void red_Element_1w_x64(uint64_t *const c, uint64_t *const a)
{
#ifdef __BMI2__
#ifdef __ADX__
	__asm__ __volatile__(
		"   movl	$38, %%edx     # 2*c = 38 = 2^256  \n\t"
		"   mulx	32(%1),  %%r8, %%r10 # c*C[4]      \n\t"   "  xorl  %%ebx, %%ebx  \n\t"  "  adox      (%1),  %%r8  \n\t"
		"   mulx	40(%1),  %%r9, %%r11 # c*C[5]      \n\t"   "  adcx	%%r10,  %%r9  \n\t"  "  adox     8(%1),  %%r9  \n\t"
		"   mulx	48(%1), %%r10, %%rax # c*C[6]      \n\t"   "  adcx	%%r11, %%r10  \n\t"  "  adox    16(%1), %%r10  \n\t"    "  movq  %%r10, 16(%0) \n\t"
		"   mulx	56(%1), %%r11, %%rcx # c*C[7]      \n\t"   "  adcx	%%rax, %%r11  \n\t"  "  adox    24(%1), %%r11  \n\t"    "  movq  %%r11, 24(%0) \n\t"
		                                                       "  adcx	%%rbx, %%rcx  \n\t"  "  adox     %%rbx, %%rcx  \n\t"
		"   xorl    %%ebx, %%ebx                       \n\t"
		"   mulx	%%rcx, %%rax, %%rcx                \n\t"   "  adcx  %%rax,  %%r8  \n\t"  "  movq     %%r8,   (%0)  \n\t"
		                                                       "  adcx  %%rcx,  %%r9  \n\t"  "  movq     %%r9,  8(%0)  \n\t"
	:
	: "r"  (c), "r" (a)
	: "memory", "cc", "%rax", "%rbx", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11"
	);
#else
	__asm__ __volatile__(
	"   movl	$38, %%edx     # 2*c = 38 = 2^256      \n\t"
			"   mulx	32(%1), %%r8,  %%r9  # c*C[4]  \n\t"
			"   mulx	40(%1), %%r10, %%r11 # c*C[5]  \n\t"   "  addq	%%r9, %%r10   \n\t"
			"   mulx	48(%1), %%r12, %%r13 # c*C[6]  \n\t"   "  adcq	%%r11, %%r12  \n\t"
			"   mulx	56(%1), %%rax, %%rcx # c*C[7]  \n\t"   "  adcq	%%r13, %%rax  \n\t"
			"  adcq	   $0, %%rcx  \n\t"

			"  addq      (%1),  %%r8  \n\t"
			"  adcq     8(%1), %%r10  \n\t"
			"  adcq    16(%1), %%r12  \n\t"     "   movq	%%r12, 16(%0)  \n\t"
			"  adcq    24(%1), %%rax  \n\t"     "   movq	%%rax, 24(%0)  \n\t"
			"  adcq        $0, %%rcx  \n\t"

			"  mulx	%%rcx, %%rax, %%rcx   \n\t"
			"  addq     %%rax, %%r8       \n\t" "   movq    %%r8,    (%0)  \n\t"
			"  adcq     %%rcx, %%r10      \n\t" "   movq    %%r10,  8(%0)  \n\t"
	:
	: "r"  (c), "r" (a)
	: "memory", "cc", "%rax", "%rbx", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13"
	);
#endif
#else    /* Without BMI2 */
	/**
	* TODO: Multiplications using MULQ instruction.
	**/
#endif
}

static __inline void add_Element_1w_x64(uint64_t *const c, uint64_t *const a, uint64_t *const b)
{
#if __ADX__
	__asm__ __volatile__(
		"movq      (%2),  %%rax   \n\t"
		"movq     8(%2),  %%rcx   \n\t"
		"movq    16(%2),  %%r8    \n\t"
		"movq    24(%2),  %%r9    \n\t"
		"clc                      \n\t"
		"adcx      (%1),  %%rax   \n\t"
		"adcx     8(%1),  %%rcx   \n\t"
		"adcx    16(%1),  %%r8    \n\t"
		"adcx    24(%1),  %%r9    \n\t"
		"movq     %%rcx,  8(%0)   \n\t"
		"movq     %%r8 , 16(%0)   \n\t"
		"movq     %%r9 , 24(%0)   \n\t"
		"setc	 %%cl             \n\t"
		"neg 	 %%rcx            \n\t"
		"andq	 $38, %%rcx       \n\t"
		"addq	 %%rcx, %%rax     \n\t"
		"movq	 %%rax,    (%0)   \n\t"
	:
	: "r" (c), "r" (a), "r"  (b)
	: "memory","cc", "%rax", "%rcx", "%r8", "%r9"
	);
#else
	__asm__ __volatile__(
		"movq      (%2),  %%rax   \n\t"
		"movq     8(%2),  %%rcx   \n\t"
		"movq    16(%2),  %%r8    \n\t"
		"movq    24(%2),  %%r9    \n\t"
		"add       (%1),  %%rax   \n\t"
		"adc      8(%1),  %%rcx   \n\t"
		"adc     16(%1),  %%r8    \n\t"
		"adc     24(%1),  %%r9    \n\t"
		"movq     %%rcx,  8(%0)   \n\t"
		"movq     %%r8 , 16(%0)   \n\t"
		"movq     %%r9 , 24(%0)   \n\t"
		"setc	 %%cl             \n\t"
		"neg 	 %%rcx            \n\t"
		"andq	 $38, %%rcx       \n\t"
		"addq	 %%rcx, %%rax     \n\t"
		"movq	 %%rax,    (%0)   \n\t"
	:
	: "r" (c), "r" (a), "r"  (b)
	: "memory","cc", "%rax", "%rcx", "%r8", "%r9"
	);
#endif
}

static __inline void sub_Element_1w_x64(uint64_t *const __restrict c, uint64_t *const __restrict a, uint64_t *const __restrict b)
{
	__asm__ __volatile__(
		"movq      (%1),  %%rax   \n\t"
		"movq     8(%1),  %%rcx   \n\t"
		"movq    16(%1),  %%r8    \n\t"
		"movq    24(%1),  %%r9    \n\t"
		"subq      (%2),  %%rax   \n\t"
		"sbbq     8(%2),  %%rcx   \n\t"
		"sbbq    16(%2),  %%r8    \n\t"
		"sbbq    24(%2),  %%r9    \n\t"
		"movq     %%rcx,  8(%0)   \n\t"
		"movq     %%r8 , 16(%0)   \n\t"
		"movq     %%r9 , 24(%0)   \n\t"
		"setc	 %%cl             \n\t"
		"neg 	 %%rcx            \n\t"
		"andq	 $38, %%rcx       \n\t"
		"subq	 %%rcx, %%rax     \n\t"
		"movq	 %%rax,    (%0)   \n\t"
	:
	: "r" (c), "r" (a), "r"  (b)
	: "memory","cc", "%rax", "%rcx", "%r8", "%r9"
	);
}

static __inline void neg_Element_1w_x64(uint64_t *const c)
{
	Element_1w_x64 zero={0};
	sub_Element_1w_x64(c,zero,c);
}


static void mul_Element_1w_x64(argElement_1w c, argElement_1w a, argElement_1w b)
{
	Element_1w_Buffer_x64 buffer_1w;
	mul_256x256_integer_x64(buffer_1w,a,b);
	red_Element_1w_x64(c,buffer_1w);
}

static void sqr_Element_1w_x64(argElement_1w a)
{
	Element_1w_Buffer_x64 buffer_1w;
	sqr_256x256_integer_x64(buffer_1w,a);
	red_Element_1w_x64(a,buffer_1w);
}


static __inline void mul_a24_Element_1w_x64(uint64_t *const c, uint64_t *const a)
{
#ifdef __BMI2__
	/**
	 * a24 = (A+2)/4 = (486662+2)/4 = 121666
	 **/
	const uint64_t a24 = 121666;
	__asm__ __volatile__(
		"movq	%2, %%rdx             \n\t"
		"mulx	 (%1), %%rax, %%r8    \n\t"
		"mulx	8(%1), %%rcx, %%r9    \n\t"
		"movq	%%rax,  (%0)          \n\t"
		"movq	%%rcx, 8(%0)          \n\t"
		"mulx	16(%1), %%rax, %%r10  \n\t"
		"mulx	24(%1), %%rcx, %%r11  \n\t"
		"movq	%%rax, 16(%0)         \n\t"
		"movq	%%rcx, 24(%0)         \n\t"
		"movq   $38, %%rdx            \n\t"
		"mulx	%%r11, %%rax, %%rcx   \n\t"
		"addq	%%rax,   (%0)         \n\t"
		"adcq	%%r8,   8(%0)         \n\t"
		"adcq	%%r9,  16(%0)         \n\t"
		"adcq	%%r10, 24(%0)         \n\t"
	:
	: "r" (c), "r" (a), "r" (a24)
	: "cc", "%rax", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11"
	);
#else /* Without BMI2 */
	/**
	* TODO: Multiplications using MULQ instruction.
	**/
#endif
}

#define sqrn_Element_1w_x64(a,times)\
    counter = times;\
    while(counter-- > 0)\
    {\
        sqr_Element_1w_x64(a);\
    }\

static void inv_Element_1w_x64(uint64_t * const pC, uint64_t * const pA)
{
	Element_1w_x64 x0, x1, x2;
	uint64_t * T[5];
	uint64_t counter;

	T[0] = x0;
	T[1] = pC; /* x^(-1) */
	T[2] = x1;
	T[3] = x2;
	T[4] = pA; /* x */

	copy_Element_1w_x64(T[1],pA);
	sqrn_Element_1w_x64(T[1],1);
	copy_Element_1w_x64(T[2],T[1]);
	sqrn_Element_1w_x64(T[2],2);
	mul_Element_1w_x64(T[0], pA, T[2]);
	mul_Element_1w_x64(T[1], T[1], T[0]);
	copy_Element_1w_x64(T[2],T[1]);
	sqrn_Element_1w_x64(T[2],1);
	mul_Element_1w_x64(T[0], T[0], T[2]);
	copy_Element_1w_x64(T[2],T[0]);
	sqrn_Element_1w_x64(T[2],5);
	mul_Element_1w_x64(T[0], T[0], T[2]);
	copy_Element_1w_x64(T[2],T[0]);
	sqrn_Element_1w_x64(T[2],10);
	mul_Element_1w_x64(T[2], T[2], T[0]);
	copy_Element_1w_x64(T[3],T[2]);
	sqrn_Element_1w_x64(T[3],20);
	mul_Element_1w_x64(T[3], T[3], T[2]);
	sqrn_Element_1w_x64(T[3],10);
	mul_Element_1w_x64(T[3], T[3], T[0]);
	copy_Element_1w_x64(T[0],T[3]);
	sqrn_Element_1w_x64(T[0],50);
	mul_Element_1w_x64(T[0], T[0], T[3]);
	copy_Element_1w_x64(T[2],T[0]);
	sqrn_Element_1w_x64(T[2],100);
	mul_Element_1w_x64(T[2], T[2], T[0]);
	sqrn_Element_1w_x64(T[2],50);
	mul_Element_1w_x64(T[2], T[2], T[3]);
	sqrn_Element_1w_x64(T[2],5);
	mul_Element_1w_x64(T[1], T[1], T[2]);
}

static int compare_Element_1w_x64(uint64_t *A, uint64_t *B)
{
	return compare_bytes((uint8_t*)A,(uint8_t*)B,SIZE_FP25519);
}

static void invsqrt_Element_1w_x64(uint64_t *const uv_p38, uint64_t *const u, uint64_t *const v)
{
	Element_1w_x64 sqrt_minus_one = {
			0xc4ee1b274a0ea0b0,
			0x2f431806ad2fe478,
			0x2b4d00993dfbd7a7,
			0x2b8324804fc1df0b
	};
	Element_1w_x64 v2;
	Element_1w_x64 uv,uv3,uv7;
	uint64_t counter;
	Element_1w_x64 x2, x9, uv7_p58;
	uint64_t * Tab[4];

	/*  uv = u*v     */
	mul_Element_1w_x64(uv,u,v);
	copy_Element_1w_x64(v2,v);
	/*  v2 = v^2     */
	sqr_Element_1w_x64(v2);
	/*  uv3 = uv*v^2 */
	mul_Element_1w_x64(uv3,uv,v2);
	/*  v2 = (v^2)^2 */
	sqr_Element_1w_x64(v2);
	/*  uv7 = uv3*v2 */
	mul_Element_1w_x64(uv7,uv3,v2);

	Tab[0] = x2;
	Tab[1] = x9;
	Tab[2] = uv7_p58;
	Tab[3] = uv7;

	copy_Element_1w_x64(Tab[0],uv7);
	sqr_Element_1w_x64(Tab[0]);/* x^2 */
	/* 0 */
	copy_Element_1w_x64(Tab[1],Tab[0]);
	sqrn_Element_1w_x64(Tab[1],2);
	mul_Element_1w_x64(Tab[1], Tab[1], Tab[3]);
	/* 1 */
	mul_Element_1w_x64(Tab[0], Tab[0], Tab[1]);
	/* 2 */
	sqrn_Element_1w_x64(Tab[0],1);
	mul_Element_1w_x64(Tab[0], Tab[0], Tab[1]);
	/* 3 */
	copy_Element_1w_x64(Tab[1],Tab[0]);
	sqrn_Element_1w_x64(Tab[1],5);
	mul_Element_1w_x64(Tab[1], Tab[1], Tab[0]);
	/* 4 */
	copy_Element_1w_x64(Tab[2],Tab[1]);
	sqrn_Element_1w_x64(Tab[2],5);
	mul_Element_1w_x64(Tab[2], Tab[2], Tab[0]);
	/* 5 */
	copy_Element_1w_x64(Tab[1],Tab[2]);
	sqrn_Element_1w_x64(Tab[1],15);
	mul_Element_1w_x64(Tab[1], Tab[1], Tab[2]);
	/* 6 */
	copy_Element_1w_x64(Tab[2],Tab[1]);
	sqrn_Element_1w_x64(Tab[2],30);
	mul_Element_1w_x64(Tab[2], Tab[2], Tab[1]);
	/* 7 */
	copy_Element_1w_x64(Tab[1],Tab[2]);
	sqrn_Element_1w_x64(Tab[1],60);
	mul_Element_1w_x64(Tab[1], Tab[1], Tab[2]);
	/* 8 */
	sqrn_Element_1w_x64(Tab[1],5);
	mul_Element_1w_x64(Tab[1], Tab[1], Tab[0]);
	/* 9 */
	copy_Element_1w_x64(Tab[2],Tab[1]);
	sqrn_Element_1w_x64(Tab[2],125);
	mul_Element_1w_x64(Tab[2], Tab[2], Tab[1]);
	/* 10 */
	sqrn_Element_1w_x64(Tab[2],2);
	mul_Element_1w_x64(Tab[2], Tab[2], Tab[3]);

	/* uv^(p+3)/8 = uv3*uv^(p-5)/8  */
	mul_Element_1w_x64(uv_p38,uv7_p58,uv3);

	/**
	 * Checking whether
	 *      v*uv_p38^2 == -u
	 **/
	copy_Element_1w_x64(uv3,uv_p38);
	/* uv3 = uv_p38^2  */
	sqr_Element_1w_x64(uv3);
	/* uv  = uv3*v     */
	mul_Element_1w_x64(uv,uv3,v);
	/*  u  = -u        */
	neg_Element_1w_x64(u);

	if( compare_Element_1w_x64(uv,u) == 0)
	{
		/*  uv_p38 = uv_p38*sqrt(-1) */
		mul_Element_1w_x64(uv_p38, uv_p38, sqrt_minus_one);
	}
}

static void sqrt_Element_1w_x64(uint64_t * C,uint64_t * A)
{
	invsqrt_Element_1w_x64(C,C,A);
}

static __inline void fred_Element_1w_x64(uint64_t *const c)
{
	int64_t last = (((int64_t*)c)[3])>>63;
	c[3] &= ((uint64_t)1<<63)-1;
	c[0] += 19 & last;
}

/** Util functions */

static void random_Element_1w_x64(uint64_t *A)
{
	random_bytes((uint8_t*)A,SIZE_FP25519);
	A[3] &= ((uint64_t)1<<63)-1;
}


static void print_Element_1w_x64(uint64_t *A)
{
	print_bytes((uint8_t*)A,SIZE_FP25519);
}

static uint64_t * new_Element_1w_x64()
{
	return (uint64_t*) allocate_bytes(NUM_DIGITS_FP25519_X64 * sizeof(uint64_t));
}



