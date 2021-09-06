project('pw64', 'c',
  version: '0.2.0',
  meson_version: '>=0.53.2',
  default_options: ['c_std=c99', 'warning_level=3'])

src = ['base64.c', 'password.c', 'main.c']
deps = [ dependency('nettle', version: '>=3.4') ]
args = []

if meson.get_compiler('c').get_id() == 'gcc'
  args += ['-D_POSIX_C_SOURCE=2']
endif

config = configuration_data()
config.set('version', meson.project_version())
config.set('name', meson.project_name())
configure_file(
  input: 'config.h.in',
  output: 'config.h',
  configuration: config
)

pw64 = executable('pw64', src, dependencies: deps, c_args: args, install: true)
subdir('tests')
