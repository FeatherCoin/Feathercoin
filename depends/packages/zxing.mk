package=zxing
$(package)_version=0.0.1
$(package)_download_path=https://github.com/glassechidna/zxing-cpp/archive
$(package)_file_name=master.zip
$(package)_sha256_hash=f57675d77b05fb8db16a1dfddd55c1c52ed6ad5f59c49ac59cff9e3ca41387b1

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
