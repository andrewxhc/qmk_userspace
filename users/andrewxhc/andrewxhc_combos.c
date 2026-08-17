// Copyright 2026 andrewxhc
// SPDX-License-Identifier: GPL-2.0-only

#include "andrewxhc.h"

enum andrewxhc_combo_ids {
    C_ESC,
    C_MOUSE,
    C_TAB,
    C_LEADER,
    C_LEADER_SHIFT,
    C_CUT,
    C_COPY,
    C_PASTE,
    C_BSPC,
    C_DEL,
    C_LPAR,
    C_RPAR,
    C_LT,
    C_GT,
    C_LBKT,
    C_RBKT,
    C_LBRC,
    C_RBRC,
    C_AT,
    C_HASH,
    C_DOLLAR,
    C_PERCENT,
    C_GRAVE,
    C_BSLASH,
    C_EQUAL,
    C_TILDE,
    C_CARET,
    C_PLUS,
    C_STAR,
    C_AMPERSAND,
    C_UNDERSCORE,
    C_MINUS,
    C_SLASH,
    C_PIPE,
    C_COUNT,
};

#define COMBO_TERM_FAST 18
#define COMBO_TERM_SLOW 30
#define COMBO_IDLE_FAST 150
#define COMBO_IDLE_SLOW 50

// These tap payloads are reserved for the leader parser. Their hold sides
// preserve the source combo hold-taps rather than making the combos tap-only.
#define C_LEADER_MT       MT(MOD_LCTL | MOD_LSFT, KC_F19)
#define C_LEADER_SHIFT_MT MT(MOD_LCTL | MOD_LSFT | MOD_LALT, KC_F18)

// Combo keys are deliberately the DSEND keycodes at each physical position.
// COMBO_ONLY_FROM_LAYER keeps these stable while NAV or NUM is active.
const uint16_t PROGMEM c_esc[]          = {KC_C, KC_L, COMBO_END};
const uint16_t PROGMEM c_mouse[]        = {KC_L, KC_F, COMBO_END};
const uint16_t PROGMEM c_tab[]          = {U_HML_ALT(KC_S), U_HML_SFT(KC_N), COMBO_END};
const uint16_t PROGMEM c_leader[]       = {U_HML_SFT(KC_N), U_HML_CTL(KC_T), COMBO_END};
const uint16_t PROGMEM c_leader_shift[] = {U_HML_ALT(KC_S), U_HML_SFT(KC_N), U_HML_CTL(KC_T), COMBO_END};
const uint16_t PROGMEM c_cut[]          = {KC_W, KC_D, COMBO_END};
const uint16_t PROGMEM c_copy[]         = {KC_W, KC_M, COMBO_END};
const uint16_t PROGMEM c_paste[]        = {KC_M, KC_D, COMBO_END};

const uint16_t PROGMEM c_bspc[] = {KC_SCLN, KC_U, COMBO_END};
const uint16_t PROGMEM c_del[]  = {KC_U, KC_P, COMBO_END};

const uint16_t PROGMEM c_lpar[] = {U_HMR_CTL(KC_A), U_HMR_SFT(KC_E), COMBO_END};
const uint16_t PROGMEM c_rpar[] = {U_HMR_SFT(KC_E), U_HMR_ALT(KC_H), COMBO_END};
const uint16_t PROGMEM c_lt[]   = {U_HMR_CTL(KC_A), U_HMR_SFT(KC_E), COMBO_END};
const uint16_t PROGMEM c_gt[]   = {U_HMR_SFT(KC_E), U_HMR_ALT(KC_H), COMBO_END};

const uint16_t PROGMEM c_lbkt[] = {KC_O, KC_Y, COMBO_END};
const uint16_t PROGMEM c_rbkt[] = {KC_Y, KC_K, COMBO_END};
const uint16_t PROGMEM c_lbrc[] = {KC_O, KC_Y, COMBO_END};
const uint16_t PROGMEM c_rbrc[] = {KC_Y, KC_K, COMBO_END};

const uint16_t PROGMEM c_at[]      = {KC_C, U_HML_ALT(KC_S), COMBO_END};
const uint16_t PROGMEM c_hash[]    = {KC_L, U_HML_SFT(KC_N), COMBO_END};
const uint16_t PROGMEM c_dollar[]  = {KC_F, U_HML_CTL(KC_T), COMBO_END};
const uint16_t PROGMEM c_percent[] = {KC_V, KC_G, COMBO_END};

const uint16_t PROGMEM c_grave[]  = {U_HML_ALT(KC_S), KC_W, COMBO_END};
const uint16_t PROGMEM c_bslash[] = {U_HML_SFT(KC_N), KC_M, COMBO_END};
const uint16_t PROGMEM c_equal[]  = {U_HML_CTL(KC_T), KC_D, COMBO_END};
const uint16_t PROGMEM c_tilde[]  = {KC_G, KC_B, COMBO_END};

const uint16_t PROGMEM c_caret[]     = {KC_QUOT, KC_DOT, COMBO_END};
const uint16_t PROGMEM c_plus[]      = {KC_SCLN, U_HMR_CTL(KC_A), COMBO_END};
const uint16_t PROGMEM c_star[]      = {KC_U, U_HMR_SFT(KC_E), COMBO_END};
const uint16_t PROGMEM c_ampersand[] = {KC_P, U_HMR_ALT(KC_H), COMBO_END};

const uint16_t PROGMEM c_underscore[] = {KC_DOT, KC_COMM, COMBO_END};
const uint16_t PROGMEM c_minus[]      = {U_HMR_CTL(KC_A), KC_O, COMBO_END};
const uint16_t PROGMEM c_slash[]      = {U_HMR_SFT(KC_E), KC_Y, COMBO_END};
const uint16_t PROGMEM c_pipe[]       = {U_HMR_ALT(KC_H), KC_K, COMBO_END};

combo_t key_combos[C_COUNT] = {
    [C_ESC]          = COMBO(c_esc, KC_ESC),
    [C_MOUSE]        = COMBO(c_mouse, U_SMART_MOUSE),
    [C_TAB]          = COMBO(c_tab, U_COMBO_TAB),
    [C_LEADER]       = COMBO(c_leader, C_LEADER_MT),
    [C_LEADER_SHIFT] = COMBO(c_leader_shift, C_LEADER_SHIFT_MT),
    [C_CUT]          = COMBO(c_cut, U_CUT),
    [C_COPY]         = COMBO(c_copy, U_COPY),
    [C_PASTE]        = COMBO(c_paste, U_PASTE),

    [C_BSPC] = COMBO(c_bspc, KC_BSPC),
    [C_DEL]  = COMBO(c_del, KC_DEL),

    // Mixed-side modifier holds cannot be represented by QMK's MT encoding.
    [C_LPAR] = COMBO_ACTION(c_lpar),
    [C_RPAR] = COMBO_ACTION(c_rpar),
    [C_LT]   = COMBO(c_lt, KC_LT),
    [C_GT]   = COMBO(c_gt, KC_GT),

    [C_LBKT] = COMBO(c_lbkt, KC_LBRC),
    [C_RBKT] = COMBO(c_rbkt, KC_RBRC),
    [C_LBRC] = COMBO(c_lbrc, KC_LCBR),
    [C_RBRC] = COMBO(c_rbrc, KC_RCBR),

    [C_AT]      = COMBO(c_at, KC_AT),
    [C_HASH]    = COMBO(c_hash, KC_HASH),
    [C_DOLLAR]  = COMBO(c_dollar, KC_DLR),
    [C_PERCENT] = COMBO(c_percent, KC_PERC),

    [C_GRAVE]  = COMBO(c_grave, KC_GRV),
    [C_BSLASH] = COMBO(c_bslash, KC_BSLS),
    [C_EQUAL]  = COMBO(c_equal, KC_EQL),
    [C_TILDE]  = COMBO(c_tilde, KC_TILD),

    [C_CARET]     = COMBO(c_caret, KC_CIRC),
    [C_PLUS]      = COMBO(c_plus, KC_PLUS),
    [C_STAR]      = COMBO(c_star, KC_ASTR),
    [C_AMPERSAND] = COMBO(c_ampersand, KC_AMPR),

    [C_UNDERSCORE] = COMBO(c_underscore, KC_UNDS),
    [C_MINUS]      = COMBO(c_minus, KC_MINS),
    [C_SLASH]      = COMBO(c_slash, KC_SLSH),
    [C_PIPE]       = COMBO(c_pipe, KC_PIPE),
};

#define BASE_LAYER_MASK (((layer_state_t)1 << DSEND) | ((layer_state_t)1 << QWERTY))

// Key Override uses the first matching entry, so the more-specific
// Ctrl+Shift morphs must precede the Shift-only entries.
const key_override_t comma_ctrl_shift_override =
    ko_make_with_layers(MOD_MASK_CTRL | MOD_MASK_SHIFT, KC_COMM, KC_UNDS, BASE_LAYER_MASK);
const key_override_t dot_ctrl_shift_override =
    ko_make_with_layers(MOD_MASK_CTRL | MOD_MASK_SHIFT, KC_DOT, KC_GRV, BASE_LAYER_MASK);
const key_override_t comma_shift_override =
    ko_make_with_layers_and_negmods(MOD_MASK_SHIFT, KC_COMM, KC_QUES, BASE_LAYER_MASK, MOD_MASK_CTRL);
const key_override_t dot_shift_override =
    ko_make_with_layers_and_negmods(MOD_MASK_SHIFT, KC_DOT, KC_EXLM, BASE_LAYER_MASK, MOD_MASK_CTRL);

const key_override_t *key_overrides[] = {
    &comma_ctrl_shift_override,
    &dot_ctrl_shift_override,
    &comma_shift_override,
    &dot_shift_override,
};

static uint16_t combo_idle_timer;

void andrewxhc_combo_note_keypress(void) {
    combo_idle_timer = timer_read();
}

uint16_t get_combo_term(uint16_t combo_index, combo_t *combo) {
    (void)combo;
    return combo_index < C_AT ? COMBO_TERM_FAST : COMBO_TERM_SLOW;
}

static bool combo_layer_allowed(uint16_t combo_index, uint8_t layer) {
    switch (combo_index) {
        case C_LPAR:
        case C_RPAR:
        case C_LBKT:
        case C_RBKT:
            return layer == DSEND || layer == NUM;

        case C_LT:
        case C_GT:
        case C_LBRC:
        case C_RBRC:
            return layer == NAV;

        default:
            return layer == DSEND || layer == NAV || layer == NUM;
    }
}

bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    (void)combo;
    (void)keycode;
    (void)record;

    const uint8_t  layer      = get_highest_layer(layer_state | default_layer_state);
    const uint16_t idle_limit = combo_index < C_AT ? COMBO_IDLE_FAST : COMBO_IDLE_SLOW;

    return combo_layer_allowed(combo_index, layer) && timer_elapsed(combo_idle_timer) >= idle_limit;
}

typedef struct {
    uint16_t pressed_at;
    uint16_t last_tap_at;
    uint16_t tap_keycode;
    uint16_t shifted_tap_keycode;
    uint16_t tap_action;
    uint8_t  hold_mods;
    uint8_t  added_mods;
    bool     active;
    bool     hold_active;
    bool     tap_active;
    bool     had_tap;
    bool     force_tap;
} action_hold_tap_t;

static action_hold_tap_t lpar_hold_tap;
static action_hold_tap_t rpar_hold_tap;

static void action_hold_tap_activate_tap(action_hold_tap_t *state) {
    if (!state->active || state->hold_active || state->tap_active) {
        return;
    }

    const uint8_t mods = get_mods() | get_weak_mods() | get_oneshot_mods();
    caps_word_off();
    state->tap_action =
        (mods & MOD_MASK_SHIFT) ? state->shifted_tap_keycode : state->tap_keycode;
    state->tap_active = true;
    state->force_tap  = true;
    state->had_tap    = true;
    state->last_tap_at = state->pressed_at;
    register_code16(state->tap_action);
    andrewxhc_consume_oneshot_num();
    andrewxhc_num_word_break();
    andrewxhc_repeat_cancel();
}

static void action_hold_tap_press(action_hold_tap_t *state, uint8_t hold_mods,
                                  uint16_t tap_keycode,
                                  uint16_t shifted_tap_keycode) {
    state->force_tap    = state->had_tap && timer_elapsed(state->last_tap_at) <= QUICK_TAP_TERM;
    state->pressed_at   = timer_read();
    state->tap_keycode  = tap_keycode;
    state->shifted_tap_keycode = shifted_tap_keycode;
    state->tap_action   = KC_NO;
    state->hold_mods    = hold_mods;
    state->added_mods   = 0;
    state->active       = true;
    state->hold_active  = false;
    state->tap_active   = false;

    if (state->force_tap) {
        action_hold_tap_activate_tap(state);
    }
}

static void action_hold_tap_activate_hold(action_hold_tap_t *state) {
    if (!state->active || state->hold_active || state->tap_active ||
        state->force_tap) {
        return;
    }

    // Avoid releasing modifiers that were already held before this combo.
    state->added_mods = state->hold_mods & ~get_mods();
    caps_word_off();
    register_mods(state->added_mods);
    state->hold_active = true;
    andrewxhc_num_word_break();
    andrewxhc_repeat_cancel();
}

static bool is_left_or_thumb_position(keypos_t key) {
    const bool left_alpha = key.row >= 1 && key.row <= 3 && key.col < 6;
    const bool thumb =
        (key.row == 4 || key.row == 9) && key.col >= 2 && key.col <= 4;
    return left_alpha || thumb;
}

void andrewxhc_combo_note_event(keyrecord_t *record) {
    if (record->event.pressed || is_left_or_thumb_position(record->event.key)) {
        return;
    }

    // The source HRM-combo instances use hold-trigger-on-release with
    // KEYS_L + THUMBS. A release outside that allowed set forces tap;
    // allowed-side releases leave the tap-preferred timeout undecided.
    action_hold_tap_activate_tap(&lpar_hold_tap);
    action_hold_tap_activate_tap(&rpar_hold_tap);
}

static void action_hold_tap_release(action_hold_tap_t *state) {
    if (!state->active) {
        return;
    }

    if (!state->tap_active && !state->hold_active &&
        timer_elapsed(state->pressed_at) >= COMBO_HOLD_TERM) {
        action_hold_tap_activate_hold(state);
    }
    if (!state->tap_active && !state->hold_active) {
        action_hold_tap_activate_tap(state);
    }

    if (state->tap_active) {
        unregister_code16(state->tap_action);
    } else if (state->hold_active) {
        unregister_mods(state->added_mods);
    }

    state->active      = false;
    state->hold_active = false;
    state->tap_active  = false;
    state->tap_action  = KC_NO;
    state->added_mods  = 0;
    state->force_tap   = false;
}

void andrewxhc_combo_task(void) {
    if (lpar_hold_tap.active && !lpar_hold_tap.hold_active && timer_elapsed(lpar_hold_tap.pressed_at) >= COMBO_HOLD_TERM) {
        action_hold_tap_activate_hold(&lpar_hold_tap);
    }
    if (rpar_hold_tap.active && !rpar_hold_tap.hold_active && timer_elapsed(rpar_hold_tap.pressed_at) >= COMBO_HOLD_TERM) {
        action_hold_tap_activate_hold(&rpar_hold_tap);
    }
}

void process_combo_event(uint16_t combo_index, bool pressed) {
    switch (combo_index) {
        case C_LPAR:
            if (pressed) {
                andrewxhc_combo_note_keypress();
                action_hold_tap_press(&lpar_hold_tap, MOD_RSFT | MOD_LCTL,
                                      KC_LPRN, KC_LT);
            } else {
                action_hold_tap_release(&lpar_hold_tap);
            }
            break;

        case C_RPAR:
            if (pressed) {
                andrewxhc_combo_note_keypress();
                action_hold_tap_press(&rpar_hold_tap, MOD_RSFT | MOD_LALT,
                                      KC_RPRN, KC_GT);
            } else {
                action_hold_tap_release(&rpar_hold_tap);
            }
            break;
    }
}
