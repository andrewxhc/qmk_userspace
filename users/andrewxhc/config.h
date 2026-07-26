#pragma once

// Tap-hold tuning. HRMs override TAPPING_TERM through get_tapping_term().
#define TAPPING_TERM 200
#define TAPPING_TERM_PER_KEY
#define QUICK_TAP_TERM 175
#define QUICK_TAP_TERM_PER_KEY
#define PERMISSIVE_HOLD_PER_KEY
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY
#define CHORDAL_HOLD
#define FLOW_TAP_TERM 150
#define ONESHOT_TIMEOUT 900

// Combo behavior mirrors the ZMK source's strict per-combo windows.
#define COMBO_TERM 30
#define COMBO_TERM_PER_COMBO
#define COMBO_SHOULD_TRIGGER
#define COMBO_STRICT_TIMER
#define COMBO_HOLD_TERM 280
#define COMBO_ONLY_FROM_LAYER 0
#define EXTRA_SHORT_COMBOS

#define CAPS_WORD_IDLE_TIMEOUT 0
#define NO_ALT_REPEAT_KEY
#define LAYER_STATE_16BIT

#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE

// Unicode mode follows the runtime OS selector; Windows/WinCompose is default.
#define UNICODE_SELECTED_MODES UNICODE_MODE_WINCOMPOSE, UNICODE_MODE_MACOS, UNICODE_MODE_LINUX
#define UNICODE_CYCLE_PERSIST false

// Close QMK's mouse-key response to the ZMK source without adding a custom
// acceleration engine. The unit systems differ, so this cannot be exact.
#define MOUSEKEY_DELAY 0
#define MOUSEKEY_INTERVAL 16
#define MOUSEKEY_MOVE_DELTA 8
#define MOUSEKEY_MAX_SPEED 7
#define MOUSEKEY_TIME_TO_MAX 31
#define MOUSEKEY_WHEEL_DELAY 0
#define MOUSEKEY_WHEEL_INTERVAL 40
#define MOUSEKEY_WHEEL_DELTA 1
#define MOUSEKEY_WHEEL_MAX_SPEED 4
#define MOUSEKEY_WHEEL_TIME_TO_MAX 0

// The default Sofle metadata requests mirrored matrix transport for cosmetic
// features. This keymap has no display/RGB state to mirror.
#undef SPLIT_TRANSPORT_MIRROR
