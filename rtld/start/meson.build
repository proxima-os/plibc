rtld_start = static_library(
    'rtld-start',
    'start.c',
    'start.S',
    build_by_default: false,
    c_args: ['-ffat-lto-objects', '-ffreestanding', '-fno-stack-protector'],
    gnu_symbol_visibility: 'hidden',
    include_directories: inc,
    pic: true,
)
