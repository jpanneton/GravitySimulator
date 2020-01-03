#include <chrono>
#include <type_traits>
#include <utility>

namespace Time
{
    inline auto clockNow()
    {
        return std::chrono::high_resolution_clock::now();
    }

    template<class DurationType, class F, class ... Args>
    auto measureExecutionTime(F f, Args&&... args)
    {
        if constexpr (std::is_void_v<std::invoke_result_t<F, Args...>>)
        {
            const auto start = clockNow();
            f(std::forward<Args>(args)...);
            const auto end = clockNow();
            return std::chrono::duration_cast<DurationType>(end - start).count();
        }
        else
        {
            const auto start = clockNow();
            const auto result = f(std::forward<Args>(args)...);
            const auto end = clockNow();
            return std::make_pair(result, std::chrono::duration_cast<DurationType>(end - avant).count());
        }
    }
}