.PHONY: serve default vendor
.FORCE:

targets = hello jetzt eigen-hello random-values solve-dense
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

# Once build.ninja is generated, it will auto-update with meson.build
# through running ninja. By making meson.build an order-only dependency
# using the | symbol, this rule is only run if build.ninja doesn't exist.
build/%/build.ninja: | meson.build
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

debug: $(targets:%=build/debug/%) | vendor
	@ln -sf debug/compile_commands.json build/

release: $(targets:%=build/release/%) | vendor

# Install Vendor libraries

eigen_src := "https://gitlab.com/libeigen/eigen/-/archive/5.0.1/eigen-5.0.1.tar.bz2"
argparse_src := "https://github.com/p-ranav/argparse/archive/refs/tags/v3.2.tar.gz"

vendor/argparse-3.2:
	@echo -e "$(_bullet) Downloading $(_cyan)argparse 3.2$(_reset)"
	@curl -s $(argparse_src) | tar xzvf - -C vendor

vendor/eigen-5.0.1:
	@echo -e "$(_bullet) Downloading $(_cyan)eigen 5.0.1$(_reset)"
	@curl -s $(eigen_src) | tar xjvf - -C vendor

vendor: vendor/argparse-3.2 vendor/eigen-5.0.1
