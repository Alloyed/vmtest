#define UNUSED(x) (void)x
template<typename T, size_t N>
constexpr size_t ARRAYSIZE(T (&)[N]) {
    return N;
}
