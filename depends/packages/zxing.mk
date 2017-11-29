package=zxing
$(package)_version=0.0.1
$(package)_download_path=https://api.github.com/repos/glassechidna/zxing-cpp/tarball
$(package)_file_name=master
$(package)_sha256_hash=75a12556e40b28db65906768566379b6a0b6dddb7bfd281343ec239efe74799e

define $(package)_set_vars
  $(package)_cxxflags=-std=c++11
endef

define $(package)_config_cmds
  mkdir -p build &&\
  cd build &&\
  cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$(host_prefix) -std=c++11 ..
endef

define $(package)_build_cmds
 cd $($(package)_extract_dir)/build && $(MAKE)
endef

define $(package)_stage_cmds
  cd $($(package)_extract_dir)/build && $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef
