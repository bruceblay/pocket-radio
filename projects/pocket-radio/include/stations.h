#pragma once
#include <cstddef>
#include <cstdint>
// Saved selection bits follow this order; append new presets or migrate saved masks when reordering.
// Curated live stations, including optional SomaFM channels.
// Connect directly to each broadcaster.
struct Station { const char* name; const char* mood; const char* url; };
constexpr Station stations[] = {
  {"Tiki Time", "EXOTICA / SURF / ISLANDS", "https://ice2.somafm.com/tikitime-128-mp3"},
  {"Illinois St Lounge", "VINTAGE LOUNGE / EXOTICA", "https://ice2.somafm.com/illstreet-128-mp3"},
  {"KEXP", "SEATTLE / MUSIC DISCOVERY", "https://kexp-mp3-128.streamguys1.com/kexp128.mp3"},
  {"NTS 1", "GLOBAL / ADVENTUROUS DJ SETS", "https://stream-relay-geo.ntslive.net/stream"},
  {"KCRW Eclectic 24", "ECLECTIC / MUSIC ALL DAY", "https://streams.kcrw.com/e24_mp3"},
  {"Radio Paradise", "ECLECTIC / HAND-PICKED MIX", "https://stream.radioparadise.com/mp3-192"},
  {"Bossa Beyond", "BOSSA NOVA / SAMBA", "https://ice2.somafm.com/bossa-128-mp3"},
  {"Secret Agent", "CINEMATIC / LOUNGE / SPIES", "https://ice2.somafm.com/secretagent-128-mp3"},
  {"Suburbs of Goa", "DESI / GLOBAL BEATS", "https://ice2.somafm.com/suburbsofgoa-128-mp3"},
  {"Heavyweight Reggae", "REGGAE / SKA / ROCKSTEADY", "https://ice2.somafm.com/reggae-128-mp3"},
};
constexpr size_t stationCount = sizeof(stations) / sizeof(stations[0]);
static_assert(stationCount > 0, "Add at least one station");

constexpr size_t defaultStationCount = 6;
constexpr uint32_t defaultStationMask = (uint32_t(1) << defaultStationCount) - 1;
