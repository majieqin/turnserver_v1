cc_library(name = "logging",
           cppflags = [
                        "-Imedia_server_library",
                      ],
           srcs = [
                    "log.cpp",
                  ],

            deps = [
                ## NOTE(liuyong): DO NOT change the order
                "//third_party/BUILD:glog",
                "//third_party/BUILD:gflags",
                "//media_server_library/memory/BUILD:memory",
            ]
          )

