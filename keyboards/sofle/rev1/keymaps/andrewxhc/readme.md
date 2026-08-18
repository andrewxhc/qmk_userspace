# Andrew XHC's Sofle rev1 keymap

This is the wired QMK port of the Corne-ish Zen layout from `zmk-config`.
It targets `sofle/rev1` with classic ATmega32U4 Pro Micro controllers.

Build target:

```text
sofle/rev1:andrewxhc
```

The keymap-local `rules.mk` selects the shared `users/andrewxhc` module, which
contains the feature configuration and behavior implementations. The
`keymap.json` also enables Getreuer's Cyclotab community module for the app and
window swappers. See the
repository's [`PORTING.md`](../../../../../PORTING.md) for physical mapping,
reference provenance, compromises, validation, and flashing instructions.
