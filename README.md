# Andrew's QMK userspace

Wired QMK port of the Corne-ish Zen layout in
[`zmk-config`](https://github.com/andrewxhc/zmk-config), targeting a
`sofle/rev1` with classic ATmega32U4 Pro Micro controllers.

The build target is:

```text
sofle/rev1:andrewxhc
```

Initialize the pinned community-module dependency after cloning:

```text
git submodule update --init --recursive
```

QMK is pinned to commit
`f0e090f67a90f9b653faeddbf5a1c4f75e24e91a` (`0.33.11`) in the build
workflow. Both halves use the same `.hex`; with the retained stock Sofle
handedness detection, connect USB to the physical left half.

See [PORTING.md](PORTING.md) for the physical mapping, behavior inventory,
reference review, compromises, recovery procedure, and validated local build
command.
