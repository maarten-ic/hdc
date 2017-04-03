#include "hdc.hpp"
#include <fstream>
#include <memory>
//#define DEBUG
struct hdc_t {
    void* obj;
};


using namespace std;

HDCStorage* global_storage = nullptr;

/** Initializes global_storage  -- mainly due to C and Fortran */
void HDC_init(string pluginFileName, string pluginSettingsFileName) {

    // First , try to load the file under filename, if not exists try some paths
    string pluginPath = "";
    
    if (boost::filesystem::exists(pluginFileName)) {
        // OK, load this
        pluginPath = boost::filesystem::absolute(pluginFileName).string();
    } else {
        // Never mind, try some default paths -- Now I don't know how do this better...
        boost::filesystem::path p(pluginFileName);
        string strippedName = p.filename().string();
        vector<string> pluginSearchPath;
        pluginSearchPath.push_back("./");
        pluginSearchPath.push_back("./plugins");
        pluginSearchPath.push_back(".config/hdc/plugins");
        pluginSearchPath.push_back("/usr/local/lib");
        pluginSearchPath.push_back("/usr/lib");
        pluginSearchPath.push_back("/usr/local/lib64");
        pluginSearchPath.push_back("/usr/lib64");
        pluginSearchPath.push_back("/usr/local/lib/hdc");
        pluginSearchPath.push_back("/usr/lib/hdc");
        pluginSearchPath.push_back("/usr/local/lib64/hdc");
        pluginSearchPath.push_back("/usr/lib64/hdc");
        // Search all paths and stop if found
        for (auto path : pluginSearchPath) {
            string tmp = path+'/'+strippedName;
            if (boost::filesystem::exists(tmp)) {
                cout << "Plugin found: " << tmp << endl;
                pluginPath = tmp;
                break;
            }
        }
    }
    // If selected, check whether file settings file exists
    if (pluginSettingsFileName.size() != 0) {
        if (!boost::filesystem::exists(pluginSettingsFileName)) {
            cerr << "Settings file set, but does not exist: " << pluginSettingsFileName << endl;
            cerr << "Using default configuration...\n";
        }
    }

    global_storage = new HDCStorage(pluginPath,pluginSettingsFileName);
    printf("HDC_init(): HDC storage initialized.\n");
}

/** Cleans up global_storage  -- mainly due to C and Fortran */
void HDC_destroy() {
    delete global_storage;
    printf("HDC_destroy(): HDC storage destroyed.\n");
}

//---------------------------- HDC class -----------------------------------

/** Creates empty HDC with specified buffer size */
HDC::HDC(size_t _data_size) {
    // fill some data
    memset(&header,0,sizeof(header_t));
    header.buffer_size = _data_size + sizeof(header_t);
    header.data_size = _data_size;
    header.ndim = 1;
    
    if (global_storage == nullptr) {
       HDC_init("./plugins/libMDBMPlugin.so","./plugins/settings.txt");
//         HDC_init();
        atexit(HDC_destroy);
    }
    
    // Start by creating segment
    char* buffer = new char[header.buffer_size];

    // copy header there -- we need that, hopefully it will be optimized out
    memcpy(buffer,&header,sizeof(header_t));
    
    //Store to some storage
    uuid = generate_uuid_str();
    storage = global_storage;
    storage->set(uuid,buffer,header.buffer_size);
    // Now it is safe to
    if (!storage->usesBuffersDirectly()) delete[] buffer;
}

/** Default constructor. Creates empty HDC */
HDC::HDC(): HDC(0lu) {};

/** Creates empty HDC with specified type and shape */
HDC::HDC(int _ndim, size_t* _shape, TypeID _type,long _flags) {
    if (_ndim >= HDC_MAX_DIMS) {
        cerr << "Unsupported number of dimensions: " << _ndim << endl;
        exit(-2);
    }
    size_t elem_size = 1;
    memset(&header,0,sizeof(header_t));
    for (int i = 0; i < _ndim; i++) {
        header.shape[i] = _shape[i];
        elem_size *= _shape[i];
    }
    header.type = _type;
    header.flags = _flags;
    header.ndim = _ndim;
    header.data_size = elem_size * hdc_sizeof(_type);
    header.buffer_size = header.data_size + sizeof(header_t);
    char* buffer = new char[header.buffer_size];
    memcpy(buffer,&header,sizeof(header_t));
    uuid = generate_uuid_str();
    storage = global_storage;
    storage->set(uuid,buffer,header.buffer_size);
    if (!storage->usesBuffersDirectly()) delete[] buffer;
}

/** Creates empty HDC with specified buffer size */
HDC::HDC(string str) {
    // fill some data
    memset(&header,0,sizeof(header_t));
    size_t _data_size = str.length()+1;
    header.buffer_size = _data_size + sizeof(header_t);
    header.data_size = _data_size;
    header.ndim = 1;
    header.type = STRING_ID;
    
    if (global_storage == nullptr) {
//        HDC_init("./plugins/libMDBMPlugin.so","./plugins/settings.txt");
        HDC_init();
        atexit(HDC_destroy);
    }
    
    // Start by creating segment
    char* buffer = new char[header.buffer_size];

    // copy header there -- we need that, hopefully it will be optimized out
    memcpy(buffer,&header,sizeof(header_t));
    // Copy char* data
    memcpy(buffer+sizeof(header_t),str.c_str(),header.data_size);
    //Store to some storage
    uuid = generate_uuid_str();
    storage = global_storage;
    storage->set(uuid,buffer,header.buffer_size);
    // Now it is safe to
    if (!storage->usesBuffersDirectly()) delete[] buffer;
}

HDC::HDC(char* src_buffer) {
    storage = global_storage;
    uuid = generate_uuid_str();
    memcpy(&header,src_buffer,sizeof(header_t));
    auto buffer = new char[header.buffer_size];

    if (header.type == STRUCT_ID || header.type == LIST_ID) {
        try {
            bip::managed_external_buffer src_segment(bip::open_only,src_buffer+sizeof(header_t),header.buffer_size-sizeof(header_t));
            map_t* src_children = src_segment.find<map_t>("d").first;
            if (src_children == nullptr) {
                cout << "This node has no children." << endl;
                exit(50);
            }
            bip::managed_external_buffer segment(bip::create_only,buffer+sizeof(header_t),header.buffer_size-sizeof(header_t));
            auto children = segment.construct<map_t>("d")(map_t::ctor_args_list(),map_t::allocator_type(segment.get_segment_manager()));
            for (map_t::iterator it = src_children->get<by_key>().begin(); it != src_children->get<by_key>().end(); ++it) {
                HDC n(storage,it->address.c_str());
                record rec(it->key.c_str(),n.get_uuid().c_str(),segment.get_segment_manager());
                children->insert(rec);
            }
            memcpy(buffer,&header,sizeof(header_t));
        }
        catch(...)
        {
            cout << "Exception in HDC(HDC*)" << endl;
            exit(1);
        }
    } else {
        memcpy(buffer,src_buffer,header.buffer_size);
    }

    storage->set(uuid,buffer,header.buffer_size);
}

/** Copy contructor */
HDC::HDC(HDC* h) : HDC(h->get_buffer()) {};

/** Deserializing constructor */
HDC::HDC(HDCStorage* _storage, string _uuid) {
    uuid = _uuid;
    storage = _storage;
    memcpy(&header,storage->get(uuid),sizeof(header_t));
}

/** Destructor */
HDC::~HDC() {
    //cout << "destructor called\n";
    /*map_t* children;
    try {
        children = get_children_ptr();
    } catch(...) {
        return;
    };
    for (map_t::iterator it = children->begin(); it != children->end(); ++it) {
        storage->remove(it->address.c_str());
    }*/
    //storage->remove(uuid); // This is responsibility of storage from now
}

//---------------------------- Header information ----------------------------------
size_t HDC::get_datasize() {
    return header.data_size;
}
size_t HDC::get_size() {
    return header.buffer_size;
}
/** Returns type of current node. */
size_t HDC::get_type() {
    return header.type;
}
size_t HDC::get_flags()
{
    return header.flags;
}
template<typename T>
T* HDC::get_data() {
    return reinterpret_cast<T*>(get_buffer()+sizeof(header_t));
}

bool HDC::is_external() {
    return (header.flags & HDCExternal) != 0;
}
bool HDC::is_readonly() {
    return (header.flags & HDCReadOnly) != 0;
}
bool HDC::is_fortranorder() {
    return (header.flags & HDCFortranOrder) != 0;
}
void HDC::info() {
    printf("Size:\t\t%zu\n",header.buffer_size);
    printf("NDim:\t\t%d\n",get_ndim());
    printf("Shape:\t\t"); for (int i=0;i<HDC_MAX_DIMS;i++) printf("%zu,",get_shape()[i]);
    printf("\n");
    printf("Data Size:\t\t%zu\n",get_datasize());
    printf("External:\t\t%d\n",is_external());
    printf("ReadOnly:\t\t%d\n",is_readonly());
    printf("FortranOrder:\t%d\n",is_fortranorder());
    return;
}

//---------------------------- Tree manipulation -----------------------------------



bool HDC::has_child(string path)
{
    return has_child(split(path,'/'));
}

bool HDC::has_child(vector<string> vs)
{
    D(
    printf("has_child(");
    for (size_t i = 0; i < vs.size()-1; i++) printf("%s/",vs[i].c_str());
    printf("%s",vs[vs.size()-1].c_str());
    printf(")\n");
    )
    if(vs.empty()) return false; //TODO: re-do this!!!
    if (header.type != HDC_STRUCT && header.type != HDC_LIST) return false;
    string first = vs[0];
    vs.erase(vs.begin());
    map_t* children = get_children_ptr();
    if (children == nullptr) return false;
    if (children->count(first.c_str()) == 0) return false;

    if (vs.empty()) {
        return (bool)children->count(first.c_str());
    } else {
        try {
            auto it = children->find(first.c_str());
            if (it != children->end()) {
                HDC ch(storage,it->address.c_str());
                return ch.has_child(vs);
            }
            else return false; // TODO Create error HDC obj here???
        } catch (...) {
            std::cerr << "has_child(): Caught exception" << "\n";
            return false;
        }
    }
    return false; // never goes here
}

void HDC::add_child(vector<string> vs, HDC* n) {
    D(
    printf("### add_child(");
    for (long i = 0; i < vs.size()-1; i++) printf("%s/",vs[i].c_str());
    printf("%s",vs[vs.size()-1].c_str());
    printf(")\n");
    printf("%s\n",get_uuid().c_str());
    )
    // sync buffer
    auto buffer = storage->get(uuid);
    memcpy(&header,buffer,sizeof(header_t));
    int old_size = header.buffer_size;
    if (!(header.type == EMPTY_ID || header.type == STRUCT_ID)) {
        cout << "Cannot add child to this node. Data assigned???" << endl;
        return;
    }
    if (header.type == EMPTY_ID) set_type(STRUCT_ID);

    string first = vs[0];
    if (first.size() > 1024) {
        cout << "add_child(): string too long.\n";
        exit(111);
    }
    vs.erase(vs.begin());

    // load new buffer
    buffer = storage->get(uuid);
    memcpy(&header,buffer,sizeof(header_t));
    
    bip::managed_external_buffer segment(bip::open_only,buffer+sizeof(header_t),0);
    auto children = segment.find<map_t>("d").first;

    if (!vs.empty()) { // Create intermediate nodes here
        HDC h;
        add_child(first,h);
        get(first).add_child(vs,n);
    } else {
        if (children->count(first.c_str()) == 0) {
            // Try to grow buffer HDC_MAX_RESIZE_ATTEMPTS times, die if it does not help
            int redo = 1;
            for (int i=0;i<HDC_MAX_RESIZE_ATTEMPTS-1;i++) {
                if (redo == 0) break;
                try {
                    // Storing record larger than 114 bytes can mess up the heap on small buffers (1300b, key with longer than 24 characters), so we need to increase the buffer size here
                    if (segment.get_free_memory() < 4*first.size()) throw (hdc_bad_alloc()); // There can be problem to store large strings
                    record rec(first.c_str(),n->get_uuid().c_str(),segment.get_segment_manager());
                    children->insert(rec);
                    redo = 0;
                } catch (hdc_bad_alloc e) {
                    char* new_buffer = buffer_grow(buffer,max(header.buffer_size,4*first.size()));
                    if (!storage->usesBuffersDirectly()) delete[] buffer;
                    buffer = new_buffer;
                    memcpy(&header,buffer,sizeof(header_t));
                    segment = bip::managed_external_buffer(bip::open_only,buffer+sizeof(header_t),0);
                    children = segment.find<map_t>("d").first;
                    redo = 1;
                }
                if (redo == 1 && i == HDC_MAX_RESIZE_ATTEMPTS-1) {
                    fprintf(stderr,"add_child(): Could not allocate enough memory.\n");
                    exit(8);
                }
            }
        }
        if (header.buffer_size != old_size) storage->set(uuid,buffer,header.buffer_size);
    }
    return;
}

void HDC::add_child(vector<string> vs, HDC& n) {
    D(
    printf("### add_child(");
    for (long i = 0; i < vs.size()-1; i++) printf("%s/",vs[i].c_str());
    printf("%s",vs[vs.size()-1].c_str());
    printf(")\n");
    printf("%s\n",get_uuid().c_str());
    )
    // sync buffer
    auto buffer = storage->get(uuid);
    memcpy(&header,buffer,sizeof(header_t));
    int old_size = header.buffer_size;
    if (!(header.type == EMPTY_ID || header.type == STRUCT_ID)) {
        cout << "Cannot add child to this node. Data assigned???" << endl;
        return;
    }
    if (header.type == EMPTY_ID) set_type(STRUCT_ID);

    string first = vs[0];
    if (first.size() > 1024) {
        cout << "add_child(): string too long.\n";
        exit(111);
    }
    vs.erase(vs.begin());

    // load new buffer
    buffer = storage->get(uuid);
    memcpy(&header,buffer,sizeof(header_t));
    
    bip::managed_external_buffer segment(bip::open_only,buffer+sizeof(header_t),0);
    auto children = segment.find<map_t>("d").first;

    if (!vs.empty()) { // Create intermediate nodes here
        HDC h;
        add_child(first,h);
        get(first).add_child(vs,n);
    } else {
        if (children->count(first.c_str()) == 0) {
            // Try to grow buffer HDC_MAX_RESIZE_ATTEMPTS times, die if it does not help
            int redo = 1;
            for (int i=0;i<HDC_MAX_RESIZE_ATTEMPTS-1;i++) {
                if (redo == 0) break;
                try {
                    // Storing record larger than 114 bytes can mess up the heap on small buffers (1300b, key with longer than 24 characters), so we need to increase the buffer size here
                    if (segment.get_free_memory() < 4*first.size()) throw (hdc_bad_alloc()); // There can be problem to store large strings
                    record rec(first.c_str(),n.get_uuid().c_str(),segment.get_segment_manager());
                    children->insert(rec);
                    redo = 0;
                } catch (hdc_bad_alloc e) {
                    char* new_buffer = buffer_grow(buffer,max(header.buffer_size,4*first.size()));
                    if (!storage->usesBuffersDirectly()) delete[] buffer;
                    buffer = new_buffer;
                    memcpy(&header,buffer,sizeof(header_t));
                    segment = bip::managed_external_buffer(bip::open_only,buffer+sizeof(header_t),0);
                    children = segment.find<map_t>("d").first;
                    redo = 1;
                }
                if (redo == 1 && i == HDC_MAX_RESIZE_ATTEMPTS-1) {
                    fprintf(stderr,"add_child(): Could not allocate enough memory.\n");
                    exit(8);
                }
            }
        }
        if (header.buffer_size != old_size) storage->set(uuid,buffer,header.buffer_size);
    }
    return;
}

vector<string> HDC::keys() {
    vector<string> k;
    map_t* children;
    children = get_children_ptr();
    if (children == nullptr) return k;
    k.reserve(children->size());

    for (map_t::iterator it = children->begin(); it != children->end(); ++it) {
        k.push_back(it->key.c_str());
    }
    return k;
}

void HDC::add_child(string path, HDC* n)
{
    DEBUG_STDOUT("add_child("+path+")\n");
    add_child(split(path,'/'),n);
    return;
}

void HDC::add_child(string path, HDC& n)
{
    DEBUG_STDOUT("add_child("+path+")\n");
    add_child(split(path,'/'),n);
    return;
}

void HDC::delete_child(vector<string> vs) {
    D(
    printf("delete_child(");
    for (size_t i = 0; i < vs.size()-1; i++) printf("%s/",vs[i].c_str());
    printf("%s",vs[vs.size()-1].c_str());
    printf(")\n");
    )
    if (!has_child(vs) || vs.empty())  {
        return;
    }
    string first = vs[0];
    vs.erase(vs.begin());
    map_t* children = get_children_ptr();
    if (vs.empty()) {
        auto it = children->find(first.c_str());
        if (it!=children->end()) {
            storage->remove(it->address.c_str());
            children->erase(it);
        }
    } else {
        get(first.c_str()).delete_child(vs);
    }
    // set type back to empty if the only child was deleted.
    //if (children->empty()) set_type(EMPTY_ID); Not sure if to do this
    return;
}

void HDC::delete_child(string path) {
    delete_child(split(path,'/'));
    return;
}

HDC* HDC::get_ptr(vector<string> vs) {
    D(
    printf("get(");
    for (size_t i = 0; i < vs.size()-1; i++) printf("%s/",vs[i].c_str());
    printf("%s",vs[vs.size()-1].c_str());
    printf(")\n");
    )
    string first = vs[0];
    vs.erase(vs.begin());
    char* buffer = storage->get(uuid);
    header_t h;
    memcpy(&h,buffer,sizeof(header_t));
    
    auto segment = bip::managed_external_buffer(bip::open_only,buffer+sizeof(header_t),0);

    map_t* children = segment.find<map_t>("d").first;
    if (children == nullptr) {
        cout << "This node has no children." << endl;
        exit(50);
    }
    if (children->count(first.c_str())) {
        auto rec = children->find(first.c_str());
        //cout << rec->key << " " << rec->address << "\n";
        string child_uuid = rec->address.c_str();
        if (vs.empty()) {
            return new HDC(storage,child_uuid);
        }
        else {
            HDC child(storage,child_uuid);
            return child.get_ptr(vs);
        }
    } else {
        cout << "Not found" << endl;
        exit(50);
        return new HDC();
    }
}

HDC HDC::get(vector<string> vs) {
    D(
    printf("get(");
    for (size_t i = 0; i < vs.size()-1; i++) printf("%s/",vs[i].c_str());
    printf("%s",vs[vs.size()-1].c_str());
    printf(")\n");
    )
    string first = vs[0];
    vs.erase(vs.begin());
    char* buffer = storage->get(uuid);
    header_t h;
    memcpy(&h,buffer,sizeof(header_t));

    auto segment = bip::managed_external_buffer(bip::open_only,buffer+sizeof(header_t),0);

    map_t* children = segment.find<map_t>("d").first;
    if (children == nullptr) {
        cout << "This node has no children." << endl;
        exit(50);
    }
    if (children->count(first.c_str())) {
        auto rec = children->find(first.c_str());
        //cout << rec->key << " " << rec->address << "\n";
        string child_uuid = rec->address.c_str();
        if (vs.empty()) {
            HDC nn(storage,child_uuid);
            return nn;
        }
        else {
            HDC child(storage,child_uuid);
            return child.get(vs);
        }
    } else {
        cout << "Not found" << endl;
        exit(50);
        return HDC();
    }
}

HDC* HDC::get_slice(vector<string> vs, size_t i) {
    D(
    printf("get_slice(");
    for (size_t i = 0; i < vs.size()-1; i++) printf("%s/",vs[i].c_str());
    printf("%s",vs[vs.size()-1].c_str());
    printf(",%d)\n",i);
    )
    string first = vs[0];
    vs.erase(vs.begin());
    map_t* children = get_children_ptr();
    if (children->count(first.c_str())) {
        if (vs.empty()) {
            if (header.type != HDC_LIST) return this;
            if (i > children->size()) {
                cout << "Error: index out of range!" << endl;
                exit(50);
                return new HDC();
            }
            return new HDC(storage,children->get<by_index>()[i].address.c_str());
        }
        else return get(first).get_slice(vs,i);
    } else {
        printf("Not found: get_slice(");
        for (size_t i = 0; i < vs.size()-1; i++) printf("%s/",vs[i].c_str());
        printf("%s",vs[vs.size()-1].c_str());
        printf(",%zu)\n",i);
        return new HDC();
    }
    return new HDC();
}

HDC* HDC::get_slice(size_t i) {
    DEBUG_STDOUT("get_slice("+to_string(i)+")\n");
    map_t* children = get_children_ptr();
    if (header.type == LIST_ID) return new HDC(storage,children->get<by_index>()[i].address.c_str());
    return this; // return this if not list
}

HDC* HDC::get_slice(string path, size_t i) {
    return get_slice(split(path,'/'),i);
}

HDC* HDC::get_ptr(string path) {
    return get_ptr(split(path,'/'));
}

HDC HDC::get(string path) {
    return get(split(path,'/'));
}

void HDC::set_child(vector<string> vs, HDC* n) {
    D(
    printf("set_child(");
    for (size_t i = 0; i < vs.size()-1; i++) printf("%s/",vs[i].c_str());
    printf("%s",vs[vs.size()-1].c_str());
    printf(")\n");
    )
    if (!has_child(vs)) { // Nothing to set
        cout << "Nothing to set, maybe you want to add..." << endl;
        return;
    }
    string first = vs[0];
    vs.erase(vs.begin());
    bip::managed_external_buffer::allocator<record>::type ca = get_segment().get_allocator<record>();
    map_t* children = get_children_ptr();
    if (vs.empty()) {
        // check whether children container exists
        if (children == nullptr) children = get_children_ptr();
        // check whether given path exists
        if (children->count(first.c_str())) {
            shared_string str(n->get_uuid().c_str(),ca); // TODO wrap this up to try catch block
            children->modify(children->find(first.c_str()), change_node(str));
        } else {
            // TODO: get_allocator -- viz vyse...
            children->insert(record(first.c_str(),n->get_uuid().c_str(),ca));
        }
    } else get(first).set_child(vs, n);
    return;
}


void HDC::set_child(string path, HDC* n) {
    set_child(split(path,'/'), n);
    return;
}

//------------------ Data manipulation -------------------------------


void HDC::set_type(size_t _type) {
    // More to be added here later
    DEBUG_STDOUT("set_type("+to_string(header.type)+" -> "+to_string(_type)+")\n");
    char* old_buffer = storage->get(uuid);
    memcpy(&header,old_buffer,sizeof(header_t)); //sync header
    if (header.type == _type) return; // Nothing to do
    header.type = _type;
    char* new_buffer;
    memcpy(old_buffer,&header,sizeof(header_t)); //sync header back
    if (header.type == STRUCT_ID || header.type == LIST_ID) {
        if (header.data_size < HDC_NODE_SIZE_DEFAULT) {
            new_buffer = new char[HDC_NODE_SIZE_DEFAULT];
            header.data_size = HDC_NODE_SIZE_DEFAULT - sizeof(header_t);
            header.buffer_size = HDC_NODE_SIZE_DEFAULT;
            memcpy(new_buffer,&header,sizeof(header_t));
        }
        else
            new_buffer = old_buffer;
        try {
            bip::managed_external_buffer segment(bip::create_only,new_buffer+sizeof(header_t),header.data_size);
            map_t* children = segment.construct<map_t>("d")(map_t::ctor_args_list(),map_t::allocator_type(segment.get_segment_manager())); // TODO: Wrap this to auto-growing???
        }
        catch(std::exception e) {
            cout << "set_type(): " << e.what()<< endl;
            exit(66);
        }
    } else printf("Skipping\n");
    storage->set(uuid,new_buffer,header.buffer_size);
    if (new_buffer != old_buffer)
    if (!storage->usesBuffersDirectly()) delete[] new_buffer;
    return;
}

void* HDC::as_void_ptr() {
    return (void*)this;
}

void HDC::dump() {
    cout << to_json(0) << endl;
}

void HDC::resize(HDC* h, int recursively)
{
    // TODO: discuss this with Jakub and redo this.
    fprintf(stderr,"resize(): not implemented yet...\n");
    exit(-3);
    return;
}

HDC* HDC::copy(int copy_arrays) {
    return new HDC(this);
}

void HDC::set_data_c(int _ndim, size_t* _shape, void* _data, size_t _type) {
    D(printf("set_data_c(%d, {%d,%d,%d}, %f, %s)\n",_ndim,_shape[0],_shape[1],_shape[2],((double*)_data)[0],hdc_type_str(static_cast<TypeID>(_type)).c_str());)
    if (storage->has(uuid)) storage->remove(uuid);
    header.type = _type;
    header.ndim = _ndim;
    header.data_size = hdc_sizeof(to_typeid(_type));
    for (int i = 0; i < _ndim; i++) {
        header.data_size *= _shape[i];
        header.shape[i] = _shape[i];
    }
    header.buffer_size = header.data_size + sizeof(header_t);
    char* buffer = new char[header.buffer_size];
    memcpy(buffer,&header,sizeof(header_t));
    memcpy(buffer+sizeof(header_t),_data,header.data_size);
    storage->set(uuid,buffer,header.buffer_size);
    return;
}

void HDC::set_data_c(string path, int _ndim, size_t* _shape, void* _data, size_t _type) {
    if(!has_child(path)) {
        HDC h;
        add_child(path, h); // TODO: add constructor for this!!
    }
    get(path).set_data_c(_ndim, _shape, _data, _type);
}

void HDC::insert_slice(size_t i, HDC* h)
{
    DEBUG_STDOUT("insert_slice("+to_string(i)+")\n");
    memcpy(&header,storage->get(uuid),sizeof(header_t));
    size_t old_size = header.buffer_size;
    if (header.type != HDC_EMPTY && header.type != HDC_LIST) {
        cout << "Error in insert_slice(): Wrong type to call insert_slice." << endl;
        exit(7);
    }
    if (header.type == HDC_EMPTY) set_type(LIST_ID);

    auto buffer = storage->get(uuid);
    memcpy(&header,buffer,sizeof(header_t));

    bip::managed_external_buffer segment(bip::open_only,buffer+sizeof(header_t),0);
    auto children = segment.find<map_t>("d").first;

    int k = 0;
    int redo = 1;
    for (k=0;k<HDC_MAX_RESIZE_ATTEMPTS-1;k++) {
        if (redo == 0) break;
        try {
//             if (segment.get_free_memory() < 256) throw hdc_bad_alloc();
            map_t::nth_index<1>::type& ri=children->get<1>();
            ri.insert(ri.begin()+i,record(to_string(i).c_str(),h->get_uuid().c_str(),segment.get_segment_manager()));
            redo = 0;
        } catch (exception e) {
            char* new_buffer = buffer_grow(buffer, header.buffer_size);
            if (!storage->usesBuffersDirectly()) delete[] buffer;
            buffer = new_buffer;
            memcpy(&header,buffer,sizeof(header_t));
            segment = bip::managed_external_buffer(bip::open_only,buffer+sizeof(header_t),0);
            children = segment.find<map_t>("d").first;
            redo = 1;
        }
    }
    if (redo == 1 && k >= HDC_MAX_RESIZE_ATTEMPTS-1) {
        fprintf(stderr,"add_child(): Could not allocate enough memory.\n");
        exit(8);
    }
    header.shape[0] = children->size();
    memcpy(buffer,&header,sizeof(header_t));
//     cout << header.buffer_size << endl;
    if (header.buffer_size != old_size) storage->set(uuid,buffer,header.buffer_size);
    return;
}

void HDC::set_slice(size_t i, HDC* h)
{
    DEBUG_STDOUT("set_slice("+to_string(i)+")\n");
    if (header.type != LIST_ID) {
        cout << "set_slice() called on non list type node\n";
        throw exception();
    };
    auto children = get_children_ptr();
    bip::managed_external_buffer::allocator<record>::type ca = get_segment().get_allocator<record>();
    shared_string str(h->get_uuid().c_str(),ca); // TODO try-catch this (or make factory object/function???)
    if (i >= children->size()) {
        cout << "Error in set_slice: Index " << i << " >= list size." << endl;
        throw exception();
    }
    children->modify(children->iterator_to(children->get<by_index>()[i]), change_node(str));
    return;
}

void HDC::append_slice(HDC* h) {
    memcpy(&header,storage->get(uuid),sizeof(header_t));
    insert_slice(header.shape[0],h);
    return;
}

hdc_t* HDC::as_hdc_ptr() {
    hdc_t* wrap = new struct hdc_t;
    wrap->obj = (void*) this;
    return wrap;
}

string HDC::get_type_str() {
    switch(header.type) {
        case EMPTY_ID:
            return "null";
        case STRUCT_ID:
            return "struct";
        case LIST_ID:
            return "list";
        case INT8_ID:
            return "int8";
        case INT16_ID:
            return "int16";
        case INT32_ID:
            return "int32";
        case INT64_ID:
            return "int64";
        case UINT8_ID:
            return "int8";
        case UINT16_ID:
            return "int16";
        case UINT32_ID:
            return "int32";
        case UINT64_ID:
            return "int64";
        case FLOAT_ID:
            return "float32";
        case DOUBLE_ID:
            return "float64";
        case STRING_ID:
            return "string";
        case BOOL_ID:
            return "bool";
        case ERROR_ID:
            return "error";
        default:
            return "unknown";
    };
    return "unknown";
}

string HDC::get_type_str(string path) {
    return get(path).get_type_str();
}

string HDC::get_datashape_str(string path) {
    return get(path).get_datashape_str();
}

string HDC::get_datashape_str() {
    // TODO: redo this
    return "get_datashape_str() is not implemented yet.";
}

int HDC::get_ndim() {
    memcpy(&header,storage->get(uuid),sizeof(header_t)); // TODO: do this in more sophisticately
    return header.ndim;
}

size_t* HDC::get_shape() {
    memcpy(&header,storage->get(uuid),sizeof(header_t));
    return header.shape;
}

int HDC::get_ndim(string path) {
    //TODO: make more error-proof - add has check -> make it as function???
    memcpy(&header,storage->get(uuid),sizeof(header_t));
    return get(path).get_ndim();
}

size_t* HDC::get_shape(string path) {
    memcpy(&header,storage->get(uuid),sizeof(header_t));
    return get(path).get_shape();
}


size_t HDC::childs_count()
{
    return get_children_ptr()->size();
}

char* HDC::get_buffer() {
    return storage->get(uuid);
}
string HDC::get_uuid() {
    return uuid;
}

// allocator stuff
bip::managed_external_buffer HDC::get_segment() {
    char* buffer = storage->get(uuid);
    bip::managed_external_buffer segment;
    // attach managed buffer
    try {
        segment = bip::managed_external_buffer(bip::open_only, buffer+sizeof(header_t), 0);
    }
    catch (std::exception& e) {
        std::cerr << "get_segment(): Caught: " << e.what() << "\n";
        exit(1);
        segment = bip::managed_external_buffer(bip::create_only, buffer+sizeof(header_t), header.buffer_size-sizeof(header_t));
    }
    // FIXME: not necessary
    return segment;
}

map_t* HDC::get_children_ptr() {
    if (header.type != HDC_STRUCT && header.type != HDC_LIST) return nullptr;
    char* buffer = storage->get(uuid);
    auto segment = bip::managed_external_buffer(bip::open_only, buffer+sizeof(header_t), header.buffer_size-sizeof(header_t));
    return segment.find<map_t>("d").first;
}
/* Grows underlying storage by given extra size, it does nothing if extra_size <= 0.*/
void HDC::grow(size_t extra_size) {
    if (extra_size <= 0) return;
    char* old_buffer = storage->get(uuid);
    memcpy(&header,old_buffer,sizeof(header_t));
    auto new_size = header.data_size + extra_size;
    D(printf("Growing %luB->%luB\n",header.data_size,new_size);)
    char* new_buffer = buffer_grow(old_buffer, extra_size);
    memcpy(&header,new_buffer,sizeof(header_t));
    storage->set(uuid,new_buffer,new_size);
    if (!storage->usesBuffersDirectly()) delete[] old_buffer;
    if (!storage->usesBuffersDirectly()) delete[] new_buffer;
    return;
}


char* HDC::get_data_ptr() {
    if (header.data_size == 0) return nullptr;
    return (char*)(storage->get(uuid)+sizeof(header_t));
}

void HDC::delete_data() {
    auto buffer = storage->get(uuid);
    memcpy(&header,buffer,sizeof(header_t));
    if ((header.type == HDC_LIST || header.type == HDC_STRUCT) && header.data_size > 0) {
        try {
            auto segment = bip::managed_external_buffer(bip::open_only,buffer+sizeof(header_t),0);
            map_t* children = segment.find<map_t>("d").first;
            map_t::nth_index<1>::type& ri=children->get<1>();
            for (auto it = ri.begin(); it != ri.end(); ++it) {
                HDC h(storage,it->address.c_str());
                h.delete_data();
            }
        } catch (std::exception& e) {
            std::cerr << "Caught exception in delete_data(): " << e.what() << std::endl;
            exit(8);
        }
    }
    storage->remove(uuid);
    uuid = "";
}
/* grows buffer provided buffer (copies to larger), it does nothing if extra_size <= 0.*/
char* buffer_grow(char* old_buffer, size_t extra_size) {
    DEBUG_STDOUT("grow_buffer(extra_size = "+to_string(extra_size)+")\n");
    if (extra_size <= 0 || old_buffer == nullptr) return old_buffer;
    //load header
    header_t header;
    memcpy(&header,old_buffer,sizeof(header_t));
    auto new_size = header.data_size + extra_size;
    char* new_buffer = new char[new_size];
    //memset(new_buffer,0,new_size);
    // if there were children, resize the segment
    if ((header.type == HDC_LIST || header.type == HDC_STRUCT) && header.data_size > 0) {
        try {
            // try to open old children
            auto old_segment = bip::managed_external_buffer(bip::open_only,old_buffer+sizeof(header_t),0);
            map_t* old_children = old_segment.find<map_t>("d").first;
            // if there are some, copy them
            if (old_children != nullptr) {
                auto new_segment = bip::managed_external_buffer(bip::create_only,new_buffer+sizeof(header_t),new_size);
                map_t* new_children = new_segment.construct<map_t>("d")(map_t::ctor_args_list(),new_segment.get_segment_manager());
                map_t::nth_index<1>::type& ri=old_children->get<1>();
                for (auto it = ri.begin(); it != ri.end(); ++it) {
                    record rec(it->key.c_str(),it->address.c_str(),new_segment.get_segment_manager());
                    new_children->insert(rec);
                }
            } else cout << "buffer_grow(): children not found\n";
        } catch (std::exception& e) {
            std::cerr << "Caught exception in buffer_grow( extra_size = " << extra_size << "): " << e.what() << std::endl;
            exit(8);
        }
    } else {
        // copy old data to new buffer or something like this here, throw warning now
        DEBUG_STDOUT("Warning: buffer size increased, but no data copied!!!\n");
    }
    // finalize header and copy it to the new buffer
    header.data_size = new_size;
    header.buffer_size = new_size+sizeof(header_t);
    memcpy(new_buffer,&header,sizeof(header_t));
    return new_buffer;
}
