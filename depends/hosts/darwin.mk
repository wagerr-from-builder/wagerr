#OSX_MIN_VERSION=10.10
#OSX_SDK_VERSION=10.11
#OSX_SDK=$(SDK_PATH)/MacOSX$(OSX_SDK_VERSION).sdk

OSX_MIN_VERSION=10.14
OSX_SDK_VERSION=10.15.6
XCODE_VERSION=12.1
XCODE_BUILD_ID=12A7403
LD64_VERSION=609

OSX_SDK=$(SDK_PATH)/Xcode-$(XCODE_VERSION)-$(XCODE_BUILD_ID)-extracted-SDK-with-libcxx-headers


LD64_VERSION=253.9
darwin_CC=clang -target $(host) -mmacosx-version-min=$(OSX_MIN_VERSION) --sysroot $(OSX_SDK) -mlinker-version=$(LD64_VERSION)
darwin_CXX=clang++ -target $(host) -mmacosx-version-min=$(OSX_MIN_VERSION) --sysroot $(OSX_SDK) -mlinker-version=$(LD64_VERSION) -stdlib=libc++

darwin_CFLAGS=-pipe
darwin_CXXFLAGS=$(darwin_CFLAGS)

darwin_release_CFLAGS=-O2
darwin_release_CXXFLAGS=$(darwin_release_CFLAGS)

darwin_debug_CFLAGS=-O1
darwin_debug_CXXFLAGS=$(darwin_debug_CFLAGS)

darwin_native_toolchain=native_cctools
