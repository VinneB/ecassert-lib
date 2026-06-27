#include "ecassert.h"
#include "stdio.h"
#include "stdlib.h"
#include <string.h>

unsigned int eca_m_uint_1 = 0;
unsigned int eca_m_uint_2 = 0;

unsigned short eca_m_ushort_1 = 0;
unsigned short eca_m_ushort_2 = 0;

unsigned char eca_m_uchar_1 = 0;
unsigned char eca_m_uchar_2 = 0;

unsigned long eca_m_ulong_1 = 0;
unsigned long eca_m_ulong_2 = 0;

unsigned long long eca_m_ulonglong_1 = 0;
unsigned long long eca_m_ulonglong_2 = 0;

const unsigned int MAX_PRINT_ARRAY_SIZE = 10;
const unsigned int MAX_MISMATCH_INDEX_ARR_SIZE = 10;
const unsigned int MISMATCH_COLUMN_COUNT = 3;

const unsigned int INITIAL_TESTS_SIZE = 64;
const unsigned int INITIAL_MODULES_SIZE = 64;
const unsigned int INTIIAL_TRACE_DATA_SIZE = 256;

const unsigned int MAX_NUM_CHAR_PRINT_TO_TRACE = 64;

void eca_tostring_element_hex(void *element, size_t element_size, char *output_str);
eca_to_string registered_to_string = eca_tostring_element_hex;
eca_to_string prev_registered_to_string = eca_tostring_element_hex;

struct eca_test *current_test = NULL;

typedef unsigned char byte;
typedef uint32_t element;

struct module {
  const char *module_name;
  struct eca_test *tests;
  unsigned int test_capacity;
  unsigned int test_size;
  unsigned int fail_count;
};

struct suite {
  struct module *modules;
  unsigned int modules_capacity;
  unsigned int modules_size;
  unsigned int test_count;
  unsigned int test_failed_count;
  unsigned char is_init;
} suite = {NULL, 0, 0, 0, 0, 0};

struct trace {
  char *data;
  unsigned int size;
  unsigned int capacity;
};

struct eca_test {
  char const *module_name;
  char const *test_name;
  eca_test_func test;
  eca_status status;
  struct trace trace;
  unsigned char has_test_ran;
};

void eca_register_to_string(eca_to_string to_string_func) {
  prev_registered_to_string = registered_to_string;
  registered_to_string = to_string_func;
}

void eca_restore_prev_to_string() {
  registered_to_string = prev_registered_to_string;
}

unsigned int realloc_pointer(void **p, unsigned int capacity, size_t element_size, unsigned int inc_amount) {
  unsigned int new_capacity = capacity + (capacity >> 1);
  new_capacity = (new_capacity < capacity + inc_amount) ? capacity + inc_amount : new_capacity;
  void *tmp = realloc(*p, new_capacity * element_size);
  if (tmp == NULL) {
    fprintf(stderr, "malloc failed. Could reallocate pointer. Terminating...\n");
    exit(1);
  }
  memset(tmp + capacity, 0, new_capacity - capacity);
  *p = tmp;
  return new_capacity;
}

element to_element(void *raw_element, size_t element_size) {
  element packed_element = 0;
  byte *casted_element = (byte *)raw_element;
  for (size_t i = 0; i < element_size; i++) {
    byte shift_amount = i * 8;
    packed_element = packed_element | (*(casted_element + i) << shift_amount);
  }
  return packed_element;
}

inline void print_element(struct trace *trace, void *element, size_t element_size) {
  if (trace->size + MAX_NUM_CHAR_PRINT_TO_TRACE > trace->capacity) {
    trace->capacity = realloc_pointer((void **)&trace->data, trace->capacity, sizeof(char), MAX_NUM_CHAR_PRINT_TO_TRACE);
  }
  char element_str[MAX_NUM_CHAR_PRINT_TO_TRACE];
  registered_to_string(element, element_size, element_str);
  trace->size += snprintf(trace->data + trace->size, trace->capacity - trace->size, "%s", element_str);
}

void print_string_to_trace_int_args(struct trace *trace, char *string, int arg1, int arg2, int arg3) {
  if (trace->size + MAX_NUM_CHAR_PRINT_TO_TRACE > trace->capacity) {
    trace->capacity = realloc_pointer((void **)&trace->data, trace->capacity, sizeof(char), MAX_NUM_CHAR_PRINT_TO_TRACE);
  }
  trace->size += snprintf(trace->data + trace->size, trace->capacity - trace->size, string, arg1, arg2, arg3);
}
void print_string_to_trace(struct trace *trace, char *string) {
  if (trace->size + MAX_NUM_CHAR_PRINT_TO_TRACE > trace->capacity) {
    trace->capacity = realloc_pointer((void **)&trace->data, trace->capacity, sizeof(char), MAX_NUM_CHAR_PRINT_TO_TRACE);
  }
  trace->size += snprintf(trace->data + trace->size, trace->capacity - trace->size, "%s", string);
}

void print_arr(struct trace *trace, void *arr, unsigned int arr_size, size_t element_size) {
  byte *p;
  print_string_to_trace(trace, "[");
  for (p = (byte *)arr; p < (byte *)arr + ((arr_size - 1) * element_size);
       p = p + element_size) {
    print_element(trace, p, element_size);
    print_string_to_trace(trace, ", ");
  }
  print_element(trace, p, element_size);
  print_string_to_trace(trace, "]");
}

void eca_setup() {
  if (suite.is_init) {
    fprintf(stderr, "Attempting to setup an already initialized test suite. Terminating...\n");
    eca_cleanup();
    exit(1);
  }
  suite.modules_capacity = INITIAL_MODULES_SIZE;
  suite.modules_size = 0;
  suite.test_count = 0;
  suite.test_failed_count = 0;
  suite.is_init = 1;
  suite.modules = malloc(sizeof(struct module) * INITIAL_MODULES_SIZE);
  memset(suite.modules, 0, sizeof(struct module) * INITIAL_MODULES_SIZE);
  printf("debug: malloced modules\n");
  if (suite.modules == NULL) {
    fprintf(stderr, "malloc failed. Could not init test suite. Terminating...\n");
    eca_cleanup();
    exit(1);
  }
  registered_to_string = eca_tostring_element_hex;
}

void eca_cleanup() {
  if (!suite.is_init) {
    return;
  }
  printf("Cleaning up...\n");
  printf("%d\n", suite.modules_capacity);
  for (struct module *p = suite.modules; p < suite.modules + suite.modules_size; p = p + 1) {
    printf("debug: freeing test\n");
    for (struct eca_test *q = p->tests; q < p->tests + p->test_size; q = q + 1) {
      free(q->trace.data);
    }
    free(p->tests);
  }
  printf("free 2\n");
  free(suite.modules);
  suite.modules = NULL;
  suite.modules_capacity = 0;
  suite.modules_size = 0;
  suite.test_count = 0;
  suite.test_failed_count = 0;
  suite.is_init = 0;
}

void eca_register_test(const char *module_name, const char *test_name, eca_test_func test) {
  if (!suite.is_init) {
    fprintf(stderr, "Attemping to register test with non-initialized test suite. Run eca_setup() first. Terminating...");
    eca_cleanup();
    exit(1);
  }
  // Find module
  unsigned char found_module = 0;
  struct module *module = NULL;
  for (unsigned int i = 0; i < suite.modules_size; i++) {
    if (suite.modules[i].module_name == module_name) {
      found_module = 1;
      module = suite.modules + i;
      printf("debug: found module\n");
      break;
    }
  }
  if (!found_module && suite.modules_size == suite.modules_capacity) {
    printf("debug: reallocing modules\n");
    suite.modules_capacity = realloc_pointer((void **)&suite.modules, suite.modules_capacity, sizeof(struct module), 1);
  }
  if (!found_module) {
    printf("debug: initing module\n");
    module = suite.modules + suite.modules_size;
    module->tests = malloc(sizeof(struct eca_test) * INITIAL_TESTS_SIZE);
    if (module->tests == NULL) {
      eca_cleanup();
      fprintf(stderr, "malloc failed. Could not register test. Terminating...\n");
      exit(1);
    }
    memset(module->tests, 0, sizeof(struct eca_test) * INITIAL_TESTS_SIZE);
    module->module_name = module_name;
    module->test_size = 0;
    module->test_capacity = INITIAL_TESTS_SIZE;
    module->fail_count = 0;
    suite.modules_size++;
    printf("finished initing module\n");
  }
  // Add test
  struct trace trace = {NULL, 0, 0};
  trace.data = malloc(sizeof(char) * MAX_NUM_CHAR_PRINT_TO_TRACE);
  if (trace.data == NULL) {
    eca_cleanup();
    fprintf(stderr, "malloc failed. Could not register test. Terminating...\n");
    exit(1);
  }
  memset(trace.data, 0, sizeof(char) * MAX_NUM_CHAR_PRINT_TO_TRACE);
  struct eca_test new_test = {module_name, test_name, test, ECA_FAIL, trace, 0};
  if (module->test_size == module->test_capacity) {
    module->test_capacity = realloc_pointer((void **)&module->tests, module->test_capacity, sizeof(struct eca_test), 1);
  }
  *(module->tests + module->test_size) = new_test;
  module->test_size++;
  suite.test_count++;
}

void eca_run_tests() {
  printf("Running %d tests...\n", suite.test_count);
  for (struct module *p = suite.modules; p < suite.modules + suite.modules_size; p = p + 1) {
    for (struct eca_test *q = p->tests; q < p->tests + p->test_size; q = q + 1) {
      current_test = q;
      q->status = q->test();
      q->has_test_ran = 1;
      if (q->status == ECA_FAIL) {
        p->fail_count++;
        suite.test_failed_count++;
      }
    }
  }
  current_test = NULL;
  printf("TEST RESULTS\n");
  for (struct module *p = suite.modules; p < suite.modules + suite.modules_size; p = p + 1) {
    printf("--- MODULE %s - %s - %d / %d TESTS PASSED ---\n", p->module_name, (p->fail_count > 0) ? "FAILED" : "PASSED", p->test_size - p->fail_count, p->test_size);
    for (struct eca_test *q = p->tests; q < p->tests + p->test_size; q = q + 1) {
      if (q->status == ECA_FAIL) {
        printf("TEST \"%s\" FAILED\n", q->test_name);
        printf("%s\n", q->trace.data);
      }
    }
  }
}

// In general, a is the expected value and b is the test value
unsigned int eca_assert_primitive(void *expected, void *actual, size_t element_size, char print_msg) {
  byte *a = (byte *)expected;
  byte *b = (byte *)actual;
  for (size_t i = 0; i < element_size; i++) {
    if (*(a + i) == *(b + i)) {
      continue;
    }
    if (print_msg) {
      print_string_to_trace(&(current_test->trace), "ASSERT FAILED\n");
    }
    if (element_size <= sizeof(element) || registered_to_string != eca_tostring_element_hex) {
      print_string_to_trace(&(current_test->trace), "\texpected = ");
      print_element(&(current_test->trace), expected, element_size);
      print_string_to_trace(&(current_test->trace), "\n\tactual = ");
      print_element(&(current_test->trace), actual, element_size);
      print_string_to_trace(&(current_test->trace), "\n");
    }
    return ECA_FAIL;
  }
  return ECA_PASS;
}

unsigned int eca_assert_array(void *expected, unsigned int expected_size, void *actual, unsigned int actual_size, size_t element_size, char print_msg) {
  eca_status return_val = ECA_PASS;
  unsigned int smallest_size =
      (actual_size < expected_size) ? actual_size : expected_size;
  if (actual_size != expected_size) {
    if (print_msg) {
      print_string_to_trace_int_args(&(current_test->trace), "ASSERT FAILED\n\texpected size = %d\n\tactual size = %d\nTruncating both arrays to smaller size (%d)\n", expected_size, actual_size, smallest_size);
    }
    return_val = ECA_FAIL;
  }
  byte *a = (byte *)actual;
  byte *b = (byte *)expected;
  size_t byte_len = element_size * smallest_size;
  unsigned int mismatch_arr[MAX_MISMATCH_INDEX_ARR_SIZE];
  unsigned int mismatch_arr_index = 0;
  char surpassed_mismatch_arr_index = 0;
  for (size_t i = 0; i < byte_len; i++) {
    if (*(a + i) == *(b + i)) {
      continue;
    }
    if (mismatch_arr_index == MAX_MISMATCH_INDEX_ARR_SIZE) {
      surpassed_mismatch_arr_index = 1;
      break;
    }
    mismatch_arr[mismatch_arr_index++] = i / element_size;
    // Jump forward to end of element
    i += element_size - (i % element_size) - 1;
    return_val = ECA_FAIL;
  }

  if (mismatch_arr_index > 0 && print_msg) {
    print_string_to_trace(&(current_test->trace), "ASSERT FAILED\n\tvalue mismatch\n");
    if (actual_size <= MAX_PRINT_ARRAY_SIZE && expected_size <= MAX_PRINT_ARRAY_SIZE && (element_size <= sizeof(element) || registered_to_string != eca_tostring_element_hex)) {
      print_string_to_trace(&(current_test->trace), "\texpected = ");
      print_arr(&(current_test->trace), expected, smallest_size, element_size);
      print_string_to_trace(&(current_test->trace), "\n\tactual = ");
      print_arr(&(current_test->trace), actual, smallest_size, element_size);
      print_string_to_trace(&(current_test->trace), "\n");
    } else if (element_size <= sizeof(element)) {
      for (unsigned int i = 0; i < mismatch_arr_index; i++) {
        print_string_to_trace_int_args(&(current_test->trace), "\t| idx %3d: expected = ", mismatch_arr[i], 0, 0);
        print_element(&(current_test->trace), a + (mismatch_arr[i] * element_size), element_size);
        print_string_to_trace(&(current_test->trace), " ; actual = ");
        print_element(&(current_test->trace), b + (mismatch_arr[i] * element_size), element_size);
        if (((i % MISMATCH_COLUMN_COUNT) == MISMATCH_COLUMN_COUNT - 1) || i == mismatch_arr_index - 1) {
          print_string_to_trace(&(current_test->trace), " |\n");
        }
      }
      if (surpassed_mismatch_arr_index) {
        print_string_to_trace(&(current_test->trace), "\t... (more not shown\n");
      }
    }
  }
  return return_val;
}

// To string functions

inline void eca_tostring_int(void *element, size_t element_size, char *str) {
  snprintf(str, MAX_NUM_CHAR_PRINT_TO_TRACE, "%d", *((int *)element));
}

void eca_tostring_short(void *element, size_t element_size, char *str) {
  snprintf(str, MAX_NUM_CHAR_PRINT_TO_TRACE, "%hd", *((short *)element));
}
void eca_tostring_char(void *element, size_t element_size, char *str) {
  snprintf(str, MAX_NUM_CHAR_PRINT_TO_TRACE, "%c", *((char *)element));
}
void eca_tostring_long(void *element, size_t element_size, char *str) {
  snprintf(str, MAX_NUM_CHAR_PRINT_TO_TRACE, "%ld", *((long *)element));
}
void eca_tostring_longlong(void *element, size_t element_size, char *str) {
  snprintf(str, MAX_NUM_CHAR_PRINT_TO_TRACE, "%lld", *((long long *)element));
}
void eca_tostring_element_hex(void *element, size_t element_size, char *output_str) {
  snprintf(output_str, MAX_NUM_CHAR_PRINT_TO_TRACE, "0x%0*x", (int)(element_size * 2), to_element(element, element_size));
}
