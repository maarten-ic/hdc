/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class dev_libhdc_HDC */

#ifndef _Included_dev_libhdc_HDC
#define _Included_dev_libhdc_HDC
#ifdef __cplusplus
extern "C" {
#endif
#undef dev_libhdc_HDC_HDC_EMPTY
#define dev_libhdc_HDC_HDC_EMPTY 0L
#undef dev_libhdc_HDC_HDC_STRUCT
#define dev_libhdc_HDC_HDC_STRUCT 1L
#undef dev_libhdc_HDC_HDC_LIST
#define dev_libhdc_HDC_HDC_LIST 2L
#undef dev_libhdc_HDC_HDC_INT8
#define dev_libhdc_HDC_HDC_INT8 3L
#undef dev_libhdc_HDC_HDC_INT16
#define dev_libhdc_HDC_HDC_INT16 4L
#undef dev_libhdc_HDC_HDC_INT32
#define dev_libhdc_HDC_HDC_INT32 5L
#undef dev_libhdc_HDC_HDC_INT64
#define dev_libhdc_HDC_HDC_INT64 6L
#undef dev_libhdc_HDC_HDC_UINT8
#define dev_libhdc_HDC_HDC_UINT8 7L
#undef dev_libhdc_HDC_HDC_UINT16
#define dev_libhdc_HDC_HDC_UINT16 8L
#undef dev_libhdc_HDC_HDC_UINT32
#define dev_libhdc_HDC_HDC_UINT32 9L
#undef dev_libhdc_HDC_HDC_UINT64
#define dev_libhdc_HDC_HDC_UINT64 10L
#undef dev_libhdc_HDC_HDC_FLOAT
#define dev_libhdc_HDC_HDC_FLOAT 11L
#undef dev_libhdc_HDC_HDC_DOUBLE
#define dev_libhdc_HDC_HDC_DOUBLE 12L
#undef dev_libhdc_HDC_HDC_STRING
#define dev_libhdc_HDC_HDC_STRING 13L
#undef dev_libhdc_HDC_HDC_BOOL
#define dev_libhdc_HDC_HDC_BOOL 14L
#undef dev_libhdc_HDC_HDC_ERROR
#define dev_libhdc_HDC_HDC_ERROR 15L
/*
 * Class:     dev_libhdc_HDC
 * Method:    create
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_dev_libhdc_HDC_create
  (JNIEnv *, jobject, jstring);

/*
 * Class:     dev_libhdc_HDC
 * Method:    add_child
 * Signature: (Ljava/lang/String;Ldev/libhdc/HDC;)V
 */
JNIEXPORT void JNICALL Java_dev_libhdc_HDC_add_1child
  (JNIEnv *, jobject, jstring, jobject);

/*
 * Class:     dev_libhdc_HDC
 * Method:    set_child
 * Signature: (Ljava/lang/String;Ldev/libhdc/HDC;)V
 */
JNIEXPORT void JNICALL Java_dev_libhdc_HDC_set_1child
  (JNIEnv *, jobject, jstring, jobject);

/*
 * Class:     dev_libhdc_HDC
 * Method:    delete_child
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_dev_libhdc_HDC_delete_1child
  (JNIEnv *, jobject, jstring);

/*
 * Class:     dev_libhdc_HDC
 * Method:    append
 * Signature: (Ldev/libhdc/HDC;)V
 */
JNIEXPORT void JNICALL Java_dev_libhdc_HDC_append
  (JNIEnv *, jobject, jobject);

/*
 * Class:     dev_libhdc_HDC
 * Method:    get
 * Signature: (Ljava/lang/String;)Ldev/libhdc/HDC;
 */
JNIEXPORT jobject JNICALL Java_dev_libhdc_HDC_get__Ljava_lang_String_2
  (JNIEnv *, jobject, jstring);

/*
 * Class:     dev_libhdc_HDC
 * Method:    get
 * Signature: (J)Ldev/libhdc/HDC;
 */
JNIEXPORT jobject JNICALL Java_dev_libhdc_HDC_get__J
  (JNIEnv *, jobject, jlong);

/*
 * Class:     dev_libhdc_HDC
 * Method:    keys
 * Signature: ()Ljava/util/ArrayList;
 */
JNIEXPORT jobject JNICALL Java_dev_libhdc_HDC_keys
  (JNIEnv *, jobject);

/*
 * Class:     dev_libhdc_HDC
 * Method:    exists
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_dev_libhdc_HDC_exists
  (JNIEnv *, jobject, jstring);

/*
 * Class:     dev_libhdc_HDC
 * Method:    set_data
 * Signature: (Ljava/util/ArrayList;[I)V
 */
JNIEXPORT void JNICALL Java_dev_libhdc_HDC_set_1data__Ljava_util_ArrayList_2_3I
  (JNIEnv *, jobject, jobject, jintArray);

/*
 * Class:     dev_libhdc_HDC
 * Method:    set_data
 * Signature: (Ljava/util/ArrayList;[J)V
 */
JNIEXPORT void JNICALL Java_dev_libhdc_HDC_set_1data__Ljava_util_ArrayList_2_3J
  (JNIEnv *, jobject, jobject, jlongArray);

/*
 * Class:     dev_libhdc_HDC
 * Method:    set_data
 * Signature: (Ljava/util/ArrayList;[F)V
 */
JNIEXPORT void JNICALL Java_dev_libhdc_HDC_set_1data__Ljava_util_ArrayList_2_3F
  (JNIEnv *, jobject, jobject, jfloatArray);

/*
 * Class:     dev_libhdc_HDC
 * Method:    set_data
 * Signature: (Ljava/util/ArrayList;[D)V
 */
JNIEXPORT void JNICALL Java_dev_libhdc_HDC_set_1data__Ljava_util_ArrayList_2_3D
  (JNIEnv *, jobject, jobject, jdoubleArray);

/*
 * Class:     dev_libhdc_HDC
 * Method:    get_data
 * Signature: ()Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_dev_libhdc_HDC_get_1data
  (JNIEnv *, jobject);

/*
 * Class:     dev_libhdc_HDC
 * Method:    get_rank
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_dev_libhdc_HDC_get_1rank
  (JNIEnv *, jobject);

/*
 * Class:     dev_libhdc_HDC
 * Method:    get_shape
 * Signature: ()Ljava/util/ArrayList;
 */
JNIEXPORT jobject JNICALL Java_dev_libhdc_HDC_get_1shape
  (JNIEnv *, jobject);

/*
 * Class:     dev_libhdc_HDC
 * Method:    serialize
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_dev_libhdc_HDC_serialize
  (JNIEnv *, jobject, jstring);

/*
 * Class:     dev_libhdc_HDC
 * Method:    save
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_dev_libhdc_HDC_save
  (JNIEnv *, jobject, jstring, jstring);

/*
 * Class:     dev_libhdc_HDC
 * Method:    load
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ldev/libhdc/HDC;
 */
JNIEXPORT jobject JNICALL Java_dev_libhdc_HDC_load__Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv *, jobject, jstring, jstring);

/*
 * Class:     dev_libhdc_HDC
 * Method:    print_info
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_dev_libhdc_HDC_print_1info
  (JNIEnv *, jobject);

/*
 * Class:     dev_libhdc_HDC
 * Method:    get_type
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_dev_libhdc_HDC_get_1type
  (JNIEnv *, jobject);

/*
 * Class:     dev_libhdc_HDC
 * Method:    get_type_str
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_dev_libhdc_HDC_get_1type_1str
  (JNIEnv *, jobject);

/*
 * Class:     dev_libhdc_HDC
 * Method:    dump
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_dev_libhdc_HDC_dump
  (JNIEnv *, jobject);

/*
 * Class:     dev_libhdc_HDC
 * Method:    init
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_dev_libhdc_HDC_init
  (JNIEnv *, jclass);

/*
 * Class:     dev_libhdc_HDC
 * Method:    load
 * Signature: (Ljava/lang/String;)Ldev/libhdc/HDC;
 */
JNIEXPORT jobject JNICALL Java_dev_libhdc_HDC_load__Ljava_lang_String_2
  (JNIEnv *, jclass, jstring);

#ifdef __cplusplus
}
#endif
#endif