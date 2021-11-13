#ifndef NIOA_PSO_DATALOADER_H
#define NIOA_PSO_DATALOADER_H

#include <string>
#include "mkp.h"

class DataLoader {
public:
    static std::vector<MKPInstance> load_mknap2(const std::string &path, const std::vector<int> &instance_ids = {});
};


#endif //NIOA_PSO_DATALOADER_H
