
#ifndef _RULES_H_
#define _RULES_H_

#ifdef __AVR__
#include <avr/pgmspace.h>
#endif

constexpr char SHIFT_PORT = 3;
constexpr char SHIFT_TRIGGER = 6;
constexpr char SHIFT_ACTION = 6;

enum Inputs {
/* a0p0 */ input_pietro_pokoj_asi = 0 << SHIFT_PORT,
/* a0p1 */ input_pietro_pokoj_agi,
/* a0p2 */ input_pietro_goscinny,
/* a0p3 */ input_parter_gabinet,
/* a0p4 */ input_pietro_lazienka_lewy,
/* a0p5 */ input_pietro_lazienka_prawy,
/* a0p6 */ input_pietro_gospodarczy_lewy,
/* a0p7 */ input_pietro_gospodarczy_prawy,
/* a4p0 */ input_pietro_garderoba_lewa = 4 << SHIFT_PORT,
/* a4p1 */ input_pietro_garderoba_prawa,
/* a4p2 */ input_parter_kuchnia_scianka_lewy,
/* a4p3 */ input_parter_kuchnia_scianka_prawy,
/* a4p4 */ input_parter_kuchnia_okap_lewy,
/* a4p5 */ input_parter_kuchnia_okap_prawy,
/* a4p6 */ input_parter_pomaranczowy,
/* a4p7 */ input_parter_naroznik_prawy,
/* a3p0 */ input_pietro_schody_podwojny_lewy = 3 << SHIFT_PORT,
/* a3p1 */ input_pietro_schody_podwojny_prawy,
/* a3p2 */ input_parter_schody_podwojny_lewy,
/* a3p3 */ input_parter_schody_podwojny_prawy,
/* a3p4 */ input_pietro_schody_pojedynczy,
/* a3p5 */ input_parter_kuchnia_ekspres,
/* a3p6 */ input_parter_sypialnia,
/* a3p7 */ input_parter_lazienka,
/* a2p0 */ input_parter_przedsionek_lewy = 2 << SHIFT_PORT,
/* a2p1 */ input_parter_przedsionek_prawy,
/* a2p2 */ input_parter_wc,
/* a2p3 */ input_piwnica_zejscie_gora,
/* a2p4 */ input_piwnica_zejscie_dol,
/* a2p5 */ input_piwnica_wejscie_lewy,
/* a2p6 */ input_piwnica_wejscie_srodkowy,
/* a2p7 */ input_piwnica_wejscie_prawy,
/* a1p0 */ input_piwnica_sala_pojedynczy = 1 << SHIFT_PORT,
/* a1p1 */ input_piwnica_sala_podwojny_lewy,
/* a1p2 */ input_piwnica_sala_podwojny_prawy,
/* a1p3 */ input_piwnica_kotlownia,
/* a1p4 */ input_piwnica_warsztat,
/* a1p5 */ input_pietro_hol,
/* a1p6 */ input_parter_hol,
/* a1p7 */ input_parter_naroznik_lewy,
};

enum Outputs {
/* a0p0 */ output_pietro_pokoj_asi_sufit = 0 << SHIFT_PORT,
/* a0p1 */ output_pietro_garderoba_lewa_sufit,
/* a0p2 */ output_pietro_pokoj_agi_sufit,
/* a0p3 */ output_pietro_garderoba_prawa_sufit,
/* a0p4 */ output_pietro_lazienka_sufit,
/* a0p5 */ output_pietro_gospodarczy_sufit,
/* a0p6 */ output_pietro_goscinny_sufit,
/* a0p7 */ output_pietro_hol_kinkiet,
/* a4p0 */ output_pietro_lazienka_kinkiet = 4 << SHIFT_PORT,
/* a4p1 */ output_pietro_hol_sufit,
/* a4p2 */ output_pietro_gospodarczy_wanna,
/* a4p3 */ output_parter_gabinet_sufit,
/* a4p4 */ output_parter_salon_sufit,
/* a4p5 */ output_parter_salon_podwieszany,
/* a4p6 */ output_parter_jadalnia,
/* a4p7 */ output_parter_kuchnia,
/* a3p0 */ output_parter_ekspres = 3 << SHIFT_PORT,
/* a3p1 */ output_piwnica_spocznik,
/* a3p2 */ output_parter_hol_sufit,
/* a3p3 */ output_pietro_schody,
/* a3p4 */ output_parter_sypialnia_sufit,
/* a3p5 */ output_parter_lazienka,
/* a3p6 */ output_parter_przedsionek,
/* a3p7 */ output_parter_wc,
/* a2p0 */ output_piwnica_schody = 2 << SHIFT_PORT,
/* a2p1 */ output_piwnica_hol,
/* a2p2 */ output_piwnica_kotlownia,
/* a2p3 */ output_piwnica_warsztat,
/* a2p4 */ output_piwnica_sala,
/* a2p5 */ output_piwnica_sala_komin,
/* a2p6 */ output_piwnica_sala_kanapa,
};

enum Trigger {
  None,
  ShortPress,
  MediumPress, // 2-5 seconds
  LongPress, // over 5 seconds
};

enum Effect {
  TimerReset = 1,  // keep on for next 60 seconds
  Toggle,
  AllOff,
};

#define WHEN_PRESSED_SHORT(X) (ShortPress << SHIFT_TRIGGER | input_##X)
#define WHEN_PRESSED_MEDIUM(X) (MediumPress << SHIFT_TRIGGER | input_##X)
#define WHEN_PRESSED_LONG(X) (LongPress << SHIFT_TRIGGER | input_##X)
#define TOGGLE(X) (Toggle << SHIFT_ACTION | output_##X)
#define TIMER_RESET(X) (TimerReset << SHIFT_ACTION | output_##X)
#define ALL_OFF(x) (AllOff << SHIFT_ACTION)

constexpr PROGMEM unsigned char rules[] = {
 WHEN_PRESSED_SHORT(pietro_gospodarczy_prawy), TOGGLE(pietro_gospodarczy_sufit),
 WHEN_PRESSED_MEDIUM(pietro_pokoj_agi), TOGGLE(pietro_hol_kinkiet),
 WHEN_PRESSED_SHORT(pietro_pokoj_agi), TOGGLE(pietro_pokoj_agi_sufit),
 WHEN_PRESSED_SHORT(parter_wc), TOGGLE(parter_wc),
 WHEN_PRESSED_SHORT(parter_naroznik_lewy), TOGGLE(parter_salon_sufit),
 WHEN_PRESSED_SHORT(parter_hol), TOGGLE(parter_hol_sufit),
 WHEN_PRESSED_SHORT(piwnica_zejscie_dol), TOGGLE(piwnica_schody),
 WHEN_PRESSED_SHORT(pietro_gospodarczy_lewy), TOGGLE(pietro_gospodarczy_wanna),
 WHEN_PRESSED_SHORT(parter_kuchnia_ekspres), TOGGLE(parter_ekspres),
 WHEN_PRESSED_SHORT(piwnica_wejscie_lewy), TOGGLE(piwnica_hol),
 WHEN_PRESSED_SHORT(parter_schody_podwojny_prawy), TOGGLE(pietro_schody),
 WHEN_PRESSED_SHORT(piwnica_zejscie_gora), TOGGLE(piwnica_spocznik),
 WHEN_PRESSED_SHORT(piwnica_sala_podwojny_prawy), TOGGLE(piwnica_sala_kanapa),
 WHEN_PRESSED_SHORT(parter_naroznik_prawy), TOGGLE(parter_salon_podwieszany),
 WHEN_PRESSED_SHORT(piwnica_wejscie_srodkowy), TOGGLE(piwnica_schody),
 WHEN_PRESSED_SHORT(pietro_lazienka_prawy), TOGGLE(pietro_lazienka_kinkiet),
 WHEN_PRESSED_SHORT(pietro_schody_podwojny_lewy), TOGGLE(pietro_schody),
 WHEN_PRESSED_SHORT(parter_sypialnia), TOGGLE(parter_sypialnia_sufit),
 WHEN_PRESSED_SHORT(piwnica_sala_pojedynczy), TOGGLE(piwnica_sala),
 WHEN_PRESSED_SHORT(parter_gabinet), TOGGLE(parter_gabinet_sufit),
 WHEN_PRESSED_MEDIUM(pietro_goscinny), TOGGLE(pietro_hol_kinkiet),
 WHEN_PRESSED_SHORT(pietro_goscinny), TOGGLE(pietro_goscinny_sufit),
 WHEN_PRESSED_SHORT(parter_schody_podwojny_lewy), TOGGLE(pietro_hol_sufit),
 WHEN_PRESSED_SHORT(piwnica_wejscie_prawy), TOGGLE(piwnica_spocznik),
 WHEN_PRESSED_SHORT(parter_lazienka), TOGGLE(parter_lazienka),
 WHEN_PRESSED_MEDIUM(parter_kuchnia_scianka_lewy), TOGGLE(parter_kuchnia),
 WHEN_PRESSED_SHORT(parter_kuchnia_scianka_lewy), TOGGLE(parter_jadalnia),
 WHEN_PRESSED_MEDIUM(parter_przedsionek_lewy), ALL_OFF(AllOff),
 WHEN_PRESSED_SHORT(parter_przedsionek_lewy), TOGGLE(parter_hol_sufit),
 WHEN_PRESSED_SHORT(parter_kuchnia_okap_lewy), TOGGLE(parter_jadalnia),
 WHEN_PRESSED_SHORT(parter_kuchnia_okap_prawy), TOGGLE(parter_kuchnia),
 WHEN_PRESSED_MEDIUM(pietro_pokoj_asi), TOGGLE(pietro_hol_kinkiet),
 WHEN_PRESSED_SHORT(pietro_pokoj_asi), TOGGLE(pietro_pokoj_asi_sufit),
 WHEN_PRESSED_SHORT(pietro_garderoba_prawa), TOGGLE(pietro_garderoba_prawa_sufit),
 WHEN_PRESSED_SHORT(piwnica_warsztat), TOGGLE(piwnica_warsztat),
 WHEN_PRESSED_SHORT(parter_kuchnia_scianka_prawy), TOGGLE(parter_kuchnia),
 WHEN_PRESSED_SHORT(parter_przedsionek_prawy), TOGGLE(parter_przedsionek),
 WHEN_PRESSED_MEDIUM(pietro_garderoba_lewa), TOGGLE(pietro_garderoba_prawa_sufit),
 WHEN_PRESSED_SHORT(pietro_garderoba_lewa), TOGGLE(pietro_garderoba_lewa_sufit),
 WHEN_PRESSED_SHORT(piwnica_kotlownia), TOGGLE(piwnica_kotlownia),
 WHEN_PRESSED_SHORT(pietro_lazienka_lewy), TOGGLE(pietro_lazienka_sufit),
 WHEN_PRESSED_SHORT(pietro_schody_podwojny_prawy), TOGGLE(pietro_hol_kinkiet),
 WHEN_PRESSED_SHORT(pietro_schody_pojedynczy), TOGGLE(pietro_hol_sufit),
 WHEN_PRESSED_SHORT(piwnica_sala_podwojny_lewy), TOGGLE(piwnica_sala_komin),
 WHEN_PRESSED_MEDIUM(parter_pomaranczowy), TOGGLE(parter_salon_podwieszany),
 WHEN_PRESSED_SHORT(parter_pomaranczowy), TOGGLE(parter_salon_sufit),

};

const int rulesSize = 92;

#endif // _RULES_H_