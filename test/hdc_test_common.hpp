#include "catch.hpp"
#include "hdc.hpp"
#include <boost/filesystem.hpp>

size_t factorial(size_t n)
{
    return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

bool in_vector(const std::string& str, const std::vector<std::string>& vector)
{
    return (std::find(vector.begin(), vector.end(), str) != vector.end());
}

bool in_map(const std::string& str, const std::unordered_map<std::string, std::string>& map)
{
    return (map.find(str) != map.end());
}

std::string make_tmp_name(const std::string& suffix = "h5")
{
    boost::filesystem::path temp = boost::filesystem::unique_path();
    const std::string tempstr = temp.native()+"."+suffix;
    return tempstr;
}

#ifndef HDC_TEST_COMMON_HPP
#define HDC_TEST_COMMON_HPP

template <typename T, typename U>
bool arrcmp(T* arr1, U* arr2, size_t n_elem)
{
    for (size_t i = 0; i < n_elem; i++) {
        if (arr1[i] != arr2[i]) return false;
    }
    return true;
}

namespace std {

bool operator==(int8_t a, std::complex<double> b) { return std::complex<double>(a) == b; }
bool operator==(std::complex<double> b, int8_t a) { return std::complex<double>(a) == b; }
bool operator==(uint8_t a, std::complex<double> b) { return std::complex<double>(a) == b; }
bool operator==(std::complex<double> b, uint8_t a) { return std::complex<double>(a) == b; }
bool operator==(int16_t a, std::complex<double> b) { return std::complex<double>(a) == b; }
bool operator==(std::complex<double> b, int16_t a) { return std::complex<double>(a) == b; }
bool operator==(uint16_t a, std::complex<double> b) { return std::complex<double>(a) == b; }
bool operator==(std::complex<double> b, uint16_t a) { return std::complex<double>(a) == b; }
bool operator==(int32_t a, std::complex<double> b) { return std::complex<double>(a) == b; }
bool operator==(std::complex<double> b, int32_t a) { return std::complex<double>(a) == b; }
bool operator==(uint32_t a, std::complex<double> b) { return std::complex<double>(a) == b; }
bool operator==(std::complex<double> b, uint32_t a) { return std::complex<double>(a) == b; }
bool operator==(int64_t a, std::complex<double> b) { return std::complex<double>(a) == b; }
bool operator==(std::complex<double> b, int64_t a) { return std::complex<double>(a) == b; }
bool operator==(uint64_t a, std::complex<double> b) { return std::complex<double>(a) == b; }
bool operator==(std::complex<double> b, uint64_t a) { return std::complex<double>(a) == b; }
bool operator==(float a, std::complex<double> b) { return std::complex<double>(a) == b; }
bool operator==(std::complex<double> b, float a) { return std::complex<double>(a) == b; }

}  // namespace std

#define ALL_NUMERIC_TYPES int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::complex<double>

#define PREPARE_TREE()                                                                              \
    std::vector<size_t> shape = {4};                                                                \
    double data_double[] = {0.0,1000.0,1.0e-200,1.0e200};                                           \
    int8_t data_int8[] = {-7,2,3,4};                                                                \
    int16_t data_int16[] = {-7000,2000,3000,4000};                                                  \
    int32_t data_int32[] = {-70000000,10000000,20000000,300000000};                                 \
    int64_t data_int64[] = {-70000000,10000000,20000000,300000000};                                 \
    uint8_t data_uint8[] = {7,2,3,4};                                                               \
    uint16_t data_uint16[] = {7000,2000,3000,4000};                                                 \
    uint32_t data_uint32[] = {70000000,10000000,20000000,300000000};                                \
    uint64_t data_uint64[] = {70000000,10000000,20000000,300000000};                                \
    float data_float[] = {0.0,1000.0,1.0e-20,1.0e20};                                               \
    bool data_bool[] = {false,true,false,true};                                                     \
    std::complex<double> data_complex[] = {{1,0}, {0, 1}, {1.141, -1.141}, {3.141, 2.718}};         \
    HDC tree;                                                                                       \
    HDC scalar;                                                                                     \
    scalar.set_data(333.333);                                                                       \
    tree.add_child("aaa/bbb/_scalar", scalar);                                                      \
    tree["aaa/bbb/double"].set_data<double>(shape,data_double);                                     \
    tree["aaa/bbb/float"].set_data<float>(shape,data_float);                                        \
    tree["aaa/bbb/double2"].set_data<double>(shape,data_double);                                    \
    tree["aaa/bbb/int8"].set_data<int8_t>(shape,data_int8);                                         \
    tree["aaa/bbb/int16"].set_data<int16_t>(shape,data_int16);                                      \
    tree["aaa/bbb/int32"].set_data<int32_t>(shape,data_int32);                                      \
    tree["aaa/bbb/int64"].set_data<int64_t>(shape,data_int64);                                      \
    tree["aaa/bbb/uint8"].set_data<uint8_t>(shape,data_uint8);                                      \
    tree["aaa/bbb/uint16"].set_data<uint16_t>(shape,data_uint16);                                   \
    tree["aaa/bbb/uint32"].set_data<uint32_t>(shape,data_uint32);                                   \
    tree["aaa/bbb/uint64"].set_data<uint64_t>(shape,data_uint64);                                   \
    tree["aaa/bbb/bool"].set_data<bool>(shape,data_bool);                                           \
    tree["aaa/bbb/complex"].set_data<std::complex<double>>(shape,data_complex);                     \
    HDC ch;                                                                                         \
    tree.add_child("aaa/bbb/empty", ch);                                                            \
    HDC list;                                                                                       \
    HDC lch;                                                                                        \
    for (int i = 0; i < 5; i++) {                                                                   \
        HDC lch;                                                                                    \
        if (i % 2 == 0) { lch.set_data(i); } else { lch.set_string(std::to_string(i)); }            \
        list.append(lch);                                                                           \
    }                                                                                               \
    tree.add_child("aaa/list", list);                                                               \
    HDC str("Lorem ipsum dolor sit amet, consectetuer adipiscing elit.");                           \
    tree.add_child("aaa/string", str);                                                              \


#endif //HDC_TEST_COMMON_HPP
