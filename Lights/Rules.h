
#ifndef _RULES_H_
#define _RULES_H_

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

constexpr char SHIFT_PORT = 3;
constexpr char SHIFT_TRIGGER = 6;
constexpr char SHIFT_ACTION = 6;

enum Inputs {
/* a0p0 */ przycisk_pietro_pokoj_asi = 0 << SHIFT_PORT,
/* a0p1 */ przycisk_pietro_pokoj_agi,
/* a0p2 */ przycisk_pietro_goscinny,
/* a0p3 */ przycisk_parter_gabinet,
/* a0p4 */ przycisk_pietro_lazienka_lewy,
/* a0p5 */ przycisk_pietro_lazienka_prawy,
/* a0p6 */ przycisk_pietro_gospodarczy_lewy,
/* a0p7 */ przycisk_pietro_gospodarczy_prawy,
/* a4p0 */ przycisk_pietro_garderoba_lewa = 4 << SHIFT_PORT,
/* a4p1 */ przycisk_pietro_garderoba_prawa,
/* a4p2 */ przycisk_parter_kuchnia_scianka_lewy,
/* a4p3 */ przycisk_parter_kuchnia_scianka_prawy,
/* a4p4 */ przycisk_parter_kuchnia_okap_lewy,
/* a4p5 */ przycisk_parter_kuchnia_okap_prawy,
/* a4p6 */ przycisk_parter_pomaranczowy,
/* a4p7 */ przycisk_parter_naroznik_prawy,
/* a2p0 */ przycisk_pietro_schody_podwojny_lewy = 2 << SHIFT_PORT,
/* a2p1 */ przycisk_pietro_schody_podwojny_prawy,
/* a2p2 */ przycisk_parter_schody_podwojny_lewy,
/* a2p3 */ przycisk_parter_schody_podwojny_prawy,
/* a2p4 */ przycisk_pietro_schody_pojedynczy,
/* a2p5 */ przycisk_parter_kuchnia_ekspres,
/* a2p6 */ przycisk_parter_sypialnia,
/* a2p7 */ przycisk_parter_lazienka,
/* a3p0 */ przycisk_parter_przedsionek = 3 << SHIFT_PORT,
/* a3p1 */ przycisk_parter_wc,
/* a3p2 */ przycisk_piwnica_zejscie_gora,
/* a3p3 */ przycisk_piwnica_zejscie_dol,
/* a3p4 */ przycisk_piwnica_wejscie_lewy,
/* a3p5 */ przycisk_piwnica_wejscie_srodkowy,
/* a3p6 */ przycisk_piwnica_wejscie_prawy,
/* a3p7 */ przycisk_piwnica_warsztat,
/* a1p0 */ przycisk_piwnica_sala_pojedynczy = 1 << SHIFT_PORT,
/* a1p1 */ przycisk_piwnica_sala_podwojny_lewy,
/* a1p2 */ przycisk_piwnica_sala_podwojny_prawy,
/* a1p3 */ przycisk_piwnica_kotlownia_prawy,
/* a1p4 */ przycisk_piwnica_kotlownia_lewy,
/* a1p5 */ przycisk_pietro_hol,
/* a1p6 */ przycisk_parter_hol,
/* a1p7 */ przycisk_parter_naroznik_lewy,
};

enum Outputs {
/* a0p0 */ punkt_pietro_pokoj_asi_sufit = 0 << SHIFT_PORT,
/* a0p1 */ punkt_pietro_garderoba_lewa_sufit,
/* a0p2 */ punkt_pietro_pokoj_agi_sufit,
/* a0p3 */ punkt_pietro_garderoba_prawa_sufit,
/* a0p4 */ punkt_pietro_lazienka_sufit,
/* a0p5 */ punkt_pietro_gospodarczy_sufit,
/* a0p6 */ punkt_pietro_goscinny_sufit,
/* a0p7 */ punkt_pietro_hol_kinkiet,
/* a4p0 */ punkt_pietro_lazienka_kinkiet = 4 << SHIFT_PORT,
/* a4p1 */ punkt_pietro_hol_sufit,
/* a4p2 */ punkt_pietro_gospodarczy_wanna,
/* a4p3 */ punkt_parter_gabinet_sufit,
/* a4p4 */ punkt_parter_salon_sufit,
/* a4p5 */ punkt_parter_salon_podwieszany,
/* a4p6 */ punkt_parter_jadalnia,
/* a4p7 */ punkt_parter_kuchnia,
/* a2p0 */ punkt_parter_ekspres = 2 << SHIFT_PORT,
/* a2p1 */ punkt_niewiadome,
/* a2p2 */ punkt_parter_hol_sufit,
/* a2p3 */ punkt_pietro_schody,
/* a2p4 */ punkt_parter_sypialnia_sufit,
/* a2p5 */ punkt_parter_lazienka,
/* a2p6 */ punkt_parter_przedsionek,
/* a2p7 */ punkt_parter_wc,
/* a3p0 */ punkt_piwnica_schody = 3 << SHIFT_PORT,
/* a3p1 */ punkt_piwnica_spocznik,
/* a3p2 */ punkt_piwnica_hol,
/* a3p3 */ punkt_piwnica_kotlownia_kociol,
/* a3p4 */ punkt_piwnica_kotlownia_regaly,
/* a3p5 */ punkt_piwnica_warsztat,
/* a3p6 */ punkt_piwnica_sala_srodek,
/* a3p7 */ punkt_piwnica_sala_boki,
/* a1p0 */ punkt_piwnica_sala_komin = 1 << SHIFT_PORT,
/* a1p1 */ punkt_piwnica_sala_kanapa,
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

#define WHEN_PRESSED_SHORT(X) (ShortPress << SHIFT_TRIGGER | przycisk_##X)
#define WHEN_PRESSED_MEDIUM(X) (MediumPress << SHIFT_TRIGGER | przycisk_##X)
#define WHEN_PRESSED_LONG(X) (LongPress << SHIFT_TRIGGER | przycisk_##X)
#define TOGGLE(X) (Toggle << SHIFT_ACTION | punkt_##X)
#define TIMER_RESET(X) (TimerReset << SHIFT_ACTION | punkt_##X)
#define ALL_OFF(x) (AllOff << SHIFT_ACTION)

const PROGMEM unsigned char rules[] = {
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
 WHEN_PRESSED_MEDIUM(piwnica_sala_podwojny_prawy), TOGGLE(piwnica_sala_kanapa),
 WHEN_PRESSED_SHORT(piwnica_sala_podwojny_prawy), TOGGLE(piwnica_sala_boki),
 WHEN_PRESSED_SHORT(parter_naroznik_prawy), TOGGLE(parter_salon_podwieszany),
 WHEN_PRESSED_SHORT(piwnica_wejscie_srodkowy), TOGGLE(piwnica_schody),
 WHEN_PRESSED_SHORT(pietro_lazienka_prawy), TOGGLE(pietro_lazienka_kinkiet),
 WHEN_PRESSED_SHORT(pietro_schody_podwojny_lewy), TOGGLE(pietro_schody),
 WHEN_PRESSED_SHORT(parter_sypialnia), TOGGLE(parter_sypialnia_sufit),
 WHEN_PRESSED_SHORT(piwnica_sala_pojedynczy), TOGGLE(piwnica_sala_komin),
 WHEN_PRESSED_SHORT(parter_gabinet), TOGGLE(parter_gabinet_sufit),
 WHEN_PRESSED_MEDIUM(pietro_goscinny), TOGGLE(pietro_hol_kinkiet),
 WHEN_PRESSED_SHORT(pietro_goscinny), TOGGLE(pietro_goscinny_sufit),
 WHEN_PRESSED_SHORT(parter_schody_podwojny_lewy), TOGGLE(pietro_hol_sufit),
 WHEN_PRESSED_SHORT(piwnica_wejscie_prawy), TOGGLE(piwnica_spocznik),
 WHEN_PRESSED_SHORT(parter_lazienka), TOGGLE(parter_lazienka),
 WHEN_PRESSED_LONG(parter_przedsionek), ALL_OFF(AllOff),
 WHEN_PRESSED_MEDIUM(parter_przedsionek), TOGGLE(parter_hol_sufit),
 WHEN_PRESSED_SHORT(parter_przedsionek), TOGGLE(parter_przedsionek),
 WHEN_PRESSED_SHORT(parter_kuchnia_scianka_lewy), TOGGLE(parter_jadalnia),
 WHEN_PRESSED_SHORT(piwnica_kotlownia_lewy), TOGGLE(piwnica_kotlownia_kociol),
 WHEN_PRESSED_SHORT(parter_kuchnia_okap_lewy), TOGGLE(parter_jadalnia),
 WHEN_PRESSED_SHORT(parter_kuchnia_okap_prawy), TOGGLE(parter_kuchnia),
 WHEN_PRESSED_MEDIUM(pietro_pokoj_asi), TOGGLE(pietro_hol_kinkiet),
 WHEN_PRESSED_SHORT(pietro_pokoj_asi), TOGGLE(pietro_pokoj_asi_sufit),
 WHEN_PRESSED_SHORT(pietro_garderoba_prawa), TOGGLE(pietro_garderoba_prawa_sufit),
 WHEN_PRESSED_SHORT(piwnica_warsztat), TOGGLE(piwnica_warsztat),
 WHEN_PRESSED_SHORT(parter_kuchnia_scianka_prawy), TOGGLE(parter_kuchnia),
 WHEN_PRESSED_SHORT(pietro_garderoba_lewa), TOGGLE(pietro_garderoba_lewa_sufit),
 WHEN_PRESSED_SHORT(pietro_lazienka_lewy), TOGGLE(pietro_lazienka_sufit),
 WHEN_PRESSED_SHORT(pietro_schody_podwojny_prawy), TOGGLE(pietro_hol_kinkiet),
 WHEN_PRESSED_SHORT(pietro_schody_pojedynczy), TOGGLE(pietro_hol_sufit),
 WHEN_PRESSED_SHORT(piwnica_sala_podwojny_lewy), TOGGLE(piwnica_sala_srodek),
 WHEN_PRESSED_SHORT(piwnica_kotlownia_prawy), TOGGLE(piwnica_kotlownia_regaly),
 WHEN_PRESSED_SHORT(parter_pomaranczowy), TOGGLE(parter_salon_sufit),

};

const int rulesSize = 90;

#endif // _RULES_H_