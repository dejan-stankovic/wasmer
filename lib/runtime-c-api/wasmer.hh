#ifndef WASMER_H
#define WASMER_H

#include <cstdarg>
#include <cstdint>
#include <cstdlib>

enum class wasmer_result_t {
  WASMER_OK = 1,
  WASMER_ERROR = 2,
};

enum class wasmer_value_tag : uint32_t {
  WASM_I32,
  WASM_I64,
  WASM_F32,
  WASM_F64,
};

struct wasmer_import_object_t;

struct wasmer_instance_context_t;

struct wasmer_instance_t;

struct wasmer_global_t {

};

union wasmer_value {
  int32_t I32;
  int64_t I64;
  float F32;
  double F64;
};

struct wasmer_value_t {
  wasmer_value_tag tag;
  wasmer_value value;
};

struct wasmer_global_descriptor_t {
  bool mutable_;
  wasmer_value_tag kind;
};

struct wasmer_memory_t {

};

struct wasmer_limits_t {
  uint32_t min;
  uint32_t max;
};

struct wasmer_table_t {

};

extern "C" {

/// Frees memory for the given Global
void wasmer_global_destroy(wasmer_global_t *global);

/// Gets the value stored by the given Global
wasmer_value_t wasmer_global_get(wasmer_global_t *global);

/// Returns a descriptor (type, mutability) of the given Global
wasmer_global_descriptor_t wasmer_global_get_descriptor(wasmer_global_t *global);

/// Creates a new Global and returns a pointer to it.
/// The caller owns the object and should call `wasmer_global_destroy` to free it.
wasmer_global_t *wasmer_global_new(wasmer_value_t value, bool mutable_);

/// Sets the value stored by the given Global
void wasmer_global_set(wasmer_global_t *global, wasmer_value_t value);

/// Frees memory for the given ImportObject
void wasmer_import_object_destroy(wasmer_import_object_t *import_object);

/// Creates a new ImportObject and returns a pointer to it.
/// The caller owns the object and should call `wasmer_import_object_destroy` to free it.
wasmer_import_object_t *wasmer_import_object_new();

/// Registers a `func` with provided `name` and `namespace` into the ImportObject.
/// Returns `wasmer_result_t::WASMER_OK` upon success.
/// Returns `wasmer_result_t::WASMER_ERROR` upon failure. Use `wasmer_last_error_length`
/// and `wasmer_last_error_message` to get an error message.
void wasmer_imports_set_import_func(wasmer_import_object_t *import_object,
                                    const char *namespace_,
                                    const char *name,
                                    void (*func)(void *data),
                                    const wasmer_value_tag *params,
                                    int params_len,
                                    const wasmer_value_tag *returns,
                                    int returns_len);

/// Calls an instances exported function by `name` with the provided parameters.
/// Results are set using the provided `results` pointer.
/// Returns `wasmer_result_t::WASMER_OK` upon success.
/// Returns `wasmer_result_t::WASMER_ERROR` upon failure. Use `wasmer_last_error_length`
/// and `wasmer_last_error_message` to get an error message.
wasmer_result_t wasmer_instance_call(wasmer_instance_t *instance,
                                     const char *name,
                                     const wasmer_value_t *params,
                                     int params_len,
                                     wasmer_value_t *results,
                                     int results_len);

/// Gets the memory within the context at the index `memory_idx`.
/// The index is always 0 until multiple memories are supported.
const wasmer_memory_t *wasmer_instance_context_memory(wasmer_instance_context_t *ctx,
                                                      uint32_t memory_idx);

/// Frees memory for the given Instance
void wasmer_instance_destroy(wasmer_instance_t *instance);

/// Creates a new Instance from the given wasm bytes and imports.
/// Returns `wasmer_result_t::WASMER_OK` upon success.
/// Returns `wasmer_result_t::WASMER_ERROR` upon failure. Use `wasmer_last_error_length`
/// and `wasmer_last_error_message` to get an error message.
wasmer_result_t wasmer_instantiate(wasmer_instance_t **instance,
                                   uint8_t *wasm_bytes,
                                   uint32_t wasm_bytes_len,
                                   wasmer_import_object_t *import_object);

/// Gets the length in bytes of the last error.
/// This can be used to dynamically allocate a buffer with the correct number of
/// bytes needed to store a message.
/// # Example
/// ```
/// int error_len = wasmer_last_error_length();
/// char *error_str = malloc(error_len);
/// ```
int wasmer_last_error_length();

/// Stores the last error message into the provided buffer up to the given `length`.
/// The `length` parameter must be large enough to store the last error message.
/// Returns the length of the string in bytes.
/// Returns `-1` if an error occurs.
/// # Example
/// ```
/// int error_len = wasmer_last_error_length();
/// char *error_str = malloc(error_len);
/// wasmer_last_error_message(error_str, error_len);
/// printf("Error str: `%s`\n", error_str);
/// ```
int wasmer_last_error_message(char *buffer, int length);

/// Gets the start pointer to the bytes within a Memory
uint8_t *wasmer_memory_data(wasmer_memory_t *mem);

/// Gets the size in bytes of a Memory
uint32_t wasmer_memory_data_length(wasmer_memory_t *mem);

/// Frees memory for the given Memory
void wasmer_memory_destroy(wasmer_memory_t *memory);

/// Grows a Memory by the given number of pages.
/// Returns `wasmer_result_t::WASMER_OK` upon success.
/// Returns `wasmer_result_t::WASMER_ERROR` upon failure. Use `wasmer_last_error_length`
/// and `wasmer_last_error_message` to get an error message.
wasmer_result_t wasmer_memory_grow(wasmer_memory_t *memory, uint32_t delta);

/// Returns the current length in pages of the given memory
uint32_t wasmer_memory_length(wasmer_memory_t *memory);

/// Creates a new Memory for the given descriptor and initializes the given
/// pointer to pointer to a pointer to the new memory.
/// The caller owns the object and should call `wasmer_memory_destroy` to free it.
/// Returns `wasmer_result_t::WASMER_OK` upon success.
/// Returns `wasmer_result_t::WASMER_ERROR` upon failure. Use `wasmer_last_error_length`
/// and `wasmer_last_error_message` to get an error message.
wasmer_result_t wasmer_memory_new(wasmer_memory_t **memory, wasmer_limits_t limits);

/// Frees memory for the given Table
void wasmer_table_destroy(wasmer_table_t *table);

/// Grows a Table by the given number of elements.
/// Returns `wasmer_result_t::WASMER_OK` upon success.
/// Returns `wasmer_result_t::WASMER_ERROR` upon failure. Use `wasmer_last_error_length`
/// and `wasmer_last_error_message` to get an error message.
wasmer_result_t wasmer_table_grow(wasmer_table_t *table, uint32_t delta);

/// Returns the current length of the given Table
uint32_t wasmer_table_length(wasmer_table_t *table);

/// Creates a new Table for the given descriptor and initializes the given
/// pointer to pointer to a pointer to the new Table.
/// The caller owns the object and should call `wasmer_table_destroy` to free it.
/// Returns `wasmer_result_t::WASMER_OK` upon success.
/// Returns `wasmer_result_t::WASMER_ERROR` upon failure. Use `wasmer_last_error_length`
/// and `wasmer_last_error_message` to get an error message.
wasmer_result_t wasmer_table_new(wasmer_table_t **table, wasmer_limits_t limits);

/// Returns true for valid wasm bytes and false for invalid bytes
bool wasmer_validate(uint8_t *wasm_bytes, uint32_t wasm_bytes_len);

} // extern "C"

#endif // WASMER_H