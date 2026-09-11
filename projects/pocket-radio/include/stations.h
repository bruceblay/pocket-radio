#pragma once
#include <cstddef>
#include <cstdint>
// Saved selection bits follow this order; append new presets or migrate saved masks when reordering.
// Curated live stations, including optional SomaFM channels.
// Connect directly to each broadcaster.
struct Station { const char* name; const char* mood; const char* url; const char* description; };
constexpr Station stations[] = {
  {"Tiki Time", "EXOTICA / SURF / ISLANDS", "https://ice2.somafm.com/tikitime-128-mp3", "Exotica, surf and island sounds"},
  {"Illinois St Lounge", "VINTAGE LOUNGE / EXOTICA", "https://ice2.somafm.com/illstreet-128-mp3", "Vintage lounge and exotica"},
  {"KEXP", "SEATTLE / MUSIC DISCOVERY", "https://kexp-mp3-128.streamguys1.com/kexp128.mp3", "Music discovery from Seattle"},
  {"NTS 1", "GLOBAL / ADVENTUROUS DJ SETS", "https://stream-relay-geo.ntslive.net/stream", "Adventurous DJ sets from around the world"},
  {"KCRW Eclectic 24", "ECLECTIC / MUSIC ALL DAY", "https://streams.kcrw.com/e24_mp3", "An eclectic mix, all day"},
  {"Radio Paradise", "ECLECTIC / HAND-PICKED MIX", "https://stream.radioparadise.com/mp3-192", "Hand-picked music across genres"},
  {"Bossa Beyond", "BOSSA NOVA / SAMBA", "https://ice2.somafm.com/bossa-128-mp3", "Bossa nova and samba"},
  {"Secret Agent", "CINEMATIC / LOUNGE / SPIES", "https://ice2.somafm.com/secretagent-128-mp3", "Cinematic sounds and lounge"},
  {"Suburbs of Goa", "DESI / GLOBAL BEATS", "https://ice2.somafm.com/suburbsofgoa-128-mp3", "Desi sounds and global beats"},
  {"Heavyweight Reggae", "REGGAE / SKA / ROCKSTEADY", "https://ice2.somafm.com/reggae-128-mp3", "Reggae, ska and rocksteady"},
};
constexpr size_t stationCount = sizeof(stations) / sizeof(stations[0]);
static_assert(stationCount > 0, "Add at least one station");

constexpr size_t defaultStationCount = 6;
constexpr uint32_t defaultStationMask = (uint32_t(1) << defaultStationCount) - 1;
