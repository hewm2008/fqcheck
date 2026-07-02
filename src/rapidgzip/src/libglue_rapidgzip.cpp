/*
 * libglue_rapidgzip.cpp — C wrapper for rapidgzip parallel decompression
 */
#include "libglue_rapidgzip.hpp"

#include "rapidgzip/rapidgzip.hpp"
#include "filereader/Standard.hpp"
#include <cstring>
#include <cstdio>
#include <unistd.h>

namespace {

using RgzipReader = rapidgzip::ParallelGzipReader<>;
struct RgzipCookie {
    RgzipReader *rg;
};

} // anonymous namespace

static ssize_t
rgzip_read(void *cookie, char *buf, size_t size)
{
    auto *c = static_cast<RgzipCookie *>(cookie);
    return c->rg->read(buf, size);
}

static int
rgzip_close(void *cookie)
{
    auto *c = static_cast<RgzipCookie *>(cookie);
    delete c->rg;
    delete c;
    return 0;
}

extern "C" long long
glue_rapidgzip_decompress_fd(const char *input_path,
                             int output_fd,
                             int num_threads)
{
    try {
        auto fileReader = std::make_unique<rapidgzip::StandardFileReader>(input_path);
        rapidgzip::ParallelGzipReader reader(
            std::move(fileReader),
            num_threads > 0 ? static_cast<size_t>(num_threads) : 0);

        long long total = 0;
        char buf[1024 * 1024];  /* 1MB buffer */

        while (true) {
            size_t n = reader.read(buf, sizeof(buf));
            if (n == 0) break;
            ssize_t written = write(output_fd, buf, n);
            if (written < 0) return -1;
            total += written;
        }
        return total;
    } catch (const std::exception &e) {
        fprintf(stderr, "[rapidgzip] error: %s\n", e.what());
        return -1;
    }
}

extern "C" FILE *
glue_rapidgzip_fopen(const char *input_path, int num_threads)
{
    try {
        auto fileReader = std::make_unique<rapidgzip::StandardFileReader>(input_path);
        auto *rg = new rapidgzip::ParallelGzipReader<>(
            std::move(fileReader),
            num_threads > 0 ? static_cast<size_t>(num_threads) : 0);
        auto *cookie = new RgzipCookie{rg};
        cookie_io_functions_t io = {rgzip_read, nullptr, nullptr, rgzip_close};
        return fopencookie(cookie, "r", io);
    } catch (const std::exception &e) {
        fprintf(stderr, "[rapidgzip] error: %s\n", e.what());
        return nullptr;
    }
}

extern "C" const char *
glue_rapidgzip_version(void)
{
    return "rapidgzip v0.16.0 (wrapped)";
}
