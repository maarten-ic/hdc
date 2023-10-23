#include "hdc_utils.h"

#include <boost/tokenizer.hpp>
#include <boost/variant.hpp>
#include <tuple>
#include <typeinfo>

#include "hdc_helpers.h"

extern HDCGlobal hdc_global;

/* ------------------------- UUID generation ----------------------------- */

uuid_str_t generate_uuid_str()
{
    boost::uuids::basic_random_generator<boost::mt19937> gen(&(hdc_global.ran));
    boost::uuids::uuid u = gen();
    return boost::lexical_cast<uuid_str_t>(u);
}

boost::uuids::uuid generate_uuid()
{
    boost::uuids::basic_random_generator<boost::mt19937> gen(&(hdc_global.ran));
    boost::uuids::uuid u = gen();
    return u;
}

std::string uuid_to_str(char* uuid)
{
    boost::uuids::uuid tmp;
    memcpy(&tmp, uuid, HDC_UUID_LENGTH);
    return boost::uuids::to_string(tmp);
}


/* -------------------------  String manipulation ----------------------------- */

hdc_path_t split(const std::string& s)
{
    hdc_path_t parts;
    typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
    boost::char_separator<char> sep("/]", "[");
    tokenizer tok{ s, sep };
    bool is_index = false;
    bool was_bracket = false;
    for (const auto& t : tok) {
        was_bracket = (t.operator[](0) == '[');
        if (was_bracket) {
            is_index = true;
            continue;
        }
        boost::variant<size_t, std::string> var;
        if (is_index) { var = boost::lexical_cast<size_t>(t); }
        else { var = t; }
        parts.push_back(var);
        if (is_index) is_index = false;
    }
    return parts;
}

hdc_path_t split_no_brackets(const std::string& s)
{
    hdc_path_t parts;
    typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
    boost::char_separator<char> sep("/", "");
    tokenizer tok{ s, sep };
    boost::variant<size_t, std::string> var;
    for (const auto& t : tok) {
        try {
            var = boost::lexical_cast<size_t>(t);
        } catch (boost::bad_lexical_cast& e) {
            var = t;
        }
        parts.push_back(var);
    }
    return parts;
}

/* -------------------------  Types Definitions  ------------------------- */


size_t hdc_sizeof(hdc_type_t type)
{
    switch (type) {
        case HDC_EMPTY:
        case HDC_LIST:
        case HDC_STRUCT:
        case HDC_ERROR:
            return 0;
        case HDC_STRING:
            return sizeof(char);
        case HDC_UINT8:
        case HDC_INT8:
            return sizeof(int8_t);
        case HDC_UINT16:
        case HDC_INT16:
            return sizeof(int16_t);
        case HDC_UINT32:
        case HDC_INT32:
            return sizeof(int32_t);
        case HDC_UINT64:
        case HDC_INT64:
            return sizeof(int64_t);
        case HDC_FLOAT:
            return sizeof(float);
        case HDC_DOUBLE:
            return sizeof(double);
        case HDC_BOOL:
            return sizeof(bool);
        case HDC_COMPLEX:
            return sizeof(std::complex<double>);
        default:
            throw HDCException("hdc_sizeof(): Wrong type " + std::to_string(type) + "\n");
    }
}


bool hdc_is_numeric (hdc_type_t type)
{
    switch (type) {
        case HDC_EMPTY:
        case HDC_LIST:
        case HDC_STRUCT:
        case HDC_ERROR:
        case HDC_STRING:
            return false;
        case HDC_UINT8:
        case HDC_INT8:
        case HDC_UINT16:
        case HDC_INT16:
        case HDC_UINT32:
        case HDC_INT32:
        case HDC_UINT64:
        case HDC_INT64:
        case HDC_FLOAT:
        case HDC_DOUBLE:
        case HDC_BOOL:
        case HDC_COMPLEX:
            return true;
        default:
            throw HDCException("Unknown type " + std::to_string(type));
    }
}


std::string hdc_type_str(hdc_type_t type)
{
    switch (type) {
        case HDC_EMPTY: return "HDC_EMPTY";
        case HDC_LIST: return "HDC_LIST";
        case HDC_STRUCT: return "HDC_STRUCT";
        case HDC_ERROR: return "HDC_ERROR";
        case HDC_STRING: return "HDC_STRING";
        case HDC_UINT8: return "HDC_UINT8";
        case HDC_INT8: return "HDC_INT8";
        case HDC_UINT16: return "HDC_UINT16";
        case HDC_INT16: return "HDC_INT16";
        case HDC_UINT32: return "HDC_UINT32";
        case HDC_INT32: return "HDC_INT32";
        case HDC_UINT64: return "HDC_UINT64";
        case HDC_INT64: return "HDC_INT64";
        case HDC_FLOAT: return "HDC_FLOAT";
        case HDC_DOUBLE: return "HDC_DOUBLE";
        case HDC_BOOL: return "HDC_BOOL";
        case HDC_COMPLEX: return "HDC_COMPLEX";
        default:
            throw HDCException("Unknown type " + std::to_string(type));
    }
}

template <typename T>
hdc_type_t to_typeid(T a)
{
    throw HDCException("hdc_sizeof(): Wrong type " + std::to_string(a) + "\n");
}

hdc_type_t to_typeid(double a UNUSED)
{ return HDC_DOUBLE; }

hdc_type_t to_typeid(float a UNUSED)
{ return HDC_FLOAT; }

hdc_type_t to_typeid(int64_t a UNUSED)
{ return HDC_INT64; }

hdc_type_t to_typeid(int32_t a UNUSED)
{ return HDC_INT32; }

hdc_type_t to_typeid(int16_t a UNUSED)
{ return HDC_INT16; }

hdc_type_t to_typeid(int8_t a UNUSED)
{ return HDC_INT8; }

hdc_type_t to_typeid(char a UNUSED)
{ return HDC_INT8; }

hdc_type_t to_typeid(uint64_t a UNUSED)
{ return HDC_UINT64; }

hdc_type_t to_typeid(uint32_t a UNUSED)
{ return HDC_UINT32; }

hdc_type_t to_typeid(uint16_t a UNUSED)
{ return HDC_UINT16; }

hdc_type_t to_typeid(uint8_t a UNUSED)
{ return HDC_UINT8; }

hdc_type_t to_typeid(std::string a UNUSED)
{ return HDC_STRING; }

hdc_type_t to_typeid(char* a UNUSED)
{ return HDC_STRING; }

hdc_type_t to_typeid(char const* a UNUSED)
{ return HDC_STRING; }

hdc_type_t to_typeid(bool a UNUSED)
{ return HDC_BOOL; }

hdc_type_t to_typeid(std::complex<double> a UNUSED)
{ return HDC_COMPLEX; }

#ifndef __APPLE__
hdc_type_t to_typeid(std::_Bit_reference a UNUSED) {return HDC_BOOL;}
#else
hdc_type_t to_typeid(unsigned long a UNUSED) { return HDC_UINT64; }
hdc_type_t to_typeid(long a UNUSED) { return HDC_INT64; }
#endif

hdc_type_t to_typeid(const std::type_info& t)
{
    if (t == typeid(std::string) || t == typeid(char) || t == typeid(char*)) {
        return HDC_STRING;
    } else if (t == typeid(double)) {
        return HDC_DOUBLE;
    } else if (t == typeid(int) || t == typeid(short)) {
        return HDC_INT64;
    } else {
        return HDC_ERROR;
    }
}

hdc_type_t uda_str_to_typeid(std::string& str)
{
    if (str == "STRING") {
        return HDC_STRING;
    } else if (str == "STRING *") {
        std::cerr << "Warning: STRING * type is not supported!\n";
        return HDC_LIST;
    } else if (str == "int") {
        return HDC_INT64;
    } else if (str == "short") {
        return HDC_INT64;
    } else if (str == "double") {
        return HDC_DOUBLE;
    }
    return HDC_ERROR;
}

hdc_type_t decode_numpy_type(char kind, int8_t itemsize)
{
    hdc_type_t type;
    switch (kind) {
        case 'i':
            switch (itemsize) {
                case 1:
                    type = HDC_INT8;
                    break;
                case 2:
                    type = HDC_INT16;
                    break;
                case 4:
                    type = HDC_INT32;
                    break;
                case 8:
                    type = HDC_INT64;
                    break;
                default:
                    throw HDCException("set_data_Py: bad itemsize");
            };
            break;
        case 'u':
            switch (itemsize) {
                case 1:
                    type = HDC_UINT8;
                    break;
                case 2:
                    type = HDC_UINT16;
                    break;
                case 4:
                    type = HDC_UINT32;
                    break;
                case 8:
                    type = HDC_UINT64;
                    break;
                default:
                    throw HDCException("set_data_Py: bad itemsize");
            };
            break;
        case 'f':
            switch (itemsize) {
                case 4:
                    type = HDC_FLOAT;
                    break;
                case 8:
                    type = HDC_DOUBLE;
                    break;
                default:
                    throw HDCException("set_data_Py: bad itemsize");
            };
            break;
        case 'b':
            type = HDC_BOOL;
            break;
        case 'c':
            switch (itemsize) {
                case 16:
                    type = HDC_COMPLEX;
                    break;
                default:
                    throw HDCException("set_data_Py: bad itemsize");
            }
            break;
        default:
            throw HDCException("set_data_Py: bad kind");
    }
    return type;
}


/* -------------------------  Buffer Manipulation  ------------------------- */

template <typename T>
void transpose_buf(char* new_buffer, char* buffer, int8_t rank, std::vector<size_t> shape, bool fortranOrder)
{
    std::vector<size_t> new_shape(rank);
    for (int i = 0; i < rank; i++) {
        new_shape[i] = shape[rank - i - 1];
    }
    andres::CoordinateOrder order;
    andres::CoordinateOrder new_order;
    if (fortranOrder) {
        order = andres::LastMajorOrder;
        new_order = andres::FirstMajorOrder;
    } else {
        order = andres::FirstMajorOrder;
        new_order = andres::LastMajorOrder;
    }
    andres::View<T> view(&shape[0], &shape[0] + rank, reinterpret_cast<T*>(buffer), order);
    andres::View<T> new_view(&new_shape[0], &new_shape[0] + rank, reinterpret_cast<T*>(new_buffer), new_order);
    if (rank == 1) {
        for (size_t i0 = 0; i0 < shape[0]; i0++)
            new_view(i0) = view(i0);
    } else if (rank == 2) {
        for (size_t i0 = 0; i0 < shape[0]; i0++)
            for (size_t i1 = 0; i1 < shape[1]; i1++)
                new_view(i1, i0) = view(i0, i1);
    } else if (rank == 3) {
        for (size_t i0 = 0; i0 < shape[0]; i0++)
            for (size_t i1 = 0; i1 < shape[1]; i1++)
                for (size_t i2 = 0; i2 < shape[2]; i2++)
                    new_view(i2, i1, i0) = view(i0, i1, i2);
    } else if (rank == 4) {
        for (size_t i0 = 0; i0 < shape[0]; i0++)
            for (size_t i1 = 0; i1 < shape[1]; i1++)
                for (size_t i2 = 0; i2 < shape[2]; i2++)
                    for (size_t i3 = 0; i3 < shape[3]; i3++)
                        new_view(i3, i2, i1, i0) = view(i0, i1, i2, i3);
    } else if (rank == 5) {
        for (size_t i0 = 0; i0 < shape[0]; i0++)
            for (size_t i1 = 0; i1 < shape[1]; i1++)
                for (size_t i2 = 0; i2 < shape[2]; i2++)
                    for (size_t i3 = 0; i3 < shape[3]; i3++)
                        for (size_t i4 = 0; i4 < shape[4]; i4++)
                            new_view(i4, i3, i2, i1, i0) = view(i0, i1, i2, i3, i4);
    } else {
        throw std::runtime_error("transpose buffer: unsupported dimension.");
    }
}

void transpose_buffer(char* new_buffer, char* buffer, int8_t rank, std::vector<size_t> shape, hdc_type_t type_,
                      bool fortranOrder)
{
    switch (type_) {
        case (HDC_INT8): {
            transpose_buf<int8_t>(new_buffer, buffer, rank, shape, fortranOrder);
            break;
        }
        case (HDC_INT16): {
            transpose_buf<int16_t>(new_buffer, buffer, rank, shape, fortranOrder);
            break;
        }
        case (HDC_INT32): {
            transpose_buf<int32_t>(new_buffer, buffer, rank, shape, fortranOrder);
            break;
        }
        case (HDC_INT64): {
            transpose_buf<int64_t>(new_buffer, buffer, rank, shape, fortranOrder);
            break;
        }
        case (HDC_UINT8): {
            transpose_buf<uint8_t>(new_buffer, buffer, rank, shape, fortranOrder);
            break;
        }
        case (HDC_UINT16): {
            transpose_buf<uint16_t>(new_buffer, buffer, rank, shape, fortranOrder);
            break;
        }
        case (HDC_UINT32): {
            transpose_buf<uint32_t>(new_buffer, buffer, rank, shape, fortranOrder);
            break;
        }
        case (HDC_UINT64): {
            transpose_buf<uint64_t>(new_buffer, buffer, rank, shape, fortranOrder);
            break;
        }
        case (HDC_FLOAT): {
            transpose_buf<float>(new_buffer, buffer, rank, shape, fortranOrder);
            break;
        }
        case (HDC_DOUBLE): {
            transpose_buf<double>(new_buffer, buffer, rank, shape, fortranOrder);
            break;
        }
        case (HDC_BOOL): {
            transpose_buf<bool>(new_buffer, buffer, rank, shape, fortranOrder);
            break;
        }
        case (HDC_COMPLEX): {
            transpose_buf<std::complex<double>>(new_buffer, buffer, rank, shape, fortranOrder);
            break;
        }
        case (HDC_STRUCT):
        case (HDC_LIST):
        case (HDC_EMPTY):
        case (HDC_STRING):
            throw HDCException(
                    "transpose_buffer(): hdc_type_t = " + std::to_string((size_t)type_) + " cannot be transposed.");
        default: {
            throw HDCException(
                    "transpose_buffer(): hdc_type_t = " + std::to_string((size_t)type_) + " not supported yet.");
        }
    }
}

/**
 * Check if a file exists
 * @return true if and only if the file exists, false else
 */
bool fileExists(const std::string& file)
{
    struct stat buf;
    return (stat(file.c_str(), &buf) == 0);
}
