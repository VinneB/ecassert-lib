#ifndef eca_H
#define eca_H

#include <stdlib.h>
#include <stdint.h>

extern struct eca_test *eca_current_test;

typedef enum { ECA_PASS, ECA_FAIL } eca_status;
typedef eca_status (*eca_test_func)();
typedef void (*eca_to_string)(void *, size_t, char *);

struct eca_test;

// Macro vars
extern unsigned int eca_m_uint_1;
extern unsigned int eca_m_uint_2;

extern unsigned short eca_m_ushort_1;
extern unsigned short eca_m_ushort_2;

extern unsigned char eca_m_uchar_1;
extern unsigned char eca_m_uchar_2;

extern unsigned long eca_m_ulong_1;
extern unsigned long eca_m_ulong_2;

extern unsigned long long eca_m_ulonglong_1;
extern unsigned long long eca_m_ulonglong_2;

// built-in to_string functions

void eca_tostring_int(void *, size_t, char *);
void eca_tostring_short(void *, size_t, char *);
void eca_tostring_char(void *, size_t, char *);
void eca_tostring_long(void *, size_t, char *);
void eca_tostring_longlong(void *, size_t, char *);
void eca_tostring_element_hex(void *element, size_t element_size, char *output_str);

// Meta suite management functions

void eca_register_test(const char *module_name, const char *test_name, eca_test_func test);
void eca_setup();
void eca_cleanup();
void eca_run_tests();
void eca_register_to_string(eca_to_string to_string_func);
void eca_restore_prev_to_string();

// Assert functions

unsigned int eca_assert_primitive(void *expected, void *actual, size_t element_size, char print_msg);

unsigned int eca_assert_array(void *expected, unsigned int expected_size, void *actual, unsigned int actual_size, size_t element_size, char print_msg);

#define ECA_ASSERT(expected, actual, element_size)                                \
  if (eca_assert_primitive(expected, actual, element_size, 1)) {                                                     \
    return ECA_FAIL;                                                                    \
  };

#define ECA_ASSERT_INT(expected, actual) \
  eca_m_uint_1 = eca_m_uint_2 = 0; \
  eca_m_uint_1 = (unsigned int) expected; \
  eca_m_uint_2 = (unsigned int) actual; \
  eca_register_to_string(eca_tostring_int); \
  if (eca_assert_primitive((void *)&eca_m_uint_1, (void *)&eca_m_uint_2, sizeof(eca_m_uint_1), 1)) {             \
    eca_restore_prev_to_string(); \
    return ECA_FAIL;             \
  }; \
  eca_restore_prev_to_string(); \

#define ECA_ASSERT_SHORT(expected, actual) \
  eca_m_ushort_1 = eca_m_ushort_2 = 0; \
  eca_m_ushort_1 = (unsigned short) expected; \
  eca_m_ushort_2 = (unsigned short) actual; \
  eca_register_to_string(eca_tostring_short); \
  if (eca_assert_primitive((void *)&eca_m_ushort_1, (void *)&eca_m_ushort_2, sizeof(eca_m_ushort_1), 1)) {             \
    eca_restore_prev_to_string(); \
    return ECA_FAIL;             \
  }; \
  eca_restore_prev_to_string(); \

#define ECA_ASSERT_CHAR(expected, actual) \
  eca_m_uchar_1 = eca_m_uchar_2 = 0; \
  eca_m_uchar_1 = (unsigned char) expected; \
  eca_m_uchar_2 = (unsigned char) actual; \
  eca_register_to_string(eca_tostring_char); \
  if (eca_assert_primitive((void *)&eca_m_uchar_1, (void *)&eca_m_uchar_2, sizeof(eca_m_uchar_1), 1)) {             \
    eca_restore_prev_to_string(); \
    return ECA_FAIL;             \
  }; \
  eca_restore_prev_to_string(); \

#define ECA_ASSERT_LONG(expected, actual) \
  eca_m_ulong_1 = eca_m_ulong_2 = 0; \
  eca_m_ulong_1 = (unsigned long) expected; \
  eca_m_ulong_2 = (unsigned long) actual; \
  eca_register_to_string(eca_tostring_long); \
  if (eca_assert_primitive((void *)&eca_m_ulong_1, (void *)&eca_m_ulong_2, sizeof(eca_m_ulong_1), 1)) {             \
    eca_restore_prev_to_string(); \
    return ECA_FAIL;             \
  }; \
  eca_restore_prev_to_string(); \

#define ECA_ASSERT_LONGLONG(expected, actual) \
  eca_m_ulonglong_1 = eca_m_ulonglong_2 = 0; \
  eca_m_ulonglong_1 = (unsigned long) expected; \
  eca_m_ulonglong_2 = (unsigned long) actual; \
  eca_register_to_string(eca_tostring_long); \
  if (eca_assert_primitive((void *)&eca_m_ulonglong_1, (void *)&eca_m_ulonglong_2, sizeof(eca_m_ulonglong_1), 1)) {             \
    eca_restore_prev_to_string(); \
    return ECA_FAIL;             \
  }; \
  eca_restore_prev_to_string(); \

#define ECA_ASSERT_ARR(expected, expected_size, actual, actual_size, element_size) \
  if (eca_assert_array(expected, expected_size, actual, actual_size, element_size, 1)) { \
    return ECA_FAIL; \
  } \


#define ECA_ASSERT_ARR_INT(expected, expected_size, actual, actual_size) \
  eca_register_to_string(eca_tostring_int); \
  if (eca_assert_array(expected, expected_size, actual, actual_size, sizeof(unsigned int), 1)) { \
    eca_restore_prev_to_string(); \
    return ECA_FAIL; \
  } \
  eca_restore_prev_to_string(); \

#define ECA_ASSERT_ARR_SHORT(expected, expected_size, actual, actual_size) \
  eca_register_to_string(eca_tostring_short); \
  if (eca_assert_array(expected, expected_size, actual, actual_size, sizeof(unsigned short), 1)) { \
    eca_restore_prev_to_string(); \
    return ECA_FAIL; \
  } \
  eca_restore_prev_to_string(); \

#define ECA_ASSERT_ARR_CHAR(expected, expected_size, actual, actual_size) \
  eca_register_to_string(eca_tostring_char); \
  if (eca_assert_array(expected, expected_size, actual, actual_size, sizeof(unsigned char), 1)) { \
    eca_restore_prev_to_string(); \
    return ECA_FAIL; \
  } \
  eca_restore_prev_to_string(); \

#define ECA_ASSERT_ARR_LONG(expected, expected_size, actual, actual_size) \
  eca_register_to_string(eca_tostring_long); \
  if (eca_assert_array(expected, expected_size, actual, actual_size, sizeof(unsigned long), 1)) { \
    eca_restore_prev_to_string(); \
    return ECA_FAIL; \
  } \
  eca_restore_prev_to_string(); \

#define ECA_ASSERT_ARR_LONGLONG(expected, expected_size, actual, actual_size) \
  eca_register_to_string(eca_tostring_longlong); \
  if (eca_assert_array(expected, expected_size, actual, actual_size, sizeof(unsigned long long), 1)) { \
    eca_restore_prev_to_string(); \
    return ECA_FAIL; \
  } \
  eca_restore_prev_to_string(); \

#endif // eca_H
