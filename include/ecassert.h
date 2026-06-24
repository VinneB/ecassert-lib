#ifndef eca_H
#define eca_H

#include <stdlib.h>
#include <stdint.h>

extern struct eca_test *eca_current_test;

typedef enum { ECA_PASS, ECA_FAIL } eca_status;
typedef eca_status (*eca_test_func)();

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

// Meta suite management functions

void eca_register_test(const char *module_name, const char *test_name, eca_test_func test);
void eca_setup();
void eca_cleanup();
void eca_run_tests();

// Assert functions

unsigned int eca_assert_primitive(void *expected, void *actual, size_t element_size, char print_msg);

unsigned int eca_assert_array(void *actual, unsigned int actual_size, void *expected, unsigned int expected_size, size_t element_size, char print_msg);

#define ECA_EQUAL_RET_POINTER(expected_p, actual_p, element_size)                                \
  if (ec_assert_primitive(expected_p, actual_p, element_size, 1)) {                                                     \
    return ECA_FAIL;                                                                    \
  };

#define ECA_ASSERT_INT(a, b) \
  eca_m_uint_1 = eca_m_uint_2 = 0; \
  eca_m_uint_1 = (unsigned int) a; \
  eca_m_uint_2 = (unsigned int) b; \
  if (eca_assert_primitive((void *)&eca_m_uint_1, (void *)&eca_m_uint_2, sizeof(eca_m_uint_1), 1)) {             \
    return ECA_FAIL;             \
  };

#define ECA_ASSERT_SHORT(a, b) \
  eca_m_ushort_1 = eca_m_ushort_2 = 0; \
  eca_m_ushort_1 = (unsigned short) a; \
  eca_m_ushort_2 = (unsigned short) b; \
  if (eca_assert_primitive((void *)&eca_m_ushort_1, (void *)&eca_m_M_ushort_2, sizeof(eca_m_M_ushort_1), 1)) {             \
    return ECA_FAIL;             \
  };

#define ECA_ASSERT_CHAR(a, b) \
  eca_m_uchar_1 = eca_m_uchar_2 = 0; \
  eca_m_uchar_1 = (unsigned char) a; \
  eca_m_uchar_2 = (unsigned char) b; \
  if (eca_assert_primitive((void *)&eca_m_uchar_1, (void *)&eca_m_uchar_2, sizeof(eca_m_uchar_1), 1)) {             \
    return ECA_FAIL;             \
  };

#define ECA_ASSERT_LONG(a, b) \
  eca_m_ulong_1 = eca_m_ulong_2 = 0; \
  eca_m_ulong_1 = (unsigned long) a; \
  eca_m_ulong_2 = (unsigned long) b; \
  if (eca_assert_primitive((void *)&eca_m_ulong_1, (void *)&eca_m_ulong_2, sizeof(eca_m_ulong_1), 1)) {             \
    return ECA_FAIL;             \
  };

#define ECA_ASSERT_LONGLONG(a, b) \
  eca_m_ulonglong_1 = eca_m_ulonglong_2 = 0; \
  eca_m_ulonglong_1 = (unsigned long) a; \
  eca_m_ulonglong_2 = (unsigned long) b; \
  if (eca_assert_primitive((void *)&eca_m_ulonglong_1, (void *)&eca_m_ulonglong_2, sizeof(eca_m_ulonglong_1), 1)) {             \
    return ECA_FAIL;             \
  };

#define ECA_ASSERT_ARR_INT(a, a_size, b, b_size) \
  if (eca_assert_array(a, a_size, b, b_size, sizeof(unsigned int), 1)) { \
    return ECA_FAIL; \
  } \

#define ECA_ASSERT_ARR_SHORT(a, a_size, b, b_size) \
  if (eca_assert_array(a, a_size, b, b_size, sizeof(unsigned short), 1)) { \
    return ECA_FAIL; \
  } \

#define ECA_ASSERT_ARR_CHAR(a, a_size, b, b_size) \
  if (eca_assert_array(a, a_size, b, b_size, sizeof(unsigned char), 1)) { \
    return ECA_FAIL; \
  } \

#define ECA_ASSERT_ARR_LONG(a, a_size, b, b_size) \
  if (eca_assert_array(a, a_size, b, b_size, sizeof(unsigned long), 1)) { \
    return ECA_FAIL; \
  } \

#define ECA_ASSERT_ARR_LONGLONG(a, a_size, b, b_size) \
  if (eca_assert_array(a, a_size, b, b_size, sizeof(unsigned long), 1)) { \
    return ECA_FAIL; \
  } \

#endif // eca_H
