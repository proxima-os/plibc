crt0_args = []

if get_option('debug')
    crt0_args += '-g'
endif

crt0 = custom_target(
    'crt0',
    build_by_default: true,
    command: compiler.cmd_array() + ['@INPUT@', '-o', '@OUTPUT@', '-c'] + crt0_args,
    input: files('crt0.S'),
    install: true,
    install_dir: get_option('libdir'),
    install_tag: 'runtime',
    output: 'crt0.o',
)
