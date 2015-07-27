proto_filetype = FileType([".proto"])

def proto_library(name, src, deps = None):
    proto_cc_deps = [
        "@protobuf//:protoc",
    ]
    cc_deps = [
        "@protobuf//:protobuf"
    ]
    command = "$(location @protobuf//:protoc) --cpp_out=$(GENDIR)/"
    command += " $(location %s)" % (src)

    basename = src[0:-5]
    cc_proto_name = name + "_cc_proto"
    header_outputs = [
        basename + "pb.h",
    ]
    outputs = header_outputs + [
        basename + "pb.cc",
    ]
      
    native.genrule(
        name = cc_proto_name,
        srcs = [ src ] + proto_cc_deps,
        cmd = command,
        outs = outputs,
    )
    native.cc_library(
        name = name,
        hdrs = header_outputs,
        srcs = [
            ":" + cc_proto_name
        ],
        deps = cc_deps,
    )
