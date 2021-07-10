#include "ivec2.h"

void IVec2_add(IVec2 *this, IVec2 *other) {
    this->x += other->x;
    this->y += other->y;
}