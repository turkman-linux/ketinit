project('init', 'c')

init_files = [
    'src/cgroup.c',
    'src/service.c',
    'src/util.c',
    'src/main.c'
]
service_files = run_command('find', 'services','-type','f',check:true).stdout().strip().split('\n')

add_project_arguments(['-Wall', '-Wextra', '-Werror'], language: 'c')
add_project_arguments(['-fno-plt', '-fvisibility=hidden', '-s'], language: 'c')

executable('ketinit', init_files, install: true,  install_dir : get_option('bindir'))
foreach service: service_files
    install_data(service, install_dir : '/etc/ket/')
endforeach
