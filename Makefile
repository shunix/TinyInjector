all:
	@ndk-build -B\
	 NDK_PROJECT_PATH=.\
	 NDK_APPLICATION_MK=./Application.mk\
	 NDK_APP_DST_DIR=./build/\
	 NDK_APP_OUT=./build/
clean:
	@rm -rf ./build
install:
	@adb push build/injector /sdcard/
	@adb shell su -c "cp /storage/emulated/0/injector /data/local/"
	@adb shell su -c "chmod 777 /data/local/injector"
