.PHONY: all unit wasm test site preview clean

TARGETS := targets/nts-1_mkii targets/nts-3_kaoss

export GCC_BIN_PATH
export EMCC_BIN_PATH

all: unit

unit:
	@for target_dir in $(TARGETS); do \
		$(MAKE) -C $$target_dir install; \
	done

wasm:
	@for target_dir in $(TARGETS); do \
		$(MAKE) -C $$target_dir wasm-ci; \
	done

test:
	node tests/nts1-midi.test.mjs

site: unit wasm
	bash scripts/assemble-site.sh dist

preview: site
	python3 scripts/serve.py dist

clean:
	@for target_dir in $(TARGETS); do \
		$(MAKE) -C $$target_dir clean; \
	done
	rm -rf dist
