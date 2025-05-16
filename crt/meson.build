crt1_args = []

if get_option('debug')
    crt1_args += '-g'
endif

crt1 = custom_target(
    'crt1',
    build_by_default: true,
    command: compiler.cmd_array() + ['@INPUT@', '-o', '@OUTPUT@', '-c'] + crt1_args,
    input: files('crt1.S'),
    install: true,
    install_dir: get_option('libdir'),
    install_tag: 'runtime',
    output: 'crt1.o',
)
