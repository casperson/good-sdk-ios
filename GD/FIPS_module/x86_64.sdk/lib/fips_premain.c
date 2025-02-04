/* ====================================================================
 * Copyright (c) 2005 The OpenSSL Project. Rights for redistribution
 * and usage in source and binary forms are granted according to the
 * OpenSSL license.
 */

 /** Copyright (c) Visto Corporation dba Good Technology, 2014.
 * All rights reserved
 * This is a proprietary modified version of an original file
 * under the above license. 
 */
 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__unix) || defined(__unix__) || defined(__vxworks) || defined(__ANDROID__) || defined(__APPLE__)
#include <unistd.h>
#endif

#ifndef FINGERPRINT_PREMAIN_DSO_LOAD

#if defined(__GNUC__) && __GNUC__>=2
  void FINGERPRINT_premain(void) __attribute__((constructor));
  /* Most commonly this results in pointer to premain to be dropped
   * to .ctors segment, which is traversed by GCC crtbegin.o upon
   * program startup. Except on a.out OpenBSD where it results in
   * _GLOBAL_$I$premain() {premain();} being auto-generated by
   * compiler... But one way or another this is believed to cover
   * *all* GCC targets. */
#elif defined(_MSC_VER)
# ifdef _WINDLL
  __declspec(dllexport)	/* this is essentially cosmetics... */
# endif
  void FINGERPRINT_premain(void);
  static int premain_wrapper(void) { FINGERPRINT_premain(); return 0; }
# ifdef _WIN64
# pragma section(".CRT$XCU",read)
  __declspec(allocate(".CRT$XCU"))
# else
# pragma data_seg(".CRT$XCU")
# endif
  static int (*p)(void) = premain_wrapper;
  /* This results in pointer to premain to appear in .CRT segment,
   * which is traversed by Visual C run-time initialization code.
   * This applies to both Win32 and [all flavors of] Win64. */
# pragma data_seg()
#elif defined(__SUNPRO_C)
  void FINGERPRINT_premain(void);
# pragma init(FINGERPRINT_premain)
  /* This results in a call to premain to appear in .init segment. */
#elif defined(__DECC) && (defined(__VMS) || defined(VMS))
  void FINGERPRINT_premain(void);
# pragma __nostandard
  globaldef { "LIB$INITIALIZ" } readonly _align (LONGWORD)
	int spare[8] = {0};
  globaldef { "LIB$INITIALIZE" } readonly _align (LONGWORD)
	void (*x_FINGERPRINT_premain)(void) = FINGERPRINT_premain;
  /* Refer to LIB$INITIALIZE to ensure it exists in the image. */
  int lib$initialize();
  globaldef int (*lib_init_ref)() = lib$initialize;
# pragma __standard
#elif defined(_TMS320C6X)
# if defined(__TI_EABI__)
  asm("\t.sect \".init_array\"\n\t.align 4\n\t.field FINGERPRINT_premain,32");
# else
  asm("\t.sect \".pinit\"\n\t.align 4\n\t.field _FINGERPRINT_premain,32");
# endif
#elif 0
  The rest has to be taken care of through command line:

	-Wl,-init,FINGERPRINT_premain		on OSF1 and IRIX
	-Wl,+init,FINGERPRINT_premain		on HP-UX
	-Wl,-binitfini:FINGERPRINT_premain	on AIX

  On ELF platforms this results in a call to premain to appear in
  .init segment...
#endif

#ifndef HMAC_SHA1_SIG
#define HMAC_SHA1_SIG "?have to make sure this string is unique"
#endif

#if defined(_MSC_VER)
# pragma const_seg("fipsro")
# pragma const_seg()
  __declspec(allocate("fipsro"))
#endif
static const unsigned char FINGERPRINT_ascii_value[41] = HMAC_SHA1_SIG;

#define atox(c) ((c)>='a'?((c)-'a'+10):((c)>='A'?(c)-'A'+10:(c)-'0'))

extern const void         *FIPS_text_start(),  *FIPS_text_end();
extern const unsigned char FIPS_rodata_start[], FIPS_rodata_end[];
extern unsigned char       FIPS_signature[20];
extern unsigned int        FIPS_incore_fingerprint(unsigned char *,unsigned int);

/*
 * As name suggests this code is executed prior main(). We use this
 * opportunity to fingerprint sequestered code in virtual address
 * space of target application.
 */
void FINGERPRINT_premain(void)
{ 

	#pragma message("GD SDK Info! FIPS mode is not adhered too in X86-64 Simulator builds")

    fprintf(stderr,"GD SDK Info! FIPS mode is not adhered too in X86-64 Simulator builds\n");
    fflush(stdout);
}

#else

#include <openssl/bio.h>
#include <openssl/dso.h>
#include <openssl/err.h>

int main(int argc,char *argv[])
{ DSO *dso;
  DSO_FUNC_TYPE func;
  BIO *bio_err;

    if (argc < 2)
    {	fprintf (stderr,"usage: %s libcrypto.dso\n",argv[0]);
	return 1;
    }

    if ((bio_err=BIO_new(BIO_s_file())) == NULL)
    {	fprintf (stderr,"unable to allocate BIO\n");
	return 1;
    }
    BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);
    ERR_load_crypto_strings();

    dso = DSO_load(NULL,argv[1],NULL,DSO_FLAG_NO_NAME_TRANSLATION);
    if (dso == NULL)
    {	ERR_print_errors(bio_err);
	return 1;
    }

    /* This is not normally reached, because FINGERPRINT_premain should
     * have executed and terminated application already upon DSO_load... */
    func = DSO_bind_func(dso,"FINGERPRINT_premain");
    if (func == NULL)
    {	ERR_print_errors(bio_err);
	return 1;
    }

    (*func)();

  return 0;
}

#endif
