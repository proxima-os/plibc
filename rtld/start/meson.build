rtld_start_sources += files(
    'start.c',
)

rtld_start = static_library(
    'rtld-start',
    build_by_default: false,
    c_args: ['-ffat-lto-objects', '-ffreestanding', '-fno-stack-protector'],
    gnu_symbol_visibility: 'hidden',
    include_directories: inc,
    pic: true,
    sources: rtld_start_sources,
)
