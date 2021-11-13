#include <iostream>
#include "DataLoader.h"
#include "BenchmarkServices.h"

int main() {
//    auto mkp_instances = DataLoader::load_mknap2("../data/mknap2.txt");
    BenchmarkServices::run();
    return 0;
}