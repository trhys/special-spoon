#pragma once

#include <string>
#include <variant>
#include <unordered_map>

namespace Spoon
{
    struct Metric
    {
        std::variant<int, float, std::string> value = 0.0f;

        // Returns the value of the metric as the specified type.
        // Throws std::bad_variant_access if the type does not match.
        template<typename T>
        T Value() const
        {
            return std::get<T>(value);
        }
    };

    class Metrics
    {
    public:
        Metrics() = default;
        ~Metrics() = default;

        using T = std::variant<int, float, std::string>;
        // Monitors the specified metric by storing it with the given name.
        void Monitor(const std::string& name, const T& value)
        {
            metrics[name] = Metric{value};
        }

        // Retrieves the metric associated with the given name.
        // Throws std::out_of_range if the metric does not exist.
        Metric Get(const std::string& name)
        {
            return metrics[name];
        }
    private:
        std::unordered_map<std::string, Metric> metrics;
    };
}