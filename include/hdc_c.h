#ifndef HDC_C_H
#define HDC_C_H

#ifdef __cplusplus
extern "C" {
#endif
#include "hdc_types.h"
// ------------------------ Constructors --------------------------- //
/**
* @brief Default constructor. Creates empty HDC.
*/
hdc_t hdc_new_empty();

/**
* @brief Constructs HDC from string/uri
*
* @param str str: String/URI
*/
hdc_t hdc_new_string(const char* str);

/**
* @brief Constructs empty HDC of given (preallocated) size.
*
* @param size_t p_size: Size in bytes.
*/
hdc_t hdc_new_size(size_t size);

/**
* @brief Constructs HDC with preallocated Array of given rank, shape and type
*
* @param size_t rank: Array rank
* @param size_t shape: Array shape
* @param hdc_type_t type: Array type
*/
hdc_t hdc_new_array(size_t rank, size_t* shape, hdc_type_t type);

/**
 * @brief Constructs HDC with preallocated Array of given rank, shape and type
 *
 * @param rank p_rank: Array rank
 * @param shape p_shape: Array shape
 * @param type p_type: Array type
 * @param flags p_flags: Additional HDC flags
 * @return hdc_t
 */
hdc_t hdc_new_array2(size_t rank, size_t* shape, hdc_type_t type, size_t flags);

/**
 * @brief Alernative constuctor to be used in Fortran.
 *
 * @return void*
 */
void* hdc_new_void_ptr();

// ----------------------- Node manipulation ---------------------- //

/**
 * @brief Adds node to tree under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param child p_child: HDC node to be stored/set
 */
void hdc_add_child(hdc_t tree, const char* path, hdc_t child);

/**
 * @brief Sets (replaces) node to tree under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param i p_i: index of node. Indexing starts from zero.
 * @return hdc_t
 */
void hdc_set_child(hdc_t tree, const char* path, hdc_t child);

/**
 * @brief Deletes node on the given path from tree
 *
 * @param tree p_tree: HDC node
 * @param child p_child: HDC node to be stored/set
 */
void hdc_delete_child(hdc_t tree, const char* path);

/**
 * @brief Returns node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return hdc_t
 */
hdc_t hdc_get(hdc_t tree, const char* path);

/**
 * @brief Sets node to i-th slice of current node.
 *
 * @param tree p_tree: HDC node
 * @param i p_i: index of node. Indexing starts from zero.
 * @param child p_child: HDC node to be stored/set
 */
hdc_t hdc_get_slice(hdc_t tree, const char* path, size_t i);

/**
 * @brief Appends node to list node
 *
 * @param tree p_tree: HDC node
 * @param child p_child: HDC node to be stored/set
 */
void hdc_append_slice(hdc_t tree, hdc_t child);

/**
 * @brief Sets (replaces) node to list node
 *
 * @param tree p_tree: HDC node
 * @param i p_i: index of node. Indexing starts from zero.
 * @param child p_child: HDC node to be stored/set
 */
void hdc_set_slice(hdc_t tree, size_t i, hdc_t child);

/**
 * @brief Inserts node on the given position/index
 *
 * @param tree p_tree: HDC node
 * @param i p_i: index of node. Indexing starts from zero.
 * @param child p_child: HDC node to be stored/set
 */
void hdc_insert_slice(hdc_t tree, size_t i, hdc_t child);

/**
 * @brief Checks whether the node under given path exists.
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return bool
 */
bool hdc_exists(hdc_t tree, const char* path);

/**
 * @brief Checks whether the node under given path is external or not.
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return bool
 */
bool hdc_is_external(hdc_t tree, const char* path);

/**
 * @brief Prints out info about the node (type, rank, shape, flags)
 *
 * @param tree p_tree: HDC node
 */
void hdc_print_info(hdc_t tree);

/**
 * @brief Returns uuid of node.
 *
 * @param tree p_tree: HDC node
 * @return const char*
 */
const char* hdc_get_uuid(hdc_t tree);


/**
 * @brief Returns rank of the node
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return size_t
 */
size_t hdc_get_rank(hdc_t tree, const char* path);

/**
 * @brief Returns type of the node
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return size_t
 */
size_t hdc_get_type(hdc_t tree, const char* path);

/**
 * @brief Saven number of keys of node children and their list into provided variables.
 *
 * @param tree p_tree: HDC node
 * @param keys p_keys:...
 * @param nkeys p_nkeys:...
 */
void hdc_keys(hdc_t tree, char** keys, size_t* nkeys);

/**
 * @brief Returns copy of src HDC node.
 *
 * @param src p_src:...
 * @param deep_copy p_deep_copy:...
 * @return hdc_t
 */
hdc_t hdc_copy(hdc_t src, bool deep_copy);

/**
 * @brief Performs serialization of tree to JSON
 *
 * @param tree p_tree: HDC node
 * @param filename p_filename:...
 * @param mode p_mode:...
 */
//void hdc_to_json(hdc_t tree, const char* filename, int mode);


/**
 * @brief Prints out json string of HDC tree under the node.
 *
 * @param tree p_tree: HDC node
 */
void hdc_dump(hdc_t tree);

/**
 * @brief Returns char* with JSON representation of the node (sub)trre
 *
 * @param tree p_tree: HDC node
 * @return const char*
 */
const char* hdc_dumps(hdc_t tree);


/**
 * @brief returns data shape string of node
 *
 * @param tree p_tree: HDC node
 * @return const char*
 */
const char* hdc_get_datashape_str(hdc_t tree);

/**
 * @brief Returns text representation of node type. e.g. unit8/int16/float/double/boolean/string
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return const char*
 */
const char* hdc_get_type_str(hdc_t tree, const char* path);

/**
 * @brief Loads HDC tree from given URI and data path.
 *
 * @param uri p_uri: URI string
 * @param datapath p_datapath: Path to the data within the file (e.g. JSON or HDF5)
 * @return hdc_t
 */
hdc_t hdc_load(const char* uri, const char* datapath);

/**
 * @brief Saves HDC node into given URI
 *
 * @param tree p_tree: HDC node
 * @param uri p_uri: URI string
 */
void hdc_save(hdc_t tree, const char* uri);

/**
 * @brief Serializes HDC node into provided buffer
 *
 * @param tree p_tree: HDC node
 * @param buffer p_buffer: char buffer where to put the serialized node
 */
void hdc_serialize(hdc_t tree, char* buffer);

/**
 * @brief Returns HDC from given string
 *
 * @param tree p_tree: HDC node
 */
hdc_t hdc_deserialize(const char* str);

/**
 * @brief Returns slice from HDC list node.
 *
 * @param tree p_tree: HDC node
 * @return size_t
 */
size_t hdc_get_size(hdc_t tree);

/**
 * @brief Returns list and number of found HDC storage plugins
 *
 * @param keys p_keys:...
 * @param num p_num:...
 */
void hdc_search_plugins(char** keys, int* num);

/**
 * @brief Cleans up the node (recursively deletes all descendands)
 *
 * @param tree p_tree: HDC node
 */
void hdc_clean(hdc_t node);

// ----------------------- Data setters --------------------------- //

/**
 * @brief Sets int8 scalar data under the given path and tree
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param data p_data: data encapsulated into hdc_data_t struct.
 */
void hdc_set_int8_scalar(hdc_t tree, const char* path, int8_t data);

/**
 * @brief Sets int8 array data under the given path and tree
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param rank p_rank: Array rank
 * @param shape p_shape: Array shape
 * @param data p_data: data encapsulated into hdc_data_t struct.
 * @param _flags p__flags: Additional HDC flags
 */
void hdc_set_int8(hdc_t tree, const char* path, int rank, size_t* shape, void* data, hdc_flags_t _flags);

/**
 * @brief Sets int16 scalar data under the given path and tree
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param data p_data: data encapsulated into hdc_data_t struct.
 */
void hdc_set_int16_scalar(hdc_t tree, const char* path, int16_t data);

/**
 * @brief Sets int32 array data under the given path and tree
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param rank p_rank: Array rank
 * @param shape p_shape: Array shape
 * @param data p_data: data encapsulated into hdc_data_t struct.
 * @param _flags p__flags: Additional HDC flags
 */
void hdc_set_int16(hdc_t tree, const char* path, int rank, size_t* shape, void* data, hdc_flags_t _flags);

/**
 * @brief Sets int32 scalar data under the given path and tree
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param data p_data: data encapsulated into hdc_data_t struct.
 */
void hdc_set_int32_scalar(hdc_t tree, const char* path, int32_t data);

/**
 * @brief Sets int32 array data under the given path and tree
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param rank p_rank: Array rank
 * @param shape p_shape: Array shape
 * @param data p_data: data encapsulated into hdc_data_t struct.
 * @param _flags p__flags: Additional HDC flags
 */
void hdc_set_int32(hdc_t tree, const char* path, int rank, size_t* shape, void* data, hdc_flags_t _flags);

/**
 * @brief Sets int64 scalar data under the given path and tree
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param data p_data: data encapsulated into hdc_data_t struct.
 */
void hdc_set_int64_scalar(hdc_t tree, const char* path, int64_t data);

/**
 * @brief Sets int64 array data under the given path and tree
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param rank p_rank: Array rank
 * @param shape p_shape: Array shape
 * @param data p_data: data encapsulated into hdc_data_t struct.
 * @param _flags p__flags: Additional HDC flags
 */
void hdc_set_int64(hdc_t tree, const char* path, int rank, size_t* shape, void* data, hdc_flags_t _flags);

/**
 * @brief Sets float scalar data under the given path and tree
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param str p_str: String
 */
void hdc_set_float_scalar(hdc_t tree, const char* path, float data);

/**
 * @brief Sets float array data under the given path and tree
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param rank p_rank: Array rank
 * @param shape p_shape: Array shape
 * @param data p_data: data encapsulated into hdc_data_t struct.
 * @param _flags p__flags: Additional HDC flags
 */
void hdc_set_float(hdc_t tree, const char* path, int rank, size_t* shape, void* data, hdc_flags_t _flags);

/**
 * @brief Sets double scalar data under the given path and tree
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return const char*
 */
void hdc_set_double_scalar(hdc_t tree, const char* path, double data);

/**
 * @brief Sets double array data under the given path and tree
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param rank p_rank: Array rank
 * @param shape p_shape: Array shape
 * @param data p_data: data encapsulated into hdc_data_t struct.
 * @param _flags p__flags: Additional HDC flags
 */
void hdc_set_double(hdc_t tree, const char* path, int rank, size_t* shape, void* data, hdc_flags_t _flags);

/**
 * @brief Sets double complex scalar data under the given path and tree
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return const char*
 */
void hdc_set_complex_scalar(hdc_t tree, const char* path, double _Complex data);

/**
 * @brief Sets double complex array data under the given path and tree
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param rank p_rank: Array rank
 * @param shape p_shape: Array shape
 * @param data p_data: data encapsulated into hdc_data_t struct.
 * @param _flags p__flags: Additional HDC flags
 */
void hdc_set_complex(hdc_t tree, const char* path, int rank, size_t* shape, void* data, hdc_flags_t _flags);

/**
 * @brief Sets string data under the given path and tree
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param str p_str: String
 */
void hdc_set_string(hdc_t tree, const char* path, const char* str);

// -------------------------------- Data getters -----------------------------------------------//


/**
 * @brief Returns data encapsulated in hdc_data_t struct
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return hdc_data_t
 */
struct hdc_data_t hdc_get_data(hdc_t tree, const char* path);

/**
 * @brief Returns string from the given HDC node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return const char*
 */
const char* hdc_as_string(hdc_t tree, const char* path);

/**
 * @brief Returns int8 array data from the given HDC node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return int8_t*
 */
int8_t* hdc_as_int8_array(hdc_t tree, const char* path);

/**
 * @brief Returns int16 array data from the given HDC node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return int16_t*
 */
int16_t* hdc_as_int16_array(hdc_t tree, const char* path);

/**
 * @brief Returns int32 array data from the given HDC node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return int32_t*
 */
int32_t* hdc_as_int32_array(hdc_t tree, const char* path);

/**
 * @brief Returns int64 array data from the given HDC node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return int64_t*
 */
int64_t* hdc_as_int64_array(hdc_t tree, const char* path);

/**
 * @brief Returns double array data from the given HDC node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return double*
 */
double* hdc_as_double_array(hdc_t tree, const char* path);

/**
 * @brief Returns double array data from the given HDC node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return double _Complex*
 */
double _Complex* hdc_as_complex_array(hdc_t tree, const char* path);

/**
 * @brief Returns float array data from the given HDC node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return float*
 */
float* hdc_as_float_array(hdc_t tree, const char* path);

/**
 * @brief Returnds void pointer to the data from the given HDC node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return void*
 */
void* hdc_as_voidptr(hdc_t tree, const char* path);

/**
 * @brief Returns double scalar data from the given HDC node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return double
 */
double  hdc_as_double_scalar(hdc_t tree, const char* path);

/**
 * @brief Returns double scalar data from the given HDC node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return double _Complex
 */
double _Complex hdc_as_complex_scalar(hdc_t tree, const char* path);

/**
 * @brief Returns float scalar data from the given HDC node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return float
 */
float   hdc_as_float_scalar(hdc_t tree, const char* path);

/**
 * @brief Returns int8 scalar data from the given HDC node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return int8_t
 */
int8_t  hdc_as_int8_scalar(hdc_t tree, const char* path);

/**
 * @brief Returns int16 scalar data from the given HDC node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return int16_t
 */
int16_t hdc_as_int16_scalar(hdc_t tree, const char* path);

/**
 * @brief Returns int32 scalar data from the given HDC node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return int32_t
 */
int32_t hdc_as_int32_scalar(hdc_t tree, const char* path);

/**
 * @brief Returns int64 scalar data from the given HDC node under the given path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @return int64_t
 */
int64_t hdc_as_int64_scalar(hdc_t tree, const char* path);

/**
 * @brief Sets hdc_data_t encapulated data into the given node and path
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param data p_data: data encapsulated into hdc_data_t struct.
 */
void hdc_set_data(hdc_t tree, const char* path, struct hdc_data_t data);

/**
 * @brief Sets external hdc_data_t encapulated data into the given node and path. Only pointer will be stored. This function does not care about the data ownership.
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param data p_data: data encapsulated into hdc_data_t struct.
 */
void hdc_set_external(hdc_t tree, const char* path, struct hdc_data_t data);

/**
 * @brief Sets scalar data of the given type into the given node and path.
 *
 * @param tree p_tree: HDC node
 * @param path p_path: Path string
 * @param data p_data: data encapsulated into hdc_data_t struct.
 * @param _type p__type:...
 */
void hdc_set_scalar(hdc_t tree, const char* path, void* data, hdc_type_t _type);

// ----------------------------- Static methods wrappers ----------------------------------------//

/**
 * @brief Performs initialization of Storage
 *
 * @param storagePluginFileName p_storagePluginFileName: The file path of the HDC storage plugin (.so library) to be used.
 * @param storagePluginSettingsFileName p_storagePluginSettingsFileName: The file path of the additional settings of the storage plugin.
 */
void hdc_init(const char* storagePluginFileName, const char* storagePluginSettingsFileName);

/**
 * @brief ...
 *
 */
void hdc_destroy();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HDC_C_H
