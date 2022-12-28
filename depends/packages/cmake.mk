package=cmake
$(package)_version=3.14.7
$(package)_download_path=https://github.com/Kitware/CMake/releases/download/v
3.14.7/
$(package)_file_name=$(package)-$($(package)_version).tar.gz
$(package)_sha256_hash=9221993e0af3e6d10124d840ff24f5b2f3b884416fca04d3312cb0
388dec1385
$(package)_patches=cmake-debian-gcc-8.patch

define $(package)_preprocess_cmds
  patch -p1 < $($(package)_patch_dir)/cmake-debian-gcc-8.patch
endef

define $(package)_config_cmds
  export CC="" && \
  export CXX="" && \
  ./bootstrap --prefix=$(host_prefix) -- -DCMAKE_USE_OPENSSL=OFF
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef
