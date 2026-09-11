#include <cassert>
#include "StationSelection.h"
#include "stations.h"
int main() {
  StationSelection expanded(stationCount, defaultStationMask);
  assert(stationCount == 10 && expanded.count() == 6);
  for (size_t i = 0; i < stationCount; ++i) assert(expanded.includes(i) == (i < 6));
  assert(expanded.set(defaultStationMask | (1 << 9)));
  assert(expanded.next(5) == 9 && expanded.next(9) == 0);
  assert(expanded.set(1 << 2)); // An existing customized dial survives catalog expansion.
  assert(expanded.count() == 1 && expanded.first() == 2);
  StationSelection selection(6);
  assert(selection.count() == 6 && selection.next(5) == 0);
  assert(selection.set((1 << 0) | (1 << 3) | (1 << 5)));
  assert(selection.count() == 3 && selection.first() == 0);
  assert(selection.next(0) == 3 && selection.next(3) == 5 && selection.next(5) == 0);
  assert(!selection.includes(1) && !selection.includes(32));
  assert(!selection.set(0) && !selection.set(1 << 6));
  assert(selection.count() == 3); // Invalid submissions preserve the selection.
  assert(selection.set(1 << 4));
  assert(selection.first() == 4 && selection.next(4) == 4 && selection.count() == 1);
  StationSelection restored(6);
  assert(restored.set(selection.mask()) && restored.first() == 4);
}
