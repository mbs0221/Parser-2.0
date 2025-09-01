#include "interpreter/type_registry.h"
int main() { auto r = TypeRegistry::getInstance(); auto t = r->getType("string"); return t ? 0 : 1; }
