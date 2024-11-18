build: clean
	meson setup build --prefix=/
	ninja -C build
install:
	ninja -C build install
clean:
	rm -rf build || true
