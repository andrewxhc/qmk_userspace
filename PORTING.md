# ZMK to QMK port record

This file records the provenance, implemented behavior, and intentional
compromises for the `sofle/rev1:andrewxhc` port. It is a decision log, not a
replacement for the keymap source.

## Reproducible inputs

- Source configuration: `zmk-config` commit
  [`45155a4959e9c4a461076b9d8ab33c1cf16b27df`](https://github.com/andrewxhc/zmk-config/commit/45155a4959e9c4a461076b9d8ab33c1cf16b27df).
- Target firmware: QMK tag `0.33.11`, exact commit
  [`f0e090f67a90f9b653faeddbf5a1c4f75e24e91a`](https://github.com/qmk/qmk_firmware/tree/f0e090f67a90f9b653faeddbf5a1c4f75e24e91a).
  The GitHub Actions workflow pins this SHA rather than a moving branch.
- Target keyboard: `sofle/rev1` with the classic ATmega32U4 Pro Micro
  controller. At the pinned QMK revision, Sofle rev1 declares
  [`"development_board": "promicro"`](https://github.com/qmk/qmk_firmware/blob/f0e090f67a90f9b653faeddbf5a1c4f75e24e91a/keyboards/sofle/rev1/keyboard.json),
  and the QMK development-board defaults map `promicro` to
  [`atmega32u4`](https://github.com/qmk/qmk_firmware/blob/f0e090f67a90f9b653faeddbf5a1c4f75e24e91a/data/mappings/defaults.hjson).
- Port entry points:
  [`keymap.c`](keyboards/sofle/rev1/keymaps/andrewxhc/keymap.c) and the shared
  [`users/andrewxhc`](users/andrewxhc/) module.

QMK's [External Userspace](https://docs.qmk.fm/newbs_external_userspace)
layout is used so the keymap remains separate from `qmk_firmware`.

### Pinned CLI scaffold reconciliation

A fresh keymap was generated from the pinned QMK `0.33.11` checkout into the
configured external userspace on the isolated `sofle-default` branch. This
kept the generated files separate from the existing port while preserving the
exact path selection performed by the CLI:

```powershell
qmk config user.overlay_dir="C:/Keyboard Firmware/qmk_userspace"
qmk new-keymap -kb sofle/rev1 -km andrewxhc
```

The CLI placed the scaffold at
`keyboards/sofle/rev1/keymaps/andrewxhc`. QMK can resolve a keymap from the
shared parent location `keyboards/sofle/keymaps`, but in a fresh external
userspace it creates the fully qualified `sofle/rev1` location. The port was
therefore moved to the generated revision-specific path. The build target
remains `sofle/rev1:andrewxhc`.

The generated scaffold remains available in commit `a954c67` on the
`sofle-default` branch for comparison; that stock scaffold was not merged into
the port.

The reconciliation produced four behavior-neutral changes:

- moved the keymap from the shared `keyboards/sofle/keymaps` directory to the
  CLI-generated `keyboards/sofle/rev1/keymaps` directory;
- added keymap-local `config.h` and `readme.md` files to match the scaffold's
  file structure;
- kept the actual compile-time settings in `users/andrewxhc/config.h`, where
  the external-userspace `USER_NAME := andrewxhc` hook loads them; and
- changed the first custom keycode spelling from `SAFE_RANGE` to the
  scaffold's `QK_USER` (`SAFE_RANGE` is an alias for the same value at this
  QMK revision).

The scaffold's stock `LAYOUT`, default Sofle layers, `TRI_LAYER_ENABLE`
configuration, and GPL-2.0-or-later boilerplate were not copied. They describe
the stock keymap rather than this port: `LAYOUT_ZEN`, the APP+NUM state-aware
tri-layer callback, the existing GPL-2.0-only license, and
`USER_NAME := andrewxhc` remain intentional.

## Hardware and physical mapping

The Corne-ish Zen source has 42 positions (3x6+3 per half); the Sofle `LAYOUT`
has 60. `LAYOUT_ZEN` maps the 42 source positions onto the lower three Sofle
rows and the inner three thumb keys on each half. The remaining 18 positions
are hard-disabled with `KC_NO` on every layer:

- all 12 number-row switches;
- both encoder-push switches; and
- the four outer thumb switches.

Encoder rotation, OLED, RGB, and the other cosmetic split state are disabled.
`SPLIT_TRANSPORT_MIRROR` is also removed because it only mirrors master-side
matrix state back to the slave for cosmetic consumers; normal split matrix
transport remains enabled.

Both halves use the same firmware image. This port intentionally retains QMK's
stock Pro Micro split detection: the USB-powered half becomes master through
VBUS detection, and with no handedness override the master is treated as the
left half. Therefore **USB must be connected to the left half**. Supporting
right-side USB later requires an explicit handedness strategy and new
validation; see QMK's [split handedness and Pro Micro VBUS
documentation](https://docs.qmk.fm/features/split_keyboard#setting-handedness).

### Recovery detail

Bootmagic is enabled at its default raw matrix position, row 0 column 0. That
position is the **top-left physical Sofle number-row switch**, not the
`U_ESC_TAB` key one row below. Although the keymap disables it, Bootmagic scans
the raw matrix before normal key processing, so holding that physical switch
while plugging the left half into USB remains the recovery path. Bootmagic also
clears EEPROM; see the official [Bootmagic
documentation](https://docs.qmk.fm/features/bootmagic).

## Layers and runtime selection

| Layer | Access and purpose |
| --- | --- |
| `DSEND` | Startup base. Select from `SYS` or leader sequence `DSEND`. |
| `QWERTY` | Alternate base. Select from `SYS` or leader sequence `QWERTY`. |
| `SYM` | Hold the Space/Symbol thumb key; its tap is Space or the shifted `. ` + one-shot Shift macro. |
| `NAV` | Hold either navigation thumb (`Backspace` or `Enter`) for navigation and editing. |
| `NUM` | Hold Smart Num; tap starts Num Word and a second tap within 200 ms changes it to one-shot `NUM`. |
| `SYS` | Hold the outer top-right base key for OS selection, media, reset/boot, smart-layer controls, and base-layer selection. |
| `MOU` | Smart Mouse toggle or the `L+F` source-position combo. |
| `BTN` | Smart Button control on `SYS`; supplies mouse buttons and mirrored editing/modifier keys. |
| `APP` | Hold App Launcher, or hold the `.` layer-tap on `NUM`, for app/window controls and Meh shortcuts. |
| `FN` | QMK tri-layer: active while both `APP` and `NUM` are active. |

The `SYS` layer selects Windows, macOS, or Linux behavior manually. Windows
with WinCompose Unicode input is the power-up default. OS-aware shortcuts and
Unicode mode change together. These selections are deliberately volatile:
they are updated in RAM without an EEPROM write, and reset to Windows on
restart. `DSEND`/`QWERTY` selection also uses `layer_move()`/`TO()`, not a
persistent default-layer write, so startup returns to `DSEND`.

## Feature matrix

| Source behavior | QMK implementation | Fidelity / decision |
| --- | --- | --- |
| Home-row mods | Native Mod-Tap with per-key 280 ms tapping terms, 175 ms quick tap, [Chordal Hold and Flow Tap](https://docs.qmk.fm/tap_hold) at 150 ms | Source opposite-hand rule retained; all active thumb positions return `'*'` handedness so thumb chords remain eligible holds. Flow Tap scope is weighed below. |
| Ordinary layer/mod taps | Native `LT`/`MT`, per-key permissive hold, hold-on-other-key-press, and quick-tap callbacks | Native where QMK can encode both outcomes. |
| Arbitrary OS navigation hold-taps | Small local tap-preferred state machine | Needed because their hold side is an arbitrary shortcut rather than a layer or modifier. Terms remain 220 ms, or 800 ms for line/document start/end. |
| 34 source combos | Native QMK Combo tables with strict timers, per-combo 18/30 ms terms, 150/50 ms prior-idle gates, and source layer gates | Position and timing intent retained. Combo inputs are anchored to `DSEND` physical keycodes; the three native combo Mod-Taps use Chordal Hold positional gating and timeout-only tap-preferred holds. |
| Parenthesis combo hold sides | `COMBO_ACTION` state machines | QMK `MT` cannot encode mixed-side modifier sets (`RShift+LCtrl` and `RShift+LAlt`). Shift morphs, 175 ms quick tap, 280 ms timeout, and left-hand/thumb hold-trigger-on-release are reproduced explicitly. |
| Comma/dot modifier morphs | Native layer-scoped [Key Overrides](https://docs.qmk.fm/features/key_overrides) | Shift gives `?`/`!`; Ctrl+Shift gives `_`/grave. Native suppression and release handling were preferred to another record hook. |
| Magic Shift | Native Repeat Key plus local tap dispatch | Tap repeats a recent unmodified alpha (1200 ms), otherwise one-shot Shift; shifted tap toggles Caps Word; hold is Shift. The strict history rules were checked against the source's pinned [`zmk-adaptive-key`](https://github.com/urob/zmk-adaptive-key/tree/18cc2795a64ccb394d1a7b7d591a5bfe3f25924b). |
| Caps Word | Native [Caps Word](https://docs.qmk.fm/features/caps_word), no idle timeout | See the source inconsistency note below. |
| Num Word / sticky Num | Local continuation policy around native layer and one-shot-layer APIs | Digits and selected numeric punctuation continue the word; the breaking key still performs its underlying action. Silent layer, mouse, and leader events are preserved to match the pinned [`zmk-auto-layer`](https://github.com/urob/zmk-auto-layer/blob/dfa7c695ffde41ef8064245453595d50e198ae1d/src/behaviors/behavior_auto_layer.c) keycode-event model. |
| Smart Mouse / Smart Button | Local state flags plus layer transitions and physical-position ignore sets | Mirrors the source tri-state smart-layer use cases. The Smart Button stale-state defect is intentionally fixed. |
| App/window swappers | Getreuer's Cyclotab community module, configured for `Alt+Tab` and `Alt+grave` with no timeout | Repeated forward/reverse steps and release on APP-layer exit are delegated to a maintained QMK module. Unlike the source tri-state, Cyclotab consumes an unrelated interrupting press and has no arbitrary ignored-position list. |
| OS-aware actions | Runtime Windows/macOS/Linux action table, with the pressed action cached until release | Avoids releasing a different chord if OS is changed while an action is held. |
| Greek and compose leader actions | Custom prefix parser plus QMK Unicode Common | All 24 Greek sequences, shifted uppercase, Lenny compose, layer selection, reset, and boot are represented. |
| Pointer and wheel keys | Native [Mouse Keys](https://docs.qmk.fm/features/mouse_keys) with tuned intervals and acceleration values | Approximation only; see mouse compromise below. |
| Wired split | Stock Sofle serial split and Pro Micro VBUS master detection | Same image on both halves, left-half USB only. |
| BLE, battery/sleep, ZMK output selection, widgets | Omitted | The selected Sofle/Pro Micro target is wired-only. OLED/RGB/encoder features are also intentionally out of scope. |

## Decisions after reviewing references

The local `references/` checkout trees were used for patterns and trade-off
comparison and are not runtime dependencies. Runtime modules are explicitly
identified below and pinned under `modules/`.

| Reference | Revision reviewed | What it informed |
| --- | --- | --- |
| [`official`](references/official/) / [qmk/qmk_userspace](https://github.com/qmk/qmk_userspace/tree/caa715695de182564fed9a62ab1bd3b0aff7c4c0) | `caa7156` | External-userspace directory layout, `qmk.json` target, and reusable build workflow. |
| [`getreuer`](references/getreuer/) / [getreuer/qmk-keymap](https://github.com/getreuer/qmk-keymap/tree/783791dd9eebe9a3db8b16bd32b7dc76bf0e6122) | `783791d` | Current native Chordal Hold, Flow Tap, callback tuning, and Key Override usage in a large keymap. |
| [`getreuer-modules`](modules/getreuer/) / [getreuer/qmk-modules](https://github.com/getreuer/qmk-modules/tree/788e0f3f2d98c5cd57a88616e2cc883bc9fb3310) | `788e0f3` | Cyclotab is a pinned runtime dependency for app/window switching; Achordion, Tap Flow, and Custom Shift Keys remain comparison points. |
| [`drashna`](references/drashna/) / [drashna/qmk_userspace](https://github.com/drashna/qmk_userspace/tree/936e5fc86a84fa2908bac407b87a89ae0a9b0d61) | `936e5fc` | Organization of mature shared userspace code, callback composition, and feature gating. |
| [`filterpaper`](references/filterpaper/) / [filterpaper/qmk_userspace](https://github.com/filterpaper/qmk_userspace/tree/2f08f3e) | `2f08f3e` | A compact custom contextual-HRM implementation and preprocessor-generated combo actions; useful comparison points, but unnecessary beside pinned core Chordal Hold and explicit combo tables. |
| [`holykeebs`](references/holykeebs/) / [holykeebs/qmk-userspace](https://github.com/holykeebs/qmk-userspace/tree/58c61e5) | `58c61e5` | Cross-keyboard external-userspace organization and QMK/Vial compatibility gating; inspected, but this single pinned-QMK target does not need the compatibility layer. |
| [`karbou12-develop`](references/karbou12-develop/) / [karbou12/qmk_userspace](https://github.com/karbou12/qmk_userspace/tree/e4ecc40) | `e4ecc40` | Shared tap-hold, combo, and Key Override modules plus a current userspace 1.1 manifest; Vial/dynamic-keymap machinery was intentionally not imported. |
| [`urob-zmk-config`](references/urob-zmk-config/) / [urob/zmk-config](https://github.com/urob/zmk-config/tree/908e50817bd23b3209cd04a2c7cc3685b043fb61) | `908e508` | The ZMK HRM, combo, leader, and physical-position idioms against which the source semantics were checked. |

Achordion/Tap Flow and Custom Shift Keys are credible alternatives, but the
pinned QMK core already supplies the required Chordal Hold, Flow Tap, and Key
Override hooks. The native versions were selected because they operate inside
QMK's tap-hold and report-processing paths, require less local integration
code, and cover the required behavior. Custom code was retained only where the
core representation is insufficient (mixed-side combo modifiers, arbitrary
hold actions, tri-state-style state, and leader semantics).

## Explicit compromises and corrections

### HRM prior-idle translation is deliberately scoped

ZMK's `require-prior-idle-ms = <150>` is a broad behavior-level prior-idle
shortcut. QMK's native equivalent, Flow Tap, is designed to be filtered through
`get_flow_tap_term()`. Three options were considered:

1. return 150 ms after every preceding key;
2. maintain a separate local last-event tracker to emulate ZMK more literally;
3. use the established native-QMK typing-key scope from the Getreuer reference.

Option 3 was selected, extended to Space: Flow Tap applies after letters and
common typing punctuation, but not after navigation, function, layer, or
modifier actions. This keeps normal typing immediate while avoiding a forced
tap in unusual shortcut sequences. The 150 ms timing, Chordal Hold handedness,
280 ms tapping term, and 175 ms quick-tap term remain unchanged.

### Native quick-tap windows use QMK's event model

ZMK measures `quick-tap-ms` from the first press to the repeated press. Native
QMK Mod-Tap/Layer-Tap measures `QUICK_TAP_TERM` from the first release to the
repeated press, as illustrated in QMK's [Quick Tap Term
documentation](https://docs.qmk.fm/tap_hold#quick-tap-term). The locally
implemented arbitrary and mixed-modifier hold-taps use ZMK's press-to-press
window, but replacing every native QMK tap-hold solely to change this boundary
would forfeit core Chordal Hold, Flow Tap, and action buffering. The native
event model is therefore retained and documented. It only differs after a
nontrivially long first tap, where QMK grants a longer repeat window.

### QMK shares the one-shot timeout

The source explicitly sets sticky Shift to 900 ms, while ZMK's sticky layer
default is 1000 ms. QMK exposes one `ONESHOT_TIMEOUT` for both one-shot
modifiers and layers. The port uses 900 ms, preserving the explicitly tuned and
more frequently used Magic Shift behavior; sticky `NUM` consequently expires
100 ms earlier than its ZMK default. A private timer/state implementation would
remove that small difference but add code alongside otherwise suitable core
one-shot APIs, so it was not selected. See ZMK's [sticky-layer
default](https://zmk.dev/docs/keymaps/behaviors/sticky-layer) and QMK's
[one-shot API](https://docs.qmk.fm/one_shot_keys).

### Leader: custom only after comparing alternatives

QMK's native Leader feature was not rejected merely because one sequence was
long. The alternatives were weighed:

1. **Use native Leader unchanged.** Small and core-maintained, but QMK 0.33.11
   has a five-key buffer, always uses a finite inter-key timeout, swallows keys
   that make a sequence invalid until the core later times out or fills its
   buffer, and has no equivalent of the source's
   `ignore-keys = <LSHFT RSHFT>`. See the official
   [Leader documentation](https://docs.qmk.fm/features/leader_key), the pinned
   [five-entry buffer](https://github.com/qmk/qmk_firmware/blob/f0e090f67a90f9b653faeddbf5a1c4f75e24e91a/quantum/leader.c#L17-L18),
   and [event processing](https://github.com/qmk/qmk_firmware/blob/f0e090f67a90f9b653faeddbf5a1c4f75e24e91a/quantum/process_keycode/process_leader.c#L23-L36).
2. **Shorten `QWERTY` to five letters.** This would preserve core simplicity
   but intentionally change established source input.
3. **Use a hybrid parser only for `QWERTY`.** This reduces custom code but
   creates two timing, Shift, invalid-key, and release models.
4. **Use one bounded custom prefix parser.** More code to maintain, but one
   coherent behavior for every sequence.

Option 4 was selected. The bounded 29-sequence parser has no timeout, accepts
the source's six-letter `Q W E R T Y`, passes the first invalid key through
while cancelling the sequence, lets Shift pass without becoming sequence
input, and retains captured-release semantics.

The source is internally inconsistent here: its pinned ZMK leader module also
defaults to a five-key sequence buffer, and this repository has no override.
The declared six-character `QWERTY` initializer therefore is not trustworthy
as compiled ZMK behavior. This port treats the six letters as the declared
intent after weighing the alternatives above; it does not silently shorten
the sequence just to fit QMK core.

### Mouse acceleration is approximate

ZMK's move value `600`, scroll value `20`, exponent, time-to-max, and per-layer
scalers are not numerically interchangeable with QMK Mouse Keys' host-step and
interval model. The port uses QMK's native accelerated mode, including a 16 ms
movement interval, and does not add a custom acceleration engine. Exact cursor
distance, acceleration curve, and NAV/FN scaling are therefore not claimed to
be 1:1.

### Smart Button stale-state bug is fixed

The source tri-state Smart Button path can turn the layer off through an
ignored exit while leaving toggle intent stale, making a later activation
re-toggle unexpectedly. The port always clears the local flag together with
the layer. This is an intentional correction, not a fidelity regression.

### Overlapping NUM ownership is made explicit

Both firmware stacks ultimately represent an active layer with one bit, so a
momentary `NUM` release can erase a concurrently active Num Word bit. The port
tracks the logical Num Word and one-shot owners and reasserts `NUM` while
either remains active. Explicit `TO(DSEND)`/`TO(QWERTY)` and the equivalent
leader actions first cancel those owners, preserving the source's base-switch
intent. This avoids a stale-state edge case rather than reproducing it.

### Caps Word follows the written intent

The source comment says modifiers deactivate Caps Word, while the compiled
input also carries a stale `/delete-property/ ignore-modifiers;` directive.
Rather than reproducing an ambiguous mechanism, the port follows the explicit
comment: QMK's default Caps Word policy treats modifiers as word-breaking. No
custom continuation callback is installed, and the idle timeout is disabled.

### Transparent smart-layer invariant

The ZMK file defines an `XXX = &none` convenience token but does not actually
use it in any layer; every visually blank layer entry is `___ = &trans`.
`keymap.c` therefore uses `KC_TRNS` for all of those entries. This is
especially important on smart/automatic layers: a non-ignored exit position
must fall through so the same press can close the smart layer and still
perform the lower-layer action.

This does not affect the 18 unused physical Sofle switches, which are hard
`KC_NO` inside `LAYOUT_ZEN`. If a future layer needs another true dead
position, use `KC_NO` explicitly rather than changing the transparent token.

## Validation

The final image was compiled against QMK `0.33.11` at
`f0e090f67a90f9b653faeddbf5a1c4f75e24e91a`. Compiler, linker, HEX generation,
copy, and size-check stages all completed.

- Flash: **27,926 / 28,672 bytes** (97%, 746 bytes free). QMK reports its
  expected "approaching the maximum" warning; the image remains within the
  ATmega32U4 application limit.
- Static SRAM: **1,081 / 2,560 bytes** (`.data` 408 + `.bss` 673, about 42%).
  Runtime stack usage is not included in this static figure.
- Artifact: `sofle_rev1_andrewxhc.hex`
- SHA-256:
  `4FBCEA9644FA38863B1C326720BEE5713B4637C4FAC237FCFD4650E33390D76D`
- `git diff --check` passes. A final source/reference parity audit found no
  remaining concrete runtime defect beyond the compromises documented above.
- The post-scaffold parity rerun found 10 source/target layers, 34
  source/target combos, and 31 source leader declarations mapping to 29 wired
  target sequences plus the two documented USB/BLE omissions.
- The rebuilt HEX retained the exact pre-scaffold SHA-256 above, confirming
  that the structural and metadata reconciliation did not change firmware
  behavior. The pinned QMK and ZMK checkouts were clean after temporary
  scaffold cleanup.

This is compile-time and static validation; the tap-hold timing, pointing
speed, bootloader entry, and split transport still need on-hardware acceptance
testing.

## Building

`qmk.json` declares the single build target `sofle/rev1:andrewxhc`. On this
Windows workspace, QMK MSYS is installed at `C:\QMK_MSYS`. A reproducible
PowerShell build against the pinned local QMK checkout is:

The pinned Make logic splits userspace search paths on whitespace, so QMK CLI
cannot discover this particular checkout through its long path (`Keyboard
Firmware`). Windows supplies the no-space 8.3 alias `C:\KEYBOA~1`; the
validated local command is:

```powershell
$env:MSYSTEM = 'MINGW64'
$env:MSYS2_PATH_TYPE = 'inherit'
& 'C:\QMK_MSYS\usr\bin\bash.exe' -lc `
  "make -C /c/KEYBOA~1/qmk_firmware 'sofle/rev1:andrewxhc' QMK_USERSPACE=/c/KEYBOA~1/qmk_userspace"
```

In a checkout whose path has no spaces, the normal commands remain `qmk
compile -kb sofle/rev1 -km andrewxhc` and `qmk userspace-compile`, as described
under [External Userspace
compilation](https://docs.qmk.fm/newbs_external_userspace#compiling-external-userspace-build-targets).
Pushing the repository also invokes the reusable QMK userspace workflow at the
exact QMK SHA recorded above.

Flash the generated `sofle_rev1_andrewxhc.hex` to **each controller
individually**. Disconnect USB before connecting or disconnecting the TRRS
cable, then use left-side USB for normal operation.
