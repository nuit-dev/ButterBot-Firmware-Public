file(GLOB_RECURSE sources ./**/*.cpp ./**/*.c)
list(FILTER sources EXCLUDE REGEX "lex_data_include*")

idf_component_register(SRCS "${sources}" INCLUDE_DIRS "include")

target_compile_definitions(${COMPONENT_LIB} PRIVATE CST_USER_MALLOC)
target_include_directories(${COMPONENT_LIB} PRIVATE include/lang/usenglish include/lang/cmulex)

# These files are converted to single-precision (ESP32-S3 FPU is SP-only) — catch accidental double promotions
set_source_files_properties(src/cg/cst_mlsa.c src/cg/cst_mlpg.c src/cg/cst_vc.c PROPERTIES COMPILE_OPTIONS "-Wdouble-promotion")
