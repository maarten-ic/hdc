#ifndef HDC_HPP
#define HDC_HPP

#include <iostream>

// marray - array views
#include "andres/marray.hxx"

// some other stuff -- to be reduced later
#include <complex>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <list>
#include <map>
#include <sstream>
#include <cstdbool>
#include <unordered_map>
#include <vector>
#include <memory>
// our stuff
#include "hdc_errors.hpp"
#include "hdc_global.hpp"
#include "hdc_helpers.h"
#include "hdc_map.h"
#include "hdc_storage.h"
#include "hdc_types.h"
#include "hdc_utils.h"
#include "serialization/hdc_serializer.h"

// #define DEBUG

using namespace std;
extern HDCGlobal hdc_global;

using byte = unsigned char;
using hdc_index_t = boost::variant<size_t, std::string>;
using hdc_path_t = std::list<hdc_index_t>;

class HDC {
private:
    boost::uuids::uuid uuid;
    HDCStorage* storage;

/* ------------------------------- methods ----------------------------------------- */

    /**
    * @brief Adds node as child under the given path.
    *
    * @param path p_path: path under which to add node
    * @param n p_n: node
    */
    void add_child(hdc_path_t& path, const HDC& n);

    /**
    * @brief Sets child node under the given path.
    *
    * @param path p_path: path uder which to set node
    * @param n p_n: node
    */
    void set_child(hdc_path_t& path, const HDC& n);

    /**
    * @brief Deletes the child node from the given path.
    *
    * @param path p_path: path in the subtree of this node
    */
    bool delete_child(hdc_path_t& path, bool prune);

    /**
    * @brief Returns the child node at given path.
    *
    * @param path p_path: path in the subtree of this node
    * @return HDC
    */
    HDC get(hdc_path_t& path);

    /**
    * @brief Returns the child node at given path.
    *
    * @param path p_path: path in the subtree of this node
    * @return HDC
    */
    HDC get(hdc_path_t& path) const;

    /**
    * @brief Returns true if a child node exists under given path. Single (no-recursive) version. This is not mentioned to be run by users.
    *
    * @param index p_index: size_t index if this node has/should have type HDC_LIST, or std::string if this node has/should have type HDC_STRUCT
    * @return bool
    */
    bool exists_single(hdc_index_t index) const;

    /**
    * @brief Adds node as child under the given path. Single (no-recursive) version. This is not mentioned to be run by users.
    *
    * @param path p_path: path in the subtree of this node
    * @param n p_n: HDC node
    */
    void add_child_single(const std::string& path, const HDC& n);

    /**
    * @brief Returns a copy of header containing metadata
    *
    * @return hdc_header_t
    */
    hdc_header_t get_header() const;

    /**
    * @brief Returns pointer to header containing metadata
    *
    * @return hdc_header_t
    */
    hdc_header_t* get_header_ptr() const;

    /**
    * @brief Sets a child node (single level = no recursion)
    *
    * @param path p_path: path in the subtree of this node
    * @param n p_n: HDC node
    */
    void set_child_single(hdc_index_t path, const HDC& n);

public:
    /**
    * @brief Returns a child node under a given path. Single (no-recursive) version. This is not mentioned to be run by users.
    *
    * @param index p_index: size_t index if this node has/should have type HDC_LIST, or std::string if this node has/should have type HDC_STRUCT
    * @return HDC
    */
    HDC get_single(hdc_index_t index);

    /**
    * @brief Returns a child node under a given path. Single (no-recursive) version. This is not mentioned to be run by users.
    *
    * @param index p_index: size_t index if this node has/should have type HDC_LIST, or std::string if this node has/should have type HDC_STRUCT
    * @return HDC
    */
    HDC get_single(hdc_index_t index) const;

    /**
    * @brief Creates empty HDC with specified data size.
    *
    * @param byte_size p_byte_size: Size in bytes.
    */
    explicit HDC(size_t byte_size);

    /**
    * @brief Default constructor. Creates empty HDC.
    *
    */
    HDC();

    /**
    * @brief Creates empty HDC with specified type and shape
    *
    * @param shape p_shape: Shape of the data
    * @param type p_type: Type of the data (e.g. HDC_INT32)
    * @param flags p_flags: Flags the node should have (e.g. HDCFortranOrder)
    */
    HDC(const std::vector<size_t>& shape, hdc_type_t type, hdc_flags_t flags = HDCDefault);

    /**
     * @brief Creates HDC node filled by data from hdc_data_t struct
     *
     * @param obj p_obj: hdc_data_t struct
     */
    explicit HDC(hdc_data_t obj);

    /**
    * @brief Constructor from string
    *
    * @param str p_str: String
    */
    explicit HDC(const std::string& str);

    /**
    * @brief Copy contructor
    *
    * @param h p_h: HDC object to be copied
    */
    HDC(const HDC& h);

    /**
     * @brief Move constructor
     *
     * @param h p_h: reference to HDC object to be copied
     */
    HDC(HDC&& h) noexcept;

    /**
    * @brief Deserializing constructor
    *
    * @param _storage p__storage: storage of source node
    * @param _uuid p__uuid: UUID of source node
    */
    HDC(HDCStorage* _storage, const std::string& _uuid);

    /**
    * @brief Intermediate node constructor from boost::uuids::uuid
    *
    * @param _storage p__storage: storage of source node
    * @param _uuid p__uuid: UUID of source node
    */
    HDC(HDCStorage* _storage, boost::uuids::uuid _uuid);

    /**
    * @brief Constructs HDC object from hdc_t struct - C/FORTRAN interoperability
    *
    * @param obj p_obj: reference to hdc_t object.
    */
    explicit HDC(hdc_t& obj);

    /**
    * @brief Constructs HDC object from scalar and type - MATLAB interoperability
    *
    * @param data p_data: void pointer to scalar
    * @param t p_t: type of the data (hdc_type_t)
    */
    HDC(void* data, hdc_type_t t);

    /**
    * @brief Constructs HDC object from std::vector
    *
    * @param T p_T: data type
    * @param data p_data: vector with data
    * @param flags p_flags: flags
    */
    template <typename T>
    explicit HDC(std::vector<T> data, hdc_flags_t flags = HDCDefault)
    {
        HDC_STORAGE_INIT()
        size_t rank = 1;
        size_t data_size = sizeof(T) * data.size();
        auto buffer_size = data_size + sizeof(hdc_header_t);
        auto buffer = std::make_unique<char[]>(buffer_size);
        auto header = reinterpret_cast<hdc_header_t*>(buffer.get());
        header->type = to_typeid(data[0]);
        header->flags = flags;
        header->rank = rank;
        header->data_size = data_size;
        header->buffer_size = buffer_size;
        header->shape[0] = data.size();
        memcpy(buffer.get() + sizeof(hdc_header_t), data.data(), data_size);
        uuid = generate_uuid();
        storage = hdc_global.storage;
        storage->set(uuid, buffer.get(), buffer_size);
    }

    /**
    * @brief Destructor
    *
    */
    ~HDC() = default;

    /**
    * @brief Parses command line arguments.
    *
    * @param argc p_argc: Number of arguments.
    * @param argv p_argv: char* array of arguments.
    */
    static void parse_cmdline(int argc, const char* argv[]);

    /**
    * @brief Loads settings from file.
    *
    * @param configPath p_configPath: Path to the settings file.
    */
    static void load_config(std::string configPath = "./hdc.conf:~/.config/hdc.conf");

    /**
    * @brief Searches for available storage plugins.
    *
    * @param searchPath p_searchPath: Comma separated list of folders where to look for storage plugins.
    */
    static std::unordered_map<std::string, std::string> search_plugins(const std::string& customSearchPath = "");

    /**
    * @brief Sets the default storage to be used.
    *
    * @param storage p_storage: string name of storage to be used - e.g. "umap", "mdbm", or "redis"
    * @param storage_cmd p_storage_cmd: JSON string with parameters specific for each storage.
    */
    static void set_storage(std::string storage = "umap", std::string storage_cmd = "");

    /**
    * @brief Returns a directory path where the HDC library is loaded from.
    *
    * @return std::string
    */
    static std::string get_library_dir();

    /**
    * @brief Initializes hdc_global.storage  -- mainly due to C and Fortran
    *
    * @param storage p_storage: string name of storage to be used - e.g. "umap", "mdbm", or "redis"
    * @param storage_options p_storage_options: JSON string with parameters specific for each storage.
    */
    static void init(const std::string& storage = "", const std::string& storage_options = "");

    /**
    * @brief Sets the default storage options, builds the boost::property_tree with default settings structure to be redefined later by parse_cmdline(), load_config() or set_storage() functions.
    *
    * @param storage p_storage: Storage string specifier - e.g. "umap", "mdb", "redis"
    * @param storage_options p_storage_options: JSON encoded settings string
    */
    static void set_default_storage_options(const std::string& storage, const std::string& storage_options = "");

    /**
    * @brief Returns map of children if there are any, otherwise it returns empty map
    *
    * @return std::map< HDC >
    */
    std::map<std::string, HDC> get_children() const;

    bool has_children() const;

    /**
    * @brief Returns map of children if there are any, otherwise it returns empty map
    *
    * @return int
    */
    std::vector<HDC> get_slices() const;

    /**
    * @brief Makes copy of object. Copies at least node buffer. If deep_copy specified, it also copies all children nodes.
    *
    * @param deep_copy p_deep_copy: true = deep copy (copies also all the children), false = copy just current node, keeping children references untouched
    * @return HDC
    */
    HDC copy(bool deep_copy = true);

    /**
    * @brief removes node's children
    *
    */
    void clean();

    /**
    * @brief Cleans up the hdc_global.storage  -- mainly due to C and Fortran.
    *
    */
    static void destroy();

    /**
    * @brief Returns true if a node under specified path exists and false otherwise.
    *
    * @param path p_path: path in the subtree of this node
    * @return bool
    */
    bool exists(hdc_path_t& path) const;

    /**
    * @brief Returns true if a node under specified path exists and false otherwise.
    *
    * @param index p_index: size_t index if this node has/should have type HDC_LIST, or std::string if this node has/should have type HDC_STRUCT
    * @return bool
    */
    bool exists(size_t index) const;

    /**
    * @brief Returns true if a node under specified index exists and false otherwise.
    *
    * @param index p_index: size_t index if this node has/should have type HDC_LIST, or std::string if this node has/should have type HDC_STRUCT
    * @return HDC
    */
    HDC get(size_t index);

    /**
    * @brief Returns true if a node under specified index exists and false otherwise.
    *
    * @param index p_index: size_t index if this node has/should have type HDC_LIST, or std::string if this node has/should have type HDC_STRUCT
    * @return HDC
    */
    HDC get(size_t index) const;

    /**
    * @brief Returns the available space for data in buffer (in bytes)
    *
    * @return size_t
    */
    size_t get_datasize() const;

    /**
    * @brief Returns the the size of object buffer (= header+data, in bytes)
    *
    * @return size_t
    */
    size_t get_size() const;


    /**
    * @brief Returns type of current node.
    *
    * @return size_t
    */
    size_t get_type() const;

    /**
    * @brief Returns the size of a single item in bytes
    *
    * @return size_t
    */
    size_t get_itemsize() const;

    /**
    * @brief Returns object flags (i.e. array ordering)
    *
    * @return size_t
    */
    size_t get_flags() const;

    /**
    * @brief Returns true if type is not one of HDC_LIST or HDC_STRUCT
    *
    * @return bool
    */
    bool is_terminal() const;

    /**
    * @brief Returns true if type is not one of HDC_LIST or HDC_STRUCT
    *
    * @return bool
    */
    static bool _is_terminal(size_t t);

    /**
    * @brief Returns a child node. If not found it will be created first.
    *
    * @param index p_index: size_t index if this node has/should have type HDC_LIST, or std::string if this node has/should have type HDC_STRUCT
    * @return HDC
    */
    HDC get_or_create(size_t index);

    /**
    * @brief Returns a child node. If not found it will be created first.
    *
    * @param path p_path: path in the subtree of this node
    * @return HDC
    */
    HDC get_or_create(const std::string& path);

    /**
     * @brief Returns a child node. If not found it will be created first.
     *
     * @param path p_path: path in the subtree of this node
     * @return HDC
     */
    HDC get_or_create(hdc_path_t& path);

    /**
     * @brief Returns true if storages and uuids are the same.
     *
     * @param path other: a node to compare with
     */
    inline bool operator==(const HDC& other) const {
        return (this->get_storage() == other.get_storage() && this->get_uuid() == other.get_uuid());
    }

    /**
     * @brief Returns true if storages or uuids are different.
     *
     * @param path other: a node to compare with
     * @return bool
     */
    inline bool operator!=(const HDC& other) const {
        return !operator==(other);
    }

    /**
    * @brief Access the node under provided path. The node is automatically created if not found.
    *
    * @param index p_index: size_t index if this node has/should have type HDC_LIST, or std::string if this node has/should have type HDC_STRUCT
    * @return HDC
    */
    HDC operator[](size_t index) const;

    /**
    * @brief Access the node under provided path. The node is automatically created if not found.
    *
    * @param index p_index: size_t index if this node has/should have type HDC_LIST, or std::string if this node has/should have type HDC_STRUCT
    * @return HDC&
    */
    HDC operator[](size_t index);

    /**
    * @brief Access the node under provided path. The node is automatically created if not found.
    *
    * @param path p_path: path in the subtree of this node
    * @return HDC
    */
    HDC operator[](const std::string& path) const;

    /**
    * @brief Access the node under provided path. The node is automatically created if not found.
    *
    * @param path p_path: path in the subtree of this node
    * @return HDC&
    */
    HDC operator[](const std::string& path);

    /**
    * @brief Assigns data to a node.
    *
    * @param T p_T: Desired data type.
    * @return HDC&
    */
    template <typename T>
    HDC& operator=(T d)
    {
        set_data(d);
        return *this;
    }

    /**
    * @brief Assigns char* data to a node.
    *
    * @param str p_str: char* string
    * @return HDC&
    */
    HDC& operator=(char const* str);

    /**
    * @brief Assigns string data to a node.
    *
    * @param str p_str: string to be set.
    * @return HDC&
    */
    HDC& operator=(const std::string& str);

    /**
    * @brief Assigns copy of a supplied node to a node.
    *
    * @param other p_other: source node.
    * @return HDC&
    */
    HDC& operator=(const HDC& other);

    /**
     * @brief Assigns copy of a supplied node to a node.
     *
     * @param other p_other: source node.
     * @return HDC&
     */
    HDC& operator=(HDC&& other) noexcept;

    /**
     * @brief returns true if this node and other are the same (uuids and storages are the same).
     *
     * @param other p_other: node to compare with.
     * @return bool
     */
    bool equals(const HDC& other) const;

    /**
    * @brief Sets provided data to a HDC node.
    *
    * @param T p_T: Desired data type.
    * @param shape p_shape: Shape of the data
    * @param data p_data: Pointer to data
    * @param flags p_flags: Flags the node should have (e.g. HDCFortranOrder)
    */
    template <typename T>
    void set_data(const std::vector<size_t>& shape, T* data, hdc_flags_t flags = HDCDefault)
    {
        auto rank = shape.size();
        // Start with determining of the buffer size
        auto data_size = sizeof(T);
        for (size_t i = 0; i < rank; i++) {
            data_size *= shape[i];
        }
        auto buffer_size = data_size + sizeof(hdc_header_t);

        auto buffer = get_buffer();
        auto header = reinterpret_cast<hdc_header_t*>(buffer);
        if (header->flags & HDCExternal) {
                throw HDCException("set_data(): Trying to set externally owned data. Please, ommit this, use set_external(), or re-create the node.");
        }
        if (header->buffer_size == buffer_size) {
            storage->lock(uuid);
            // update header
            memset(header->shape, 0, HDC_MAX_DIMS * sizeof(size_t));
            for (size_t i = 0; i < rank; i++) {
                header->shape[i] = shape[i];
            }
            header->flags = flags;
            header->type = to_typeid(data[0]);
            header->rank = rank;
            // set data
            memcpy(buffer + sizeof(hdc_header_t), data, data_size);
            storage->unlock(uuid);
            if (!storage->memory_mapped()) storage->set(uuid, buffer, header->buffer_size);
        } else {
            // create new buffer
            auto new_buffer = std::make_unique<char[]>(buffer_size);
            // set header
            header = reinterpret_cast<hdc_header_t*>(new_buffer.get());
            header->buffer_size = buffer_size;
            header->data_size = data_size;
            memset(header->shape, 0, HDC_MAX_DIMS * sizeof(size_t));
            for (size_t i = 0; i < rank; i++) {
                header->shape[i] = shape[i];
            }
            header->flags = flags;
            header->type = to_typeid(data[0]);
            header->rank = rank;
            // set data
            memcpy(new_buffer.get() + sizeof(hdc_header_t), data, data_size);
            // replace buffer
            storage->set(uuid, new_buffer.get(), buffer_size);
        }
    }

    /**
    * @brief Sets provided data to a HDC node.
    *
    * @param T p_T: Desired data type.
    * @param shape p_shape: Shape of the data
    * @param data p_data: Pointer to data
    * @param flags p_flags: Flags the node should have (e.g. HDCFortranOrder)
    */
    template <typename T>
    void set_external(const std::vector<size_t>& shape, T* data, hdc_flags_t flags = HDCDefault)
    {
        auto rank = shape.size();
        // Start with determining of the buffer size
        auto data_size = sizeof(void*);
        auto buffer_size = data_size + sizeof(hdc_header_t);

        auto buffer = get_buffer();
        auto header = reinterpret_cast<hdc_header_t*>(buffer);
        if (header->buffer_size == buffer_size) {
            storage->lock(uuid);
            // update header
            memset(header->shape, 0, HDC_MAX_DIMS * sizeof(size_t));
            for (size_t i = 0; i < rank; i++) {
                header->shape[i] = shape[i];
            }
            header->flags = flags | HDCExternal;
            header->type = to_typeid(data[0]);
            header->rank = rank;
            // set data
            memcpy(buffer + sizeof(hdc_header_t), &data, data_size);
            storage->unlock(uuid);

            if (!storage->memory_mapped()) storage->set(uuid, buffer, header->buffer_size);

        } else {
            // create new buffer
            auto new_buffer = std::make_unique<char[]>(buffer_size);
            // set header
            header = reinterpret_cast<hdc_header_t*>(new_buffer.get());
            header->buffer_size = buffer_size;
            header->data_size = data_size;
            memset(header->shape, 0, HDC_MAX_DIMS * sizeof(size_t));
            for (size_t i = 0; i < rank; i++) {
                header->shape[i] = shape[i];
            }
            header->flags = flags | HDCExternal;
            header->type = to_typeid(data[0]);
            header->rank = rank;
            // set data
            memcpy(new_buffer.get() + sizeof(hdc_header_t), &data, data_size);
            storage->set(uuid, new_buffer.get(), buffer_size);
        }
    }

    /**
    * @brief Sets provided data to a HDC node.
    *
    * @param T p_T: Desired data type.
    * @param data p_data: Pointer to data
    */
    template <typename T>
    void set_external(T* data)
    {
        auto data_size = sizeof(void*);
        auto buffer_size = data_size + sizeof(hdc_header_t);
        auto buffer = std::make_unique<char[]>(buffer_size);
        auto header = reinterpret_cast<hdc_header_t*>(buffer.get());
        header->type = to_typeid(*data);
        header->data_size = data_size;
        header->buffer_size = buffer_size;
        header->flags = HDCExternal;
        memcpy(buffer.get() + sizeof(hdc_header_t), &data, data_size);
        storage->set(uuid, buffer.get(), buffer_size);
    }

    /**
    * @brief Sets provided data to a HDC node.
    *
    * @param T p_T: Desired data type.
    * @param shape p_shape: Shape of the data
    * @param data p_data: Pointer to data
    * @param flags p_flags: Flags the node should have (e.g. HDCFortranOrder)
    */
    template <typename T>
    void set_data(initializer_list<size_t> shape, T* data, hdc_flags_t flags = HDCDefault)
    {
        std::vector<size_t> _shape = shape;
        set_data(_shape, data, flags);
    }

    /**
     * @brief Sets provided data pointer to a HDC node. This function stores only pointer to data, not the data itself. User is responsible for its proper (de)allocation.
     *
    * @param T p_T: Desired data type.
    * @param shape p_shape: Shape of the data
    * @param data p_data: Pointer to data
    * @param flags p_flags: Flags the node should have (e.g. HDCFortranOrder)
     */
    template <typename T>
    void set_external(initializer_list<size_t> shape, T* data, hdc_flags_t flags = HDCDefault)
    {
        std::vector<size_t> _shape = shape;
        set_external(_shape, data, flags);
    }

    /**
    * @brief Sets provided data to a HDC node.
    *
    * @param T p_T: Desired data type.
    * @param data p_data: Pointer to data
    * @param flags p_flags: Flags the node should have (e.g. HDCFortranOrder)
    */
    template <typename T>
    void set_data(initializer_list<T> data, hdc_flags_t flags = HDCDefault)
    {
        DEBUG_STDOUT(std::string(
                             "template<typename T> void set_data(initializer_list<T> data, hdc_flags_t flags = HDCDefault)") +
                             to_string(data[0]));
        vector<T> vec = data;
        set_data(1, { vec.size() }, &vec[0], flags);
    }

    /**
    * @brief Sets data to current node from vector<T> data. This function is primarily designed for interoperability with Python
    *
    * @param T p_T: Desired data type.
    * @param data p_data: Pointer to data
    */
    template <typename T>
    void set_data(vector<T> data)
    {
        DEBUG_STDOUT(std::string("template <typename T> void set_data(vector<T> data)") + to_string(data[0]));
        if (get_children_ptr() != nullptr) {
            DEBUG_STDERR("The node has already children set...");
            return;
        }
        std::vector<size_t> shape = { data.size() };
        set_data<T>(shape, &data[0]);
    }

    /**
    * @brief Sets string to current node
    *
    * @param str p_str: string to set.
    */
    void set_string(const std::string& str)
    {
        auto data_size = str.length() + 1;
        auto buffer_size = data_size + sizeof(hdc_header_t);
        if (storage->has(uuid)) {
            storage->remove(uuid);
        }
        auto buffer = std::make_unique<char[]>(buffer_size);
        auto header = reinterpret_cast<hdc_header_t*>(buffer.get());
        header->data_size = data_size;
        header->type = HDC_STRING;
        header->rank = 1;
        header->shape[0] = str.length();
        header->buffer_size = buffer_size;
        memcpy(buffer.get() + sizeof(hdc_header_t), str.c_str(), header->data_size);
        storage->set(uuid, buffer.get(), header->buffer_size);
    }

    /**
     * @brief Sets provided string to a node. Mostly for a FORTAN purposes
     *
     * @param str p_str: char* with the string
     * @param len p_len: length of string without terminal null character.
     */
    void set_string(const char* str, size_t len)
    {
        auto data_size = len + 1;
        auto buffer_size = data_size + sizeof(hdc_header_t);
        if (storage->has(uuid)) {
            storage->remove(uuid);
        }
        auto buffer = std::make_unique<char[]>(buffer_size);
        auto header = reinterpret_cast<hdc_header_t*>(buffer.get());
        header->data_size = data_size;
        header->type = HDC_STRING;
        header->rank = 1;
        header->shape[0] = len;
        header->buffer_size = buffer_size;
        memcpy(buffer.get() + sizeof(hdc_header_t), str, len);
        *(buffer.get() + sizeof(hdc_header_t) + len) = '\0';
        storage->set(uuid, buffer.get(), header->buffer_size);
    }

    /**
    * @brief Sets data based on shape and type. Used for C bindings.
    *
    * @param shape p_shape: Shape of the data
    * @param data p_data: Pointer to data
    * @param type p_type: Type of the data (e.g. HDC_INT32)
    * @param flags p_flags: Flags the node should have (e.g. HDCFortranOrder)
    */
    void set_data_c(const std::vector<size_t>& shape, void* data, hdc_type_t type, hdc_flags_t flags = HDCDefault);

    /**
    * @brief Sets external data based on shape and type. Used for C bindings. This function stores only pointer to data, not the data itself. User is responsible for its proper (de)allocation.
    *
    * @param shape p_shape: Shape of the data
    * @param data p_data: Pointer to data
    * @param type p_type: Type of the data (e.g. HDC_INT32)
    * @param flags p_flags: Flags the node should have (e.g. HDCFortranOrder)
    */
    void set_external_c(const std::vector<size_t>& shape, void* data, hdc_type_t type, hdc_flags_t flags = HDCDefault);

    /**
     * @brief Sets data based on shape and type. Used for Python bindings.
     *
     * @param shape p_shape: Shape of the data
     * @param data p_data: Pointer to data
     * @param kind p_kind: Type of the data: "i" integer, "u" unsigned int, "f" float/double, "b" bool, "c" complex
     * @param itemsize p_itemsize: Byte size of a data item - eg. 1 (u)int8/bool/char, 2 (u)int16, 4 (u)int32/float, 8 (u)int64/double, 16 complex
     * @param flags p_flags: Flags the node should have (e.g. HDCFortranOrder)
     */
    void
    set_data_Py(const std::vector<size_t>& shape, void* data, char kind, int8_t itemsize, hdc_flags_t flags = HDCDefault);

    /**
     * @brief Sets external data based on shape and type. Used for Python bindings. This function stores only pointer to data, not the data itself. User is responsible for its proper (de)allocation.
     *
     * @param shape p_shape: Shape of the data
     * @param data p_data: Pointer to data
     * @param kind p_kind: Type of the data: "i" integer, "u" unsigned int, "f" float/double, "b" bool, "c" complex
     * @param itemsize p_itemsize: Byte size of a data item - eg. 1 (u)int8/bool/char, 2 (u)int16, 4 (u)int32/float, 8 (u)int64/double, 16 complex
     * @param flags p_flags: Flags the node should have (e.g. HDCFortranOrder)
     */
    void
    set_external_Py(const std::vector<size_t>& shape, void* data, char kind, int8_t itemsize, hdc_flags_t flags = HDCDefault);

    /**
    * @brief Sets const data based on shape and type. Used for C bindings.
    *
    * @param shape p_shape: Shape of the data
    * @param data p_data: Pointer to data
    * @param type p_type: Type of the data (e.g. HDC_INT32)
    * @param flags p_flags: Flags the node should have (e.g. HDCFortranOrder)
    */
    void set_data_c(const std::vector<size_t>& shape, const void* data, hdc_type_t type, hdc_flags_t flags = HDCDefault);

    /**
    * @brief Sets scalar data to given node.
    *
    * @param T p_T: Desired data type.
    * @param data p_data: Pointer to data
    */
    template <typename T>
    void set_data(T data)
    {
        auto data_size = sizeof(T);
        auto buffer_size = data_size + sizeof(hdc_header_t);
        auto buffer = std::make_unique<char[]>(buffer_size);
        auto header = reinterpret_cast<hdc_header_t*>(buffer.get());
        header->type = to_typeid(data);
        header->data_size = data_size;
        header->buffer_size = buffer_size;
        memcpy(buffer.get() + sizeof(hdc_header_t), &data, data_size);
        storage->set(uuid, buffer.get(), buffer_size);
    }

    /**
    * @brief Sets scalar data to given node - UDA version.
    *
    * @param data p_data: Pointer to data
    * @param _type p__type: Specification of data type (e.g. HDC_INT8)
    */
    void set_data(const unsigned char* data, hdc_type_t _type)
    {
        auto data_size = hdc_sizeof(_type);
        auto buffer_size = data_size + sizeof(hdc_header_t);
        auto buffer = std::make_unique<char[]>(buffer_size);
        auto header = reinterpret_cast<hdc_header_t*>(buffer.get());
        header->type = _type;
        header->data_size = data_size;
        header->buffer_size = buffer_size;
        memcpy(buffer.get() + sizeof(hdc_header_t), data, data_size);
        storage->set(uuid, buffer.get(), buffer_size);
    }

    /**
    * @brief Sets scalar data to given node - UDA version.
    *
    * @param data p_data: Pointer to data
    * @param type p_type: Type of the data (e.g. HDC_INT32)
    */
    void set_data(void* data, hdc_type_t type)
    {
        return this->set_data((const unsigned char*)data, type);
    }

    /**
    * @brief Returns shape of current node.
    *
    * @return std::vector< size_t >
    */
    std::vector<size_t> get_shape() const;

    /**
    * @brief Returns strides (Mostly for Python).
    *
    * @return std::vector< size_t >
    */
    std::vector<size_t> get_strides() const;

    /**
    * @brief Returns true if a a current node is not HDC_STRUCT or HDC_LIST
    *
    * @param t p_t: type
    * @return bool
    */
    static bool is_external(size_t t);

    /**
     * @brief Returns true if a buffer type is not HDC_STRUCT or HDC_LIST
     *
     * @param t p_t: type
     * @return bool
     */
    bool is_external() const;

    /**
     * @brief Returns true if contained data is scalar
     *
     * @param t p_t: type
     * @return bool
     */
    bool is_scalar() const;

    /**
    * @brief Returns true if a node is read-only.
    *
    * @return bool
    */
    bool is_readonly() const;

    /**
    * @brief Returns true if a node is FORTRAN order (Cloumn-major order), false for C/C++ like row-major order (which is default).
    *
    * @return bool
    */
    bool is_fortranorder() const;

    /**
    * @brief Prints some metadata found in header (e.g. rank, shape, data, type, flags, data/buffer size)
    *
    */
    void print_info() const;

    /**
    * @brief Adds HDC subtree as child with given path. If neccessary, recursively creates subnodes. Reference version.
    *
    * @param path p_path: path in the subtree of this node
    * @param n p_n: HDC node
    */
    void add_child(const std::string& path, const HDC& n);

    /**
    * @brief Sets HDC subtree to given path.
    *
    * @param path p_path: path in the subtree of this node
    * @param n p_n: HDC node
    */
    void set_child(const std::string& path, const HDC& n);

    /**
    * @brief Returns HDC node as hdc_t object, used mostly for bindings.
    *
    * @return hdc_t
    */
    hdc_t as_obj();

    /**
    * @brief Sets HDC node n under given index.
    *
    * @param index p_index: size_t index if this node has/should have type HDC_LIST, or std::string if this node has/should have type HDC_STRUCT
    * @param n p_n: HDC node
    */
    void set_child(size_t index, const HDC& n);

    /**
    * @brief Deletes HDC subtree.
    *
    * @param path p_path: path in the subtree of this node
    */
    bool delete_child(const std::string& path, bool prune=false);

    bool delete_slice(int index, bool prune=false);

    /**
    * @brief Returns the child node at given path.
    *
    * @param path p_path: path in the subtree of this node
    * @return HDC
    */
    HDC get(const std::string& path);

    /**
    * @brief Returns the child node at given path.
    *
    * @param path p_path: path in the subtree of this node
    * @return HDC
    */
    HDC get(const std::string& path) const;

    /**
    * @brief Returns true if subtree with given path with exists and false otherwise.
    *
    * @param path p_path: path in the subtree of this node
    * @return bool
    */
    bool exists(const std::string& path) const;

    /**
    * @brief Inserts node to i-th slice of current node.
    *
    * @param i p_i: index of position
    * @param h p_h: HDC node.
    */
    void insert(size_t i, const HDC& h);

    /**
    * @brief Appends given node as next available slice (similar to push_back() method seen in C++ STL containers).
    *
    * @param h p_h: HDC node stored at the end.
    */
    void append(const HDC& h);

    /**
    * @brief Allocates HDC node data into storage. It does nothing if sufficient space is already allocated.
    *
    * @param type p_data_size: desired size of the data to be allocated.
    */
    void alloc(size_t data_size);

    /**
    * @brief Sets HDC type of current node.
    *
    * @param type p_type: Type of the data (e.g. HDC_INT32)
    */
    void set_type(hdc_type_t type);

    /**
    * @brief Returns true if node is empty.
    *
    * @return bool
    */
    bool is_empty() const;

    /**
    * @brief Returns number of dimensions of node under path.
    *
    * @return size_t
    */
    size_t get_rank() const;

    /**
    * @brief Returns desirely typed pointer to data of this node.
    *
    * @param T p_T: Desired data type.
    * @return T
    */
    template <typename T>
    T* as() const
    {
        DEBUG_STDOUT(std::string("as<") + get_type_str() + ">()");
        if (!storage->has(uuid)) {
            throw HDCException("as(): Not found: " + boost::lexical_cast<uuid_str_t>(uuid) + "\n");
        }
        auto buffer = get_buffer();
        auto header = reinterpret_cast<hdc_header_t*>(buffer);
        auto data = buffer + sizeof(hdc_header_t);
        if (header->type == HDC_STRUCT || header->type == HDC_LIST) {
            throw HDCException("This is not a terminal node...");
        }
        T tp{};
        if (header->type != to_typeid(tp))
        {
            throw HDCException("as() stored and requested types do not match\n");
        }
        if (header->flags & HDCExternal) {
            T* result;
            memcpy(&result, data, sizeof(result));
            return result;
        } else {
            return reinterpret_cast<T*>(data);
        }
    }

    /**
     * @brief Return void pointer to data of this node.
     *
     * @return void*
     */
    void* as_void_ptr() const
    {
        hdc_header_t header = get_header(); // This is needed in Python for some reason
        auto buffer = get_buffer();
        auto data = buffer + sizeof(hdc_header_t);
        if (header.type == HDC_STRUCT || header.type == HDC_LIST) {
            throw std::runtime_error("This is not a terminal node...");
        }
        DEBUG_STDOUT(std::string("as_void_ptr()"));
        if (!storage->has(uuid)) {
            throw HDCException("as_void_ptr(): Not found: " + boost::lexical_cast<uuid_str_t>(uuid) + "\n");
        }
        if (header.flags & HDCExternal) {
            void* result;
            memcpy(&result, data, sizeof(result));
            return result;
        } else {
            return reinterpret_cast<void*>(data);
        }
    }

    /**
     * @brief Returns vector with data of this node casted to a given type. Unlike as(), the data are always copied.
     *
     * @param T p_T: Desired data type.
     * @return std::vector< T >
     */
    template <typename T>
    std::vector<T> as_vector() const
    {
        auto buffer = get_buffer();
        auto header = reinterpret_cast<hdc_header_t*>(buffer);
        if (header->type == HDC_STRUCT || header->type == HDC_LIST) {
            throw std::runtime_error("This is not a terminal node...");
        }
        DEBUG_STDOUT(std::string("as_vector<") + get_type_str() + ">()");
        if (!storage->has(uuid)) {
            throw HDCException("as_vector(): Not found: " + boost::lexical_cast<uuid_str_t>(uuid) + "\n");
        }
        auto data = buffer + sizeof(hdc_header_t);
        auto size_elem = hdc_sizeof(header->type);
        size_t n_elem = 0;
        if (size_elem) n_elem = header->data_size / size_elem;
        std::vector<T> result(n_elem);
        if (header->type == HDC_INT8) {
            // return std::vector<T>(reinterpret_cast<int8_t*>(data),reinterpret_cast<int8_t*>(data)+n_elem); // does not work on bionic
            for (size_t i = 0; i < n_elem; i++) result[i] = reinterpret_cast<int8_t*>(data)[i];
        } else if (header->type == HDC_INT16) {
            for (size_t i = 0; i < n_elem; i++) result[i] = reinterpret_cast<int16_t*>(data)[i];
        } else if (header->type == HDC_INT32) {
            for (size_t i = 0; i < n_elem; i++) result[i] = reinterpret_cast<int32_t*>(data)[i];
        } else if (header->type == HDC_INT64) {
            for (size_t i = 0; i < n_elem; i++) result[i] = reinterpret_cast<int64_t*>(data)[i];
        } else if (header->type == HDC_UINT8) {
            for (size_t i = 0; i < n_elem; i++) result[i] = reinterpret_cast<uint8_t*>(data)[i];
        } else if (header->type == HDC_UINT16) {
            for (size_t i = 0; i < n_elem; i++) result[i] = reinterpret_cast<uint16_t*>(data)[i];
        } else if (header->type == HDC_UINT32) {
            for (size_t i = 0; i < n_elem; i++) result[i] = reinterpret_cast<uint32_t*>(data)[i];
        } else if (header->type == HDC_UINT64) {
            for (size_t i = 0; i < n_elem; i++) result[i] = reinterpret_cast<uint64_t*>(data)[i];
        } else if (header->type == HDC_FLOAT) {
            for (size_t i = 0; i < n_elem; i++) result[i] = reinterpret_cast<float*>(data)[i];
        } else if (header->type == HDC_DOUBLE) {
            for (size_t i = 0; i < n_elem; i++) result[i] = reinterpret_cast<double*>(data)[i];
        } else {
            throw HDCException(
                    "as_vector(): requested unknown data conversion. This works for numerical types only...\n");
        }
        return result;
    }

    /**
    * @brief Returns scalar value.
    *
    * @param T p_T: Desired data type.
    * @return T
    */
    template <typename T>
    T as_scalar() const
    {
        auto buffer = get_buffer();
        auto header = reinterpret_cast<hdc_header_t*>(buffer);
        auto data = buffer + sizeof(hdc_header_t);
        if (header->type == HDC_STRUCT || header->type == HDC_LIST) {
            throw std::runtime_error("This is not a terminal node...");
        }
        DEBUG_STDOUT(std::string("as_scalar<") + get_type_str() + ">()");
        if (!storage->has(uuid)) {
            throw HDCException("as_scalar(): Not found: " + boost::lexical_cast<uuid_str_t>(uuid) + "\n");
        }
        if (!is_external()) {
            T result;
            memcpy(&result, data, sizeof(T));
            return result;
        } else {
            T* result;
            memcpy(&result, data, sizeof(result));
            return *result;
        }
    }

    /**
    * @brief Returns string. Needs to have separate function
    *
    * @return const std::__cxx11::string
    */
    std::string as_string() const;

    /**
    * @brief Dumps JSON to std::cout
    *
    */
    void dump() const;

    /**
    * @brief Returns string representing data/node type.
    *
    * @return const char*
    */
    const char* get_type_str() const;
    /**
    * @brief Returns vector of keys of a struct node and empty vector otherwise.
    *
    * @return std::vector< std::__cxx11::string >
    */
    std::vector<std::string> keys() const;

    /**
    * @brief Returns pointer to data buffer.
    *
    * @return char*
    */
    char* get_buffer() const;

    /**
     * @brief Sets data buffer to a node.
     *
     * @param buffer p_buffer: Pointer to buffer to save.
     */
    void set_buffer(char* buffer);

    /**
    * @brief Returns uuid string
    *
    * @return std::__cxx11::string
    */
    std::string get_uuid_str() const;

    /**
    * @brief Returns uuid
    *
    * @return boost::uuids::uuid
    */
    boost::uuids::uuid get_uuid() const;

    /**
    * @brief Increase size of underlying storage by extra_size.
    *
    * @param extra_size p_extra_size: Size to grow by
    */
    void grow(size_t extra_size);

    /**
    * @brief "Returns boost::managed_external_buffer object which usually contains addresses of child nodes."
    *
    * @return boost::interprocess::managed_external_buffer
    */
    boost::interprocess::managed_external_buffer get_segment() const;

    /**
    * @brief Returns pointer to map with stored children nodes.
    *
    * @return hdc_map_t*
    */
    hdc_map_t* get_children_ptr() const;

    /**
    * @brief Returns string containing JSON representation of this node and all its descendants.
    *
    * @param children p_children: hdc_map_t object storing children
    * @return std::__cxx11::string
    */
    static string hdc_map_to_json(hdc_map_t& children);

    /**
    * @brief Enlarges old buffer by extra_size parameter. Copies all data
    *
    * @param old_buffer p_old_buffer: pointer to oldbuffer
    * @param extra_size p_extra_size: extra size of which the new buffer size shoul increase.
    * @return char*
    */
    static std::unique_ptr<char[]> buffer_grow(char* old_buffer, size_t extra_size);

    /**
     * @brief Returns list of available serializers
     * @return std::list<std::string> List of keys of available serialization plugins
     */
    static std::vector<std::string> available_serializers();

    /**
    * @brief Loads HDC from given URI and datapath. (deserialization)
    *
    * @param uri p_uri: URI of file/redis hostname
    * @param datapath p_datapath: datapath within the file/redis addres
    * @return HDC
    */
    static HDC load(const std::string& uri, const std::string& datapath = "");

    /**
     * @brief Saves HDC to given URI and datapath. (serialization)
     *
    * @param uri p_uri: URI of file/redis hostname
    * @param datapath p_datapath: datapath within the file/redis addres
     */
    void save(const std::string& uri, const std::string& datapath = "") const;

    static HDC deserialize(const std::string& protocol, const std::string& string);

    std::string serialize(const std::string& protocol) const;

    /**
    * @brief Returns copy of HDC object, if deep_copy == true provides deep copy (i.e. copy of current node and also all children)
    *
    * @param h p_h: source node
    * @param deep_copy p_deep_copy: deep_copy == true provides deep copy (i.e. copy of current node and also all children)
    * @return HDC
    */
    static HDC copy(const HDC& h, bool deep_copy = false);

    /**
    * @brief Creates scalar HDC object
    *
    * @param data p_data: Void pointer to data
    * @return HDC
    */
    static HDC make_scalar(void* data, hdc_type_t t);

    
    /**
    * @brief Creates scalar HDC object from external scalar* data
    *
    * @param* data p_data: scalar of a intended type
    * @return HDC
    */
    static HDC make_external(float* data);

    /**
    * @brief Creates scalar HDC object from external scalar* data
    *
    * @param* data p_data: float scalar to be saved
    * @return HDC
    */
    static HDC make_external(double* data);
    /**
    * @brief Creates scalar HDC object from external scalar* data
    *
    * @param* data p_data: double scalar to be saved
    * @return HDC
    */
    static HDC make_external(bool* data);

    /**
    * @brief Creates scalar HDC object from external scalar* data
    *
    * @param* data p_data: complex scalar to be saved
    * @return HDC
    */
    static HDC make_external(std::complex<double>* data);

    /**
    * @brief Creates scalar HDC object from external scalar* data
    *
    * @param* data p_data: int8_t scalar to be saved
    * @return HDC
    */
    static HDC make_external(int8_t* data);

    /**
    * @brief Creates scalar HDC object from external scalar* data
    *
    * @param* data p_data: int16_t scalar to be saved
    * @return HDC
    */
    static HDC make_external(int16_t* data);

    /**
    * @brief Creates scalar HDC object from external scalar* data
    *
    * @param* data p_data: int32_t scalar to be saved
    * @return HDC
    */
    static HDC make_external(int32_t* data);

    /**
    * @brief Creates scalar HDC object from external scalar* data
    *
    * @param* data p_data: int64_t scalar to be saved
    * @return HDC
    */
    static HDC make_external(int64_t* data);

    /**
    * @brief Creates scalar HDC object from external scalar* data
    *
    * @param* data p_data: uint8_t scalar to be saved
    * @return HDC
    */
    static HDC make_external(uint8_t* data);

    /**
    * @brief Creates scalar HDC object from external scalar* data
    *
    * @param* data p_data: uint16_t scalar to be saved
    * @return HDC
    */
    static HDC make_external(uint16_t* data);

    /**
    * @brief Creates scalar HDC object from external scalar* data
    *
    * @param* data p_data: uint32_t scalar to be saved
    * @return HDC
    */
    static HDC make_external(uint32_t* data);

    /**
    * @brief Creates scalar HDC object from external scalar* data
    *
    * @param* data p_data: uint64_t scalar to be saved
    * @return hdc_data_t
    */
    static HDC make_external(uint64_t* data);

    /**
    * @brief Creates scalar HDC object
    *
    * @param data p_data: scalar of a intended type
    * @return HDC
    */
    static HDC make_scalar(float data);

    /**
    * @brief Creates scalar HDC object
    *
    * @param data p_data: float scalar to be saved
    * @return HDC
    */
    static HDC make_scalar(double data);
    /**
    * @brief Creates scalar HDC object
    *
    * @param data p_data: double scalar to be saved
    * @return HDC
    */
    static HDC make_scalar(bool data);

    /**
    * @brief Creates scalar HDC object
    *
    * @param data p_data: complex scalar to be saved
    * @return hdc_data_t
    */
    static HDC make_scalar(std::complex<double> data);

    /**
    * @brief Creates scalar HDC object
    *
    * @param data p_data: int8_t scalar to be saved
    * @return HDC
    */
    static HDC make_scalar(int8_t data);

    /**
    * @brief Creates scalar HDC object
    *
    * @param data p_data: int16_t scalar to be saved
    * @return HDC
    */
    static HDC make_scalar(int16_t data);

    /**
    * @brief Creates scalar HDC object
    *
    * @param data p_data: int32_t scalar to be saved
    * @return HDC
    */
    static HDC make_scalar(int32_t data);

    /**
    * @brief Creates scalar HDC object
    *
    * @param data p_data: int64_t scalar to be saved
    * @return HDC
    */
    static HDC make_scalar(int64_t data);

    /**
    * @brief Creates scalar HDC object
    *
    * @param data p_data: uint8_t scalar to be saved
    * @return HDC
    */
    static HDC make_scalar(uint8_t data);

    /**
    * @brief Creates scalar HDC object
    *
    * @param data p_data: uint16_t scalar to be saved
    * @return HDC
    */
    static HDC make_scalar(uint16_t data);

    /**
    * @brief Creates scalar HDC object
    *
    * @param data p_data: uint32_t scalar to be saved
    * @return HDC
    */
    static HDC make_scalar(uint32_t data);

    /**
    * @brief Creates scalar HDC object
    *
    * @param data p_data: uint64_t scalar to be saved
    * @return hdc_data_t
    */
    static HDC make_scalar(uint64_t data);
    /**
     * @brief Creates HDC node from given hdc_data_t object. This function stores only pointer to data, not the data itself. User is responsible for its proper (de)allocation.
     *
     * @param obj p_obj: hdc_data_t struct.
     * @return HDC
     */
    static HDC make_external(hdc_data_t obj);

    /**
     * @brief Creates HDC node of given shape, type and flags with external data. This function stores only pointer to data, not the data itself. User is responsible for its proper (de)allocation.
     *
     * @param shape p_shape: Shape.
     * @param type p_type: Data type e.g. HDC_FLOAT
     * @param flags p_flags: HDC Flags, e.g. HDCFortranOrder
     * @return HDC
     */
    static HDC make_external(const std::vector<size_t>& shape, hdc_type_t type, long flags);

    /**
    * @brief Returns pointer to storage of HDC object
    *
    * @return HDCStorage*
    */
    HDCStorage* get_storage() const { return this->storage; }

    /**
    * @brief Returns storage id
    *
    * @return size_t
    */
    size_t get_storage_id() const { return this->storage->id(); }

    /**
    * @brief Returns data packed as hdc_data_t struct
    *
    * @return hdc_data_t
    */
    hdc_data_t get_data() const;

    /**
    * @brief Sets data from hdc_data_t struct
    *
    * @param obj p_obj: Data to be set.
    */
    void set_data(hdc_data_t obj);

    /**
     * @brief Sets data from hdc_data_t struct. This function stores only pointer to data, not the data itself. User is responsible for its proper (de)allocation.
     *
     * @param obj p_obj: Data to be set.
     */
    void set_external(hdc_data_t obj);
};

#endif // HDC_HPP
