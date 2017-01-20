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
#include<string.h>

static unsigned char *DeriveKey(size_t *secret_len,char *PeerPubFile,char *LocalPrivFile)
{
    char *secret = NULL;
    BIO *b64;
    EVP_PKEY_CTX *ctx = NULL;
    EVP_PKEY *LocalPrivKey = NULL;
    EVP_PKEY *PeerPubKey = NULL;

    BIO *PeerPubBio = NULL;
    BIO *LocalPriBio = NULL;
    BIO *bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);

    PeerPubBio = BIO_new_file(PeerPubFile, "r");
    if (!PeerPubBio)
    {
        BIO_printf(bio_err, "Can't open parameter file %s\n", PeerPubFile);
        return NULL;
    }

    PeerPubKey = PEM_read_bio_PUBKEY(PeerPubBio, NULL,NULL,NULL);

    LocalPriBio = BIO_new_file(LocalPrivFile, "r");

    if (!LocalPriBio)
    {
        BIO_printf(bio_err, "Can't open parameter file %s\n", LocalPrivFile);
        return NULL;
    }

    LocalPrivKey = PEM_read_bio_PrivateKey(LocalPriBio, NULL,NULL,NULL);


    BIO_free(PeerPubBio);
    BIO_free(LocalPriBio);


    if (!LocalPrivKey || !PeerPubKey)
        goto end;
//
    ctx = EVP_PKEY_CTX_new(LocalPrivKey,NULL);
    EVP_PKEY_free(LocalPrivKey);
//
//
    if (!ctx)
        goto end;

    if (EVP_PKEY_derive_init(ctx) <= 0)
        goto err;

    if (EVP_PKEY_derive_set_peer(ctx, PeerPubKey) <= 0)
        goto err;

    if (EVP_PKEY_derive(ctx, NULL, secret_len) <= 0)
        goto err;

    secret = OPENSSL_malloc(*secret_len);
//

    if(!secret)
        return NULL;
//
    if (EVP_PKEY_derive(ctx, secret, secret_len) <= 0)
        goto err;

    // Generate fucking base64, but there are some bigend problem if you use base64 -d to decode output directly.
    //I don't know why, but it's work

    b64 = BIO_new(BIO_f_base64());
    BIO_push(b64,bio_err);
    BIO_write(b64,secret,strlen(secret));
    BIO_flush(b64);
    BIO_free_all(b64);

    err:
    ERR_print_errors_fp(stderr);
    goto end;

    end:
    if(ctx) EVP_PKEY_CTX_free(ctx);
    if(PeerPubKey) EVP_PKEY_free(PeerPubKey);
    return secret;

}

int main(int argc,char **argv)
{
    char *secret;
    size_t length = 32;
    if(argc < 2)
    {
        printf("argv1 is Pub,argv2 is Priv\n");
    }

    if (argc == 3)
        secret = DeriveKey(&length, argv[1],argv[2]);
    return 0;
}
