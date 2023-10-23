#include "hdc_test_common.hpp"

TEST_CASE("StringParsing", "[HDCUtils]")
{
    std::vector<bool> expected_is_num = { false, true, false, true, true, false, false };
    auto vs = split_no_brackets("flux/3444/lalala/5/4/l///l");
    CHECK(7 == vs.size());
    int i = 0;
    for (auto item : vs) {
        CHECK((item.type() == typeid(size_t)) == expected_is_num[i]);
        i++;
    }
    auto vs2 = split("flux[3444]/lalala[5][4]l///l");
    CHECK(7 == vs2.size());
    i = 0;
    for (auto item : vs2) {
        CHECK((item.type() == typeid(size_t)) == expected_is_num[i]);
        i++;
    }
}

TEST_CASE("GetPlugins","[HDC]")
{
    //HDC::search_plugins(); // This is called in main.cpp
    auto plugins = HDC::search_plugins();
    // "umap" should be always present
    CHECK(in_map("umap",plugins));
#ifdef _USE_MDBM
    // "mdbm" has been built and  should be found
    CHECK(in_map("mdbm",plugins));
#endif // _USE_MDBM
#ifdef _USE_REDIS
    CHECK(in_map("redis",plugins));
#endif // _USE_REDIS
}

TEST_CASE("EmptyNode", "[HDC]")
{
    HDC h;
    CHECK(0 == h.get_shape()[0]);
    CHECK(1 == h.get_rank());
    CHECK(HDC_EMPTY == h.get_type());
    CHECK(strcmp("null", h.get_type_str()) == 0);
    CHECK(false == h.exists("aaa"));
    CHECK(h.get_datasize() == 0);
    CHECK(h.get_size() == sizeof(hdc_header_t));
    CHECK(h.get_flags() == HDCDefault);
    CHECK(h.is_external() == false);
    CHECK(h.is_readonly() == false);
    CHECK(strcmp(h.as_string().c_str(), "null\n") == 0);
}

TEST_CASE("EmptyNodePtr", "[HDC]")
{
    HDC h;
    CHECK(0 == h.get_shape()[0]);
    CHECK(1 == h.get_rank());
    CHECK(HDC_EMPTY == h.get_type());
    CHECK(strcmp("null", h.get_type_str()) == 0);
    CHECK(false == h.exists("aaa"));
}

TEST_CASE("EmptyArrayNode", "[HDC]")
{
    std::vector<size_t> shape = { 4 };
    HDC hi8(shape, HDC_INT8);
    CHECK(1 == hi8.get_rank());
    CHECK(4 == hi8.get_shape()[0]);
    CHECK(HDC_INT8 == hi8.get_type());
    CHECK(strcmp("int8", hi8.get_type_str()) == 0);

    HDC hi32(shape, HDC_INT32);
    CHECK(1 == hi32.get_rank());
    CHECK(4 == hi32.get_shape()[0]);
    CHECK(HDC_INT32 == hi32.get_type());
    CHECK(strcmp("int32", hi32.get_type_str()) == 0);

    HDC hd = HDC(shape, HDC_DOUBLE);
    CHECK(1 == hd.get_rank());
    CHECK(4 == hd.get_shape()[0]);
    CHECK(HDC_DOUBLE == hd.get_type());
    CHECK(strcmp("float64", hd.get_type_str()) == 0);

    //check throws if more dimensions
    std::vector<size_t> sh(HDC_MAX_DIMS+1);
    for (size_t i=0; i<HDC_MAX_DIMS+1;i++) sh[i] = 1;
    CHECK_THROWS(HDC{sh,HDC_DOUBLE});
}

TEST_CASE("NodeManipulation", "[HDC]")
{
    HDC tree;
    HDC n1;
    HDC n2;
    // Try different
    CHECK(n1.get_uuid() != tree.get_uuid());
    CHECK(n2.get_uuid() != tree.get_uuid());
    CHECK(n1.get_uuid() != n2.get_uuid());

    // Try add
    tree.add_child("aaa/bbb", n1);
    CHECK(HDC_STRUCT == tree.get_type());
    CHECK(strcmp("struct", tree.get_type_str()) == 0);
    CHECK(true == tree.exists("aaa/bbb"));
    CHECK(true == tree.exists("aaa"));
    CHECK(n1.get_uuid() == tree.get("aaa/bbb").get_uuid());
    CHECK(n1.get_uuid() ==  tree.get("aaa/bbb").get_uuid());
    CHECK_THROWS(tree.get("not_here"));
    CHECK(n2.get_uuid() != tree.get("aaa/bbb").get_uuid());
    // Try add and get index
    HDC n3;
    tree.add_child("aaa/list[0]/ddd", n3);
    CHECK(tree.exists("aaa/list[0]/ddd") == true);
    HDC dummy1, dummy2, dummy3;
    tree.add_child("aaa/list[1]", dummy1);
    tree.add_child("aaa/list[0]/eee", dummy2);
    HDC list = tree.get("aaa/list");
    CHECK(HDC_LIST == list.get_type());
    tree.get("aaa/list[0]/ddd");
    CHECK(true == tree.exists("aaa/list[0]/ddd"));
    CHECK(true == tree.exists("aaa/list[0]/eee"));
    tree.get("aaa/list").get(0).add_child("kkk", dummy3); // Remove this - unnecessary
    CHECK(2LU == tree.get("aaa/list").get_shape()[0]);
    CHECK(true == tree.get("aaa/list").get(0).exists("ddd"));
    CHECK(true == tree.get("aaa/list").get(0).exists("eee"));
    // Try subtree
    HDC sub = tree.get("aaa");
    CHECK(true == sub.exists("bbb"));
    CHECK(n1.get_uuid() == sub.get("bbb").get_uuid());
    // Test set
    tree.set_child("aaa/bbb", n2);
    CHECK(true == sub.exists("bbb"));
    CHECK(n2.get_uuid() == sub.get("bbb").get_uuid());
    CHECK(n1.get_uuid() != tree.get("aaa/bbb").get_uuid());
    // Test delete
    tree.delete_child("aaa/bbb");
    CHECK(false == tree.exists("aaa/bbb"));
    CHECK(true == tree.exists("aaa"));
    tree.add_child("aaa/bbb", n1);
    tree.delete_child("aaa");
    CHECK(false == tree.exists("aaa"));
    // Try add to empty path
    HDC t;
    HDC d;
    CHECK_THROWS_AS(t.add_child("", d), HDCException);

    // check set_type wher already allocated
    HDC a(HDC_NODE_SIZE_DEFAULT);
    a.set_type(HDC_STRUCT);
    CHECK(a.get_type() == HDC_STRUCT);
}

TEST_CASE("ListManipulation", "[HDC]")
{
    HDC h;
    HDC k;
    for (size_t i = 0; i < 5; i++) {
        HDC ch("data" + std::to_string(i));
        k.append(ch);
    }
    h.add_child("k", k);

    HDC l;
    for (size_t i = 0; i < 5; i++) {
        HDC ch("data" + std::to_string(i * 10));
        l.append(ch);
    }
    h.add_child("l", l);

    CHECK(h.get("k[1]").as_string() == "data1");
    CHECK(h.get("l[1]").as_string() == "data10");
}

TEMPLATE_TEST_CASE("DataManipulation", "[HDC]", ALL_NUMERIC_TYPES)
{
    std::vector<size_t> shape = { 4 };
    TestType data[] = { 7, 20, 3, 5 };
    HDC h;
    h.set_data(shape, data);
    CHECK(to_typeid(data[0]) == h.get_type());
    CHECK(1 == h.get_rank());
    CHECK(4 == h.get_shape()[0]);
    CHECK(h.get_itemsize() == sizeof(TestType));
    TestType* data2 = h.as<TestType>();
    CHECK(memcmp(data,data2,sizeof(TestType)*4) == 0);
    data[3] = 120;
    h.set_data(shape, data);
    data2 = h.as<TestType>();
    CHECK(120 == data2[3]);
}

TEST_CASE("SetFromStruct", "[HDC]")
{
    int64_t array[5] = {1,2,3,4,5};
    hdc_data_t data;
    memset(&data,0,sizeof(hdc_data_t));
    data.rank = 1;
    data.shape[0] = 5;
    data.type = HDC_INT64;
    data.data = reinterpret_cast<char*>(array);
    HDC h(data);
    auto array2 = h.as<int64_t>();
    for (int i=0;i<5;i++) CHECK(array[i] == array2[i]);
}

TEST_CASE("SetExternal", "[HDC]")
{
    HDC node, external, e2, e3;

    int64_t array_in[4] = { 7, 2, 3, 4 };
    std::vector<size_t> shape = {4};
    node.set_data(shape,array_in);
    external.set_external(shape, array_in);
    int64_t* array_out = external.as<int64_t>();
    CHECK(*array_in == *array_out);
    CHECK(*array_in == *(int64_t*)(external.as_void_ptr()));
    CHECK(external.is_external() == true);
    auto external_str = external.serialize("json");
    auto node_str = node.serialize("json");
    CHECK(strcmp(external_str.c_str(),node_str.c_str()) == 0);

    //update
    int64_t array_in2[4] = { 70, 20, 30, 40 };
    external.set_external(shape, array_in2);
    array_out = external.as<int64_t>();
    CHECK(*array_in2 == *array_out);

    e2.set_external({1},array_in);
    array_out = e2.as<int64_t>();
    CHECK(*array_in == *array_out);

    hdc_data_t data;
    data.data = (char*)array_in;
    data.rank = 1;
    data.shape[0] = 4;
    data.flags = HDCDefault;
    data.type = HDC_INT64;
    e3.set_external(data);
    array_out = e3.as<int64_t>();
    CHECK(*array_in == *array_out);

    HDC e4 = HDC::make_external(data);
    array_out = e4.as<int64_t>();
    CHECK(*array_in == *array_out);

    //update
    hdc_data_t data2;
    memcpy(&data2,&data,sizeof(hdc_data_t));
    data2.data = (char*)array_in2;
    e3.set_external(data2);
    array_out = e3.as<int64_t>();
    CHECK(*array_in2 == *array_out);

    //constructor
    data2.flags |= HDCExternal;
    HDC e5(data2);
    array_out = e5.as<int64_t>();
    CHECK(*array_in2 == *array_out);
}

TEMPLATE_TEST_CASE("as_vector_int8", "[HDC]", ALL_NUMERIC_TYPES) {
    std::vector<int8_t> array_in = { 7, 2, 3, 4 };
    HDC h(array_in);
    auto vector_out = h.as_vector<TestType>();
    CHECK(std::equal(array_in.begin(),array_in.end(),vector_out.begin()));
}

TEMPLATE_TEST_CASE("as_vector_uint8", "[HDC]", ALL_NUMERIC_TYPES) {
    std::vector<uint8_t> array_in = { 7, 2, 3, 4 };
    HDC h(array_in);
    auto vector_out = h.as_vector<TestType>();
    CHECK(std::equal(array_in.begin(),array_in.end(),vector_out.begin()));
}

TEMPLATE_TEST_CASE("as_vector_int16", "[HDC]", ALL_NUMERIC_TYPES) {
    std::vector<int16_t> array_in = { 7, 2, 3, 4 };
    HDC h(array_in);
    auto vector_out = h.as_vector<TestType>();
    CHECK(std::equal(array_in.begin(),array_in.end(),vector_out.begin()));
}

TEMPLATE_TEST_CASE("as_vector_uint16", "[HDC]", ALL_NUMERIC_TYPES) {
    std::vector<uint16_t> array_in = { 7, 2, 3, 4 };
    HDC h(array_in);
    auto vector_out = h.as_vector<TestType>();
    CHECK(std::equal(array_in.begin(),array_in.end(),vector_out.begin()));
}

TEMPLATE_TEST_CASE("as_vector_int32", "[HDC]", ALL_NUMERIC_TYPES) {
    std::vector<int32_t> array_in = { 7, 2, 3, 4 };
    HDC h(array_in);
    auto vector_out = h.as_vector<TestType>();
    CHECK(std::equal(array_in.begin(),array_in.end(),vector_out.begin()));
}

TEMPLATE_TEST_CASE("as_vector_uint32", "[HDC]", ALL_NUMERIC_TYPES) {
    std::vector<uint32_t> array_in = { 7, 2, 3, 4 };
    HDC h(array_in);
    auto vector_out = h.as_vector<TestType>();
    CHECK(std::equal(array_in.begin(),array_in.end(),vector_out.begin()));
}

TEMPLATE_TEST_CASE("as_vector_int64", "[HDC]", ALL_NUMERIC_TYPES) {
    std::vector<int64_t> array_in = { 7, 2, 3, 4 };
    HDC h(array_in);
    auto vector_out = h.as_vector<TestType>();
    CHECK(std::equal(array_in.begin(),array_in.end(),vector_out.begin()));
}

TEMPLATE_TEST_CASE("as_vector_uint64", "[HDC]", ALL_NUMERIC_TYPES) {
    std::vector<uint64_t> array_in = { 7, 2, 3, 4 };
    HDC h(array_in);
    auto vector_out = h.as_vector<TestType>();
    CHECK(std::equal(array_in.begin(),array_in.end(),vector_out.begin()));
}

TEMPLATE_TEST_CASE("as_vector_float", "[HDC]", ALL_NUMERIC_TYPES) {
    std::vector<float> array_in = { 7, 2, 3, 4 };
    HDC h(array_in);
    auto vector_out = h.as_vector<TestType>();
    CHECK(std::equal(array_in.begin(),array_in.end(),vector_out.begin()));
}

TEMPLATE_TEST_CASE("as_vector_double", "[HDC]", ALL_NUMERIC_TYPES) {
    std::vector<double> array_in = { 7, 2, 3, 4 };
    HDC h(array_in);
    auto vector_out = h.as_vector<TestType>();
    CHECK(std::equal(array_in.begin(),array_in.end(),vector_out.begin()));
}

TEMPLATE_TEST_CASE("as_vector_complex", "[HDC]", ALL_NUMERIC_TYPES) {
    std::vector<std::complex<double>> array_in = { {7, 1}, {2, 1}, {3, 1}, {4, 1} };
    HDC h(array_in);
    // casting from std::complex is not allowed
    CHECK_THROWS(h.as_vector<TestType>());
}

TEST_CASE("as_vector_unknown", "[HDC]") {
    HDC h,n;
    h.add_child("aaa",n);
    CHECK_THROWS(h.as_vector<int8_t>());
}

TEST_CASE("StringDataManipulation", "[HDC]")
{
    HDC h;
    std::string str("lalalalala  bleble");
    h.set_string(str);
    std::string str2 = std::string(h.as_string());
    CHECK(strcmp(str.c_str(), str2.c_str()) == 0);
    CHECK(strcmp("string", h.get_type_str()) == 0);
}

TEST_CASE("GetStrides", "[HDC]")
{
    int32_t array3d[24] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };
    std::vector<size_t> shape3d = { 2, 3, 4 };
    HDC threed_f;
    threed_f.set_data<int32_t>(shape3d, array3d, HDCFortranOrder);
    auto strides_f = threed_f.get_strides();
    CHECK(4 == strides_f[0]);
    CHECK(8 == strides_f[1]);
    CHECK(24 == strides_f[2]);
    HDC threed_c;
    threed_c.set_data<int32_t>(shape3d, array3d);
    auto strides_c = threed_c.get_strides();
    CHECK(48 == strides_c[0]);
    CHECK(16 == strides_c[1]);
    CHECK(4 == strides_c[2]);
}

TEST_CASE("SliceManipulation", "[HDC]")
{
    HDC h;
    HDC sl;
    sl.set_string("1");
    HDC sl2;
    sl2.set_string("2");
    h.set_type(HDC_LIST);
    h.append(sl);
    h.append(sl2);
    CHECK(HDC_LIST == h.get_type());
    CHECK(strcmp("list", h.get_type_str()) == 0);
    CHECK(1 == h.get_rank());
    CHECK(2 == h.get_shape()[0]);
    CHECK(strcmp("1", h[0].as_string().c_str()) == 0);
    CHECK(strcmp("2", h[1].as_string().c_str()) == 0);
    HDC sl3;
    sl3.set_string("3");
    h.insert(1, sl3);
    auto keys = h.keys();
    CHECK(strcmp("3", h[1].as_string().c_str()) == 0);
    CHECK(strcmp("2", h[2].as_string().c_str()) == 0);
    HDC sl4;
    sl4.set_string("4");
    h.set_child(1, sl4);
    CHECK(strcmp("4", h[1].as_string().c_str()) == 0);
    HDC n;
    n.set_type(HDC_LIST);
    HDC ch;
    n.insert(10, ch);
    CHECK(11 == n.get_shape()[0]);
    CHECK(HDC_LIST == n.get_type());
    for (int i = 0; i < 11; i++) CHECK(HDC_EMPTY == n.get(i).get_type());
    //Check exception on assiged data
    HDC h2 = HDC::make_scalar(8.8);
    CHECK_THROWS(h2.insert(0,sl3));
}

TEST_CASE("GetKeys", "[HDC]")
{
    HDC list;
    list.set_type(HDC_LIST);
    CHECK(true == list.keys().empty());
    HDC val;
    CHECK(true == val.keys().empty());
    HDC empty;
    CHECK(true == empty.keys().empty());
    HDC tree;
    HDC h1, h2, h3;
    tree.add_child("aaa", h1);
    tree.add_child("bbb", h2);
    tree.add_child("ccc/sss", h3);
    CHECK(3u == tree.keys().size());
    vector<string> keys = tree.keys();
    for (size_t i = 0; i < keys.size(); i++) CHECK(true == tree.exists(keys[i]));
}

TEMPLATE_TEST_CASE("BracketOperatorsNum", "[HDC]", ALL_NUMERIC_TYPES)
{
    HDC tree;
    TestType num = 16;
    tree["num"] = num;
    CHECK(tree.exists("num"));
    CHECK(tree["num"].get_type() == to_typeid(num));
    CHECK(tree["num"].as_scalar<TestType>() == num);
    //test replace
    tree["num"] = static_cast<TestType>(9);
    CHECK(9 == tree["num"].as_scalar<TestType>());
}

TEST_CASE("BracketOperatorsOthers", "[HDC]")
{
    HDC tree;
    tree["aaa/bbb/ccc"];
    CHECK(tree.get_type() == HDC_STRUCT);
    CHECK(tree.exists("aaa/bbb/ccc"));
    CHECK(tree["aaa/bbb/ccc"].get_type() == HDC_EMPTY);

    HDC subtree;
    subtree["ddd"] = HDC("node");
    tree["aaa/kkk"] = subtree["ddd"];
    CHECK(strcmp(tree["aaa/kkk"].as_string().c_str(),"node") == 0);

    std::string str = "test";
    tree["str"] = str;
    tree["empty"];
    CHECK(tree.exists("str"));
    CHECK(tree.exists("empty"));
    CHECK(tree["str"].get_type() == HDC_STRING);
    CHECK(tree["empty"].get_type() == HDC_EMPTY);
    CHECK(strcmp(tree["str"].as_string().c_str(),str.c_str()) == 0);

    HDC list;
    list[0];
    list[1] = 120;
    CHECK(list[0].get_type() == HDC_EMPTY);
    CHECK(list[1].as_scalar<int8_t>() == 120);
    // test replace
    list[1] = 240l;
    CHECK(list[0].get_type() == HDC_EMPTY);
    CHECK(list[1].as_scalar<int32_t>() == 240l);
}

TEST_CASE("JsonComplete", "[HDC]")
{
    PREPARE_TREE()
    // create temporary file name
    auto fname = make_tmp_name("txt");
    auto fname2 = make_tmp_name("txt");
    // Save JSON
    tree.save("json://"+fname);
    // Load JSON
    HDC tree2 = HDC::load("json://"+fname);
    tree2.save("json://"+fname2);
    // test tree
    HDC s = tree2.get("aaa/bbb/double");

    // Test double
    CHECK(1 == s.get_rank());
    CHECK(4 == s.get_shape()[0]);
    CHECK(HDC_DOUBLE == s.get_type());
    CHECK(strcmp(tree.get("aaa/bbb/double").get_type_str(), s.get_type_str()) == 0);
    double* data_double_in = s.as<double>();
    CHECK(memcmp(data_double,data_double_in,sizeof(double)*4) == 0);

    // Test int
    s = tree2.get("aaa/bbb/int32");
    CHECK(1 == s.get_rank());
    CHECK(4 == s.get_shape()[0]);
    CHECK(HDC_INT32 == s.get_type());
    CHECK(strcmp(tree.get("aaa/bbb/int32").get_type_str(), tree2.get("aaa/bbb/int32").get_type_str()) == 0);
    int32_t* data_int_in = s.as<int32_t>();
    CHECK(memcmp(data_int32,data_int_in,sizeof(int32_t)*4) == 0);

    // Test empty
    CHECK(HDC_EMPTY == tree2["aaa/bbb/empty"].get_type());

    // Test list
    s = tree2.get("aaa/list");
    CHECK(1 == s.get_rank());
    CHECK(5 == s.get_shape()[0]);
    CHECK(HDC_LIST == s.get_type());
    CHECK(strcmp(tree.get("aaa/list").get_type_str(), tree2.get("aaa/list").get_type_str()) == 0);
    for (int i = 0; i < 5; i++)
    {
        auto t = s.get(i).get_type();
        if (i % 2 == 0) { CHECK(t == HDC_INT32); } else { CHECK(t == HDC_STRING); }
    }

    // Test string
    CHECK(strcmp(tree.get("aaa/string").as_string().c_str(), tree2.get("aaa/string").as_string().c_str()) == 0);
    auto uri = std::string("json://") + fname2;
    HDC j = HDC::load(uri,"aaa/string");
    CHECK(strcmp(j.as_string().c_str(), "Lorem ipsum dolor sit amet, consectetuer adipiscing elit.") == 0);
    if(remove(fname.c_str()) != 0) std::cerr << "Error removing file " << fname << std::endl;
    if(remove(fname2.c_str()) != 0) std::cerr << "Error removing file " << fname2 << std::endl;
}

TEST_CASE("Copy", "[HDC]")
{
    PREPARE_TREE()
    HDC _copy = tree.copy();
    auto tree_dump = tree.serialize("json");
    auto copy_dump = _copy.serialize("json");
    CHECK(strcmp(tree_dump.c_str(), copy_dump.c_str()) == 0);
    // Check also that UUIDs are the same - this behaviour can be changed later
    CHECK(tree["aaa/bbb/double"].get_uuid() != _copy["aaa/bbb/double"].get_uuid());
}

TEST_CASE("BufferGrowArray", "[HDC]")
{
    std::vector<size_t> shape = { 4 };
    double data[] = { 0.0, 1000.0, 1.0e-200, 1.0e200 };
    HDC h;
    h.set_data<double>(shape, data);
    h.grow(4096);
    double* data2 = h.as<double>();
    CHECK(h.get_datasize() == sizeof(double)*4 + 4096);
    CHECK(4 == h.get_shape()[0]);
    CHECK(h.get_type() == HDC_DOUBLE);
    CHECK(memcmp(data, data2, sizeof(double)*4) == 0);
}

TEST_CASE("BufferGrowStruct", "[HDC]")
{
    std::vector<std::string> keys = {"aaa","bbb","ccc","ddd"};
    HDC h;
    for (auto k : keys) {
        HDC ch;
        h.add_child(k,ch);
    };
    h.grow(4096);
    auto keys2 = h.keys();
    CHECK(keys2.size() == keys.size());
    CHECK(std::equal(keys.begin(),keys.end(),keys2.begin()));
}

TEST_CASE("BufferGrowList", "[HDC]")
{
    HDC h;
    size_t n = 4;
    for (size_t i=0; i<n; i++) {
        HDC ch(std::to_string(i));
        h.append(ch);
    }
    h.grow(4096);
    for (size_t i=0; i<n; i++) {
        CHECK(strcmp(h[i].as_string().c_str(), std::to_string(i).c_str()) == 0);
    }
}

TEST_CASE("GetChildren", "[HDC]")
{
    std::vector<std::string> lst = {"aaa","bbb","ccc","ddd"};
    HDC h;
    for (const auto& str: lst) {
        HDC ch(str);
        h.add_child(str,ch);
    }
    auto children = h.get_children();
    CHECK(children.size() == lst.size());
    for (size_t i=0;i<children.size();i++) {
        CHECK(strcmp(h[lst[i]].as_string().c_str(),lst[i].c_str()) == 0);
    }
}

TEST_CASE("GetSlices", "[HDC]")
{
    size_t n = 5;
    HDC h;
    for (size_t i=0;i<n;i++) {
        HDC ch(std::to_string(i));
        h.append(ch);
    }
    auto slices = h.get_slices();
    CHECK(slices.size() == n);
    for (size_t i=0;i<slices.size();i++) {
        CHECK(strcmp(h[i].as_string().c_str(),slices[i].as_string().c_str()) == 0);
    }
}

TEST_CASE("clean", "[HDC]")
{
    HDC tree;
    auto uuid = tree.get_uuid();
    tree["ch"];
    auto ch_uuid = tree["ch"].get_uuid();
    tree.clean();
    CHECK(hdc_global.storage->has(uuid) == false);
    CHECK(hdc_global.storage->has(ch_uuid) == false);
}

TEST_CASE("load", "[HDC]")
{
    CHECK_THROWS_AS(HDC::load("json://aaa|aaa", "aaa"), HDCException);
}

TEMPLATE_TEST_CASE("scalar", "[HDC]", ALL_NUMERIC_TYPES, bool)
{
    TestType    d = 111;
    HDC hd = HDC::make_scalar(d);
    CHECK(hd.as_scalar<TestType>() == d);
    CHECK(hd.get_type() == to_typeid(d));
    CHECK(hd.is_scalar());

    HDC tree, ch;
    tree.add_child("aaa",ch);
    CHECK_THROWS(tree.as_scalar<TestType>());
}

TEMPLATE_TEST_CASE("scalar_external", "[HDC]", ALL_NUMERIC_TYPES, bool)
{
    TestType    d = 111;
    HDC h;
    h.set_external(&d);
    CHECK(h.as_scalar<TestType>() == d);
    CHECK(h.get_type() == to_typeid(d));
    CHECK(h.is_scalar());

    HDC hd = HDC::make_external(&d);
    CHECK(hd.as_scalar<TestType>() == d);
    CHECK(hd.get_type() == to_typeid(d));
    CHECK(hd.is_scalar());

    HDC tree, ch;
    tree.add_child("aaa",ch);
    CHECK_THROWS(tree.as_scalar<TestType>());
}

TEST_CASE("get_data", "[HDC]")
{
    double array_in[] = { 0.0, 1000.0, 1.0e-200, 1.0e200 };
    hdc_data_t data_in;
    data_in.data = (char*)&array_in;
    data_in.type = HDC_DOUBLE;
    data_in.flags = HDCDefault;
    data_in.rank = 1;
    data_in.shape[0] = 4;
    HDC h;
    h.set_data(data_in);
    auto data_out = h.get_data();
    CHECK(data_in.type == data_out.type);
    CHECK(data_in.flags == data_out.flags);
    CHECK(data_in.rank == data_out.rank);
    CHECK(data_in.shape[0] == data_out.shape[0]);
    CHECK(memcmp(array_in,data_out.data,sizeof(double)*4) == 0);

    //update
    double array_in2[] = { 110.0, 100.0, 1.0e-20, 1.0e20 };
    data_in.data = (char*)&array_in2;
    h.set_data(data_in);
    auto data_out2 = h.get_data();
    CHECK(memcmp(array_in2,data_out2.data,sizeof(double)*4) == 0);

    //check if constructor throws if large rank supplied;
    data_in.rank = 20;
    CHECK_THROWS(HDC(data_in));

}

TEST_CASE("hdc_t_manipulation", "[HDC]")
{
    HDC h;
    auto obj = h.as_obj();
    HDC h2(obj);
    CHECK(h.get_uuid() == h2.get_uuid());
}

TEMPLATE_TEST_CASE("JSONArrays","[HDC]", bool, int32_t, double) {
    size_t n = 5;
    auto s = factorial(n+1);
    TestType* array = new TestType[s];
    for (size_t i=0; i<s; i++) array[i] = i + 0.1;

    size_t** shapes = new size_t*[n];
    for(size_t i = 0; i < n; i++) {
        shapes[i] = new size_t[n];
        memset(shapes[i],0,n*sizeof(size_t));
    }

    for (size_t d=0; d<n; d++) {
        auto r = 1;
        for (size_t i=0; i<d; i++) {
            shapes[d][i] = i+2;
            r *= (i+2);
        }
        shapes[d][d]=s/r;
    }
    for (size_t d=1; d<=n; d++) {
        std::vector<size_t> shape(&(shapes[d-1][0]),&(shapes[d-1][0])+d);
        HDC h;
        h.set_data(shape,array);
        auto fname = make_tmp_name("txt");
        auto uri = std::string("json://") + fname;
        h.save(uri);
        HDC j = HDC::load(uri,"");
        auto array_j = j.as<TestType>();
        CHECK(memcmp(array,array_j,n*sizeof(TestType)) == 0);
        auto shape_j = j.get_shape();
        CHECK(memcmp(shape_j.data(),shape.data(),shape.size()*sizeof(size_t)) == 0);
        if(remove(fname.c_str()) != 0) std::cerr << "Error removing file " << fname << std::endl;
    }
    delete[] array;
    for(size_t i = 0; i < n; i++) delete[] shapes[i];
    delete[] shapes;
}

#ifdef _USE_HDF5
TEST_CASE("HDF5Tree", "[HDC]")
{
    PREPARE_TREE()
    // create temporary file name
    auto fname = make_tmp_name("h5");
    tree.save("hdf5://" + fname);
    HDC tree2 = HDC::load("hdf5://" + fname);
    double data = tree2.get("aaa/bbb/_scalar").as<double>()[0];
    CHECK(333.333 == data);
    auto path = std::string("hdf5://")+fname+"|/data/aaa/bbb/_scalar";
    HDC h5 = HDC::load(path);
    data = h5.as<double>()[0];
    CHECK(333.333 == data);
    if(remove(fname.c_str()) != 0) std::cerr << "Error removing file " << fname << std::endl;
}

TEMPLATE_TEST_CASE("HDF5Arrays", "[HDC]", ALL_NUMERIC_TYPES) {
    size_t n = 5;
    auto s = factorial(n+1);
    TestType* array = new TestType[s];
    for (size_t i=0; i<s; i++) array[i] = i + 0.1;

    size_t** shapes = new size_t*[n];
    for(size_t i = 0; i < n; i++) {
        shapes[i] = new size_t[n];
        memset(shapes[i],0,n*sizeof(size_t));
    }

    for (size_t d=0; d<n; d++) {
        auto r = 1;
        for (size_t i=0; i<d; i++) {
            shapes[d][i] = i+2;
            r *= (i+2);
        }
        shapes[d][d]=s/r;
    }

    auto testFlags = HDCDefault;
    for (size_t d=1; d<=n; d++) {
        // create temporary file name
        auto fname = make_tmp_name("h5");
        HDC h;
        std::vector<size_t> shape(&(shapes[d-1][0]),&(shapes[d-1][0])+d);
        h.set_data<TestType>(shape,array,testFlags);
        h.save("hdf5://" + fname);
        auto uri = std::string("hdf5://") + fname;
        HDC j = HDC::load(uri,"");
        auto array_j = j.as<TestType>();
        CHECK(memcmp(array,array_j,sizeof(TestType)*n) == 0);
        auto shape_j = j.get_shape();
        CHECK(memcmp(array,array_j,shape.size()*sizeof(size_t)) == 0);
        if(remove(fname.c_str()) != 0) std::cerr << "Error removing file " << fname << std::endl;
    }

    testFlags = HDCFortranOrder;
    for (size_t d=1; d<=n; d++) {
        // create temporary file name
        auto fname = make_tmp_name("h5");
        HDC h;
        std::vector<size_t> shape(&(shapes[d-1][0]),&(shapes[d-1][0])+d);
        h.set_data<TestType>(shape,array,testFlags);
        h.save("hdf5://" + fname);
        auto uri = std::string("hdf5://") + fname;
        HDC j = HDC::load(uri,"");
        auto array_j = j.as<TestType>();
        CHECK(memcmp(array,array_j,sizeof(TestType)*n) == 0);
        auto shape_j = j.get_shape();
        CHECK(memcmp(array,array_j,shape.size()*sizeof(size_t)) == 0);
        if(remove(fname.c_str()) != 0) std::cerr << "Error removing file " << fname << std::endl;
    }
    delete[] array;
    for(size_t i = 0; i < n; i++) delete[] shapes[i];
    delete[] shapes;
}

#endif

TEST_CASE("UDA", "[HDC]")
{
#ifdef _USE_UDA
    HDC h = HDC::load("uda://HELP::help()");
    std::string expected =
            "\nHelp\tList of HELP plugin functions:\n"
            "\n"
            "services()\tReturns a list of available services with descriptions\n"
            "ping()\t\tReturn the Local Server Time in seconds and microseonds\n"
            "servertime()\tReturn the Local Server Time in seconds and microseonds\n\n";
    CHECK(h.as_string() == expected);
#else
    CHECK_THROWS(HDC::load("uda://HELP::help()"));
    CHECK_THROWS(HDC::load("uda_new://HELP::help()"));
#endif
}
