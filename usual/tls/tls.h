/* $OpenBSD: tls.h,v 1.12 2015/03/31 14:03:38 jsing Exp $ */
/*
 * Copyright (c) 2014 Joel Sing <jsing@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _USUAL_HEADER_TLS_H_
#define _USUAL_HEADER_TLS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <usual/base.h>

#define TLS_API	20141031

#define TLS_PROTOCOL_TLSv1_0	(1 << 1)
#define TLS_PROTOCOL_TLSv1_1	(1 << 2)
#define TLS_PROTOCOL_TLSv1_2	(1 << 3)
#define TLS_PROTOCOL_TLSv1 \
	(TLS_PROTOCOL_TLSv1_0|TLS_PROTOCOL_TLSv1_1|TLS_PROTOCOL_TLSv1_2)

#define TLS_PROTOCOLS_ALL TLS_PROTOCOL_TLSv1
#define TLS_PROTOCOLS_DEFAULT TLS_PROTOCOL_TLSv1_2

#define TLS_WANT_POLLIN		-2
#define TLS_WANT_POLLOUT	-3
#define TLS_NO_CERT		-4
#define TLS_NO_OCSP		-5

struct tls;
struct tls_config;

#define TLS_OCSP_STATUS_SUCCESSFUL	0
#define TLS_OCSP_STATUS_MALFORMED	1
#define TLS_OCSP_STATUS_INTERNALERR	2
#define TLS_OCSP_STATUS_TRYLATER	3
#define TLS_OCSP_STATUS_SIGREQUIRED	5
#define TLS_OCSP_STATUS_UNAUTHORIZED	6

struct tls_ocsp_info {
	int status;
	int reason;
	time_t this_update;
	time_t next_update;
	time_t revocation_time;
};

int tls_init(void);

const char *tls_error(struct tls *_ctx);

struct tls_config *tls_config_new(void);
void tls_config_free(struct tls_config *_config);

int tls_config_set_ca_file(struct tls_config *_config, const char *_ca_file);
int tls_config_set_ca_path(struct tls_config *_config, const char *_ca_path);
int tls_config_set_ca_mem(struct tls_config *_config, const uint8_t *_ca,
    size_t _len);
int tls_config_set_cert_file(struct tls_config *_config,
    const char *_cert_file);
int tls_config_set_cert_mem(struct tls_config *_config, const uint8_t *_cert,
    size_t _len);
int tls_config_set_ciphers(struct tls_config *_config, const char *_ciphers);
int tls_config_set_dheparams(struct tls_config *_config, const char *_params);
int tls_config_set_ecdhecurve(struct tls_config *_config, const char *_name);
int tls_config_set_key_file(struct tls_config *_config, const char *_key_file);
int tls_config_set_key_mem(struct tls_config *_config, const uint8_t *_key,
    size_t _len);
int tls_config_set_ocsp_stapling_file(struct tls_config *_config, const char *_blob_file);
int tls_config_set_ocsp_stapling_mem(struct tls_config *_config, const uint8_t *_blob, size_t _len);
void tls_config_set_protocols(struct tls_config *_config, uint32_t _protocols);
void tls_config_set_verify_depth(struct tls_config *_config, int _verify_depth);

void tls_config_prefer_ciphers_client(struct tls_config *_config);
void tls_config_prefer_ciphers_server(struct tls_config *_config);

void tls_config_insecure_noverifycert(struct tls_config *_config);
void tls_config_insecure_noverifyname(struct tls_config *_config);
void tls_config_verify(struct tls_config *_config);

void tls_config_verify_client(struct tls_config *_config);
void tls_config_verify_client_optional(struct tls_config *_config);

void tls_config_clear_keys(struct tls_config *_config);
int tls_config_parse_protocols(uint32_t *_protocols, const char *_protostr);

struct tls *tls_client(void);
struct tls *tls_server(void);
int tls_configure(struct tls *_ctx, struct tls_config *_config);
void tls_reset(struct tls *_ctx);
void tls_free(struct tls *_ctx);

int tls_accept_fds(struct tls *_ctx, struct tls **_cctx, int _fd_read,
    int _fd_write);
int tls_accept_socket(struct tls *_ctx, struct tls **_cctx, int _socket);
int tls_connect(struct tls *_ctx, const char *_host, const char *_port);
int tls_connect_fds(struct tls *_ctx, int _fd_read, int _fd_write,
    const char *_servername);
int tls_connect_servername(struct tls *_ctx, const char *_host,
    const char *_port, const char *_servername);
int tls_connect_socket(struct tls *_ctx, int _s, const char *_servername);
int tls_handshake(struct tls *_ctx);
ssize_t tls_read(struct tls *_ctx, void *_buf, size_t _buflen);
ssize_t tls_write(struct tls *_ctx, const void *_buf, size_t _buflen);
int tls_close(struct tls *_ctx);

int tls_peer_cert_provided(struct tls *ctx);
int tls_peer_cert_contains_name(struct tls *ctx, const char *name);

const char * tls_peer_cert_hash(struct tls *_ctx);
const char * tls_peer_cert_issuer(struct tls *ctx);
const char * tls_peer_cert_subject(struct tls *ctx);

uint8_t *tls_load_file(const char *_file, size_t *_len, char *_password);

ssize_t tls_get_connection_info(struct tls *ctx, char *buf, size_t buflen);

int tls_ocsp_refresh_stapling(struct tls **ocsp_ctx_p, int *async_fd_p, struct tls_config *config);
int tls_ocsp_check_peer(struct tls **ocsp_ctx_p, int *async_fd_p, struct tls *client);

struct tls_ocsp_info *tls_get_ocsp_info(struct tls *ctx);
void tls_ocsp_info_free(struct tls_ocsp_info *info);

#ifdef __cplusplus
}
#endif

#endif /* HEADER_TLS_H */
