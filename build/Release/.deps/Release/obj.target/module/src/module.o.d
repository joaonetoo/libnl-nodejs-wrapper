cmd_Release/obj.target/module/src/module.o := cc '-DNODE_GYP_MODULE_NAME=module' '-DUSING_UV_SHARED=1' '-DUSING_V8_SHARED=1' '-DV8_DEPRECATION_WARNINGS=1' '-DV8_DEPRECATION_WARNINGS' '-DV8_IMMINENT_DEPRECATION_WARNINGS' '-D_LARGEFILE_SOURCE' '-D_FILE_OFFSET_BITS=64' '-DOPENSSL_THREADS' '-DBUILDING_NODE_EXTENSION' -I/home/giuan/.node-gyp/12.4.0/include/node -I/home/giuan/.node-gyp/12.4.0/src -I/home/giuan/.node-gyp/12.4.0/deps/openssl/config -I/home/giuan/.node-gyp/12.4.0/deps/openssl/openssl/include -I/home/giuan/.node-gyp/12.4.0/deps/uv/include -I/home/giuan/.node-gyp/12.4.0/deps/zlib -I/home/giuan/.node-gyp/12.4.0/deps/v8/include -I/usr/include/libnl3  -fPIC -pthread -Wall -Wextra -Wno-unused-parameter -m64 -O3 -fno-omit-frame-pointer  -MMD -MF ./Release/.deps/Release/obj.target/module/src/module.o.d.raw   -c -o Release/obj.target/module/src/module.o ../src/module.c
Release/obj.target/module/src/module.o: ../src/module.c \
 /home/giuan/.node-gyp/12.4.0/include/node/node_api.h \
 /home/giuan/.node-gyp/12.4.0/include/node/js_native_api.h \
 /home/giuan/.node-gyp/12.4.0/include/node/js_native_api_types.h \
 /home/giuan/.node-gyp/12.4.0/include/node/node_api_types.h \
 /usr/include/libnl3/netlink/cli/utils.h \
 /usr/include/libnl3/netlink/netlink.h \
 /usr/include/libnl3/netlink/netlink-compat.h \
 /usr/include/libnl3/netlink/version.h \
 /usr/include/libnl3/netlink/errno.h /usr/include/libnl3/netlink/types.h \
 /usr/include/libnl3/netlink/handlers.h \
 /usr/include/libnl3/netlink/netlink-kernel.h \
 /usr/include/libnl3/netlink/socket.h \
 /usr/include/libnl3/netlink/object.h /usr/include/libnl3/netlink/utils.h \
 /usr/include/libnl3/netlink/list.h /usr/include/libnl3/netlink/addr.h \
 /usr/include/libnl3/netlink/route/rtnl.h \
 /usr/include/libnl3/netlink/route/link.h \
 /usr/include/libnl3/netlink/cache.h /usr/include/libnl3/netlink/msg.h \
 /usr/include/libnl3/netlink/attr.h /usr/include/libnl3/netlink/data.h \
 /usr/include/libnl3/netlink/route/addr.h \
 /usr/include/libnl3/netlink/route/neighbour.h \
 /usr/include/libnl3/netlink/route/neightbl.h \
 /usr/include/libnl3/netlink/route/route.h \
 /usr/include/libnl3/netlink/route/nexthop.h \
 /usr/include/libnl3/netlink/route/rule.h \
 /usr/include/libnl3/netlink/route/qdisc.h \
 /usr/include/libnl3/netlink/route/tc.h \
 /usr/include/libnl3/netlink/route/class.h \
 /usr/include/libnl3/netlink/route/classifier.h \
 /usr/include/libnl3/netlink/route/cls/ematch.h \
 /usr/include/libnl3/netlink/fib_lookup/lookup.h \
 /usr/include/libnl3/netlink/fib_lookup/request.h \
 /usr/include/libnl3/netlink/genl/genl.h \
 /usr/include/libnl3/netlink/genl/ctrl.h \
 /usr/include/libnl3/netlink/genl/mngt.h \
 /usr/include/libnl3/netlink/netfilter/ct.h \
 /usr/include/libnl3/netlink/cli/route.h \
 /usr/include/libnl3/netlink/cli/link.h
../src/module.c:
/home/giuan/.node-gyp/12.4.0/include/node/node_api.h:
/home/giuan/.node-gyp/12.4.0/include/node/js_native_api.h:
/home/giuan/.node-gyp/12.4.0/include/node/js_native_api_types.h:
/home/giuan/.node-gyp/12.4.0/include/node/node_api_types.h:
/usr/include/libnl3/netlink/cli/utils.h:
/usr/include/libnl3/netlink/netlink.h:
/usr/include/libnl3/netlink/netlink-compat.h:
/usr/include/libnl3/netlink/version.h:
/usr/include/libnl3/netlink/errno.h:
/usr/include/libnl3/netlink/types.h:
/usr/include/libnl3/netlink/handlers.h:
/usr/include/libnl3/netlink/netlink-kernel.h:
/usr/include/libnl3/netlink/socket.h:
/usr/include/libnl3/netlink/object.h:
/usr/include/libnl3/netlink/utils.h:
/usr/include/libnl3/netlink/list.h:
/usr/include/libnl3/netlink/addr.h:
/usr/include/libnl3/netlink/route/rtnl.h:
/usr/include/libnl3/netlink/route/link.h:
/usr/include/libnl3/netlink/cache.h:
/usr/include/libnl3/netlink/msg.h:
/usr/include/libnl3/netlink/attr.h:
/usr/include/libnl3/netlink/data.h:
/usr/include/libnl3/netlink/route/addr.h:
/usr/include/libnl3/netlink/route/neighbour.h:
/usr/include/libnl3/netlink/route/neightbl.h:
/usr/include/libnl3/netlink/route/route.h:
/usr/include/libnl3/netlink/route/nexthop.h:
/usr/include/libnl3/netlink/route/rule.h:
/usr/include/libnl3/netlink/route/qdisc.h:
/usr/include/libnl3/netlink/route/tc.h:
/usr/include/libnl3/netlink/route/class.h:
/usr/include/libnl3/netlink/route/classifier.h:
/usr/include/libnl3/netlink/route/cls/ematch.h:
/usr/include/libnl3/netlink/fib_lookup/lookup.h:
/usr/include/libnl3/netlink/fib_lookup/request.h:
/usr/include/libnl3/netlink/genl/genl.h:
/usr/include/libnl3/netlink/genl/ctrl.h:
/usr/include/libnl3/netlink/genl/mngt.h:
/usr/include/libnl3/netlink/netfilter/ct.h:
/usr/include/libnl3/netlink/cli/route.h:
/usr/include/libnl3/netlink/cli/link.h:
