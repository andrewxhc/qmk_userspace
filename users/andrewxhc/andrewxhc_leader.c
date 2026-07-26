// Copyright 2026 Andrew XHC
// SPDX-License-Identifier: GPL-2.0-only

#include "andrewxhc.h"

enum {
    LEADER_MAX_LENGTH = 6,
    LEADER_COMPOSE_DELAY_MS = 20,
};

enum leader_action {
    LEADER_GREEK_ALPHA,
    LEADER_GREEK_BETA,
    LEADER_GREEK_GAMMA,
    LEADER_GREEK_DELTA,
    LEADER_GREEK_EPSILON,
    LEADER_GREEK_ZETA,
    LEADER_GREEK_ETA,
    LEADER_GREEK_THETA,
    LEADER_GREEK_IOTA,
    LEADER_GREEK_KAPPA,
    LEADER_GREEK_LAMBDA,
    LEADER_GREEK_MU,
    LEADER_GREEK_NU,
    LEADER_GREEK_XI,
    LEADER_GREEK_OMICRON,
    LEADER_GREEK_PI,
    LEADER_GREEK_RHO,
    LEADER_GREEK_SIGMA,
    LEADER_GREEK_TAU,
    LEADER_GREEK_UPSILON,
    LEADER_GREEK_PHI,
    LEADER_GREEK_CHI,
    LEADER_GREEK_PSI,
    LEADER_GREEK_OMEGA,
    LEADER_LENNY,
    LEADER_LAYER_DSEND,
    LEADER_LAYER_QWERTY,
    LEADER_RESET,
    LEADER_BOOT,
};

typedef struct {
    uint8_t keys[LEADER_MAX_LENGTH];
    uint8_t length;
    uint8_t action;
} leader_sequence_t;

#define LEADER_SEQ2(k0, k1, action)                 \
    {                                               \
        {k0, k1}, 2, action                         \
    }
#define LEADER_SEQ4(k0, k1, k2, k3, action)         \
    {                                               \
        {k0, k1, k2, k3}, 4, action                 \
    }
#define LEADER_SEQ5(k0, k1, k2, k3, k4, action)     \
    {                                               \
        {k0, k1, k2, k3, k4}, 5, action             \
    }
#define LEADER_SEQ6(k0, k1, k2, k3, k4, k5, action) \
    {                                               \
        {k0, k1, k2, k3, k4, k5}, 6, action         \
    }

static const leader_sequence_t PROGMEM leader_sequences[] = {
    LEADER_SEQ2(KC_E, KC_A, LEADER_GREEK_ALPHA),
    LEADER_SEQ2(KC_E, KC_B, LEADER_GREEK_BETA),
    LEADER_SEQ2(KC_E, KC_G, LEADER_GREEK_GAMMA),
    LEADER_SEQ2(KC_E, KC_D, LEADER_GREEK_DELTA),
    LEADER_SEQ2(KC_E, KC_E, LEADER_GREEK_EPSILON),
    LEADER_SEQ2(KC_E, KC_Z, LEADER_GREEK_ZETA),
    LEADER_SEQ2(KC_E, KC_H, LEADER_GREEK_ETA),
    LEADER_SEQ2(KC_E, KC_V, LEADER_GREEK_THETA),
    LEADER_SEQ2(KC_E, KC_I, LEADER_GREEK_IOTA),
    LEADER_SEQ2(KC_E, KC_K, LEADER_GREEK_KAPPA),
    LEADER_SEQ2(KC_E, KC_L, LEADER_GREEK_LAMBDA),
    LEADER_SEQ2(KC_E, KC_M, LEADER_GREEK_MU),
    LEADER_SEQ2(KC_E, KC_N, LEADER_GREEK_NU),
    LEADER_SEQ2(KC_E, KC_X, LEADER_GREEK_XI),
    LEADER_SEQ2(KC_E, KC_O, LEADER_GREEK_OMICRON),
    LEADER_SEQ2(KC_E, KC_P, LEADER_GREEK_PI),
    LEADER_SEQ2(KC_E, KC_R, LEADER_GREEK_RHO),
    LEADER_SEQ2(KC_E, KC_S, LEADER_GREEK_SIGMA),
    LEADER_SEQ2(KC_E, KC_T, LEADER_GREEK_TAU),
    LEADER_SEQ2(KC_E, KC_U, LEADER_GREEK_UPSILON),
    LEADER_SEQ2(KC_E, KC_F, LEADER_GREEK_PHI),
    LEADER_SEQ2(KC_E, KC_C, LEADER_GREEK_CHI),
    LEADER_SEQ2(KC_E, KC_Y, LEADER_GREEK_PSI),
    LEADER_SEQ2(KC_E, KC_W, LEADER_GREEK_OMEGA),
    LEADER_SEQ2(KC_L, KC_F, LEADER_LENNY),
    LEADER_SEQ5(KC_D, KC_S, KC_E, KC_N, KC_D, LEADER_LAYER_DSEND),
    LEADER_SEQ6(KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, LEADER_LAYER_QWERTY),
    LEADER_SEQ5(KC_R, KC_E, KC_S, KC_E, KC_T, LEADER_RESET),
    LEADER_SEQ4(KC_B, KC_O, KC_O, KC_T, LEADER_BOOT),
};

static const uint16_t PROGMEM greek_lower_code_points[] = {
    0x03B1, // alpha
    0x03B2, // beta
    0x03B3, // gamma
    0x03B4, // delta
    0x03B5, // epsilon
    0x03B6, // zeta
    0x03B7, // eta
    0x03B8, // theta
    0x03B9, // iota
    0x03BA, // kappa
    0x03BB, // lambda
    0x03BC, // mu
    0x03BD, // nu
    0x03BE, // xi
    0x03BF, // omicron
    0x03C0, // pi
    0x03C1, // rho
    0x03C3, // sigma
    0x03C4, // tau
    0x03C5, // upsilon
    0x03D5, // phi symbol, matching ZMK's UC_EL_PHI
    0x03C7, // chi
    0x03C8, // psi
    0x03C9, // omega
};

enum {
    LEADER_SEQUENCE_COUNT = ARRAY_SIZE(leader_sequences),
    LEADER_GREEK_COUNT = ARRAY_SIZE(greek_lower_code_points),
};

STATIC_ASSERT(LEADER_SEQUENCE_COUNT <= 32, "Leader candidate mask is too small");
STATIC_ASSERT(LEADER_GREEK_COUNT == LEADER_GREEK_OMEGA + 1, "Greek action table is incomplete");

#define LEADER_ALL_CANDIDATES ((UINT32_C(1) << LEADER_SEQUENCE_COUNT) - UINT32_C(1))

static bool     leader_active;
static bool     leader_shifted;
static uint8_t  leader_length;
static uint32_t leader_candidates;

static keypos_t captured_positions[LEADER_MAX_LENGTH];
static uint8_t  captured_slots;

bool andrewxhc_leader_is_active(void) {
    return leader_active;
}

static bool same_key_position(keypos_t lhs, keypos_t rhs) {
    return lhs.row == rhs.row && lhs.col == rhs.col;
}

static void capture_position(keypos_t position) {
    for (uint8_t slot = 0; slot < LEADER_MAX_LENGTH; ++slot) {
        const uint8_t slot_mask = UINT8_C(1) << slot;
        if (!(captured_slots & slot_mask)) {
            captured_positions[slot] = position;
            captured_slots |= slot_mask;
            return;
        }
    }
}

static bool release_captured_position(keypos_t position) {
    for (uint8_t slot = 0; slot < LEADER_MAX_LENGTH; ++slot) {
        const uint8_t slot_mask = UINT8_C(1) << slot;
        if ((captured_slots & slot_mask) && same_key_position(captured_positions[slot], position)) {
            captured_slots &= ~slot_mask;
            return true;
        }
    }
    return false;
}

static bool is_shift_passthrough(uint16_t keycode, const keyrecord_t *record) {
    if (keycode == KC_LSFT || keycode == KC_RSFT) {
        return true;
    }

    if (IS_QK_MOD_TAP(keycode) && record->tap.count == 0) {
        const uint8_t mods = mod_config(QK_MOD_TAP_GET_MODS(keycode));
        return (mods & MOD_MASK_SHIFT) && !(mods & ~MOD_MASK_SHIFT);
    }

    return false;
}

static uint16_t normalize_tapped_keycode(uint16_t keycode, const keyrecord_t *record) {
    if (IS_QK_MOD_TAP(keycode)) {
        if (record->tap.count == 0) {
            return KC_NO;
        }
        keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
    } else if (IS_QK_LAYER_TAP(keycode)) {
        if (record->tap.count == 0) {
            return KC_NO;
        }
        keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
    }

    if (IS_QK_MODS(keycode)) {
        keycode = QK_MODS_GET_BASIC_KEYCODE(keycode);
    }

    return keycode;
}

static void deactivate_leader(void) {
    leader_active = false;
    leader_shifted = false;
    leader_length = 0;
    leader_candidates = 0;
}

static void send_lenny_compose(void) {
    tap_code_delay(KC_RALT, LEADER_COMPOSE_DELAY_MS);
    wait_ms(LEADER_COMPOSE_DELAY_MS);
    tap_code_delay(KC_L, LEADER_COMPOSE_DELAY_MS);
    wait_ms(LEADER_COMPOSE_DELAY_MS);
    tap_code_delay(KC_F, LEADER_COMPOSE_DELAY_MS);
    wait_ms(LEADER_COMPOSE_DELAY_MS);
}

static void perform_leader_action(uint8_t action, bool shifted) {
    if (action < LEADER_GREEK_COUNT) {
        uint16_t code_point = pgm_read_word(&greek_lower_code_points[action]);
        if (shifted) {
            // Every selected uppercase code point is lower - 0x20 except
            // U+03D5 GREEK PHI SYMBOL, whose uppercase is U+03A6.
            code_point = code_point == 0x03D5 ? 0x03A6 : code_point - 0x20;
        }
        andrewxhc_num_word_break();
        register_unicode(code_point);
        return;
    }

    switch (action) {
        case LEADER_LENNY:
            andrewxhc_num_word_break();
            send_lenny_compose();
            break;
        case LEADER_LAYER_DSEND:
            andrewxhc_num_word_break();
            andrewxhc_consume_oneshot_num();
            layer_move(DSEND);
            break;
        case LEADER_LAYER_QWERTY:
            andrewxhc_num_word_break();
            andrewxhc_consume_oneshot_num();
            layer_move(QWERTY);
            break;
        case LEADER_RESET:
            soft_reset_keyboard();
            break;
        case LEADER_BOOT:
            reset_keyboard();
            break;
    }
}

void andrewxhc_leader_start(bool shifted) {
    // ZMK refuses a new leader activation while accepted sequence keys remain
    // held. Released keys do not keep an unfinished sequence locked.
    if (captured_slots) {
        return;
    }

    leader_active = true;
    leader_shifted = shifted;
    leader_length = 0;
    leader_candidates = LEADER_ALL_CANDIDATES;
    andrewxhc_repeat_cancel();

    if (shifted) {
        // The shifted-leader chord is represented as a one-shot Shift. Keep
        // that intent in leader_shifted, but do not leak the modifier into
        // sequence entry or the key following the leader action.
        del_oneshot_mods(MOD_MASK_SHIFT);
    }
}

bool andrewxhc_leader_process(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        if (release_captured_position(record->event.key)) {
            return false;
        }
        return true;
    }

    if (!leader_active || is_shift_passthrough(keycode, record)) {
        return true;
    }

    const uint16_t normalized = normalize_tapped_keycode(keycode, record);
    if (normalized < KC_A || normalized > KC_Z || leader_length >= LEADER_MAX_LENGTH) {
        deactivate_leader();
        andrewxhc_repeat_note_passthrough(keycode, record);
        return true;
    }

    const uint8_t  next_length = leader_length + 1;
    uint32_t       next_candidates = 0;
    uint8_t        completed_action = UINT8_MAX;

    for (uint8_t index = 0; index < LEADER_SEQUENCE_COUNT; ++index) {
        const uint32_t candidate_mask = UINT32_C(1) << index;
        if (!(leader_candidates & candidate_mask)) {
            continue;
        }

        if (pgm_read_byte(&leader_sequences[index].keys[leader_length]) != normalized) {
            continue;
        }

        next_candidates |= candidate_mask;
        if (pgm_read_byte(&leader_sequences[index].length) == next_length && completed_action == UINT8_MAX) {
            completed_action = pgm_read_byte(&leader_sequences[index].action);
        }
    }

    if (!next_candidates) {
        deactivate_leader();
        andrewxhc_repeat_note_passthrough(keycode, record);
        return true;
    }

    capture_position(record->event.key);
    andrewxhc_combo_note_keypress();
    leader_length = next_length;
    leader_candidates = next_candidates;

    if (completed_action != UINT8_MAX) {
        const bool uppercase =
            leader_shifted || ((get_mods() | get_weak_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT);
        deactivate_leader();
        perform_leader_action(completed_action, uppercase);
    }

    return false;
}
