#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <iostream>
#include <string>

int main() {
    const char* host = "10.0.16.143";
    const char* port = "443";
    const char* path = "/_/rweb/realms";

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    const SSL_METHOD* method = TLS_client_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    // 强制使用 TLS1.2
    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
    SSL_CTX_set_max_proto_version(ctx, TLS1_2_VERSION);

    // ❗禁用证书验证（自签证书专用）
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);

    BIO* bio = BIO_new_ssl_connect(ctx);
    if (!bio) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return 1;
    }

    std::string address = std::string(host) + ":" + port;
    BIO_set_conn_hostname(bio, address.c_str());

    SSL* ssl = nullptr;
    BIO_get_ssl(bio, &ssl);
    if (!ssl) {
        std::cerr << "Failed to get SSL pointer from BIO\n";
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return 1;
    }
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

    // 使用 SSL_connect 进行连接
    if (SSL_connect(ssl) <= 0) {
        std::cerr << "SSL_connect failed to establish connection to " << host << "\n";
        ERR_print_errors_fp(stderr);
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return 1;
    }

    std::cout << "SSL connection established with " << SSL_get_cipher(ssl) << "\n";

    // 构造 HTTP 请求
    std::string request = "GET " + std::string(path) + " HTTP/1.1\r\n"
                          "Host: " + host + "\r\n"
                          "Connection: close\r\n\r\n";
    BIO_write(bio, request.c_str(), request.length());

    char buffer[4096];
    int bytes = 0;
    while ((bytes = BIO_read(bio, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';
        std::cout << buffer;
    }
    std::cout << std::endl;

    // 清理资源
    BIO_free_all(bio);
    SSL_CTX_free(ctx);
    EVP_cleanup();

    return 0;
}
