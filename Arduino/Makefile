SKETCH := app
CORE := arduino:avr
FQBN := $(CORE):uno
PORT ?= /dev/ttyUSB0
BAUDRATE := 115200
BUILD_DIR := build
DOCS_DIR := docs
LIBS_DIR := libs

.PHONY: clangd setup build flash clean all monitor install-libs

all: build

clangd:
	arduino-cli compile $(SKETCH) --fqbn $(FQBN) \
		--only-compilation-database --build-path $(BUILD_DIR)

setup:
	arduino-cli core update-index
	arduino-cli core install $(CORE)

build:
	arduino-cli compile $(SKETCH) --fqbn $(FQBN) \
		--build-path $(BUILD_DIR)

flash: build
	arduino-cli upload $(SKETCH) --fqbn $(FQBN) \
		--port $(PORT) --build-path $(BUILD_DIR)

monitor:
	arduino-cli monitor --port $(PORT) \
		--config baudrate=$(BAUDRATE)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(DOCS_DIR)


install-libs:
	arduino-cli config set library.enable_unsafe_install true
	@for zip in $(LIBS_DIR)/*.zip; do \
		arduino-cli lib install --zip-path "$$zip" || exit 1; \
	done

docs:
	doxygen Doxyfile