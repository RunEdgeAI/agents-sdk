# Main library build recipe

# Python for python execution tool
cc_import(
    name = "python",
    shared_library = select({
        "@platforms//os:linux": "lib/linux/libpython3.11.so",
        "@platforms//os:macos": "lib/macos/libpython3.11.dylib",
        "@platforms//os:windows": "lib/windows/libpython3.11.dll",
        "//conditions:default": None,
    }),
)

# Main library
cc_import(
    name = "agents_cpp",
    hdrs = glob([
        "include/agents-cpp/**/*.h",
        "include/spdlog/**/*.h"
    ]),
    includes = [
        "include",
    ],
    shared_library = select({
        "@platforms//os:linux": "lib/linux/libagents_cpp_shared_lib.so",
        "@platforms//os:macos": "lib/macos/libagents_cpp_shared_lib.dylib",
        "@platforms//os:windows": "lib/windows/libagents_cpp_shared_lib.dll",
        "//conditions:default": None,
    }),
    deps = [
        "@nlohmann_json//:json",
        ":python"
    ],
    visibility = ["//visibility:public"]
)

# End of BUILD file