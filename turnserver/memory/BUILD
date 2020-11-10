cc_library(name = "memory",
           cppflags = [
                        "-Imedia_server_library",
                      ],
           srcs = [
                    "mem_slice.cpp",
                    "mem_slice_span.cpp",
                    "mem_buf.cpp",
                  ],
           deps = [
                  ]
          )

cc_test(name = "memory_test.exe",
        cppflags = [
          "-Imedia_server_library",
        ],
        srcs = [
          "mem_proxy_test.cpp",
          "mem_slice_span_test.cpp"
        ],
        deps = [
          ":memory",
        ]
        )
