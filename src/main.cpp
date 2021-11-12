#include <iostream>
#include "DataLoader.h"

int main() {
    auto mkp_instances = DataLoader::load_mknap2("../data/mknap2.txt");
    return 0;
}