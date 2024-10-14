build: clean
	meson setup build
	ninja -C build
install:
	ninja -C build install
clean:
	rm -rf build || true
