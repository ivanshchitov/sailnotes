INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/include/QEverCloud.h \
    $$PWD/thrift.h \
    $$PWD/http.h \
    $$PWD/public.h \
    $$PWD/exceptions.h \
    $$PWD/globals.h \
    $$PWD/impl.h \
    $$PWD/thumbnail.h \
    $$PWD/AsyncResult.h \
    $$PWD/Optional.h \
    $$PWD/EverCloudException.h \
    $$PWD/EventLoopFinisher.h \
    $$PWD/generated/constants.h \
    $$PWD/generated/services.h \
    $$PWD/generated/types.h \
    $$PWD/generated/types_impl.h \
    $$PWD/generated/EDAMErrorCode.h


SOURCES += \
    $$PWD/exceptions.cpp \
    $$PWD/http.cpp \
    $$PWD/services_nongenerated.cpp \
    $$PWD/AsyncResult.cpp \
    $$PWD/EventLoopFinisher.cpp \
    $$PWD/generated/constants.cpp \
    $$PWD/generated/services.cpp  \
    $$PWD/generated/types.cpp





