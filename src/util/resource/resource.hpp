#ifndef OPENCBDC_TX_SRC_RESOURCE_H_
#define OPENCBDC_TX_SRC_RESOURCE_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/sysinfo.h>

#include <optional>
#include <thread>
#include <vector>

namespace cbdc {
    struct cpu {
        char id[6];
        int maybe_user;
        int maybe_nice;
        int maybe_system;
        int maybe_idle;
        int total;
    };

    struct memory {
        unsigned long totalram;
        unsigned long freeram;
        unsigned int mem_unit;
    };

    using cpus = std::vector<cpu>;
    
    auto get_cpu_usage() -> std::optional<cpus>;
    auto get_mem_usage() -> std::optional<memory>;
}

#endif