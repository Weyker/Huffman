function(add_exec EXEC_NAME)
  set(TARGET_ID "${EXEC_NAME}")

  add_executable(${TARGET_ID} ${ARGN})
  target_link_libraries(${TARGET_ID} Huffman)
  set_property(TARGET ${TARGET_ID} PROPERTY CXX_EXTENSIONS OFF)

  # Set warnings on AppleClang and Clang COMPILER IDS
  target_compile_options(${TARGET_ID} PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>>:
  -Weverything
  -Wno-c++98-compat
  -Wno-c++98-compat-pedantic
  -Wno-padded
  -Wno-sign-conversion
  -Wno-global-constructors
  >)

  add_test(NAME ${EXEC_NAME} COMMAND ${TARGET_ID})
endfunction()

add_exec("compressor" "compressor.cpp")
add_exec("decompressor" "decompressor.cpp")
