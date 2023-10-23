#ifndef HDC_UTILS_H
#define HDC_UTILS_H
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <tuple>
#include <complex>
// add uuid generation
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/variant.hpp>
#include "hdc_types.h"
#include "hdc_errors.hpp"
#include "hdc_global.hpp"
#include <andres/marray.hxx>
#include <sys/stat.h>

#define PRINT_INFO(buffer)                                                          \
    auto header = reinterpret_cast<hdc_header_t*>(buffer);                          \
    printf("Size:\t\t%zu\n", header->buffer_size);                                  \
    printf("NDim:\t\t%zu\n", header->rank);                                         \
    printf("Shape:\t\t");                                                           \
    for (size_t i = 0; i < HDC_MAX_DIMS; i++) printf("%zu,", header->shape[i]);     \
    printf("\n");                                                                   \
    printf("Data Size:\t\t%zu\n", header->data_size);                               \
    printf("External:\t\t%d\n", is_external());                                     \
    printf("ReadOnly:\t\t%d\n", is_readonly());                                     \
    printf("FortranOrder:\t%d\n", is_fortranorder());                               \
    std::cout << "Type:\t" << header->type << "\n";                                 \

using hdc_index_t = boost::variant<size_t,std::string>;
using hdc_path_t = std::list<hdc_index_t>;
using uuid_str_t = std::string;

/* ------------------------- UUID generation ----------------------------- */

std::string uuid_to_str(char* uuid);

boost::uuids::uuid generate_uuid();

uuid_str_t generate_uuid_str();
/* -------------------------  String manipulation ----------------------------- */
hdc_path_t split(const std::string& s);
hdc_path_t split_no_brackets(const std::string& s);
/* -------------------------  Types Definitions  ------------------------- */
size_t hdc_sizeof (hdc_type_t type);
bool hdc_is_primitive_type(hdc_type_t type);
std::string hdc_type_str(hdc_type_t type);

template <typename T>
hdc_type_t to_typeid(T a);
hdc_type_t to_typeid(double a);
hdc_type_t to_typeid(float a);
hdc_type_t to_typeid(int64_t a);
hdc_type_t to_typeid(int32_t a);
hdc_type_t to_typeid(int16_t a);
hdc_type_t to_typeid(int8_t a);
hdc_type_t to_typeid(uint64_t a);
hdc_type_t to_typeid(uint32_t a);
hdc_type_t to_typeid(uint16_t a);
hdc_type_t to_typeid(uint8_t a);
hdc_type_t to_typeid(char a);
hdc_type_t to_typeid(std::string a);
hdc_type_t to_typeid(char* a);
hdc_type_t to_typeid(bool a);
hdc_type_t to_typeid(std::complex<double> a);
#ifndef __APPLE__
hdc_type_t to_typeid(std::_Bit_reference a); //workaround for bool
#else
hdc_type_t to_typeid(long a);
hdc_type_t to_typeid(unsigned long a);
#endif
hdc_type_t numpy_format_to_typeid(std::string format, size_t itemsize);
hdc_type_t to_typeid(const std::type_info& t);
hdc_type_t uda_str_to_typeid(std::string& str);
bool hdc_is_numeric (hdc_type_t);
hdc_type_t decode_numpy_type(char kind, int8_t itemsize) ;

/* -------------------------  Other stuff ----------------------------- */

void hello__();

/* -------------------------  Buffer Manipulation  ------------------------- */

void transpose_buffer(char* new_buffer, char* buffer, int8_t rank, std::vector<size_t> shape, hdc_type_t type_, bool fortranOrder = false);

bool fileExists(const std::string& file);



#endif
