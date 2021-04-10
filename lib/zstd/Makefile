zstd_DIR	= $(NXDK_DIR)/lib/zstd/zstd
zstd_SRCS	=	$(zstd_DIR)/lib/common/debug.c \
				$(zstd_DIR)/lib/common/entropy_common.c \
				$(zstd_DIR)/lib/common/error_private.c \
				$(zstd_DIR)/lib/common/fse_decompress.c \
				$(zstd_DIR)/lib/common/pool.c \
				$(zstd_DIR)/lib/common/threading.c \
				$(zstd_DIR)/lib/common/xxhash.c \
				$(zstd_DIR)/lib/common/zstd_common.c
zstd_SRCS	+=	$(zstd_DIR)/lib/compress/fse_compress.c \
				$(zstd_DIR)/lib/compress/hist.c \
				$(zstd_DIR)/lib/compress/huf_compress.c \
				$(zstd_DIR)/lib/compress/zstdmt_compress.c \
				$(zstd_DIR)/lib/compress/zstd_compress.c \
				$(zstd_DIR)/lib/compress/zstd_compress_literals.c \
				$(zstd_DIR)/lib/compress/zstd_compress_sequences.c \
				$(zstd_DIR)/lib/compress/zstd_compress_superblock.c \
				$(zstd_DIR)/lib/compress/zstd_double_fast.c \
				$(zstd_DIR)/lib/compress/zstd_fast.c \
				$(zstd_DIR)/lib/compress/zstd_lazy.c \
				$(zstd_DIR)/lib/compress/zstd_ldm.c \
				$(zstd_DIR)/lib/compress/zstd_opt.c
zstd_SRCS	+=	$(zstd_DIR)/lib/decompress/huf_decompress.c \
				$(zstd_DIR)/lib/decompress/zstd_ddict.c \
				$(zstd_DIR)/lib/decompress/zstd_decompress.c \
				$(zstd_DIR)/lib/decompress/zstd_decompress_block.c
zstd_SRCS	+=	$(zstd_DIR)/lib/dictBuilder/cover.c \
				$(zstd_DIR)/lib/dictBuilder/divsufsort.c \
				$(zstd_DIR)/lib/dictBuilder/fastcover.c \
				$(zstd_DIR)/lib/dictBuilder/zdict.c

zstd_OBJS = $(addsuffix .obj, $(basename $(zstd_SRCS)))

NXDK_CFLAGS += -I $(zstd_DIR)/lib

$(NXDK_DIR)/lib/libzstd.lib: $(zstd_OBJS)

main.exe: $(NXDK_DIR)/lib/libzstd.lib

CLEANRULES += clean-zstd

.PHONY: clean-zstd
clean-zstd:
	$(VE)rm -f $(zstd_OBJS) $(NXDK_DIR)/lib/libzstd.lib
