#include "resource.hpp"

namespace cbdc {
    auto get_cpu_usage( /* param */) -> std::optional<cpus> {
        auto file_name = "/proc/stat";
        char* maybe_line = NULL;
        size_t len = 0;

        const auto processor_count = std::thread::hardware_concurrency();
        if ( 0 == processor_count ) {
            return std::nullopt;
        }

        auto maybe_cpu_cores = sysconf(_SC_NPROCESSORS_ONLN);
        auto sz_num_cpu_cores = static_cast<size_t>(maybe_cpu_cores) + 1;

        FILE *p_stat = NULL;
        p_stat = fopen(file_name, "r");

        cpus data = cpus();

        for (size_t i{0}; i < sz_num_cpu_cores; i++) {
            [[maybe_unused]] size_t read = getline(&maybe_line, &len, p_stat);
            cpu current = cpu();
            int got = sscanf(maybe_line, "%s %d %d %d %d", 
                                            current.id, 
                                            &current.maybe_user,
                                            &current.maybe_nice,
                                            &current.maybe_system,
                                            &current.maybe_idle
            );

            if ( 0 == got ) continue;

            current.total = current.maybe_user + current.maybe_nice + current.maybe_system + current.maybe_idle;

            data.push_back(current);
        }

        fclose(p_stat);

        return data;
    }

    auto get_mem_usage() -> std::optional<memory> {
        struct sysinfo mem_info;

        const auto processor_count = std::thread::hardware_concurrency();
        if ( 0 == processor_count ) {
            return std::nullopt;
        }

        sysinfo(&mem_info);

        auto ret = memory();
        ret.totalram = mem_info.totalram;
        ret.freeram = mem_info.freeram;
        ret.mem_unit = mem_info.mem_unit;

        return ret;
    }
}