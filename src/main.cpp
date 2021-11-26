#include <iostream>
#include "DataLoader.h"
#include "BenchmarkServices.h"

int main() {
//    auto mkp_instances_all = DataLoader::load_mknap2("../data/mknap2.txt");
//    auto mkp_instances_selected = DataLoader::load_mknap2("../data/mknap2.txt", {0, 1, 2, 46, 47});
    BenchmarkServices::run();
    return 0;
}