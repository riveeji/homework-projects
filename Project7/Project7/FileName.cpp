#include "БъЭЗ.h"

int positionIdentifier(int x1, int y1, int x2, int y2, int x, int y) {
    return (x2 - x1) * (y - y1) - (x - x1) * (y2 - y1);
}
