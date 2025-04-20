libc_common = static_library(
    'c-common',
    'sys/auxv/getauxval.c',
    'start.c',
    build_by_default: false,
    c_args: ['-ffat-lto-objects', '-ffreestanding', '-Wno-prio-ctor-dtor'],
    gnu_symbol_visibility: 'hidden',
    include_directories: inc,
    pic: true,
)
