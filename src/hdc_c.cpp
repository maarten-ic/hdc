#include "hdc_c.h"
#include "hdc.hpp"
#include "hdc_types.h"

extern "C" {
hdc_t hdc_new_empty()
{
    return HDC().as_obj();
}

hdc_t hdc_new_size(size_t size)
{
    return HDC(size).as_obj();
}

hdc_t hdc_new_string(const char* str)
{
    return HDC(str).as_obj();
}

hdc_t hdc_new_array(size_t rank, size_t* shape_in, hdc_type_t type)
{
    std::vector<size_t> shape(&shape_in[0],&shape_in[0]+rank);
    return HDC(shape,type).as_obj();
}

hdc_t hdc_new_array2(size_t rank, size_t* shape_in, hdc_type_t type, size_t flags)
{
    std::vector<size_t> shape(&shape_in[0],&shape_in[0]+rank);
    return HDC(shape,type,flags).as_obj();
}

void hdc_print_info(hdc_t tree UNUSED)
{
    HDC(tree).print_info();
    return;
}

void hdc_add_child(hdc_t tree, const char* path, hdc_t child)
{
    HDC node(child);
    HDC(tree).add_child(path, node);
    return;
}

void hdc_set_child(hdc_t tree, const char* path, hdc_t child)
{
    HDC node(child);
    HDC(tree).set_child(path, node);
    return;
}

void hdc_delete_child(hdc_t tree, const char* path)
{
    HDC(tree).delete_child(path);
    return;
}

hdc_t hdc_get(hdc_t tree, const char* path)
{
    return HDC(tree).get(path).as_obj();
}

hdc_t hdc_get_slice(hdc_t tree, const char* path, size_t i)
{
    return HDC(tree).get(path).get(i).as_obj();
}

void hdc_set_slice(hdc_t tree, size_t i, hdc_t child)
{
    HDC node(child);
    HDC(tree).set_child(i,node);
}

void hdc_insert_slice(hdc_t tree, size_t i, hdc_t child)
{
    HDC node(child);
    HDC(tree).insert(i,node);
}

void hdc_append_slice(hdc_t tree, hdc_t child)
{
    HDC node(child);
    HDC(tree).append(node);
}

bool hdc_exists(hdc_t tree, const char* path)
{
    return HDC(tree).exists(path);
}

bool hdc_is_external(hdc_t tree, const char* path)
{
    return HDC(tree).get(path).is_external();
}

bool hdc_is_scalar(hdc_t tree, const char* path)
{
    return HDC(tree).get(path).is_scalar();
}

void hdc_set_int8(hdc_t tree, const char* path, int rank, size_t* shape_in, void* data, hdc_flags_t _flags)
{
    std::vector<size_t> shape(shape_in, shape_in+rank);
    HDC(tree).get_or_create(path).set_data_c(shape, data, HDC_INT8, _flags);
    return;
}

void hdc_set_int16(hdc_t tree, const char* path, int rank, size_t* shape_in, void* data, hdc_flags_t _flags)
{
    std::vector<size_t> shape(shape_in, shape_in+rank);
    HDC(tree).get_or_create(path).set_data_c(shape, data, HDC_INT16, _flags);
    return;
}

void hdc_set_int32(hdc_t tree, const char* path, int rank, size_t* shape_in, void* data, hdc_flags_t _flags)
{
    std::vector<size_t> shape(shape_in, shape_in+rank);
    HDC(tree).get_or_create(path).set_data_c(shape, data, HDC_INT32, _flags);
    return;
}

void hdc_set_int64(hdc_t tree, const char* path, int rank, size_t* shape_in, void* data, hdc_flags_t _flags)
{
    std::vector<size_t> shape(shape_in, shape_in+rank);
    HDC(tree).get_or_create(path).set_data_c(shape, data, HDC_INT64, _flags);
    return;
}

size_t hdc_get_rank(hdc_t tree, const char* path)
{
    return HDC(tree).get(path).get_rank();
}

size_t hdc_get_type(hdc_t tree, const char* path)
{
    return HDC(tree).get(path).get_type();
}

void* hdc_as_voidptr(hdc_t tree, const char* path)
{
    return HDC(tree).get(path).as_void_ptr();
}

int8_t* hdc_as_int8_array(hdc_t tree, const char* path)
{
    return HDC(tree).get(path).as<int8_t>();
}

int16_t* hdc_as_int16_array(hdc_t tree, const char* path)
{
    return HDC(tree).get(path).as<int16_t>();
}

int32_t* hdc_as_int32_array(hdc_t tree, const char* path)
{
    return HDC(tree).get(path).as<int32_t>();
}

int64_t* hdc_as_int64_array(hdc_t tree, const char* path)
{
    return HDC(tree).get(path).as<int64_t>();
}


double* hdc_as_double_array(hdc_t tree, const char* path)
{
    return HDC(tree).get(path).as<double>();
}

double _Complex* hdc_as_complex_array(hdc_t tree, const char* path)
{
    auto value = HDC(tree).get(path).as<std::complex<double>>();
    return reinterpret_cast<double _Complex*>(value);
}


float* hdc_as_float_array(hdc_t tree, const char* path)
{
    return HDC(tree).get(path).as<float>();
}

void hdc_set_int8_scalar(hdc_t tree, const char* path, int8_t data)
{
    HDC(tree).get_or_create(path).set_data(data);
    return;
}

void hdc_set_int16_scalar(hdc_t tree, const char* path, int16_t data)
{
    HDC(tree).get_or_create(path).set_data(data);
    return;
}

void hdc_set_int32_scalar(hdc_t tree, const char* path, int32_t data)
{
    HDC(tree).get_or_create(path).set_data(data);
    return;
}

void hdc_set_int64_scalar(hdc_t tree, const char* path, int64_t data)
{
    HDC(tree).get_or_create(path).set_data(data);
    return;
}

void hdc_set_double(hdc_t tree, const char* path, int rank, size_t* shape_in, void* data, hdc_flags_t _flags)
{
    std::vector<size_t> shape(shape_in, shape_in+rank);
    HDC(tree).get_or_create(path).set_data_c(shape, data, HDC_DOUBLE, _flags);
    return;
}

void hdc_set_double_scalar(hdc_t tree, const char* path, double data)
{
    HDC(tree).get_or_create(path).set_data(data);
    return;
}

void hdc_set_complex(hdc_t tree, const char* path, int rank, size_t* shape_in, void* data, hdc_flags_t _flags)
{
    std::vector<size_t> shape(shape_in, shape_in+rank);
    HDC(tree).get_or_create(path).set_data_c(shape, data, HDC_COMPLEX, _flags);
    return;
}

void hdc_set_complex_scalar(hdc_t tree, const char* path, double _Complex data)
{
    HDC(tree).get_or_create(path).set_data(*reinterpret_cast<std::complex<double>*>(&data));
    return;
}


void hdc_set_float(hdc_t tree, const char* path, int rank, size_t* shape_in, void* data,
                        hdc_flags_t _flags UNUSED)
{
    std::vector<size_t> shape(shape_in, shape_in+rank);
    HDC(tree).get_or_create(path).set_data_c(shape, data, HDC_FLOAT);
    return;
}

void hdc_set_float_scalar(hdc_t tree, const char* path, float data)
{
    HDC(tree).get_or_create(path).set_data(data);
    return;
}

hdc_t hdc_copy(hdc_t src, bool deep_copy)
{
    HDC h(src);
    return HDC(h).copy(deep_copy).as_obj();
}

void hdc_clean(hdc_t node)
{
    return HDC(node).clean();
}

void hdc_set_string(hdc_t tree, const char* path, const char* str)
{
    HDC(tree).get_or_create(path).set_string(str);
    return;
}

const char* hdc_as_string(hdc_t tree, const char* path)
{
    auto data = HDC(tree).get(path).get_data();
    return reinterpret_cast<const char *>(data.data);
}

double hdc_as_double_scalar(hdc_t tree, const char* path)
{
    return (HDC(tree).get(path).as<double>())[0];
}

double _Complex hdc_as_complex_scalar(hdc_t tree, const char* path)
{
    auto value = HDC(tree).get(path).as<std::complex<double>>();
    return reinterpret_cast<double _Complex*>(value)[0];
}

float hdc_as_float_scalar(hdc_t tree, const char* path)
{
    return (HDC(tree).get(path).as<float>())[0];
}

int8_t hdc_as_int8_scalar(hdc_t tree, const char* path)
{
    return (HDC(tree).get(path).as<int8_t>())[0];
}

int16_t hdc_as_int16_scalar(hdc_t tree, const char* path)
{
    return (HDC(tree).get(path).as<int16_t>())[0];
}

int32_t hdc_as_int32_scalar(hdc_t tree, const char* path)
{
    return (HDC(tree).get(path).as<int32_t>())[0];
}

int64_t hdc_as_int64_scalar(hdc_t tree, const char* path)
{
    return (HDC(tree).get(path).as<int64_t>())[0];
}


const char* hdc_get_type_str(hdc_t tree, const char* path)
{
    return (HDC(tree).get(path).get_type_str());
}

//void hdc_to_json(hdc_t tree, const char* filename, int mode)
//{
//    HDC(tree).to_json(filename, mode);
//}

void hdc_dump(hdc_t tree)
{
    HDC(tree).dump();
}

void hdc_keys(hdc_t tree, char** keys, size_t* nkeys)
{
    HDC t = HDC(tree);
    if (t.get_type() != HDC_STRUCT) {
        *nkeys = 0;
        return;
    }
    vector<string> cppkeys = HDC(tree).keys();
    *nkeys = cppkeys.size();
    for (size_t i = 0; i < *nkeys; i++) {
        strcpy(keys[i], cppkeys[i].c_str());
    }
}

//const char* hdc_dumps(hdc_t tree)
//{
//    stringstream tmp;
//    tmp << HDC(tree).to_json(0);
//    string dump_str = tmp.str();
//    return strdup(dump_str.c_str());
//}

void hdc_init(const char* pluginFileName, const char* pluginSettingsFileName)
{
    if (strlen(pluginFileName) != 0) {
        if (strlen(pluginSettingsFileName) == 0) {
            HDC::init(std::string(pluginFileName));
        } else {
            HDC::init(std::string(pluginFileName), std::string(pluginSettingsFileName));
        }
    } else {
        HDC::init();
    }
}

void hdc_destroy()
{
    if (hdc_global.storage != nullptr) HDC::destroy();
}

void hdc_search_plugins(char** keys, int* num)
{
    auto plugins = HDC::search_plugins();
    size_t i=0;
    for (auto plugin: plugins) {
        strcpy(keys[i++],plugin.first.c_str());
    }
    *num = plugins.size();
}

hdc_t hdc_load(const char* uri, const char* datapath)
{
    return HDC::load(uri, datapath).as_obj();
}

void hdc_save(hdc_t tree, const char* uri)
{
    HDC(tree).save(uri);
}

void hdc_serialize(hdc_t tree, char* buffer)
{
    stringstream tmp;
    tmp << HDC(tree).serialize("hdc");
    strcpy(buffer,tmp.str().c_str());
}

hdc_t hdc_deserialize(const char* str)
{
    return HDC::deserialize("hdc", str).as_obj();
}

hdc_data_t hdc_get_data(hdc_t tree, const char* path)
{
    return HDC(tree).get(path).get_data();
}

void hdc_set_data(hdc_t tree, const char* path, hdc_data_t data)
{
    HDC(tree).get_or_create(path).set_data(data);
}

void hdc_set_external(hdc_t tree, const char* path, hdc_data_t data)
{
    HDC(tree).get_or_create(path).set_external(data);
}

size_t hdc_get_size(hdc_t tree)
{
    return HDC(tree).get_size();
}

void hdc_set_scalar(hdc_t tree, const char* path, void* data, hdc_type_t _type)
{
    return HDC(tree).get_or_create(path).set_data((void*)data,_type);
}

// end extern C
}
