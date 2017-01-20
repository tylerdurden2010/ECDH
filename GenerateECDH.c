//
//
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <stdio.h>
#include <openssl/err.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>
#include <openssl/pem.h>


static void *generate_pem_file(EVP_PKEY *evp_pkey,char *PubKey,char *PrivKey)
{
    FILE *fp;

    if((fp = fopen(PubKey, "w")) == NULL)
        goto err;

    if (!PEM_write_PUBKEY(fp, evp_pkey))
        goto err;

    if((fp = fopen(PrivKey, "w")) == NULL)
        goto err;

    if (!PEM_write_PrivateKey(fp, evp_pkey, NULL, NULL, 0, 0, NULL))
        goto err;


    printf("\tGenerated EC Key Pair.\n\tWritten PEM files.\n");
    goto end;

    err:
    ERR_print_errors_fp(stderr);
    goto end;

    end:
    fclose(fp);
}



int main(int argc , char **argv)
{

    EVP_PKEY_CTX *pctx = NULL, *kctx = NULL;

    EVP_PKEY *pkey = NULL, *peerkey, *params = NULL;

    /* Create the context for parameter generation */
    if(NULL == (pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL))) goto err;

    /* Initialise the parameter generation */
    if(1 != EVP_PKEY_paramgen_init(pctx))  goto err;

    /* NIST-Prime-256 = ANSI X9.62 Prime 256v1 curve in OpenSSL enum value */
    if(1 != EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_X9_62_prime256v1)) goto err;

    /* Create the parameter object params */
    if (!EVP_PKEY_paramgen(pctx, &params)) goto err;

    /* Create the context for the key generation */
    if(NULL == (kctx = EVP_PKEY_CTX_new(params, NULL))) goto err;

    /* Generate the key */
    if(1 != EVP_PKEY_keygen_init(kctx)) goto err;
    if (1 != EVP_PKEY_keygen(kctx, &pkey)) goto err;

    /* Generate the Key PEM files  */
    if (argc == 3)
        generate_pem_file(pkey,argv[1],argv[2]);
    else
    {
        printf("./GenECDH PubKeyFilename PrivKeyFilename\n");
        return 0;
    }

    goto end;

    err:

    ERR_print_errors_fp(stderr);

    end:

    if(pkey) EVP_PKEY_free(pkey);
    if(kctx) EVP_PKEY_CTX_free(kctx);
    if(params) EVP_PKEY_free(params);
    if(pctx) EVP_PKEY_CTX_free(pctx);
    return 0;


}
