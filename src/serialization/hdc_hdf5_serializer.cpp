#include "serialization/hdc_hdf5_serializer.h"

namespace {

struct h5_read_opdata {
    unsigned recurs;     /* Recursion level.  0=root */
    struct h5_read_opdata* prev;      /* Pointer to previous opdata */
    HDC_HDF5_ADDR_T addr;       /* H5::Group address */
    // pointer to HDC node, anchors traversal to
    HDC* node;
    std::string ref_path;

    h5_read_opdata()
            : recurs{ 0 }, prev{ nullptr }, node{ nullptr }
    {}
};

void write_node(const HDC& h, H5::H5File& file, const std::string& path);
hdc_type_t hdf5_type_to_hdc_type(hid_t hdf5_dtype_id, const std::string& ref_path);
void hdf5_dataset_to_hdc(hid_t hdf5_dset_id, const std::string& ref_path, HDC& dest);
int h5_group_check(h5_read_opdata* od, HDC_HDF5_ADDR_T target_addr);
herr_t h5l_iterate_traverse_op_func(hid_t hdf5_id, const char* hdf5_path, const H5L_info_t*, void* hdf5_operator_data);
void hdf5_group_to_hdc(hid_t hdf5_group_id, const std::string& ref_path, HDC& dest);
void hdf5_tree_to_hdc(hid_t hdf5_id, const std::string& ref_path, HDC& dest);
void hdf5_read(hid_t hdf5_id, const std::string& hdf5_path, HDC& dest);
void hdf5_read(const std::string& file_path, const std::string& hdf5_path, HDC& node);
#if ((H5_VERS_MAJOR == 1) && (H5_VERS_MINOR < 12))
bool hdc_h5_addr_cmp(haddr_t addr1, haddr_t addr2)
{
    return addr1 == addr2;
}
#else
bool hdc_h5_addr_cmp(H5O_token_t& addr1, H5O_token_t& addr2)
{
    return (memcmp(addr1.__data, addr2.__data, H5O_MAX_TOKEN_SIZE) == 0);
}
#endif

void hdf5_dataset_to_hdc(hid_t hdf5_dset_id, const std::string& ref_path, HDC& dest)
{
    DEBUG_STDOUT("hdf5_dataset_to_hdc(" + to_string(hdf5_dset_id) + "," + ref_path + ",...)");
    hid_t h5_dspace_id = H5Dget_space(hdf5_dset_id);
    HDC_CHECK_HDF5_ERROR_WITH_REF(h5_dspace_id,
                                  ref_path,
                                  "Error reading HDF5 Dataspace: "
                                          << hdf5_dset_id);
    // check for empty case
    if (H5Sget_simple_extent_type(h5_dspace_id) == H5S_NULL) {
        // change to empty
        dest.set_type(HDC_EMPTY);
    } else {

        hid_t h5_dtype_id = H5Dget_type(hdf5_dset_id);

        if (H5Tget_class(h5_dtype_id) == H5T_REFERENCE) {
            // Inspired by this: https://www.physics.ohio-state.edu/~wilkins/computing/HDF/hdf5tutorial/reftoobj.html
            size_t nelems = H5Sget_simple_extent_npoints(h5_dspace_id);
            size_t rank = H5Sget_simple_extent_ndims(h5_dspace_id);
            if (rank > 1) throw HDCException("Cannot handle array of refferences of rank > 1");
            std::vector<hobj_ref_t> ref_out(nelems);
            hid_t h5_status = H5Dread(hdf5_dset_id, H5T_STD_REF_OBJ, H5S_ALL, H5S_ALL, H5P_DEFAULT, ref_out.data());
            HDC_CHECK_HDF5_ERROR_WITH_REF(h5_status,
                                          ref_path,
                                          "Error reading HDF5 Dataset: "
                                                  << hdf5_dset_id);
            dest.set_type(HDC_LIST);
            for (size_t i = 0; i < nelems; i++) {
#if ((H5_VERS_MAJOR == 1) && (H5_VERS_MINOR < 10))
                auto dsetv_id = H5Rdereference(hdf5_dset_id, H5R_OBJECT, &ref_out[i]);
#else
                auto dsetv_id = H5Rdereference2(hdf5_dset_id, H5P_DEFAULT, H5R_OBJECT, &ref_out[i]);
#endif
                HDC h;
                hdf5_tree_to_hdc(dsetv_id, "", h);
                dest.append(h);
            }
            return;
        }
        HDC_CHECK_HDF5_ERROR_WITH_REF(h5_dtype_id,
                                      ref_path,
                                      "Error reading HDF5 Datatype: "
                                              << hdf5_dset_id);

        size_t nelems = H5Sget_simple_extent_npoints(h5_dspace_id);
        size_t rank = H5Sget_simple_extent_ndims(h5_dspace_id);
        hdc_type_t dt = hdf5_type_to_hdc_type(h5_dtype_id, ref_path);
        if (dt == HDC_STRING) nelems++;
        hid_t h5_status = 0;
        char* buffer = new char[nelems * hdc_sizeof(dt)];
        memset(buffer, 0, nelems * hdc_sizeof(dt)); //Necessary for pyhdc.__str__()
        h5_status = H5Dread(hdf5_dset_id, h5_dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer);
        std::vector<hsize_t> hshape(rank);
        H5Sget_simple_extent_dims(h5_dspace_id, hshape.data(), nullptr);
        std::vector<size_t> shape(rank);
        for (size_t i = 0; i < rank; i++) shape[i] = hshape[i];
        if (dt == HDC_STRING) {
            dest.set_string(buffer);
        } else { dest.set_data_c(shape, buffer, dt); } //TODO: do something more inteligent here
        delete[] buffer;
        HDC_CHECK_HDF5_ERROR_WITH_REF(h5_status,
                                      ref_path,
                                      "Error reading HDF5 Dataset: "
                                              << hdf5_dset_id);

        HDC_CHECK_HDF5_ERROR_WITH_REF(H5Tclose(h5_dtype_id),
                                      ref_path,
                                      "Error closing HDF5 Datatype: "
                                              << h5_dtype_id);

    }

    HDC_CHECK_HDF5_ERROR_WITH_REF(H5Sclose(h5_dspace_id),
                                  ref_path,
                                  "Error closing HDF5 Dataspace: "
                                          << h5_dspace_id);

}

hdc_type_t hdf5_type_to_hdc_type(hid_t hdf5_dtype_id, const std::string& ref_path UNUSED)
{
    DEBUG_STDOUT("hdf5_type_to_hdc_type(" + to_string(hdf5_dtype_id) + "," + ref_path + ")");
    hdc_type_t res;
    if (H5Tequal(hdf5_dtype_id, H5T_STD_I8LE)) {
        res = HDC_INT8;
    } else if (H5Tequal(hdf5_dtype_id, H5T_STD_I16LE)) {
        res = HDC_INT16;
    } else if (H5Tequal(hdf5_dtype_id, H5T_STD_I32LE)) {
        res = HDC_INT32;
    } else if (H5Tequal(hdf5_dtype_id, H5T_STD_I64LE)) {
        res = HDC_INT64;
    } else if (H5Tequal(hdf5_dtype_id, H5T_STD_U8LE)) {
        res = HDC_UINT8;
    } else if (H5Tequal(hdf5_dtype_id, H5T_STD_U16LE)) {
        res = HDC_UINT16;
    } else if (H5Tequal(hdf5_dtype_id, H5T_STD_U32LE)) {
        res = HDC_UINT32;
    } else if (H5Tequal(hdf5_dtype_id, H5T_STD_U64LE)) {
        res = HDC_UINT64;
    } else if (H5Tequal(hdf5_dtype_id, H5T_IEEE_F32LE)) {
        res = HDC_FLOAT;
    } else if (H5Tequal(hdf5_dtype_id, H5T_IEEE_F64LE)) {
        res = HDC_DOUBLE;
    } else if (H5Tequal(hdf5_dtype_id, H5T_C_S1)) {
        res = HDC_STRING;
    } else if (H5Tget_class(hdf5_dtype_id) == H5T_class_t::H5T_COMPOUND){
        res = HDC_COMPLEX;
    } else if (H5Tequal(hdf5_dtype_id, H5T_ENUM)) {
        //TODO: Here it can be probably more things.
        res = HDC_BOOL;
    } else {
        throw HDCException("Error with HDF5 DataType to DataType Leaf Conversion");
    }
    return res;
}

int h5_group_check(h5_read_opdata* od, HDC_HDF5_ADDR_T target_addr)
{
    DEBUG_STDOUT("h5_group_check(h5_read_opdata *od, HDC_HDF5_ADDR_T target_addr)");
    if (hdc_h5_addr_cmp(od->addr,target_addr)) {
        /* Addresses match */
        return 1;
    } else if (!od->recurs) {
        /* Root group reached with no matches */
        return 0;
    } else {
        /* Recursively examine the next node */
        return h5_group_check(od->prev, target_addr);
    }
}

herr_t h5l_iterate_traverse_op_func(hid_t hdf5_id, const char* hdf5_path, const H5L_info_t*, void* hdf5_operator_data)
{
    DEBUG_STDOUT(
            "h5_literate_traverse_op_func(hid_t hdf5_id,const char *hdf5_path,const H5L_info_t *, void *hdf5_operator_data)");
    herr_t h5_status = 0;
    herr_t h5_return_val = 0;
    H5O_info_t h5_info_buf;
    /* Type conversion */
    h5_read_opdata* h5_od = (struct h5_read_opdata*) hdf5_operator_data;


    /*
     * Get type of the object and display its name and type.
     * The name of the object is passed to this function by
     * the Library.
     */
#if ((H5_VERS_MAJOR == 1) && (H5_VERS_MINOR < 12))
    h5_status = H5Oget_info_by_name(hdf5_id, hdf5_path, &h5_info_buf, H5P_DEFAULT);
#else
    h5_status = H5Oget_info_by_name(hdf5_id, hdf5_path, &h5_info_buf, H5O_INFO_ALL, H5P_DEFAULT);
#endif

    HDC_CHECK_HDF5_ERROR_WITH_REF(h5_status,
                                  h5_od->ref_path,
                                  "Error fetching HDF5 Object info: "
                                          << " parent: " << hdf5_id
                                          << " path:" << hdf5_path);

    std::string chld_ref_path = h5_od->ref_path +
                                std::string("/") +
                                std::string(hdf5_path);
    switch (h5_info_buf.type) {
        case H5O_TYPE_GROUP: {
            /*
             * Check group address against linked list of operator
             * data structures.  We will always run the check, as the
             * reference count cannot be relied upon if there are
             * symbolic links, and H5Oget_info_by_name always follows
             * symbolic links.  Alternatively we could use H5Lget_info
             * and never recurse on groups discovered by symbolic
             * links, however it could still fail if an object's
             * reference count was manually manipulated with
             * H5Odecr_refcount.
             */
            if (h5_group_check(h5_od, h5_info_buf.HDC_HDF5_ADDR_NAME))
            {
                // skip cycles in the graph ...
            } else {
                hid_t h5_group_id = H5Gopen(hdf5_id,
                                            hdf5_path,
                                            H5P_DEFAULT);
                HDC_CHECK_HDF5_ERROR_WITH_REF(h5_group_id,
                                              h5_od->ref_path,
                                              "Error opening HDF5 "
                                                      << "H5::Group: "
                                                      << " parent: "
                                                      << hdf5_id
                                                      << " path:"
                                                      << hdf5_path);

                // execute traversal for this group
                HDC chld_node;
                h5_od->node->add_child(hdf5_path, chld_node);
                hdf5_group_to_hdc(h5_group_id,
                                  chld_ref_path,
                                  chld_node);

                // close the group
                HDC_CHECK_HDF5_ERROR_WITH_REF(H5Gclose(h5_group_id),
                                              h5_od->ref_path,
                                              "Error closing HDF5 "
                                                      << "H5::Group: "
                                                      << h5_group_id);
            }
            break;
        }
        case H5O_TYPE_DATASET: {
            HDC leaf;
            h5_od->node->add_child(hdf5_path, leaf);
            // open hdf5 dataset at path
            hid_t h5_dset_id = H5Dopen(hdf5_id,
                                       hdf5_path,
                                       H5P_DEFAULT);

            HDC_CHECK_HDF5_ERROR_WITH_REF(h5_dset_id,
                                          h5_od->ref_path,
                                          "Error opening HDF5 "
                                                  << " Dataset: "
                                                  << " parent: "
                                                  << hdf5_id
                                                  << " path:"
                                                  << hdf5_path);

            hdf5_dataset_to_hdc(h5_dset_id,
                                chld_ref_path,
                                leaf);

            // close the dataset
            HDC_CHECK_HDF5_ERROR_WITH_REF(H5Dclose(h5_dset_id),
                                          h5_od->ref_path,
                                          "Error closing HDF5 "
                                                  << " Dataset: "
                                                  << h5_dset_id);
            break;
        }
        default: {
            throw HDCException("h5_literate_traverse_op_func(): Unsupported H5O_TYPE\n");
        }
    }

    return h5_return_val;
}

void hdf5_group_to_hdc(hid_t hdf5_group_id, const std::string& ref_path, HDC& dest)
{
    DEBUG_STDOUT("void hdf5_group_to_hdc(" + to_string(hdf5_group_id) + "," + ref_path + "," + ")");
    //dest.set_type(HDC_EMPTY);

    // get info, we need to get the obj token for cycle tracking
    H5O_info_t h5_info_buf;
#if ((H5_VERS_MAJOR == 1) && (H5_VERS_MINOR < 12))
    herr_t h5_status = H5Oget_info(hdf5_group_id, &h5_info_buf);
#else
    herr_t h5_status = H5Oget_info(hdf5_group_id, &h5_info_buf, H5O_INFO_ALL);
#endif
    // setup the callback struct we will use for  H5Literate
    struct h5_read_opdata h5_od;
    // setup linked list tracking that allows us to detect cycles
    h5_od.recurs = 0;
    h5_od.prev = nullptr;
    h5_od.addr = h5_info_buf.HDC_HDF5_ADDR_NAME;
    // attach the pointer to our node
    h5_od.node = &dest;
    // keep ref path
    h5_od.ref_path = ref_path;

    H5_index_t h5_grp_index_type = H5_INDEX_NAME;

    // check for creation order index using propertylist

    hid_t h5_gc_plist = H5Gget_create_plist(hdf5_group_id);

    if (HDC_HDF5_VALID_ID(h5_gc_plist)) {
        unsigned int h5_gc_flags = 0;
        h5_status = H5Pget_link_creation_order(h5_gc_plist,
                                               &h5_gc_flags);

        // first make sure we have the link creation order plist
        if (HDC_HDF5_STATUS_OK(h5_status)) {
            // check that we have both order_tracked and order_indexed
            if (h5_gc_flags & (H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED)) {
                // if so, we can use creation order in h5literate
                h5_grp_index_type = H5_INDEX_CRT_ORDER;
            }
        }

        HDC_CHECK_HDF5_ERROR_WITH_REF(H5Pclose(h5_gc_plist),
                                      ref_path,
                                      "Failed to close HDF5 "
                                              << "H5P_GROUP_CREATE "
                                              << "property list: "
                                              << h5_gc_plist);
    }

    // use H5Literate to traverse
    h5_status = H5Literate(hdf5_group_id,
                           h5_grp_index_type,
                           H5_ITER_INC,
                           nullptr,
                           h5l_iterate_traverse_op_func,
                           (void*)&h5_od);

    HDC_CHECK_HDF5_ERROR_WITH_REF(h5_status,
                                  ref_path,
                                  "Error calling H5Literate to "
                                          << "traverse and read HDF5 "
                                          << "hierarchy: "
                                          << hdf5_group_id);
}

void hdf5_tree_to_hdc(hid_t hdf5_id, const std::string& ref_path, HDC& dest)
{
    DEBUG_STDOUT("hdf5_tree_to_hdc(hid_t hdf5_id, const std::string  &ref_path, HDC& dest)");
    herr_t h5_status = 0;
    H5O_info_t h5_info_buf;
#if ((H5_VERS_MAJOR == 1) && (H5_VERS_MINOR < 12))
    h5_status = H5Oget_info(hdf5_id, &h5_info_buf);
#else
    h5_status = H5Oget_info(hdf5_id, &h5_info_buf, H5O_INFO_ALL);
#endif
    HDC_CHECK_HDF5_ERROR_WITH_REF(h5_status, ref_path, "Error fetching HDF5 object " << "info from: " << hdf5_id);
    switch (h5_info_buf.type) {
        // use a H5Literate traversal
        case H5O_TYPE_GROUP: {
            hdf5_group_to_hdc(hdf5_id, ref_path, dest);
            break;
        }
            // if hdf5_id + hdf5_path points directly to a dataset
            // skip the H5Literate traversal
        case H5O_TYPE_DATASET: {
            hdf5_dataset_to_hdc(hdf5_id, ref_path, dest);
            break;
        }
            // unsupported types
        case H5O_TYPE_UNKNOWN: {
            HDC_HDF5_ERROR(ref_path, "Cannot read HDF5 Object : " << "(type == H5O_TYPE_UNKNOWN )");
        }
        case H5O_TYPE_NAMED_DATATYPE: {
            HDC_HDF5_ERROR(ref_path, "Cannot read HDF5 Object " << "(type == H5O_TYPE_NAMED_DATATYPE )");
        }
        case H5O_TYPE_NTYPES: {
            HDC_HDF5_ERROR(ref_path,
                           "Cannot read HDF5 Object " << "(type == H5O_TYPE_NTYPES [This is an invalid HDF5 type!]");
        }
        default: {
            HDC_HDF5_ERROR(ref_path, "Cannot read HDF5 Object (type == Unknown )");
        }
    }
}

void hdf5_read(hid_t hdf5_id, const std::string& hdf5_path, HDC& dest)
{
    DEBUG_STDOUT("hdf5_read(hid_t hdf5_id, std::string hdf5_path, HDC& dest)");
    // get hdf5 object at path, then call read_hdf5_tree_into_conduit_node
    hid_t h5_child_obj = H5Oopen(hdf5_id, hdf5_path.c_str(), H5P_DEFAULT);
    HDC_CHECK_HDF5_ERROR(h5_child_obj, "Failed to fetch HDF5 object from: " << hdf5_id << ":" << hdf5_path);
    hdf5_tree_to_hdc(h5_child_obj, hdf5_path, dest);
    HDC_CHECK_HDF5_ERROR(H5Oclose(h5_child_obj), "Failed to close HDF5 Object: " << h5_child_obj);
}

void hdf5_read(const std::string& file_path, const std::string& hdf5_path, HDC& node)
{
    DEBUG_STDOUT("hdf5_read(const std::string& file_path, const std::string& hdf5_path, HDC& node)");
    // open file
    hid_t h5_file_id = H5Fopen(file_path.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    HDC_CHECK_HDF5_ERROR(h5_file_id, "Error opening HDF5 file for reading: " << file_path);
    hdf5_read(h5_file_id, hdf5_path, node);
    // close the hdf5 file
    HDC_CHECK_HDF5_ERROR(H5Fclose(h5_file_id), "Error closing HDF5 file: " << file_path);
}

const char* ref_group_name = "__hdc";

H5::DataType HDCtype2HDF5(hdc_type_t t)
{
    switch (t) {
        case HDC_STRUCT:
        case HDC_LIST:
            return H5::PredType::STD_REF_OBJ;
        case HDC_BOOL:
            return H5::PredType::NATIVE_UINT_LEAST8;
        case HDC_INT8:
            return H5::PredType::NATIVE_INT_LEAST8;
        case HDC_INT16:
            return H5::PredType::NATIVE_INT_LEAST16;
        case HDC_INT32:
            return H5::PredType::NATIVE_INT_LEAST32;
        case HDC_INT64:
            return H5::PredType::NATIVE_INT_LEAST64;
        case HDC_UINT8:
            return H5::PredType::NATIVE_UINT_LEAST8;
        case HDC_UINT16:
            return H5::PredType::NATIVE_UINT_LEAST16;
        case HDC_UINT32:
            return H5::PredType::NATIVE_UINT_LEAST32;
        case HDC_UINT64:
            return H5::PredType::NATIVE_UINT_LEAST64;
        case HDC_FLOAT:
            return H5::PredType::NATIVE_FLOAT;
        case HDC_DOUBLE:
            return H5::PredType::NATIVE_DOUBLE;
        case HDC_COMPLEX:
        {
            auto dtype = H5::CompType(2*sizeof(double));
            dtype.insertMember("real", 0, H5::PredType::NATIVE_DOUBLE);
            dtype.insertMember("imaginary", sizeof(double), H5::PredType::NATIVE_DOUBLE);
            return dtype;
        }
        case HDC_STRING:
            return H5::PredType::C_S1;
        case HDC_EMPTY:
            return H5::PredType::NATIVE_DOUBLE;
        default:
            throw HDCException("hdf5_tree_to_hdc(): Unknown data type: " + std::to_string(t) + "\n");
    }
    return H5::PredType::NATIVE_UINT_LEAST32;
}

bool pathExists(hid_t id, const std::string& path)
{
  return H5Lexists( id, path.c_str(), H5P_DEFAULT ) > 0;
}

void write_node(const HDC& h, H5::H5File& file, const std::string& path)
{
    auto buffer = h.get_buffer();
    auto header = reinterpret_cast<hdc_header_t*>(buffer);
    auto data = buffer + sizeof(hdc_header_t);
    std::vector<char> new_buffer(header->buffer_size);
    if (h.is_fortranorder()) {
        transpose_buffer(new_buffer.data() + sizeof(hdc_header_t), h.get_buffer() + sizeof(hdc_header_t), h.get_rank(),
                         h.get_shape(), (hdc_type_t)h.get_type(),
                         h.is_fortranorder());
        data = new_buffer.data() + sizeof(hdc_header_t);
    }
    H5std_string DATASET_NAME(path);
    try {
        if (header->type == HDC_STRUCT) {
            hdc_map_t* children = h.get_children_ptr();
            if (children != nullptr) {
                H5::Group group(file.createGroup(path));
                for (const auto& child : children->get<1>()) {
                    auto key = child.key.c_str();
                    auto uuid = child.address;
                    write_node(HDC(hdc_global.storage, uuid), file, path + "/" + key);
                }
            }
            return;
        } else if (header->type == HDC_LIST) {
            hdc_map_t* children = h.get_children_ptr();
            if (children != nullptr) {
                H5::Group ref_group;
                if (pathExists(file.getId(), ref_group_name))
                    ref_group=file.openGroup(ref_group_name);
                else
                    ref_group=file.createGroup(ref_group_name);
                size_t n_child = children->size();
                auto wbuf = new hobj_ref_t[n_child];
                size_t i = 0;
                for (const auto& child : children->get<1>()) {
                    auto uuid = child.address;
                    //TODO: We could probably re-do this using get_children() method
                    HDC hh(hdc_global.storage, uuid);
                    auto _uuid = hh.get_uuid_str();
                    std::string full_path = std::string(ref_group_name) + "/" + _uuid;
                    write_node(hh, file, full_path.c_str());
                    auto ret = H5Rcreate(&wbuf[i], file.getId(), full_path.c_str(), H5R_OBJECT, -1);
                    if (ret < 0) {
                        throw HDCException("Could not create refference to " + full_path);
                    }
                    i++;
                }

                hsize_t ref_dims[1];
                ref_dims[0] = n_child;
                H5::DataSpace ref_dataspace(1, ref_dims);
                H5::DataSet ref_dataset = file.createDataSet(DATASET_NAME, H5::PredType::STD_REF_OBJ, ref_dataspace);
                ref_dataset.write(wbuf, H5::PredType::STD_REF_OBJ);
                delete[] wbuf;
            }
            return;
        } else if (header->type == HDC_EMPTY) {
            hsize_t dims1[] = { 1 };
            H5::DataSpace edataspace(1, dims1);
            double edata[] = { NAN };
            H5::DataSet dataset = file.createDataSet(DATASET_NAME, H5::PredType::NATIVE_DOUBLE, edataspace);
            dataset.write(edata, H5::PredType::NATIVE_DOUBLE);
        } else {
            hsize_t rank = header->rank;
            hsize_t dimsf[10];
            for (hsize_t i = 0; i < rank; i++) {
                dimsf[i] = header->shape[i];
            };
            H5::DataSpace dataspace(rank, dimsf);
            auto t = HDCtype2HDF5(header->type);
            H5::DataSet dataset = file.createDataSet(DATASET_NAME, t, dataspace);
            dataset.write(data, t);
            return;
        }
    }  // end of try block
        // catch failure caused by the H5::H5File operations
    catch (H5::FileIException& error) {
#ifdef H5_VERSION_GE
    #if H5_VERSION_GE(1, 8, 13)
        H5::Exception::printErrorStack();
    #else
        std::cerr << "write_node(): " << "FileIException" << std::endl;
    #endif
#else
        std::cerr << "write_node(): " << "FileIException" << std::endl;
#endif
    }
        // catch failure caused by the DataSet operations
    catch (H5::DataSetIException& error) {
#ifdef H5_VERSION_GE
    #if H5_VERSION_GE(1, 8, 13)
        H5::Exception::printErrorStack();
    #else
        std::cerr << "write_node(): " << "DataSetIException" << std::endl;
    #endif
#else
        std::cerr << "write_node(): " << "DataSetIException" << std::endl;
#endif
    }
        // catch failure caused by the DataSpace operations
    catch (H5::DataSpaceIException& error) {
#ifdef H5_VERSION_GE
    #if H5_VERSION_GE(1, 8, 13)
        H5::Exception::printErrorStack();
    #else
        std::cerr << "write_node(): " << "DataSpaceIException" << std::endl;
    #endif
#else
        std::cerr << "write_node(): " << "DataSpaceIException" << std::endl;
#endif
    }
        // catch failure caused by the DataSpace operations
    catch (H5::DataTypeIException& error) {
#ifdef H5_VERSION_GE
    #if H5_VERSION_GE(1, 8, 13)
        H5::Exception::printErrorStack();
    #else
        std::cerr << "write_node(): " << "DataTypeIException" << std::endl;
    #endif
#else
        std::cerr << "write_node(): " << "DataTypeIException" << std::endl;
#endif
    }
}

} // anon namespace

HDC hdc::serialization::HDF5Serialiser::deserialize(const std::string& filename, const std::string& datapath)
{
    DEBUG_STDOUT("from_hdf5(const std::string& filename, const std::string& datapath)");
    HDC hdc;
    if (!datapath.empty()) {
        hdf5_read(filename, datapath, hdc);
    } else {
        hdf5_read(filename, "/data", hdc);
    }
    return hdc;
}

void hdc::serialization::HDF5Serialiser::serialize(const HDC& hdc, const std::string& filename, const std::string& datapath UNUSED)
{
    try {
        H5std_string h5_filename(filename);
        H5::H5File file(h5_filename, H5F_ACC_TRUNC);
        write_node(hdc, file, "data");
    } catch (H5::FileIException& error) {
#ifdef H5_VERSION_GE
    #if H5_VERSION_GE(1, 8, 13)
        H5::Exception::printErrorStack();
    #else
        std::cerr << "to_hdf5(): " << "FileIException" << std::endl;
    #endif
#else
        std::cerr << "to_hdf5(): " << "FileIException" << std::endl;
#endif
        exit(-1);
    }
}

std::string hdc::serialization::HDF5Serialiser::to_string(const HDC& hdc UNUSED)
{
    return {};
}

HDC hdc::serialization::HDF5Serialiser::from_string(const std::string& string UNUSED)
{
    return HDC();
}
