// Copyright 2026 Andrew XHC
// SPDX-License-Identifier: GPL-2.0-only

#include "andrewxhc.h"

#include "repeat_key.h"
#include "unicode.h"

typedef enum {
    OS_WINDOWS,
    OS_MACOS,
    OS_LINUX,
    OS_COUNT,
} runtime_os_t;

static runtime_os_t runtime_os = OS_WINDOWS;

enum {
    OS_ACTION_COUNT = U_SCREENSHOT_REGION - U_CUT + 1,
};

// Each OS action is selected when its press is processed, then cached so its
// release unregisters that same action even if the runtime OS changes.
static const uint16_t PROGMEM os_actions[OS_ACTION_COUNT][OS_COUNT] = {
    [U_CUT - U_CUT]               = {LCTL(KC_X), LGUI(KC_X), LCTL(KC_X)},
    [U_COPY - U_CUT]              = {LCTL(KC_C), LGUI(KC_C), LCTL(KC_C)},
    [U_PASTE - U_CUT]             = {LCTL(KC_V), LGUI(KC_V), LCTL(KC_V)},
    [U_UNDO - U_CUT]              = {LCTL(KC_Z), LGUI(KC_Z), LCTL(KC_Z)},
    [U_REDO - U_CUT]              = {LCTL(KC_Y), LGUI(LSFT(KC_Z)), LCTL(LSFT(KC_Z))},
    [U_SELECT_ALL - U_CUT]        = {LCTL(KC_A), LGUI(KC_A), LCTL(KC_A)},
    [U_FIND - U_CUT]              = {LCTL(KC_F), LGUI(KC_F), LCTL(KC_F)},
    [U_NEW_TAB - U_CUT]           = {LCTL(KC_T), LGUI(KC_T), LCTL(KC_T)},
    [U_NEW_WINDOW - U_CUT]        = {LCTL(KC_N), LGUI(KC_N), LCTL(KC_N)},
    [U_PRIVATE_WINDOW - U_CUT]    = {LCTL(LSFT(KC_N)), LGUI(LSFT(KC_N)), LCTL(LSFT(KC_N))},
    [U_ADDRESS_BAR - U_CUT]       = {LALT(KC_D), LGUI(KC_L), LCTL(KC_L)},
    [U_DESKTOP_PREV - U_CUT]      = {LGUI(LCTL(KC_LEFT)), LCTL(KC_LEFT), LGUI(LCTL(KC_LEFT))},
    [U_DESKTOP_MANAGER - U_CUT]   = {LGUI(KC_TAB), LCTL(KC_UP), LALT(KC_GRV)},
    [U_DESKTOP_NEXT - U_CUT]      = {LGUI(LCTL(KC_RGHT)), LCTL(KC_RGHT), LGUI(LCTL(KC_RGHT))},
    [U_TILE_LEFT - U_CUT]         = {LGUI(KC_LEFT), LCTL(LALT(KC_LEFT)), LGUI(KC_LEFT)},
    [U_TILE_FILL - U_CUT]         = {LGUI(KC_UP), LCTL(LALT(KC_ENT)), LGUI(KC_UP)},
    [U_TILE_RIGHT - U_CUT]        = {LGUI(KC_RGHT), LCTL(LALT(KC_RGHT)), LGUI(KC_RGHT)},
    [U_TILE_RESTORE - U_CUT]      = {LGUI(KC_DOWN), LCTL(LALT(KC_BSPC)), LGUI(KC_DOWN)},
    [U_MONITOR_NEXT - U_CUT]      = {LGUI(LSFT(KC_RGHT)), LCTL(LALT(LGUI(KC_RGHT))), LGUI(LSFT(KC_RGHT))},
    [U_MINIMIZE - U_CUT]          = {LGUI(KC_DOWN), LGUI(KC_M), LGUI(KC_DOWN)},
    [U_SCREENSHOT_FULL - U_CUT]   = {LGUI(KC_PSCR), LGUI(LSFT(KC_3)), KC_PSCR},
    [U_SCREENSHOT_WINDOW - U_CUT] = {LALT(KC_PSCR), KC_NO, LALT(KC_PSCR)},
    [U_SCREENSHOT_REGION - U_CUT] = {LGUI(LSFT(KC_S)), LGUI(LSFT(KC_4)), LSFT(KC_PSCR)},
};

static uint16_t active_os_actions[OS_ACTION_COUNT];

static uint8_t effective_mods(void) {
    return get_mods() | get_weak_mods() | get_oneshot_mods();
}

static void tap_code16_pair_without_mods(uint16_t first_keycode,
                                         uint16_t second_keycode,
                                         uint8_t suppressed_mods) {
    const uint8_t saved_mods    = get_mods();
    const uint8_t saved_weak    = get_weak_mods();
    const uint8_t saved_oneshot = get_oneshot_mods();

    set_mods(saved_mods & ~suppressed_mods);
    set_weak_mods(saved_weak & ~suppressed_mods);
    set_oneshot_mods(saved_oneshot & ~suppressed_mods);
    send_keyboard_report();
    tap_code16(first_keycode);
    if (second_keycode != KC_NO) {
        tap_code16(second_keycode);
    }
    set_mods(saved_mods);
    set_weak_mods(saved_weak);
    // The emitted key consumes all armed one-shot mods, including any that
    // were temporarily suppressed to reproduce a mod-morph.
    clear_oneshot_mods();
    send_keyboard_report();
}

static void tap_code16_without_mods(uint16_t keycode, uint8_t suppressed_mods) {
    tap_code16_pair_without_mods(keycode, KC_NO, suppressed_mods);
}

static void set_runtime_os(runtime_os_t os) {
    runtime_os = os;

    // set_unicode_input_mode() persists to EEPROM. The source selection is
    // volatile, so update the common Unicode state directly instead.
    switch (os) {
        case OS_MACOS:
            unicode_config.input_mode = UNICODE_MODE_MACOS;
            break;
        case OS_LINUX:
            unicode_config.input_mode = UNICODE_MODE_LINUX;
            break;
        default:
            unicode_config.input_mode = UNICODE_MODE_WINCOMPOSE;
            break;
    }
    unicode_input_mode_set_kb(unicode_config.input_mode);
}

static uint16_t launcher_action(runtime_os_t os) {
    switch (os) {
        case OS_MACOS:
            return LGUI(KC_SPC);
        case OS_LINUX:
            return KC_LGUI;
        default:
            return LCTL(LALT(KC_P));
    }
}

static uint16_t word_backspace_action(runtime_os_t os) {
    return os == OS_MACOS ? LALT(KC_BSPC) : LCTL(KC_BSPC);
}

static void tap_macos_window_screenshot(void) {
    tap_code16_delay(LGUI(LSFT(KC_4)), 20);
    wait_ms(50);
    tap_code_delay(KC_SPC, 20);
}

static bool process_os_action(uint16_t keycode, keyrecord_t *record) {
    if (keycode < U_CUT || keycode > U_SCREENSHOT_REGION) {
        return true;
    }

    const uint8_t index = keycode - U_CUT;
    if (record->event.pressed) {
        caps_word_off();
        if (keycode == U_SCREENSHOT_WINDOW && runtime_os == OS_MACOS) {
            tap_macos_window_screenshot();
            active_os_actions[index] = KC_NO;
        } else {
            active_os_actions[index] = pgm_read_word(&os_actions[index][runtime_os]);
            register_code16(active_os_actions[index]);
        }
        andrewxhc_combo_note_keypress();
    } else if (active_os_actions[index] != KC_NO) {
        unregister_code16(active_os_actions[index]);
        active_os_actions[index] = KC_NO;
    }
    return false;
}

typedef enum {
    CHT_ESC_TAB,
    CHT_OS_START,
    CHT_OS_END,
    CHT_OS_LEFT,
    CHT_OS_RIGHT,
    CHT_OS_UP,
    CHT_OS_DOWN,
    CHT_COUNT,
} custom_hold_t;

typedef enum {
    CHT_PENDING,
    CHT_TAPPED,
    CHT_TAP_HELD,
    CHT_HELD,
} custom_hold_resolution_t;

typedef struct {
    bool                     active;
    bool                     force_tap;
    custom_hold_resolution_t resolution;
    uint32_t                 timer;
    runtime_os_t             os;
    uint16_t                 held_action;
} custom_hold_state_t;

static custom_hold_state_t custom_holds[CHT_COUNT];
static bool custom_hold_had_tap[CHT_COUNT];
static uint32_t custom_hold_last_tap[CHT_COUNT];
static keypos_t last_physical_event_key;
static bool last_physical_event_valid;

static int8_t custom_hold_index(uint16_t keycode) {
    switch (keycode) {
        case U_ESC_TAB:
            return CHT_ESC_TAB;
        case U_OS_START:
            return CHT_OS_START;
        case U_OS_END:
            return CHT_OS_END;
        case U_OS_LEFT:
            return CHT_OS_LEFT;
        case U_OS_RIGHT:
            return CHT_OS_RIGHT;
        case U_OS_UP:
            return CHT_OS_UP;
        case U_OS_DOWN:
            return CHT_OS_DOWN;
        default:
            return -1;
    }
}

static uint16_t custom_hold_term(custom_hold_t index) {
    if (index == CHT_OS_START || index == CHT_OS_END) {
        return 800;
    }
    return 220;
}

static uint16_t custom_hold_action(custom_hold_t index, bool hold, runtime_os_t os) {
    switch (index) {
        case CHT_ESC_TAB:
            return hold ? KC_ESC : KC_TAB;
        case CHT_OS_START:
            if (hold) {
                return os == OS_MACOS ? LGUI(KC_UP) : LCTL(KC_HOME);
            }
            return os == OS_MACOS ? LGUI(KC_LEFT) : KC_HOME;
        case CHT_OS_END:
            if (hold) {
                return os == OS_MACOS ? LGUI(KC_DOWN) : LCTL(KC_END);
            }
            return os == OS_MACOS ? LGUI(KC_RGHT) : KC_END;
        case CHT_OS_LEFT:
            return hold ? (os == OS_MACOS ? LALT(KC_LEFT) : LCTL(KC_LEFT)) : KC_LEFT;
        case CHT_OS_RIGHT:
            return hold ? (os == OS_MACOS ? LALT(KC_RGHT) : LCTL(KC_RGHT)) : KC_RGHT;
        case CHT_OS_UP:
            return hold ? (os == OS_MACOS ? LALT(KC_UP) : LCTL(KC_UP)) : KC_UP;
        case CHT_OS_DOWN:
            return hold ? (os == OS_MACOS ? LALT(KC_DOWN) : LCTL(KC_DOWN)) : KC_DOWN;
        default:
            return KC_NO;
    }
}

static void resolve_custom_hold(custom_hold_t index, bool hold) {
    custom_hold_state_t *state = &custom_holds[index];
    if (!state->active || state->resolution != CHT_PENDING) {
        return;
    }

    caps_word_off();
    const uint16_t action = custom_hold_action(index, hold, state->os);
    if (hold) {
        state->held_action = action;
        state->resolution  = CHT_HELD;
        register_code16(action);
        andrewxhc_combo_note_keypress();
    } else {
        state->resolution = CHT_TAPPED;
        tap_code16(action);
        custom_hold_had_tap[index]  = true;
        custom_hold_last_tap[index] = state->timer;
        andrewxhc_combo_note_keypress();
    }
}

static void press_custom_tap(custom_hold_t index) {
    custom_hold_state_t *state = &custom_holds[index];
    if (!state->active || state->resolution != CHT_PENDING) {
        return;
    }

    caps_word_off();
    state->held_action = custom_hold_action(index, false, state->os);
    state->resolution  = CHT_TAP_HELD;
    register_code16(state->held_action);
    andrewxhc_combo_note_keypress();
}

static bool process_custom_hold(uint16_t keycode, keyrecord_t *record) {
    const int8_t raw_index = custom_hold_index(keycode);
    if (raw_index < 0) {
        return true;
    }

    const custom_hold_t index = (custom_hold_t)raw_index;
    custom_hold_state_t *state = &custom_holds[index];
    if (record->event.pressed) {
        const bool force_tap =
            custom_hold_had_tap[index] && timer_elapsed32(custom_hold_last_tap[index]) <= 220;
        *state = (custom_hold_state_t){
            .active     = true,
            .force_tap  = force_tap,
            .resolution = CHT_PENDING,
            .timer      = timer_read32(),
            .os         = runtime_os,
            .held_action = KC_NO,
        };
        if (force_tap) {
            press_custom_tap(index);
        }
    } else {
        if (state->resolution == CHT_PENDING) {
            resolve_custom_hold(index, false);
        } else if (state->resolution == CHT_HELD ||
                   state->resolution == CHT_TAP_HELD) {
            unregister_code16(state->held_action);
            if (state->resolution == CHT_TAP_HELD) {
                custom_hold_had_tap[index]  = true;
                custom_hold_last_tap[index] = state->timer;
            }
        }
        state->active = false;
    }
    return false;
}

static bool alpha_repeat_eligible;
static uint32_t alpha_repeat_timer;
static bool magic_repeat_active;

static uint16_t tap_keycode(uint16_t keycode) {
    if (IS_QK_MOD_TAP(keycode)) {
        return QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
    }
    if (IS_QK_LAYER_TAP(keycode)) {
        return QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
    }
    if (IS_QK_MODS(keycode)) {
        return QK_MODS_GET_BASIC_KEYCODE(keycode);
    }
    return keycode;
}

static bool is_unmodified_alpha(uint16_t keycode, uint8_t mods) {
    const uint16_t basic = tap_keycode(keycode);
    return !IS_QK_MODS(keycode) && basic >= KC_A && basic <= KC_Z &&
           mods == 0;
}

void andrewxhc_repeat_cancel(void) {
    alpha_repeat_eligible = false;
}

void andrewxhc_repeat_note_passthrough(uint16_t keycode,
                                       keyrecord_t *record) {
    const uint8_t mods = effective_mods();
    alpha_repeat_eligible = is_unmodified_alpha(keycode, mods);
    if (alpha_repeat_eligible) {
        alpha_repeat_timer = timer_read32();
        // Repeat Key ran before the custom leader parser skipped this event.
        set_last_record(keycode, record);
        set_last_mods(mods);
    }
}

bool remember_last_key_user(uint16_t keycode, keyrecord_t *record, uint8_t *remembered_mods) {
    if (andrewxhc_leader_is_active()) {
        return false;
    }

    switch (keycode) {
        // These controls emit no host key and do not replace ZMK's adaptive
        // key history.
        case U_OS_WIN:
        case U_OS_MAC:
        case U_OS_LIN:
        case U_SMART_MOUSE:
        case U_SMART_BUTTON:
            return false;

        case U_ESC_TAB ... U_OS_DOWN:
        case U_CUT ... U_WINDOW_SWAPPER:
        case U_CANCEL:
        case U_APP_LAUNCHER:
        case U_SYM_SPACE:
        case U_NAV_BSPC:
            alpha_repeat_eligible = false;
            return false;
        case U_SMART_NUM:
        case U_MAGIC_SHIFT:
        case U_COMBO_LEADER:
        case U_COMBO_SHIFT_LEADER:
            return false;
    }

    alpha_repeat_eligible = is_unmodified_alpha(keycode, *remembered_mods);
    if (alpha_repeat_eligible) {
        alpha_repeat_timer = timer_read32();
    }
    return true;
}

static bool process_magic_shift(keyrecord_t *record) {
    if (!record->tap.count) {
        return true;
    }

    if (record->event.pressed) {
        if (effective_mods() & MOD_MASK_SHIFT) {
            del_oneshot_mods(MOD_MASK_SHIFT);
            const uint8_t saved_mods = get_mods();
            caps_word_toggle();
            // QMK clears held mods when Caps Word turns on; ZMK leaves the
            // physical modifier state intact.
            set_mods(saved_mods);
            send_keyboard_report();
            magic_repeat_active = false;
        } else if (alpha_repeat_eligible && effective_mods() == 0 &&
                   timer_elapsed32(alpha_repeat_timer) <= 1200) {
            repeat_key_invoke(&record->event);
            alpha_repeat_timer  = timer_read32();
            magic_repeat_active = true;
            andrewxhc_combo_note_keypress();
        } else {
            add_oneshot_mods(MOD_BIT(KC_LSFT));
            alpha_repeat_eligible = false;
            magic_repeat_active   = false;
        }
    } else if (magic_repeat_active) {
        repeat_key_invoke(&record->event);
        magic_repeat_active = false;
    }
    return false;
}

static bool num_word_active;
static bool smart_num_second_tap_pending;
static bool sticky_num_pending;
static uint32_t smart_num_tap_timer;
static bool num_word_break_pending;
static keypos_t num_word_break_key;

static void num_word_off(void) {
    if (num_word_active) {
        num_word_active = false;
        layer_off(NUM);
    }
}

void andrewxhc_num_word_break(void) {
    num_word_off();
}

static void num_word_on(void) {
    num_word_active = true;
    layer_on(NUM);
}

static bool num_word_continues(uint16_t keycode, keyrecord_t *record) {
    if (IS_QK_MOD_TAP(keycode) && record->tap.count == 0) {
        // The NUM zero key is Shift when held. The source Num Word does not
        // ignore modifier outputs, so that resolution ends the mode.
        return false;
    }

    const uint16_t basic = tap_keycode(keycode);
    if ((basic >= KC_0 && basic <= KC_9) || basic == KC_BSPC || basic == KC_DEL ||
        basic == KC_DOT || basic == KC_COMM || basic == KC_EQL ||
        basic == KC_MINS || basic == KC_SLSH) {
        return true;
    }
    return keycode == KC_PLUS || keycode == KC_ASTR || basic == KC_PPLS || basic == KC_PAST;
}

static bool num_word_preserves(uint16_t keycode, keyrecord_t *record) {
    // ZMK auto-layer observes emitted keycodes, not layer or mouse behavior.
    if (IS_MOUSE_KEYCODE(keycode) || IS_QK_TO(keycode) ||
        IS_QK_MOMENTARY(keycode) ||
        (IS_QK_LAYER_TAP(keycode) && record->tap.count == 0)) {
        return true;
    }

    switch (keycode) {
        case U_OS_WIN:
        case U_OS_MAC:
        case U_OS_LIN:
        case U_SMART_MOUSE:
        case U_SMART_BUTTON:
            return true;

        case U_NAV_BSPC:
            // Both tap morphs, Backspace and Delete, are continuation keys;
            // its hold is a silent layer behavior.
            return true;

        case U_COMBO_LEADER:
        case U_COMBO_SHIFT_LEADER:
            // Tap starts a silent leader behavior; hold emits modifiers.
            return record->tap.count != 0;

        case U_MAGIC_SHIFT:
            if (record->tap.count) {
                // Shift selects silent Caps Word. Repeat emits an alpha, and
                // sticky Shift immediately emits its modifier keycode in ZMK.
                return (effective_mods() & MOD_MASK_SHIFT) != 0;
            }
            return false;
    }

    return num_word_continues(keycode, record);
}

static bool process_smart_num(keyrecord_t *record) {
    if (!record->tap.count) {
        return true;
    }

    if (record->event.pressed) {
        if (smart_num_second_tap_pending && timer_elapsed32(smart_num_tap_timer) <= 200) {
            smart_num_second_tap_pending = false;
            num_word_off();
            // Defer until this consumed custom event leaves process_record();
            // otherwise QMK immediately fulfills the one-shot on the setter.
            sticky_num_pending = true;
        } else {
            num_word_on();
            smart_num_second_tap_pending = true;
            smart_num_tap_timer          = timer_read32();
        }
    }
    return false;
}

static bool process_nav_backspace(keyrecord_t *record) {
    if (!record->tap.count) {
        return true;
    }
    if (record->event.pressed) {
        const bool control = (effective_mods() & MOD_MASK_CTRL) != 0;
        tap_code16_without_mods(control ? word_backspace_action(runtime_os) : KC_BSPC,
                                control ? MOD_MASK_CTRL : 0);
        andrewxhc_combo_note_keypress();
    }
    return false;
}

static bool process_sym_space(keyrecord_t *record) {
    if (!record->tap.count) {
        return true;
    }
    if (record->event.pressed) {
        caps_word_off();
        if (effective_mods() & MOD_MASK_SHIFT) {
            tap_code16_pair_without_mods(KC_DOT, KC_SPC, MOD_MASK_SHIFT);
            add_oneshot_mods(MOD_BIT(KC_LSFT));
        } else {
            tap_code(KC_SPC);
        }
        andrewxhc_combo_note_keypress();
    }
    return false;
}

static bool process_app_launcher(keyrecord_t *record) {
    if (!record->tap.count) {
        return true;
    }
    if (record->event.pressed) {
        caps_word_off();
        tap_code16(launcher_action(runtime_os));
        andrewxhc_combo_note_keypress();
    }
    return false;
}

static bool smart_mouse_active;
static bool smart_button_active;
static bool app_swapper_active;
static bool window_swapper_active;

static int8_t zen_position(keypos_t key) {
    if (key.row >= 1 && key.row <= 3 && key.col < 6) {
        return (key.row - 1) * 12 + key.col;
    }
    if (key.row >= 6 && key.row <= 8 && key.col < 6) {
        return (key.row - 6) * 12 + 11 - key.col;
    }
    if (key.row == 4 && key.col >= 2 && key.col <= 4) {
        return 36 + key.col - 2;
    }
    if (key.row == 9 && key.col >= 2 && key.col <= 4) {
        return 43 - key.col;
    }
    return -1;
}

static bool smart_mouse_ignored(int8_t position) {
    switch (position) {
        case 3:
        case 4:
        case 7:
        case 8:
        case 9:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 31:
        case 32:
        case 33:
        case 37:
        case 38:
        case 39:
        case 40:
            return true;
        default:
            return false;
    }
}

static bool smart_button_ignored(int8_t position) {
    switch (position) {
        case 1 ... 11:
        case 13 ... 16:
        case 19 ... 22:
        case 25 ... 34:
        case 36 ... 41:
            return true;
        default:
            return false;
    }
}

static void smart_mouse_off(void) {
    if (smart_mouse_active) {
        smart_mouse_active = false;
        layer_off(MOU);
    }
}

static void smart_button_off(void) {
    if (smart_button_active) {
        smart_button_active = false;
        layer_off(BTN);
    }
}

static void stop_app_swapper(void) {
    if (app_swapper_active) {
        app_swapper_active = false;
        unregister_weak_mods(MOD_BIT(KC_LALT));
    }
}

static void stop_window_swapper(void) {
    if (window_swapper_active) {
        window_swapper_active = false;
        unregister_weak_mods(MOD_BIT(KC_LALT));
    }
}

static bool swapper_ignored(int8_t position, bool window_swapper) {
    if (window_swapper) {
        return position == 12 || position == 13 || (position >= 19 && position <= 22);
    }
    return position == 0 || position == 1 || (position >= 19 && position <= 22);
}

static bool process_swapper(bool window) {
    bool *active = window ? &window_swapper_active : &app_swapper_active;
    if (!*active) {
        *active = true;
        register_weak_mods(MOD_BIT(KC_LALT));
    }
    tap_code(window ? KC_GRV : KC_TAB);
    caps_word_off();
    andrewxhc_combo_note_keypress();
    return false;
}

static void cancel_oneshot_layer(void) {
    if (is_oneshot_layer_active()) {
        const uint8_t layer = get_oneshot_layer();
        reset_oneshot_layer();
        layer_off(layer);
    }
}

void andrewxhc_consume_oneshot_num(void) {
    if (is_oneshot_layer_active() && get_oneshot_layer() == NUM) {
        cancel_oneshot_layer();
    }
}

static void cancel_automatic_states(void) {
    caps_word_off();
    smart_num_second_tap_pending = false;
    sticky_num_pending           = false;
    num_word_off();
    smart_mouse_off();
    smart_button_off();
    stop_app_swapper();
    stop_window_swapper();
    clear_oneshot_mods();
    clear_oneshot_locked_mods();
    cancel_oneshot_layer();
}

bool pre_process_record_user(uint16_t keycode, keyrecord_t *record) {
    const int8_t position = zen_position(record->event.key);

    if (record->event.type == KEY_EVENT) {
        last_physical_event_key   = record->event.key;
        last_physical_event_valid = true;
    }

    andrewxhc_combo_note_event(record);

    if (record->event.pressed) {
        if (keycode != U_SMART_NUM) {
            smart_num_second_tap_pending = false;
        }
        if (keycode != U_MAGIC_SHIFT &&
            (IS_MODIFIER_KEYCODE(keycode) || IS_QK_MOD_TAP(keycode))) {
            // QMK's Repeat Key deliberately ignores modifier events. ZMK's
            // adaptive-key history does not, so invalidate the alpha target.
            alpha_repeat_eligible = false;
        }
    }

    if (app_swapper_active && !swapper_ignored(position, false)) {
        stop_app_swapper();
    }
    if (window_swapper_active && !swapper_ignored(position, true)) {
        stop_window_swapper();
    }

    if (smart_mouse_active && keycode != U_SMART_MOUSE && !smart_mouse_ignored(position)) {
        smart_mouse_off();
    }
    if (smart_button_active && keycode != U_SMART_BUTTON && !smart_button_ignored(position)) {
        smart_button_off();
    }

    return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed && IS_COMBOEVENT(record->event)) {
        // Successful combo output counts as activity even when its custom
        // action is consumed before post_process_record_user().
        andrewxhc_combo_note_keypress();
    }

    if (!andrewxhc_leader_process(keycode, record)) {
        return false;
    }

    if (record->event.pressed && IS_QK_TO(keycode)) {
        // ZMK &to clears every non-default layer, including automatic and
        // sticky NUM ownership, before selecting the requested base.
        num_word_off();
        andrewxhc_consume_oneshot_num();
    }

    // Evaluate Num Word after QMK's combo resolver. Suppressed constituent
    // and accepted leader keys never arrive here; final host outputs do.
    if (record->event.pressed && num_word_active && keycode != U_SMART_NUM &&
        !num_word_preserves(keycode, record)) {
        num_word_break_pending = true;
        num_word_break_key     = record->event.key;
    }

    if (!process_custom_hold(keycode, record)) {
        return false;
    }
    if (!process_os_action(keycode, record)) {
        return false;
    }

    if (keycode == U_MAGIC_SHIFT) {
        return process_magic_shift(record);
    }
    if (keycode == U_SMART_NUM) {
        return process_smart_num(record);
    }
    if (keycode == U_NAV_BSPC) {
        return process_nav_backspace(record);
    }
    if (keycode == U_SYM_SPACE) {
        return process_sym_space(record);
    }
    if (keycode == U_APP_LAUNCHER) {
        return process_app_launcher(record);
    }
    if (keycode == U_COMBO_LEADER || keycode == U_COMBO_SHIFT_LEADER) {
        if (record->tap.count && record->event.pressed) {
            andrewxhc_leader_start(keycode == U_COMBO_SHIFT_LEADER);
        }
        return record->tap.count == 0;
    }

    switch (keycode) {
        case U_OS_WIN:
            if (record->event.pressed) {
                set_runtime_os(OS_WINDOWS);
            }
            return false;
        case U_OS_MAC:
            if (record->event.pressed) {
                set_runtime_os(OS_MACOS);
            }
            return false;
        case U_OS_LIN:
            if (record->event.pressed) {
                set_runtime_os(OS_LINUX);
            }
            return false;
        case U_SWAPPER:
            if (record->event.pressed) {
                process_swapper(false);
            }
            return false;
        case U_WINDOW_SWAPPER:
            if (record->event.pressed) {
                process_swapper(true);
            }
            return false;
        case U_SMART_MOUSE:
            if (record->event.pressed) {
                const int8_t position = zen_position(record->event.key);
                if (smart_mouse_active && position == 11) {
                    smart_mouse_off();
                } else if (!smart_mouse_active) {
                    smart_mouse_active = true;
                    layer_on(MOU);
                }
            }
            return false;
        case U_SMART_BUTTON:
            if (record->event.pressed) {
                const int8_t position = zen_position(record->event.key);
                if (smart_button_active && position == 11) {
                    // Deliberately avoid the source's stale-state re-toggle bug.
                    smart_button_off();
                } else if (!smart_button_active) {
                    smart_button_active = true;
                    layer_on(BTN);
                }
            }
            return false;
        case U_CANCEL:
            if (record->event.pressed) {
                cancel_automatic_states();
                register_code(KC_CANCEL);
                andrewxhc_combo_note_keypress();
            } else {
                unregister_code(KC_CANCEL);
            }
            return false;
    }

    return true;
}

static bool is_nonmodifier_output(uint16_t keycode, keyrecord_t *record) {
    if ((IS_QK_MOD_TAP(keycode) || IS_QK_LAYER_TAP(keycode)) && record->tap.count == 0) {
        return false;
    }
    const uint16_t basic = tap_keycode(keycode);
    return IS_QK_BASIC(basic) && basic != KC_NO && basic != KC_TRNS &&
           !(basic >= KC_LCTL && basic <= KC_RGUI);
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed && is_nonmodifier_output(keycode, record)) {
        andrewxhc_combo_note_keypress();
    }

    if (num_word_break_pending &&
        record->event.key.row == num_word_break_key.row &&
        record->event.key.col == num_word_break_key.col) {
        num_word_break_pending = false;
        if (record->event.pressed) {
            num_word_off();
        }
    }
}

void matrix_scan_user(void) {
    for (uint8_t i = 0; i < CHT_COUNT; ++i) {
        if (custom_holds[i].active && custom_holds[i].resolution == CHT_PENDING &&
            !custom_holds[i].force_tap &&
            timer_elapsed32(custom_holds[i].timer) >= custom_hold_term((custom_hold_t)i)) {
            resolve_custom_hold((custom_hold_t)i, true);
        }
    }

    if (smart_num_second_tap_pending && timer_elapsed32(smart_num_tap_timer) > 200) {
        smart_num_second_tap_pending = false;
    }

    if (sticky_num_pending) {
        sticky_num_pending = false;
        set_oneshot_layer(NUM, ONESHOT_START);
        clear_oneshot_layer_state(ONESHOT_PRESSED);
    }

    // post_process_record_user() is skipped when a custom key consumes the
    // event, so finish a pending Num Word break here as a fallback.
    if (num_word_break_pending) {
        num_word_break_pending = false;
        num_word_off();
    }

    andrewxhc_combo_task();
}

layer_state_t layer_state_set_user(layer_state_t state) {
    // Num Word and one-shot NUM can overlap the native LT(NUM, ...) hold.
    // Preserve either logical owner when that LT releases its shared bit.
    if (num_word_active ||
        (is_oneshot_layer_active() && get_oneshot_layer() == NUM)) {
        state |= (layer_state_t)1 << NUM;
    }

    const layer_state_t rising = state & ~layer_state;
    state = update_tri_layer_state(state, APP, NUM, FN);

    if (rising) {
        stop_app_swapper();
        stop_window_swapper();
    }

    const layer_state_t mouse_ignored_layers =
        ((layer_state_t)1 << MOU) | ((layer_state_t)1 << NAV) |
        ((layer_state_t)1 << APP) | ((layer_state_t)1 << NUM) |
        ((layer_state_t)1 << FN);
    if (smart_mouse_active && (rising & ~mouse_ignored_layers)) {
        smart_mouse_active = false;
        state &= ~((layer_state_t)1 << MOU);
    }

    const layer_state_t button_ignored_layers =
        ((layer_state_t)1 << BTN) | ((layer_state_t)1 << NAV) |
        ((layer_state_t)1 << APP) | ((layer_state_t)1 << NUM) |
        ((layer_state_t)1 << FN);
    if (smart_button_active && (rising & ~button_ignored_layers)) {
        smart_button_active = false;
        state &= ~((layer_state_t)1 << BTN);
    }

    return state;
}

static bool is_hrm(uint16_t keycode) {
    switch (keycode) {
        case LGUI_T(KC_R):
        case LALT_T(KC_S):
        case LSFT_T(KC_N):
        case LCTL_T(KC_T):
        case LCTL_T(KC_A):
        case RSFT_T(KC_E):
        case LALT_T(KC_H):
        case LGUI_T(KC_I):
        case LGUI_T(KC_A):
        case LSFT_T(KC_D):
        case LCTL_T(KC_F):
        case LCTL_T(KC_J):
        case RSFT_T(KC_K):
        case LALT_T(KC_L):
        case LGUI_T(KC_SCLN):
            return true;
        default:
            return false;
    }
}

static bool is_left_combo_hold_tap(uint16_t keycode) {
    return keycode == U_COMBO_TAB || keycode == U_COMBO_LEADER ||
           keycode == U_COMBO_SHIFT_LEADER;
}

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    if (is_hrm(keycode) || is_left_combo_hold_tap(keycode)) {
        return 280;
    }
    return 200;
}

uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) {
    if (keycode == LSFT_T(KC_0)) {
        return 0;
    }
    return 175;
}

bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case U_MAGIC_SHIFT:
        case U_SMART_NUM:
        case U_SYM_SPACE:
        case U_NAV_BSPC:
        case U_NAV_ENTER:
        case LT(APP, KC_DOT):
            return true;
        default:
            return false;
    }
}

bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    return keycode == U_APP_LAUNCHER || keycode == LSFT_T(KC_0);
}

char chordal_hold_handedness(keypos_t key) {
    if (key.row == 4 || key.row == 9) {
        return '*';
    }
    return key.row < 5 ? 'L' : 'R';
}

bool get_chordal_hold(uint16_t tap_hold_keycode, keyrecord_t *tap_hold_record,
                      uint16_t other_keycode, keyrecord_t *other_record) {
    char other_hand;
    if (other_record->event.type == KEY_EVENT) {
        other_hand = chordal_hold_handedness(other_record->event.key);
    } else if (IS_COMBOEVENT(other_record->event) &&
               last_physical_event_valid) {
        // QMK's synthetic combo event has no physical hand. Use the raw key
        // event that immediately produced it so same-hand rolls remain taps.
        other_hand = chordal_hold_handedness(last_physical_event_key);
    } else {
        return true;
    }

    if (is_left_combo_hold_tap(tap_hold_keycode)) {
        return other_hand == 'R' || other_hand == '*';
    }
    if (is_hrm(tap_hold_keycode)) {
        const char tap_hold_hand =
            chordal_hold_handedness(tap_hold_record->event.key);
        return other_hand == '*' || tap_hold_hand != other_hand;
    }
    return true;
}

static bool flow_tap_source(uint16_t keycode) {
    const uint16_t basic = tap_keycode(keycode);
    return (basic >= KC_A && basic <= KC_Z) || basic == KC_DOT ||
           basic == KC_COMM || basic == KC_SCLN || basic == KC_SLSH ||
           basic == KC_SPC;
}

uint16_t get_flow_tap_term(uint16_t keycode, keyrecord_t *record, uint16_t prev_keycode) {
    return is_hrm(keycode) && flow_tap_source(prev_keycode) ? 150 : 0;
}
