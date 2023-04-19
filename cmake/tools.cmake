# このファイルには、オンデマンドでアクティブ化およびダウンロード可能なツールのリストが含まれています。
# 各ツールは、CMakeに追加の-DUSE_<TOOL>=<VALUE>引数を渡すことによって構成中に有効になります。

# トップレベルのプロジェクトの場合にのみツールをアクティブ化します。
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

# このコードは、USE_SANITIZER フラグを使用して、Sanitizer を有効化するものであり、利用可能な値は以下の通りです
# Address、Memory、MemoryWithOrigins、Undefined、Thread、Leak、'Address;Undefined'。
if(USE_SANITIZER OR USE_STATIC_ANALYZER)
  CPMAddPackage("gh:StableCoder/cmake-scripts#1f822d1fc87c8d7720c074cde8a278b44963c354")

  if(USE_SANITIZER)
    include(${cmake-scripts_SOURCE_DIR}/sanitizers.cmake)
  endif()

  if(USE_STATIC_ANALYZER)
    if("clang-tidy" IN_LIST USE_STATIC_ANALYZER)
      set(CLANG_TIDY
          ON
          CACHE INTERNAL ""
      )
    else()
      set(CLANG_TIDY
          OFF
          CACHE INTERNAL ""
      )
    endif()
    if("iwyu" IN_LIST USE_STATIC_ANALYZER)
      set(IWYU
          ON
          CACHE INTERNAL ""
      )
    else()
      set(IWYU
          OFF
          CACHE INTERNAL ""
      )
    endif()
    if("cppcheck" IN_LIST USE_STATIC_ANALYZER)
      set(CPPCHECK
          ON
          CACHE INTERNAL ""
      )
    else()
      set(CPPCHECK
          OFF
          CACHE INTERNAL ""
      )
    endif()

    include(${cmake-scripts_SOURCE_DIR}/tools.cmake)

    clang_tidy(${CLANG_TIDY_ARGS})
    include_what_you_use(${IWYU_ARGS})
    cppcheck(${CPPCHECK_ARGS})
  endif()
endif()

# USE_CCACHE フラグを使用して CCACHE サポートを有効化できます。可能な値は、YES、NO、または同等です。
if(USE_CCACHE)
  CPMAddPackage("gh:TheLartians/Ccache.cmake@1.2.3")
endif()
