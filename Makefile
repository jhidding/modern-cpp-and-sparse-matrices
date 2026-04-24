.PHONY: serve default
.FORCE:

targets = hello jetzt eigen-hello random-values least-squares ls-bench
vendor_deps := argparse-3.2 eigen-5.0.1 benchmark-1.9.5
run := uv run

_bullet := "\\e[1m•\\e[m"
_green := "\\e[32m"
_cyan := "\\e[36m"
_reset := "\\e[m"

default: debug

serve:
	@tmux new "$(run) zensical serve" \; splitw "$(run) entangled watch"

# Make will remove this file if not made precious!
.PRECIOUS: build/%/build.ninja

vendor_breadcrumbs := $(vendor_deps:%=vendor/%) vendor/benchmark-1.9.5/build/src/libbenchmark.a

# Once build.ninja is generated, it will auto-update with meson.build
# through running ninja. By making meson.build an order-only dependency
# using the | symbol, this rule is only run if build.ninja doesn't exist.
build/%/build.ninja: $(vendor_breadcrumbs) | meson.build
	@echo -e "$(_bullet) Running $(_cyan)meson setup$(_reset) for buildtype $(_green)$(@D:build/%=%)$(_reset)"
	@$(run) meson setup --buildtype $(@D:build/%=%) $(@D)

# By adding .FORCE we always run this rule when asked for. Ninja can find
# out if anything needs to happen or not.
define executable_template =
build/%/$(1): build/%/build.ninja .FORCE
	@echo -e "$(_bullet) Building $(_cyan)$$(@D:build/%=%)$(_reset) executable $(_green)$(1)$(_reset)"
	@$(run) ninja -C $$(@D) $(1)
endef

$(foreach tgt,$(targets),$(eval $(call executable_template,$(tgt))))

debug: $(targets:%=build/debug/%)
	@ln -sf debug/compile_commands.json build/

release: $(targets:%=build/release/%)

# Install Vendor libraries

eigen_src := "https://gitlab.com/libeigen/eigen/-/archive/5.0.1/eigen-5.0.1.tar.bz2"
argparse_src := "https://github.com/p-ranav/argparse/archive/refs/tags/v3.2.tar.gz"
benchmark_src := "https://github.com/google/benchmark/archive/refs/tags/v1.9.5.tar.gz"

vendor/.gitignore:
	@mkdir $(@D)
	@echo "*" > $@

vendor/argparse-3.2: | vendor/.gitignore
	@echo -e "$(_bullet) Downloading $(_cyan)argparse 3.2$(_reset)"
	@curl -L $(argparse_src) | tar xzf - -C vendor

vendor/eigen-5.0.1: | vendor/.gitignore
	@echo -e "$(_bullet) Downloading $(_cyan)eigen 5.0.1$(_reset)"
	@curl -L $(eigen_src) | tar xjf - -C vendor

vendor/benchmark-1.9.5: | vendor/.gitignore
	@echo -e "$(_bullet) Downloading $(_cyan)benchamrk 1.9.5$(_reset)"
	@curl -L $(benchmark_src) | tar xzf - -C vendor

vendor/benchmark-1.9.5/build/src/libbenchmark.a: | vendor/benchmark-1.9.5
	@cd vendor/benchmark-1.9.5; \
		cmake -E make_directory "build"; \
		cmake -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release -S . -B "build"; \
		cmake --build "build" --config Release

vendor: $(vendor_breadcrumbs)
