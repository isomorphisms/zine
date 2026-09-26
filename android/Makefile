# Host-only Android build. Recipes are single tool invocations, not a shell script.
root := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
out := $(root)/android/build
sdk := $(ANDROID_HOME)
ndk := $(sdk)/ndk/27.2.12479018
tools := $(sdk)/build-tools/36.0.0
llvm := $(ndk)/toolchains/llvm/prebuilt/linux-x86_64/bin
ABI ?= armeabi-v7a
compiler_armeabi-v7a := armv7a-linux-androideabi26-clang
compiler_x86_64 := x86_64-linux-android26-clang
stage := $(out)/$(ABI)
apk := $(out)/zine-$(ABI).apk
signer := $(root)/.test-signing/_/build/app/wegert-debug.keystore

.PHONY: all verify
all: $(apk)

$(stage)/lib/$(ABI)/libzine.so: $(root)/android/reader.c
	mkdir -p $(dir $@)
	$(llvm)/$(compiler_$(ABI)) -std=c17 -Os -Wall -Wextra -Werror -fPIC -shared -Wl,--no-undefined -Wl,-z,relro,-z,now $< -landroid -llog -o $@
	$(llvm)/llvm-strip --strip-unneeded $@

$(stage)/base.apk: $(root)/android/AndroidManifest.xml $(root)/poems.tsv $(root)/sequence.tsv $(wildcard $(root)/poems/*.txt)
	mkdir -p $(stage)/assets/poems
	cp $(root)/poems.tsv $(root)/sequence.tsv $(stage)/assets/
	cp $(root)/poems/*.txt $(stage)/assets/poems/
	$(tools)/aapt2 link -I $(sdk)/platforms/android-36/android.jar --manifest $< --min-sdk-version 26 --target-sdk-version 34 --version-code 1 --version-name 0.1.0 -A $(stage)/assets -o $@

$(stage)/unsigned.apk: $(stage)/base.apk $(stage)/lib/$(ABI)/libzine.so
	cp $< $@
	cd $(stage) && zip -q -0 $(abspath $@) lib/$(ABI)/libzine.so

$(apk): $(stage)/unsigned.apk
	test -f $(signer)
	$(tools)/zipalign -f -P 16 4 $< $(stage)/aligned.apk
	$(tools)/apksigner sign --ks $(signer) --ks-key-alias wegert-debug --ks-pass pass:wegert-debug --key-pass pass:wegert-debug --out $@ $(stage)/aligned.apk
	$(tools)/apksigner verify --verbose --print-certs $@
	$(tools)/apksigner verify --print-certs $@ > $@.signer.txt
	grep -Fx 'Signer #1 certificate SHA-256 digest: de9b1d47c5a65e6d46a204b79dd9ee566b9d3c9832ba81ebc4213d3392e92ff9' $@.signer.txt
	sha256sum $@ > $@.sha256

verify: $(apk)
	$(tools)/aapt2 dump badging $(apk)
	$(tools)/apksigner verify --verbose --print-certs $(apk)
