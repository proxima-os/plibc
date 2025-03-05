# rtld startup code needs to be built with different compile flags than the rest, put it in a static lib
rtld_start = static_library(
    'rtld-start',
    'start.c',
    'start.S',
    build_by_default: false,
    c_args: ['-ffreestanding', '-fno-stack-protector'],
    include_directories: [inc],
)
