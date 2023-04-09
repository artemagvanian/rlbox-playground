// ANCHOR: imports
// We're going to use RLBox in a single-threaded environment.
#define RLBOX_SINGLE_THREADED_INVOCATIONS

// All calls into the sandbox are resolved statically.
#define RLBOX_USE_STATIC_CALLS() rlbox_wasm2c_sandbox_lookup_symbol

#include <cstdio>
#include <cassert>
#include "mylib.h"
#include "mylib.wasm.h"
#include "rlbox.hpp"
#include "rlbox_wasm2c_sandbox.hpp"


using namespace std;
using namespace rlbox;

// Define base type for mylib using the noop sandbox
RLBOX_DEFINE_BASE_TYPES_FOR(mylib, wasm2c);

// Declare callback function we're going to call from sandboxed code.
void hello_cb(rlbox_sandbox_mylib &_, tainted_mylib<const char *> str);

int main(int argc, char const *argv[]) {
    // Declare and create a new sandbox
    rlbox_sandbox_mylib sandbox;
    sandbox.create_sandbox();

    // Call the library hello function:
    sandbox.invoke_sandbox_function(hello);

    // call the normal add function and check the result:
    auto ok = sandbox.invoke_sandbox_function(add, 3, 4).copy_and_verify([](unsigned ret) {
        printf("Adding... 3+4 = %d\n", ret);
        return ret == 7;
    });
    printf("OK regular? = %d\n", ok);

    // call the add_global_side_effect function 2 times and check the result:
    // defining global in main.cpp and then extern-ing it in mylib would not compile
    sandbox.invoke_sandbox_function(add_global_side_effect, 3, 4);
    auto ok_global = sandbox.invoke_sandbox_function(add_global_side_effect, 5, 4).copy_and_verify([](unsigned ret) {
        printf("Adding... 5+4 = %d\n", ret);
        return ret == 9;
    });
    printf("OK global? = %d\n", ok_global);

    // call the add_printf_side_effect function and check the result:
    auto ok_printf = sandbox.invoke_sandbox_function(add_printf_side_effect, 3, 4).copy_and_verify([](unsigned ret) {
        printf("Adding... 3+4 = %d\n", ret);
        return ret == 7;
    });
    printf("OK printf? = %d\n", ok_printf);

    // call the add_ptr_side_effect function and check the result:
    // spoiler: this won't even compile
    /*
    auto *ptr = (unsigned *) malloc(sizeof(unsigned));
    auto ok_ptr = sandbox.invoke_sandbox_function(add_ptr_side_effect, 3, 4, ptr)
            .copy_and_verify([](unsigned ret) {
                printf("Adding... 3+4 = %d\n", ret);
                return ret == 7;
            });
    printf("OK ptr? = %d\n", ok_ptr);
    printf("ptr value = %d\n", *ptr);
    */

    // call the add_file_side_effect function and check the result:
    // spoiler, will crash due to insufficient capabilities
    auto ok_file = sandbox.invoke_sandbox_function(add_file_side_effect, 3, 4).copy_and_verify([](unsigned ret) {
        printf("Adding... 3+4 = %d\n", ret);
        return ret == 7;
    });
    printf("OK file? = %d\n", ok_file);

    // Call the library echo function
    const char *helloStr = "hi hi!";
    size_t helloSize = strlen(helloStr) + 1;
    tainted_mylib<char *> taintedStr = sandbox.malloc_in_sandbox<char>(helloSize);
    strncpy(taintedStr.unverified_safe_pointer_because(helloSize, "writing to region"), helloStr, helloSize);

    sandbox.invoke_sandbox_function(echo, taintedStr);
    sandbox.free_in_sandbox(taintedStr);

    // register callback and call it
    auto cb = sandbox.register_callback(hello_cb);
    sandbox.invoke_sandbox_function(call_cb, cb);

    // destroy sandbox
    sandbox.destroy_sandbox();

    return 0;
}

void hello_cb(rlbox_sandbox_mylib &_, tainted_mylib<const char *> str) {
    auto checked_string = str.copy_and_verify_string([](unique_ptr<char[]> val) {
        assert(val != nullptr && strlen(val.get()) < 1024);
        return move(val);
    });
    printf("hello_cb: %s\n", checked_string.get());
}
