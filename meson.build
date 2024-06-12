project('init', 'c')

init_files = [
    'src/cgroup.c',
    'src/service.c',
    'src/util.c',
    'src/main.c'
]
add_project_arguments(['-Wall', '-Wextra', '-Werror'], language: 'c')
add_project_arguments(['-fno-plt', '-fvisibility=hidden', '-s'], language: 'c')
executable('init', init_files)

