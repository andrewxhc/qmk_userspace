# To-do

QMK keymap work for the Sofle (`sofle/rev1:andrewxhc`). See [PORTING.md](PORTING.md) for how the port maps the Zen's ZMK config.

## Hardware testing (November 27 build, ZMK `702883c` sync)

- [ ] Flash both halves and confirm the left half is master over USB
- [ ] FIG → MNT: hold FIG, hold MNT, then bootloader and reboot
- [ ] FIG language keys on Windows (needs Ctrl+Shift+1 / Ctrl+Shift+2 set up) and macOS
- [ ] NAV browser keys: tabs, zoom in/out/reset, reload, reopen tab, close tab, mouse back/forward
- [ ] APP hold-to-quit: tap does nothing, a 500 ms hold quits only the front app
- [ ] Window swapper back key (`S(U_WINDOW_SWAPPER)`) moves backwards on macOS
- [ ] macOS region screenshot lands on the clipboard
- [ ] Tap-hold timing, combos, mouse speed and split transport (never tested on hardware since the port)
