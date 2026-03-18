.PHONY: serve default
.FORCE:

targets := hello
run := uv run

default: debug

serve:
	@tmux new "$(run) zensical serve" \; splitw "$(run) entangled watch"

# Make will remove this file if not made precious!
.PRECIOUS: build/%/build.ninja

# Once build.ninja is generated, it will auto-update with meson.build
# through running ninja. By making meson.build an order-only dependency
# using the | symbol, this rule is only run if build.ninja doesn't exist.
build/%/build.ninja: | meson.build
	$(run) meson setup --buildtype $(@D:build/%=%) $(@D)

# By adding .FORCE we always run this rule when asked for. Ninja can find
# out if anything needs to happen or not.
build/%/hello: build/%/build.ninja .FORCE
	$(run) ninja -C $(@D) hello

debug: build/debug/hello build/debug/jetzt
release: build/release/hello build/release/jetzt
