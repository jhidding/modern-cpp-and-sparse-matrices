.PHONY: serve

run := uv run

serve:
	@tmux new "$(run) zensical serve" \; splitw "$(run) entangled watch"

